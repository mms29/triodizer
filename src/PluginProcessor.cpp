#include "PluginProcessor.h"
#include "PluginEditor.h"

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
                    juce::NormalisableRange<float>(0.0f, 60.0f, 0.1f),
                    0.0f),

                std::make_unique<juce::AudioParameterFloat>(
                    "gain",
                    "Gain",
                    juce::NormalisableRange<float>(-80.0f, -20.0f, 0.1f),
                    -20.0f),

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
                    0) 
            })
{
}

TriodeProcessor::~TriodeProcessor() = default;

//==============================================================================
void TriodeProcessor::prepareToPlay(double sr, int samplesPerBlock)
{
    sampleRate = sr;
    blockSize = samplesPerBlock;

    updateOversampler();

    for (int ch = 0; ch < 2; ++ch)
        triode[ch].prepare(sampleRate);
}

void TriodeProcessor::releaseResources()
{
    for (int ch = 0; ch < 2; ++ch)
        triode[ch].reset();
}

void TriodeProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                   juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numChannels = getTotalNumInputChannels();

    // In case user changed oversampling factor
    updateOversampler();

    // Read parameters
    float drive_dB = *parameters.getRawParameterValue("drive");
    float gain_dB = *parameters.getRawParameterValue("gain");

    float drive_G = juce::Decibels::decibelsToGain(drive_dB);
    float gain_G  = juce::Decibels::decibelsToGain(gain_dB);

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
            double Vout = triode[ch].processSample(Vin);

            // Output gain
            samples[i] = (float) (Vout * (double) gain_G);
        }
    }

    // DOWNSAMPLE BACK INTO ORIGINAL BUFFER
    oversampler->processSamplesDown(block);
}


void TriodeProcessor::updateWDFcircuit(juce::String paramName, float value){

    std::cout << "Change parameter "<< paramName << " to "<< value<< std::endl; 

    for (int ch = 0; ch < 2; ++ch){
        if      (paramName == "Ri") triode[ch].setRi(value);
        else if (paramName == "Rg") triode[ch].setRg(value);
        else if (paramName == "Ci") triode[ch].setCi(value);

        else if (paramName == "Rk") triode[ch].setRk(value);
        else if (paramName == "Ck") triode[ch].setCk(value);

        else if (paramName == "E")  triode[ch].setE(value);
        else if (paramName == "Rp") triode[ch].setRp(value);

        else if (paramName == "Co") triode[ch].setCo(value);
        else if (paramName == "Ro") triode[ch].setRo(value);

        else if (paramName == "Cp") triode[ch].setCp(value);

        else
            jassertfalse; // unknown parameter name
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

    oversampler = std::make_unique<juce::dsp::Oversampling<float>>(
        getTotalNumInputChannels(),
        stages, // 0=1x,1=2x,2=4x,3=8x
        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR);

    oversampler->initProcessing((size_t) blockSize);

    oversampler->reset();

    // std::cout << "Oversampling updated: " << (1 << stages) << "x" << std::endl;
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
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void TriodeProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml.get() != nullptr && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
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
