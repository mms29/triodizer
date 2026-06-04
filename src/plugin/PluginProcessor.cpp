#include "plugin/PluginProcessor.h"
#include "plugin/PluginEditor.h"



//==============================================================================
TubeLabProcessor::TubeLabProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo())
                             .withOutput("Output", juce::AudioChannelSet::stereo())),
        parameters(*this, nullptr,
            juce::Identifier("TubeLabParameters"),
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
                        "Fender Bassman Preamp Small",
                        "Fender Bassman Preamp",
                        "Mesa/Boogie Dual Rectifier",
                    },
                    0)
            })
{
    for (int ch = 0; ch < 2; ++ch)
        circuit[ch] = std::make_unique<DefaultCircuit>();    
    updatePreset();
}

TubeLabProcessor::~TubeLabProcessor() = default;

//==============================================================================
void TubeLabProcessor::prepareToPlay(double sr, int samplesPerBlock)
{
    sampleRate = sr;
    blockSize = samplesPerBlock;
    updateOversampler();
}

void TubeLabProcessor::releaseResources()
{
    for (int ch = 0; ch < 2; ++ch)
        circuit[ch]->reset();
}

void TubeLabProcessor::processBlock(juce::AudioBuffer<float>& buffer,
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
void TubeLabProcessor::updateOversampler()
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
void TubeLabProcessor::updatePreset()
{
    int presetChoice = (int) parameters.getRawParameterValue("preset")->load() +1;

    // Avoid rebuilding every block
    if (currentPreset == presetChoice)
        return;

    currentPreset = presetChoice; 

    for (int ch = 0; ch < 2; ++ch){
        switch(presetChoice){
            case PRESET_COMMONCATHODE: 
            {
                circuit[ch] = std::make_unique<TriodeGainStage>();
                break;
            }
            case PRESET_BASSMAN_TS: 
            {
                circuit[ch] = std::make_unique<BassmanToneStackCircuitT<float>>();
                break;
            }
            case PRESET_BASSMAN_PREAMP_SMALL: 
            {
                circuit[ch] = std::make_unique<BassmanPreampCircuit>();
                break;
            }
            case PRESET_BASSMAN_PREAMP: 
            {
                circuit[ch] = std::make_unique<FullBassmanPreampCircuit>();
                break;
            }
            case PRESET_DUAL_RECTIFIER_PREAMP: 
            {
                circuit[ch] = std::make_unique<DualRectifierPreampCircuit>();
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
    sendChangeMessage();


}
//==============================================================================
juce::AudioProcessorEditor* TubeLabProcessor::createEditor()
{
    return new TubeLabEditor(*this);
}

//==============================================================================
int TubeLabProcessor::getNumPrograms() { return 1; }
int TubeLabProcessor::getCurrentProgram() { return 0; }
void TubeLabProcessor::setCurrentProgram(int) {}
const juce::String TubeLabProcessor::getProgramName(int) { return {}; }
void TubeLabProcessor::changeProgramName(int, const juce::String&) {}

//==============================================================================
void TubeLabProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::ValueTree root("PluginState");
    root.addChild(parameters.copyState(), -1, nullptr);
    root.addChild(circuit[0]->saveState(), -1, nullptr);

    std::unique_ptr<juce::XmlElement> xml (root.createXml());
    copyXmlToBinary (*xml, destData);
}

void TubeLabProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    auto root = juce::ValueTree::fromXml(*xml);

    // Audio parameter state
    auto paramState = root.getChildWithName(parameters.state.getType());
    if (!paramState.isValid())
        return;

    parameters.replaceState(paramState);

    auto circuitState = root.getChildWithName("Circuit");
    if (!circuitState.isValid()){
        return;
    }

    updatePreset();

    for (int ch = 0; ch < 2; ++ch)
        circuit[ch]->loadState(circuitState);

}

//==============================================================================
bool TubeLabProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
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
    return new TubeLabProcessor();
}