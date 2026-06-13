#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "schematic/SchematicElement.h"
#include "schematic/TriodeElement.h"
#include "schematic/TwoTermElement.h"
#include "schematic/OneTermElement.h"
#include "gui/Knob.h"
//==============================================================================
/**
 * A single wire connecting two terminal positions on the schematic.
 */
struct Wire
{
    Wire (juce::Point<float> startPt, juce::Point<float> endPt)
        : start (startPt), end (endPt) {}

    juce::Point<float> start;
    juce::Point<float> end;
};


//==============================================================================
/**
 * Interactive schematic panel that owns all elements and wires.
 *
 * Acts as a JUCE Component placed inside the editor. Elements are drawn
 * in the order they are added — later elements appear on top.
 *
 * Clicking an element opens a popup menu so the user can pick a new
 * value from the element's getChoices() list.
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

private:
    void showPopupMenuForElement (SchematicElement* element, juce::Point<int> pos);

    //==========================================================================
    std::vector<std::unique_ptr<SchematicElement>> elements;
    std::vector<std::unique_ptr<Knob>>             controls;
    std::vector<Wire>                              wires;
    SchematicPanelListener*                        listener;
    SchematicElement*                              hoveredElement = nullptr;

    // Viewport state for pan/zoom
    juce::Point<float>                             viewOffset     = {0.0f, 0.0f};
    juce::Point<float>                             dragStartMouse = {0.0f, 0.0f};
    juce::Point<float>                             dragStartOffset = {0.0f, 0.0f};
    float                                          zoomFactor     = 1.0f;
};
