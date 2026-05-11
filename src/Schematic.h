#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include <functional>
#include <JuceHeader.h>

class SchematicElement
{
public:
    struct Terminal
    {
        juce::Point<float> pos;
        juce::String name;
    };

    struct ValueChoice
    {
        float value;
        juce::String label;
    };

    explicit SchematicElement(const juce::String& elementName);
    virtual ~SchematicElement() = default;

    //==============================================================
    // Pure virtual interface

    virtual void paint(juce::Graphics&) = 0;
    virtual void setPosition(juce::Point<float>) = 0;
    virtual void updateBounds() = 0;

    //==============================================================
    // Shared interaction

    virtual void mouseDown(const juce::MouseEvent&);

    //==============================================================
    // Value selection

    void addChoice(float value, const juce::String& label);
    void setSelectedIndex(int idx);
    float getSelectedValue() const;
    juce::String getSelectedLabel() const;

    //==============================================================
    // Accessors

    const juce::String& getName() const noexcept
    {
        return name;
    }

    const std::vector<Terminal>& getTerminals() const noexcept
    {
        return terminals;
    }

    const juce::Rectangle<float>& getBounds() const noexcept
    {
        return bounds;
    }

    //==============================================================
    // UI callback

    std::function<void(float)> onValueChanged;

protected:
    juce::String name;
    std::vector<Terminal> terminals;
    std::vector<ValueChoice> choices;
    juce::Rectangle<float> bounds;

    int selected = 0;

    void showPopupMenu(const juce::MouseEvent&);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SchematicElement)
};


class ResistorElement : public SchematicElement
{
public:
    ResistorElement(const juce::String& name,
                    juce::Point<float> start,
                    juce::Point<float> end);

    void paint(juce::Graphics&) override;

    void setPosition(juce::Point<float>) override;

    void updateBounds() override;

private:
    juce::Point<float> p0, p1;
};

//==============================================================================
/**
 * SchematicPanel — owns a collection of SchematicElements and the wires
 * that connect their terminals.
 *
 * Add elements with addElement() / addTriode() and wires with addWire().
 * All drawing and hit-testing is handled here.
 */
class SchematicPanel : public juce::Component
{
public:
    SchematicPanel();
    ~SchematicPanel() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;

    //--------------------------------------------------------------------
    /** Add a two-terminal element. Returns a pointer owned by the panel. */
    void addElement(std::unique_ptr<SchematicElement> elem);

    /** Connect two terminals with a drawn wire. */
    void addWire(SchematicElement* e0, int term0,
                 SchematicElement* e1, int term1);

    /** Remove all elements and wires. */
    void clearAll();

    //--------------------------------------------------------------------
    const std::vector<std::unique_ptr<SchematicElement>>& getElements() const { return elements_; }

private:
    //--------------------------------------------------------------------
    std::vector<std::unique_ptr<SchematicElement>> elements_;

    struct Wire
    {
        SchematicElement* elemA;
        int               termA;
        SchematicElement* elemB;
        int               termB;
    };
    std::vector<Wire> wires_;

    juce::Colour backgroundColour = juce::Colour(0xFF101020);
    juce::Colour wireColour       = juce::Colour(0xFFCCCCCC);
    juce::Colour nodeColour       = juce::Colour(0xFF888888);
    juce::Colour textColour       = juce::Colour(0xFFDDDDDD);

    void drawWires(juce::Graphics& g);
    void drawNodes(juce::Graphics& g);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SchematicPanel)
};

// Defined in Schematic.cpp — builds a common-cathode triode stage schematic.
// You can call this once from your editor constructor and then rearrange elements as needed.
void buildCommonCathodeStage(SchematicPanel& panel);