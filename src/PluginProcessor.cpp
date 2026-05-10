#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TriodeProcessor::TriodeProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo())
                                     .withOutput("Output", juce::AudioChannelSet::stereo())),
      parameters(*this, nullptr,
          "TriodeParameters",
          {
              std::make_unique<juce::AudioParameterFloat>("drive", "drive",
                  juce::NormalisableRange<float>(0.1f, 1000.0f, 0.1f), 1.0f),
              std::make_unique<juce::AudioParameterFloat>("gain", "gain",
                  juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f),
          })
{
}

TriodeProcessor::~TriodeProcessor() = default;

//==============================================================================
void TriodeProcessor::prepareToPlay(double sr, int)
{
    sampleRate = sr;
    for (int ch = 0; ch < 2; ++ch)
        triode[ch].reset();
}

void TriodeProcessor::releaseResources()
{
    for (int ch = 0; ch < 2; ++ch)
        triode[ch].reset();
}

//==============================================================================
void TriodeProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    float drive = *parameters.getRawParameterValue("drive");
    float gain = *parameters.getRawParameterValue("gain");

    int numChannels = buffer.getNumChannels();
    int numSamples = buffer.getNumSamples();

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* samples = buffer.getWritePointer(ch);

        for (int i = 0; i < numSamples; ++i)
        {
            // Input audio (~±1.0) scaled to volts
            double Vin = (double)samples[i] *  drive;

            // Process through WDF triode circuit
            double Vout = triode[ch].processSample(Vin);

            // Apply output gain and scale back to audio range
            samples[i] = (float)(Vout * (double)gain);
        }
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
