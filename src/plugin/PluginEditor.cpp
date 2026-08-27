#include "plugin/PluginEditor.h"

//==============================================================================

CathodyneEditor::CathodyneEditor (CathodyneProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      waveformTimer ([this] { waveformTimerCallback(); }, 20),
      schematicTimer ([this] { circuitTimerCallback(); }, 20),
      inspectTimer ([this] { inspectTimerCallback(); }, 1),
      waveformDisplay (p)
{
    audioProcessor.addChangeListener (this);

    //==========================================================================
    // SCHEMATIC PANEL
    //==========================================================================

    schematic = std::make_unique<SchematicPanel> (this);
    updateSchematic();
    addAndMakeVisible (*schematic);

    //==========================================================================
    // WAVEFORM DISPLAY
    //==========================================================================
    addAndMakeVisible (waveformDisplay);
    juce::Path sinepath = createSineWavePath(juce::Rectangle<float>(10, 10, 40, 40), 2.0f, 0.35f);
    // sinepath.addRoundedRectangle (0, 0, 60, 60, 10.0f);

    scopeButton = std::make_unique<PathToggleButton> (sinepath, "Scope", juce::Colours::white.withAlpha(0.5f),  getColourHotRed());
    scopeButton->setClickingTogglesState (true);
    scopeButton->onClick = [this] 
    {
        showScope = scopeButton->getToggleState();
        waveformDisplay.setVisible (showScope);
        resized();
    };
    addAndMakeVisible (*scopeButton);

    //==========================================================================
    // SIGNAL DISPLAY
    //==========================================================================
    juce::Path sigPath;
    sigPath.addEllipse (25, 25, 10, 10);
    sigPath.addEllipse (20, 20, 20, 20);
    // sigPath.addRoundedRectangle (0, 0, 60, 60, 10.0f);

    signalButton = std::make_unique<PathToggleButton> (sigPath, "Signal", juce::Colours::white.withAlpha(0.5f),  getColourLaserGreen());
    signalButton->setClickingTogglesState (true);
    signalButton->onClick = [this] 
    {
        schematic->setSignalPathActivated(signalButton->getToggleState());
    };
    addAndMakeVisible (*signalButton);



    //==========================================================================
    // INSPECT DISPLAY
    //==========================================================================


    schematic->setInspectorhActivated(false);
    inspectTogglebutton = std::make_unique<juce::TextButton>();
    inspectTogglebutton->setButtonText ("<<");
    inspectTogglebutton->setClickingTogglesState (true);
    // inspectTogglebutton->setToggleState(false);
    inspectTogglebutton->setLookAndFeel(&glowLF);
    inspectTogglebutton->onClick = [this]
    {
        inspectTogglebutton->setButtonText (inspectTogglebutton->getToggleState() ? ">>" : "<<");
        schematic->setInspectorhActivated(inspectTogglebutton->getToggleState());
        resized();
        std::cout << "inspectTogglebutton: " << inspectTogglebutton->getToggleState() << std::endl;
    };
    addAndMakeVisible (*inspectTogglebutton);

    //==========================================================================
    // OVERSAMPLING
    //==========================================================================
    oversampleSelector.addItem ("1x", 1);
    oversampleSelector.addItem ("2x", 2);
    oversampleSelector.addItem ("4x", 3);
    oversampleSelector.addItem ("8x", 4);
    updateOversampleLabels(audioProcessor.getSampleRate());
    oversampleSelector.setLookAndFeel(&glowLF);
    addAndMakeVisible (oversampleSelector);

    oversampleLabel.setText ("Oversample", juce::dontSendNotification);
    oversampleLabel.attachToComponent (&oversampleSelector, true);
    oversampleLabel.setColour (juce::Label::textColourId, getColourGrey());

    oversampleLabel.setFont (juce::Font (juce::FontOptions (FONT_SUB2)));
    addAndMakeVisible (oversampleLabel);

    oversampleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        audioProcessor.parameters, "oversample", oversampleSelector);

    //==========================================================================
    // MONO STEREO
    //==========================================================================
    auto* monoStereoParam =audioProcessor.parameters.getRawParameterValue ("monoStereo");
    const bool isStereo = monoStereoParam->load() > 0.5f;

    monoStereoButton.setButtonText (isStereo ? "Stereo" : "Mono");
    monoStereoButton.setClickingTogglesState (true);
    monoStereoButton.setLookAndFeel (&glowLF);

    monoStereoButton.onClick = [this]
    {
        monoStereoButton.setButtonText (monoStereoButton.getToggleState() ? "Stereo" : "Mono");
    };

    monoStereoAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        audioProcessor.parameters,
        "monoStereo",
        monoStereoButton);

    addAndMakeVisible (monoStereoButton);
    //==========================================================================
    // RESET VIEW
    //==========================================================================
    resetViewButton.onClick = [this] { schematic->resetView(); };
    addAndMakeVisible (resetViewButton);

    //==========================================================================
    // PRESET
    //==========================================================================

    presetSelector.setLookAndFeel(&ComboBoxLF);
    auto* preset = dynamic_cast<juce::AudioParameterChoice*> (audioProcessor.parameters.getParameter ("preset"));
    presetSelector.setButtonText (preset->getCurrentChoiceName());
    
    presetSelector.onClick = [this]
    {
        juce::PopupMenu menu, preamps, coloring;

        for (const auto& preset : presets)
        {
            auto itemId = menuIdForPreset (preset.id);

            if (juce::String (preset.category) == "Preamps")
                preamps.addItem (itemId, preset.name);

            else if (juce::String (preset.category) == "Distortion & Coloring")
                coloring.addItem (itemId, preset.name);
            else if (juce::String (preset.category) == "General")
                menu.addItem (itemId, preset.name);
        }
        menu.addSubMenu ("Preamps", preamps);
        menu.addSubMenu ("Distortion & Coloring", coloring);

        menu.setLookAndFeel (&glowLF);
        auto options = juce::PopupMenu::Options()
            .withTargetComponent (&presetSelector)
            .withMinimumWidth (presetSelector.getWidth())
            .withStandardItemHeight(40);

        menu.showMenuAsync (
            options,
            [this] (int result)
            {
                result = presetIndexFromMenuId(result);
                std::cout<<result<<std::endl;
                if (result < 0)
                    return;

                auto* parameter = audioProcessor.parameters.getParameter ("preset");

                if (parameter == nullptr)
                    return;

                parameter->setValueNotifyingHost (parameter->convertTo0to1 (static_cast<float> (result)));

                auto* preset = dynamic_cast<juce::AudioParameterChoice*> (audioProcessor.parameters.getParameter ("preset"));

                if (preset != nullptr)
                    presetSelector.setButtonText (
                        preset->getCurrentChoiceName());
            });
    };
    addAndMakeVisible (presetSelector);
    //==========================================================================
    // SIZE
    //==========================================================================
    setSize (WINDOW_WIDTH, WINDOW_HEIGHT);
    setResizable (true, true);
    setResizeLimits (WINDOW_WIDTH, WINDOW_HEIGHT/2, WINDOW_WIDTH*2, WINDOW_HEIGHT*2);
    schematic->resetView();
}

CathodyneEditor::~CathodyneEditor()
{
    audioProcessor.removeChangeListener (this);
}

//==============================================================================

void CathodyneEditor::changeListenerCallback (juce::ChangeBroadcaster*)
{
    updateSchematic();
}

void CathodyneEditor::updateSchematic()
{
    schematic->clear();

    switch (audioProcessor.getCurrentPreset())
    {
    case PRESET_BASSMAN_PREAMP:
        schematicBuilder.buildBassmanPreamp (*schematic);
        break;

    case PRESET_DUAL_RECTIFIER_PREAMP:
        schematicBuilder.buildDualRectifierPreamp (*schematic);
        break;

    case PRESET_TWIN_REVERB:
        schematicBuilder.buildTwinReverb (*schematic);
        break;

    case PRESET_DIODE_CLIPPER:
        schematicBuilder.buildDiodeClipper (*schematic);
        break;

    case PRESET_TRIODE_GAIN_STAGE:
        schematicBuilder.buildTriodeGainStage (*schematic);
        break;

    default:
        schematicBuilder.buildDefault (*schematic);
        break;
    }

    schematic->syncSchematicToCircuit();
    schematic->resized();
    schematic->resetView();
}

//==============================================================================

void CathodyneEditor::circuitTimerCallback()
{
    schematic->updateMonitoring();
}

void CathodyneEditor::inspectTimerCallback()
{
    schematic->updateInspect();
}

void CathodyneEditor::waveformTimerCallback()
{
    waveformDisplay.repaint();
}

//==============================================================================

void CathodyneEditor::paint (juce::Graphics& g)
{
    // Background
    g.fillAll (getColourBackground());

    // Top line
    juce::Path topLine;
    topLine.startNewSubPath(0.0f, WINDOW_TOP_PANEL+1.5f);
    topLine.lineTo((float) getWidth(), WINDOW_TOP_PANEL+1.5f);
    drawGlowAndCorePath(g, topLine, 0.1f, getColourNormal(), getColourAmber(), false);
    g.setColour (getColourBackground());
    g.fillRect(botRect);



    // Title
    juce::Path titleWavePath;
    juce::Font font  = juce::FontOptions (FONT_MAINTITLE);
    juce::GlyphArrangement glyphWave;
    glyphWave.addLineOfText (font, "Cathodyne", 0.0f, 0.0f);
    glyphWave.createPath (titleWavePath);
    titleWavePath.applyTransform (
        juce::AffineTransform::translation (
            titleRect.getCentreX() - titleWavePath.getBounds().getWidth()/2.0f,
            titleRect.getCentreY() - titleWavePath.getBounds().getCentreY() ));
    
    drawGlowAndCorePath (g, titleWavePath, 0.3f, getColourNormal(), getColourAmber(), false);

    // g.setFont (font);
    // g.setColour (getColourNormal()); 
    // g.drawText ("dyne",
    //             juce::Rectangle<float> (
    //                 titleRect.getX() + titleWavePath.getBounds().getRight() + FONT_MAINTITLE*0.5f, 
    //                 titleRect.getY(), 
    //                 WINDOW_TITLE_SIZE, 
    //                 WINDOW_TOP_PANEL), juce::Justification::centredLeft);
    g.setFont (FONT_SUB2);
    g.setColour (getColourGrey()); 
    g.drawText ("v0.1",titleRect, juce::Justification::centredRight);
    
    // Subtitle
    g.setFont (FONT_SUB1);
    g.drawText ("WAVE DIGITAL CIRCUIT SIMULATOR",
                juce::Rectangle<float> (
                    subtitleRect.getX(), 
                    subtitleRect.getY(), 
                    WINDOW_SUBTITLE_SIZE, 
                    WINDOW_TOP_PANEL), juce::Justification::centred);

}

void CathodyneEditor::resized()
{
    auto area = getLocalBounds();
    auto topPanel = area.removeFromTop (WINDOW_TOP_PANEL);

    topRect = topPanel;
    botRect = area;

    // Title area
    titleRect = topPanel.removeFromLeft (WINDOW_TITLE_SIZE);
    subtitleRect = topPanel.removeFromLeft (WINDOW_SUBTITLE_SIZE);

    // Preset
    auto presetArea =topPanel.removeFromLeft(WINDOW_PRESET_SIZE).reduced (50, 10);
    presetSelector.setBounds (presetArea);

    scopeButton->setBounds (topPanel.removeFromLeft (TOGGLE_BUTTON_SIZE));
    signalButton->setBounds (topPanel.removeFromLeft (TOGGLE_BUTTON_SIZE));


    // Mono stereo button
    monoStereoButton.setBounds (topPanel.removeFromRight(90).reduced (10));

    oversampleSelector.setBounds (topPanel.removeFromRight(120).reduced (10));


    auto togglebuttArea = area;
    // togglebuttArea.removeFromTop(10);
    if (inspectTogglebutton->getToggleState())
        togglebuttArea.removeFromRight(SCHEMATIC_INSPECTOR_SIZE);
    togglebuttArea = togglebuttArea.removeFromRight(INSPECTOR_BUTTON_SIZE).removeFromTop(INSPECTOR_BUTTON_SIZE).reduced (10);
    // togglebuttArea.removeFromTop(50);
    inspectTogglebutton->setBounds (togglebuttArea.reduced (10));

    // auto leftPanel = area.removeFromLeft (WINDOW_LEFT_PANEL);

    // powerButton->setBounds (leftPanel.removeFromTop (TOGGLE_BUTTON_SIZE).reduced (10, 10));
    // inspectButton->setBounds (leftPanel.removeFromTop (TOGGLE_BUTTON_SIZE).reduced (10, 10));
    // // Oversample
    // // oversampleLabel.setBounds (topPanel.removeFromRight (80).reduced (0, 30));
    // oversampleSelector.setBounds (topPanel.removeFromRight (100));

    // // Reset view button
    // resetViewButton.setBounds (topPanel.removeFromRight (80));


    // Waveform display at top
    if (showScope)
        waveformDisplay.setBounds (area.removeFromBottom (120));

        
    // Schematic takes remaining space
    schematic->setBounds (area);

    // Bottom strip for drive / gain / oversample controls



    // // Drive / gain knobs
    // driveKnob->getSlider().setBounds (bottom.removeFromLeft (120).reduced (0, 10));
    // bottom.removeFromLeft (20);
    // gainKnob->getSlider().setBounds (bottom.removeFromLeft (120).reduced (0, 10));
}

//==============================================================================

void CathodyneEditor::setCircuitParam (const int index, float newValue)
{
    audioProcessor.setCircuitParam (index, newValue);
}

void CathodyneEditor::setCircuitControl (const int index, float newValue)
{
    audioProcessor.setCircuitControl (index, newValue);
}

const MonitorValuef& CathodyneEditor::getCircuitMonitoring (const int index)
{
    return audioProcessor.getCircuitMonitoring (index);
}
void CathodyneEditor::updateCircuitMonitoring ()
{
    audioProcessor.updateCircuitMonitoring();
}
float CathodyneEditor::getCircuitParam (const int index)
{
    return audioProcessor.getCircuitParam (index);
}

float CathodyneEditor::getCircuitControl (const int index)
{
    return audioProcessor.getCircuitControl (index);
}
