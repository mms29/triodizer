#include "plugin/PluginProcessor.h"
#include "plugin/PluginEditor.h"

//==============================================================================

TubeLabProcessor::TubeLabProcessor()
    : AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo())
                           .withOutput ("Output", juce::AudioChannelSet::stereo())),
      parameters (*this, nullptr,
          juce::Identifier ("TubeLabParameters"),
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
                  juce::StringArray
                  {
                      "Default",
                      "Fender Bassman Preamp",
                      "Mesa/Boogie Dual Rectifier",
                      "Twin Reverb",
                    "Diode Clipper"

                  },
                  0)
          })
{
    oversamplingStages = (int) parameters.getRawParameterValue ("oversample")->load();
    currentPreset = (int) parameters.getRawParameterValue ("preset")->load() + 1;
    buildCircuit();
    sendChangeMessage();
}

TubeLabProcessor::~TubeLabProcessor() = default;

//==============================================================================

void TubeLabProcessor::prepareToPlay (double sr, int samplesPerBlock)
{
    sampleRate = sr;
    blockSize = samplesPerBlock;

    buildOversampler();
    prepareCircuit (oversampleRate);
}

void TubeLabProcessor::releaseResources()
{
    resetCircuit();
}

void TubeLabProcessor::processBlock (juce::AudioBuffer<float>& buffer,
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

    // float drive_dB = *parameters.getRawParameterValue ("drive");
    // float gain_dB = *parameters.getRawParameterValue ("gain");

    // float drive_G = juce::Decibels::decibelsToGain (drive_dB);
    // float gain_G = juce::Decibels::decibelsToGain (gain_dB);

    // buffer.applyGain (drive_G);

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

    for (int i = 0; i < osNumSamples; ++i)
    {
#ifdef XSIMD_HPP
        xsimd::batch<float> x { left[i],
                                right[i], 0.0F, 0.0F };

        auto y = circuit->processSample (x);

        left[i] = y.get (0);
        right[i] = y.get (1);
#else
        if (monoMode)
        {
            float x = 0.5f * (left[i] + right[i]);
            float y = circuit[0]->processSample (x);
            left[i] = y;
            right[i] = y;
        }
        else
        {
            left[i] = circuit[0]->processSample (left[i]);
            right[i] = circuit[1]->processSample (right[i]);
        }
#endif
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

void TubeLabProcessor::updateOversampler()
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

void TubeLabProcessor::updatePreset()
{
    int presetChoice = (int) parameters.getRawParameterValue ("preset")->load() + 1;

    // Avoid rebuilding every block
    if (currentPreset == presetChoice)
    {
        if (circuitReady())
            return;
    }

    currentPreset = presetChoice;
    buildCircuit();
    sendChangeMessage();
    prepareCircuit (oversampleRate);
}

//==============================================================================

void TubeLabProcessor::buildOversampler()
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

void TubeLabProcessor::prepareCircuit (double sr)
{
#ifdef XSIMD_HPP
    circuit->prepare (xsimd::broadcast (float (sr)));
#else
    for (int ch = 0; ch < 2; ++ch)
        circuit[ch]->prepare (sr);
#endif
}

void TubeLabProcessor::resetCircuit()
{
#ifdef XSIMD_HPP
    circuit->reset();
#else
    for (int ch = 0; ch < 2; ++ch)
        circuit[ch]->reset();
#endif
}

void TubeLabProcessor::buildCircuit()
{
#ifdef XSIMD_HPP
    using batch = xsimd::batch<float>;

    switch (getCurrentPreset())
    {
    case PRESET_BASSMAN_TS:
        circuit = std::make_unique<BassmanToneStackCircuitT<batch>>();
        break;

    case PRESET_BASSMAN_PREAMP_SMALL:
        circuit = std::make_unique<BassmanPreampCircuitT<batch>>();
        break;

    case PRESET_BASSMAN_PREAMP:
        circuit = std::make_unique<FullBassmanPreampCircuitT<batch>>();
        break;

    case PRESET_DUAL_RECTIFIER_PREAMP:
        circuit = std::make_unique<DefaultCircuit<batch>>();
        break;

    default:
        circuit = std::make_unique<DefaultCircuit<batch>>();
        break;
    }

#else
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

        default:
            circuit[ch] = std::make_unique<DefaultCircuit<float>>();
            break;
        }
    }
#endif
}

//==============================================================================

juce::AudioProcessorEditor* TubeLabProcessor::createEditor()
{
    return new TubeLabEditor (*this);
}

//==============================================================================

int TubeLabProcessor::getNumPrograms() { return 1; }
int TubeLabProcessor::getCurrentProgram() { return 0; }

void TubeLabProcessor::setCurrentProgram (int) {}

const juce::String TubeLabProcessor::getProgramName (int) { return {}; }

void TubeLabProcessor::changeProgramName (int, const juce::String&) {}

//==============================================================================

void TubeLabProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::ValueTree root ("PluginState");
    root.addChild (parameters.copyState(), -1, nullptr);
    root.addChild (saveCircuitState(), -1, nullptr);

    std::unique_ptr<juce::XmlElement> xml (root.createXml());
    copyXmlToBinary (*xml, destData);
}

void TubeLabProcessor::setStateInformation (const void* data, int sizeInBytes)
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

bool TubeLabProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

//==============================================================================
void TubeLabProcessor::updateCircuitMonitoring ( const int ch)
{
    circuit[ch]->updateMonitors();
}
const MonitorValuef& TubeLabProcessor::getCircuitMonitoring (const int index, const int ch) const
{
#ifdef XSIMD_HPP
    return circuit->getMonitoring (index).get (ch);
#else
    return circuit[ch]->getMonitoring (index);
#endif
}

float TubeLabProcessor::getCircuitParam (const int index, const int ch) const
{
#ifdef XSIMD_HPP
    return circuit->getParam (index).get (ch);
#else
    return circuit[ch]->getParam (index);
#endif
}

float TubeLabProcessor::getCircuitControl (const int index, const int ch) const
{
#ifdef XSIMD_HPP
    return circuit->getControl (index).get (ch);
#else
    return circuit[ch]->getControl (index);
#endif
}

void TubeLabProcessor::setCircuitParam (const int index, float value)
{
#ifdef XSIMD_HPP
    circuit->setParam (index, xsimd::broadcast<float> (value));
#else
    for (int ch = 0; ch < 2; ++ch)
        circuit[ch]->setParam (index, value);
#endif
}

void TubeLabProcessor::setCircuitControl (const int index, float value)
{
#ifdef XSIMD_HPP
    circuit->setControl (index, xsimd::broadcast<float> (value));
#else
    for (int ch = 0; ch < 2; ++ch)
        circuit[ch]->setControl (index, value);
#endif
}

//==============================================================================

void TubeLabProcessor::loadCircuitState (const juce::ValueTree& t)
{
#ifdef XSIMD_HPP
    for (int i = 0; i < circuit->getNumParam(); ++i)
    {
        auto name = "P" + juce::String (i);
        if (t.hasProperty (name))
            circuit->setParam (i, xsimd::broadcast<float> (t[name]));
    }

    for (int i = 0; i < circuit->getNumControl(); ++i)
    {
        auto name = "C" + juce::String (i);
        if (t.hasProperty (name))
            circuit->setControl (i, xsimd::broadcast<float> (t[name]));
    }

#else
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
#endif
}

juce::ValueTree TubeLabProcessor::saveCircuitState() const
{
    juce::ValueTree t ("Circuit");

#ifdef XSIMD_HPP
    for (int i = 0; i < circuit->getNumParam(); ++i)
        t.setProperty ("P" + juce::String (i), circuit->getParam (i).get (0), nullptr);

    for (int i = 0; i < circuit->getNumControl(); ++i)
        t.setProperty ("C" + juce::String (i), circuit->getControl (i).get (0), nullptr);

#else
    for (int ch = 0; ch < 2; ++ch)
    {
        for (int i = 0; i < circuit[ch]->getNumParam(); ++i)
            t.setProperty ("P" + juce::String (i), circuit[ch]->getParam (i), nullptr);

        for (int i = 0; i < circuit[ch]->getNumControl(); ++i)
            t.setProperty ("C" + juce::String (i), circuit[ch]->getControl (i), nullptr);
    }
#endif

    return t;
}

//==============================================================================

bool TubeLabProcessor::circuitReady() const
{
#ifdef XSIMD_HPP
    return circuit != nullptr;
#else
    return circuit[0] && circuit[1];
#endif
}