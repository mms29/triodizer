#include "plugin/PluginProcessor.h"
#include "plugin/PluginEditor.h"

//==============================================================================

CathodyneProcessor::CathodyneProcessor()
    : AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo())
                           .withOutput ("Output", juce::AudioChannelSet::stereo())),
      parameters (*this, nullptr,
          juce::Identifier ("CathodyneParameters"),
          {
            //   std::make_unique<juce::AudioParameterFloat> (
            //       "drive",
            //       "Drive",
            //       juce::NormalisableRange<float> (DRIVE_MIN, DRIVE_MAX, DRIVE_STEP),
            //       DRIVE_DEFAULT),

            //   std::make_unique<juce::AudioParameterFloat> (
            //       "gain",
            //       "Gain",
            //       juce::NormalisableRange<float> (GAIN_MIN, GAIN_MAX, GAIN_STEP),
            //       GAIN_DEFAULT),

              std::make_unique<juce::AudioParameterChoice> (
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

              std::make_unique<juce::AudioParameterBool> (
                  "monoStereo",
                  "Mono / Stereo",
                  false),

              std::make_unique<juce::AudioParameterChoice> (
                  "preset",
                  "Preset",
                  getPresetNames(),
                  0)
          })
{
    oversamplingStages = (int) parameters.getRawParameterValue ("oversample")->load();
    currentPreset = (int) parameters.getRawParameterValue ("preset")->load();
    buildCircuit();
    sendChangeMessage();
    circuitReadyFlag.store(true);
}

CathodyneProcessor::~CathodyneProcessor() = default;

//==============================================================================

void CathodyneProcessor::prepareToPlay (double sr, int samplesPerBlock)
{
    sampleRate = sr;
    blockSize = samplesPerBlock;

    buildOversampler();
    prepareCircuit (oversampleRate);
}

void CathodyneProcessor::releaseResources()
{
    resetCircuit();
}

void CathodyneProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                    juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numChannels = getTotalNumInputChannels();
    const int numSamples = buffer.getNumSamples();

    // In case user changed oversampling factor
    updateOversampler();
    updatePreset();

    // Read parameters
    bool monoMode = *parameters.getRawParameterValue ("monoStereo") > 0.5f;

    // Capture input samples for waveform display
    if (numChannels >= 1 && numSamples > 0)
    {
        auto* channelData = buffer.getReadPointer (0);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            waveformInputBuffer.push (channelData[i]);
        }
    }

    // Convert AudioBuffer -> AudioBlock
    juce::dsp::AudioBlock<float> block (buffer);

    // UPSAMPLE
    auto upsampledBlock = oversampler->processSamplesUp (block);

    const int osNumSamples = (int) upsampledBlock.getNumSamples();

    // PROCESS AT OVERSAMPLED RATE
    auto* left = upsampledBlock.getChannelPointer (0);
    auto* right = upsampledBlock.getChannelPointer (1);

    auto outGain = circuit[0]->getOutputGain();


    for (int i = 0; i < osNumSamples; ++i)
    {
        
        if (!monoMode)
        {
            float x = 0.5f * (left[i] + right[i]);
            float y = circuit[0]->processSample (x) * outGain;
            left[i] = y;
            right[i] = y;
        }
        else
        {
            left[i] = circuit[0]->processSample (left[i] * outGain);
            right[i] = circuit[1]->processSample (right[i] * outGain);
        }

    }

    // DOWNSAMPLE BACK INTO ORIGINAL BUFFER
    oversampler->processSamplesDown (block);

    // buffer.applyGain (gain_G);

    // Capture output samples for waveform display
    if (numChannels >= 1 && numSamples > 0)
    {
        auto* channelData = buffer.getReadPointer (0);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            waveformOutputBuffer.push (channelData[i]);
        }
    }
}

//==============================================================================

void CathodyneProcessor::updateOversampler()
{
    int stages = (int) parameters.getRawParameterValue ("oversample")->load();

    // Avoid rebuilding every block
    if (stages == oversamplingStages)
        return;

    oversamplingStages = stages;

    buildOversampler();
    resetCircuit();
    prepareCircuit (oversampleRate);
}

void CathodyneProcessor::updatePreset()
{
    int presetChoice = (int) parameters.getRawParameterValue ("preset")->load() ;

    // Avoid rebuilding every block
    if (currentPreset == presetChoice)
    {
        if (circuitReady())
            return;
    }

    circuitReadyFlag.store(false);
    currentPreset = presetChoice;
    buildCircuit();
    prepareCircuit (oversampleRate);
    sendChangeMessage();
    circuitReadyFlag.store(true);
}

//==============================================================================

void CathodyneProcessor::buildOversampler()
{
    oversampleRate = sampleRate * (1 << oversamplingStages);
    oversampler = std::make_unique<juce::dsp::Oversampling<float>> (
        getTotalNumInputChannels(),
        oversamplingStages, // 0=1x,1=2x,2=4x,3=8x
        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR);
    oversampler->initProcessing ((size_t) blockSize);
    oversampler->reset();
}

//==============================================================================

void CathodyneProcessor::prepareCircuit (double sr)
{
    for (int ch = 0; ch < 2; ++ch)
        circuit[ch]->prepare (sr);
}

void CathodyneProcessor::resetCircuit()
{
    for (int ch = 0; ch < 2; ++ch)
        circuit[ch]->reset();
}

void CathodyneProcessor::buildCircuit()
{
    for (int ch = 0; ch < 2; ++ch)
    {
        switch (getCurrentPreset())
        {

        case PRESET_BASSMAN_PREAMP:
            circuit[ch] = std::make_unique<FullBassmanPreampCircuitT<float>>();
            break;

        case PRESET_DUAL_RECTIFIER_PREAMP:
            circuit[ch] = std::make_unique<DualRectifierPreampCircuit>();
            break;

        case PRESET_TWIN_REVERB:
            circuit[ch] = std::make_unique<TwinReverbCircuit>();
            break;

        case PRESET_DIODE_CLIPPER:
            circuit[ch] = std::make_unique<DiodeClipperCircuit>();
            break;

        case PRESET_TRIODE_GAIN_STAGE:
            circuit[ch] = std::make_unique<TriodeGainStageCircuit>();
            break;

        case PRESET_FENDER_TONE_STACK:
            circuit[ch] = std::make_unique<BassmanToneStackCircuitT<float>>();
            break;

        default:
            circuit[ch] = std::make_unique<DefaultCircuit<float>>();
            break;
        }
    }
}

//==============================================================================

juce::AudioProcessorEditor* CathodyneProcessor::createEditor()
{
    return new CathodyneEditor (*this);
}

//==============================================================================

int CathodyneProcessor::getNumPrograms() { return 1; }
int CathodyneProcessor::getCurrentProgram() { return 0; }

void CathodyneProcessor::setCurrentProgram (int) {}

const juce::String CathodyneProcessor::getProgramName (int) { return {}; }

void CathodyneProcessor::changeProgramName (int, const juce::String&) {}

//==============================================================================

void CathodyneProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::ValueTree root ("PluginState");
    root.addChild (parameters.copyState(), -1, nullptr);
    root.addChild (saveCircuitState(), -1, nullptr);

    std::unique_ptr<juce::XmlElement> xml (root.createXml());
    copyXmlToBinary (*xml, destData);
}

void CathodyneProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    auto root = juce::ValueTree::fromXml (*xml);

    // Audio parameter state
    auto paramState = root.getChildWithName (parameters.state.getType());
    if (!paramState.isValid())
        return;

    parameters.replaceState (paramState);

    auto circuitState = root.getChildWithName ("Circuit");
    if (!circuitState.isValid())
        return;

    updatePreset();
    loadCircuitState (circuitState);
}

//==============================================================================

bool CathodyneProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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
    return new CathodyneProcessor();
}

//==============================================================================
void CathodyneProcessor::updateCircuitMonitoring ( const int ch)
{
    if (circuitReady())
        circuit[ch]->updateMonitors();
}
const MonitorValuef& CathodyneProcessor::getCircuitMonitoring (const int index, const int ch) const
{
    if (circuitReady())
        return circuit[ch]->getMonitoring (index);
    
    static const MonitorValuef emptyValue{};
    return emptyValue;
}

float CathodyneProcessor::getCircuitParam (const int index, const int ch) const
{
    if (circuitReady())
        return circuit[ch]->getParam (index);

    return 0.0f;
}

float CathodyneProcessor::getCircuitControl (const int index, const int ch) const
{
    if (circuitReady())
        return circuit[ch]->getControl (index);

    return 0.0f;
}

void CathodyneProcessor::setCircuitParam (const int index, float value)
{
    if (circuitReady())
        for (int ch = 0; ch < 2; ++ch)
            circuit[ch]->setParam (index, value);
}

void CathodyneProcessor::setCircuitControl (const int index, float value)
{
    if (circuitReady())
        for (int ch = 0; ch < 2; ++ch)
            circuit[ch]->setControl (index, value);
}

//==============================================================================

void CathodyneProcessor::loadCircuitState (const juce::ValueTree& t)
{
    if (!circuitReady())
        return;

    for (int ch = 0; ch < 2; ++ch)
    {
        for (int i = 0; i < circuit[ch]->getNumParam(); ++i)
        {
            auto name = "P" + juce::String (i);
            if (t.hasProperty (name))
                circuit[ch]->setParam (i, t[name]);
        }

        for (int i = 0; i < circuit[ch]->getNumControl(); ++i)
        {
            auto name = "C" + juce::String (i);
            if (t.hasProperty (name))
                circuit[ch]->setControl (i, t[name]);
        }
    }
}

juce::ValueTree CathodyneProcessor::saveCircuitState() const
{
    juce::ValueTree t ("Circuit");
    if (!circuitReady())
        return t;

    for (int ch = 0; ch < 2; ++ch)
    {
        for (int i = 0; i < circuit[ch]->getNumParam(); ++i)
            t.setProperty ("P" + juce::String (i), circuit[ch]->getParam (i), nullptr);

        for (int i = 0; i < circuit[ch]->getNumControl(); ++i)
            t.setProperty ("C" + juce::String (i), circuit[ch]->getControl (i), nullptr);
    }
    return t;
}

//==============================================================================

bool CathodyneProcessor::circuitReady() const
{
    return circuit[0] && circuit[1] && circuitReadyFlag.load();
}