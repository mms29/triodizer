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
    juce::Point<float> getViewPosition (juce::Point<float> worldPos) const;

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
    float                                          zoomFactor     = 1.0f;
    bool                                           isPanning      = false;
    bool                                           isGrabbing     = false;
    juce::Point<float>                             grabStartPos;
    juce::Point<float>                             grabStartOffset;
    juce::Point<float>                             lastMousePos;
};


class SchematicBuilder
{
public:
    SchematicBuilder () = default;

    void buildDefault(SchematicPanel& schematic);
    // void buildCommonCathodeStage(SchematicPanel& schematic);
    void buildBassmanToneStack(SchematicPanel& schematic);
    void buildBassmanPreampSmall(SchematicPanel& schematic);
    void buildBassmanPreamp(SchematicPanel& schematic);

private:

        static constexpr float XS = 50.0f;
        static constexpr float S  = 75.0f;
        static constexpr float M  = 100.0f;
        static constexpr float L  = 150.0f;
        static constexpr float XL = 200.0f;

        inline static const Terminal left   {-1.0f,  0.0f};
        inline static const Terminal right  { 1.0f,  0.0f};
        inline static const Terminal top    { 0.0f, -1.0f};
        inline static const Terminal bottom { 0.0f,  1.0f};

        inline static const Terminal leftXS   = left   * XS;
        inline static const Terminal rightXS  = right  * XS;
        inline static const Terminal topXS    = top    * XS;
        inline static const Terminal bottomXS = bottom * XS;

        inline static const Terminal leftS    = left   * S;
        inline static const Terminal rightS   = right  * S;
        inline static const Terminal topS     = top    * S;
        inline static const Terminal bottomS  = bottom * S;

        inline static const Terminal leftM    = left   * M;
        inline static const Terminal rightM   = right  * M;
        inline static const Terminal topM     = top    * M;
        inline static const Terminal bottomM  = bottom * M;

        inline static const Terminal leftL    = left   * L;
        inline static const Terminal rightL   = right  * L;
        inline static const Terminal topL     = top    * L;
        inline static const Terminal bottomL  = bottom * L;

        inline static const Terminal leftXL   = left   * XL;
        inline static const Terminal rightXL  = right  * XL;
        inline static const Terminal topXL    = top    * XL;
        inline static const Terminal bottomXL = bottom * XL;
};
