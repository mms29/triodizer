#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "schematic/SchematicElement.h"
#include "schematic/TriodeElement.h"
#include "schematic/TwoTermElement.h"
#include "schematic/OneTermElement.h"
#include "schematic/MultiTermElement.h"
#include "schematic/Inspector.h"
#include "gui/Knob.h"
#include "gui/Selector.h"


//==============================================================================
/**
 * Interactive schematic panel that owns all elements and wires.
    * It handles mouse events, drawing, and communication with the circuit model.
 */
class SchematicPanel : public juce::Component,
                        private juce::PopupMenu::Options
{
public:
    SchematicPanel(SchematicPanelListener* l);
    ~SchematicPanel() override = default;

    void clear();
    void syncSchematicToCircuit();

    void addElement (std::unique_ptr<SchematicElement> element);
    void addWire (juce::Point<float> start, juce::Point<float> end);
    void addWireElem (std::unique_ptr<WireElement> wire);
    int getNumElements() const noexcept;
    SchematicElement* getElement (juce::String name) const noexcept;

    // Viewport control
    void setZoom (float newZoom);
    void zoomIn();
    void zoomOut();
    void resetView();
    void setOffset (juce::Point<float> newOffset);
    void initOffset ();

    // Monitor / voltmeter interface
    void updateMonitoring ();
    void updateInspect ();
    //==========================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseMove (const juce::MouseEvent& e) override;
    void mouseExit (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;
    void mouseDoubleClick (const juce::MouseEvent& e) override;
    void mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;
    SchematicElement* getElementAt (juce::Point<int> position) const;

    void setSignalPathActivated(bool v) {signalPathActivated = v; }
    void setInspectorhActivated(bool v) { inspector.setVisible(v); }
    void addTubeJunctions();


private:
    void showPopupMenuForElement (SchematicElement* element, juce::Point<int> pos);

    //==========================================================================
    std::vector<std::unique_ptr<SchematicElement>> elements;
    std::vector<std::unique_ptr<Knob>>             controls;
    juce::Rectangle<float>                         controlsBounds;
    SchematicPanelListener*                        listener;
    SchematicElement*                              hoveredElement = nullptr;
    Inspector                                      inspector;
    GlowLookAndFeel                                glowLookAndFeel;


    // Viewport state for pan/zoom
    juce::Point<float>                             viewOffset     = {0.0f, 0.0f};
    juce::Point<float>                             dragStartMouse = {0.0f, 0.0f};
    juce::Point<float>                             dragStartOffset = {0.0f, 0.0f};
    float                                          zoomFactor     = 1.0f;

    // SigPath
    bool signalPathActivated = false;

};
