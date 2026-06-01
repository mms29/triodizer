#include "plugin/PluginProcessor.h"
#include "plugin/PluginEditor.h"



//==============================================================================
TriodeProcessor::TriodeProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo())
                             .withOutput("Output", juce::AudioChannelSet::stereo())),
        parameters(*this, nullptr,
            juce::Identifier("TriodeParameters"),
            {
                std::make_unique<juce::AudioParameterFloat>(
                    "drive",
                    "Drive",
                    juce::NormalisableRange<float>(DRIVE_MIN, DRIVE_MAX, DRIVE_STEP),
                    DRIVE_DEFAULT),

                std::make_unique<juce::AudioParameterFloat>(
                    "gain",
                    "Gain",
                    juce::NormalisableRange<float>(GAIN_MIN, GAIN_MAX, GAIN_STEP),
                    GAIN_DEFAULT),

                std::make_unique<juce::AudioParameterChoice>(
                    "oversample",
                    "Oversample",
                    juce::StringArray
                    {
                        "1x",
                        "2x",
                        "4x",
                        "8x"
                    },
                    0),
                std::make_unique<juce::AudioParameterChoice>(
                    "preset",
                    "Preset",
                    juce::StringArray
                    {
                        "Default",
                        "Common Cathode Stage",
                        "Fender Bassman Tone Stack",
                        "Fender Bassman Preamp",
                    },
                    0)
            })
{
    for (int ch = 0; ch < 2; ++ch)
        circuit[ch] = std::make_unique<DefaultCircuit>();    
    updatePreset();
}

TriodeProcessor::~TriodeProcessor() = default;

//==============================================================================
void TriodeProcessor::prepareToPlay(double sr, int samplesPerBlock)
{
    sampleRate = sr;
    blockSize = samplesPerBlock;
    updateOversampler();
}

void TriodeProcessor::releaseResources()
{
    for (int ch = 0; ch < 2; ++ch)
        circuit[ch]->reset();
}

void TriodeProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                   juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numChannels = getTotalNumInputChannels();
    const int numSamples = buffer.getNumSamples();

    // In case user changed oversampling factor
    updateOversampler();

    updatePreset();

    // Read parameters
    float drive_dB = *parameters.getRawParameterValue("drive");
    float gain_dB = *parameters.getRawParameterValue("gain");

    float drive_G = juce::Decibels::decibelsToGain(drive_dB);
    float gain_G  = juce::Decibels::decibelsToGain(gain_dB);

    // Capture input samples for waveform display
    if (numChannels >= 1 && numSamples > 0)
    {
        auto* channelData = buffer.getReadPointer(0);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            waveformInputBuffer.push(channelData[i]);
        }
    }

    // Convert AudioBuffer -> AudioBlock
    juce::dsp::AudioBlock<float> block(buffer);

    // UPSAMPLE
    auto upsampledBlock = oversampler->processSamplesUp(block);

    const int osNumSamples = (int) upsampledBlock.getNumSamples();

    // PROCESS AT OVERSAMPLED RATE
    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* samples = upsampledBlock.getChannelPointer((size_t) ch);

        for (int i = 0; i < osNumSamples; ++i)
        {
            // Input audio scaled by drive
            double Vin = (double) samples[i] * (double) drive_G;

            // Your nonlinear WDF triode
            double Vout = circuit[ch]->processSample(Vin);

            // Output gain
            samples[i] = (float) (Vout * (double) gain_G);
        }

    }

    // DOWNSAMPLE BACK INTO ORIGINAL BUFFER
    oversampler->processSamplesDown(block);

    // Capture output samples for waveform display
    if (numChannels >= 1 && numSamples > 0)
    {
        auto* channelData = buffer.getReadPointer(0);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            waveformOutputBuffer.push(channelData[i]);
        }
    }
}


//==============================================================================
void TriodeProcessor::updateOversampler()
{
    int stages = (int) parameters.getRawParameterValue("oversample")->load();

    // Avoid rebuilding every block
    if (stages == oversamplingStages)
        return;

    oversamplingStages = stages;

    oversampleRate = sampleRate* (1 << oversamplingStages);

    oversampler = std::make_unique<juce::dsp::Oversampling<float>>(
        getTotalNumInputChannels(),
        stages, // 0=1x,1=2x,2=4x,3=8x
        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR);

    oversampler->initProcessing((size_t) blockSize);

    oversampler->reset();


    for (int ch = 0; ch < 2; ++ch){
        circuit[ch]->reset();
        circuit[ch]->prepare(oversampleRate);
    }
}
//==============================================================================
void TriodeProcessor::updatePreset()
{
    int presetChoice = (int) parameters.getRawParameterValue("preset")->load() +1;

    // Avoid rebuilding every block
    if (currentPreset == presetChoice)
        return;

    currentPreset = presetChoice; 
    sendChangeMessage();

    for (int ch = 0; ch < 2; ++ch){
        switch(presetChoice){
            case PRESET_COMMONCATHODE: 
            {
                circuit[ch] = std::make_unique<TriodeGainStage>();
                break;
            }
            case PRESET_BASSMAN_TS: 
            {
                circuit[ch] = std::make_unique<BassmanToneStackCircuit>();
                break;
            }
            case PRESET_BASSMAN_PREAMP: 
            {
                circuit[ch] = std::make_unique<BassmanPreampCircuit>();
                break;
            }
            default: 
            {
                circuit[ch] = std::make_unique<DefaultCircuit>();
                break;
            }
        }
        circuit[ch]->prepare(oversampleRate);

    }

}
//==============================================================================
juce::AudioProcessorEditor* TriodeProcessor::createEditor()
{
    return new TriodeEditor(*this);
}

//==============================================================================
int TriodeProcessor::getNumPrograms() { return 1; }
int TriodeProcessor::getCurrentProgram() { return 0; }
void TriodeProcessor::setCurrentProgram(int) {}
const juce::String TriodeProcessor::getProgramName(int) { return {}; }
void TriodeProcessor::changeProgramName(int, const juce::String&) {}

//==============================================================================
void TriodeProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    std::cout << "Saving audio state ..." << std::endl;

    juce::ValueTree root("PluginState");
    root.addChild(parameters.copyState(), -1, nullptr);
    root.addChild(circuit[0]->saveState(), -1, nullptr);

    std::unique_ptr<juce::XmlElement> xml (root.createXml());
    copyXmlToBinary (*xml, destData);


    std::cout << "Done " << std::endl;
    std::cout << xml->toString().toStdString() << std::endl;
}

void TriodeProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::cout << "Loading audio state ..." << std::endl;
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    auto root = juce::ValueTree::fromXml(*xml);

    // Audio parameter state
    std::cout << "Trying to load " << parameters.state.getType().toString()<<""<< std::endl;
    auto paramState = root.getChildWithName(parameters.state.getType());
    if (!paramState.isValid())
        return;

    parameters.replaceState(paramState);
    std::cout << "-> Loaded parameter state" << std::endl;
    std::cout << xml->toString().toStdString() << std::endl;

    auto circuitState = root.getChildWithName("Circuit");
    if (!circuitState.isValid()){
        std::cout << "Could not retrieve audio param state" << std::endl;
        return;
    }

    updatePreset();

    for (int ch = 0; ch < 2; ++ch)
        circuit[ch]->loadState(circuitState);

    std::cout << "-> Loaded circuit state" << std::endl;

}

//==============================================================================
bool TriodeProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return true;
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TriodeProcessor();
}