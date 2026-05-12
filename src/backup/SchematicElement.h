#pragma once

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

    //==================================================
    // Must be implemented by derived elements

    virtual void paint(juce::Graphics& g) = 0;
    virtual void setPosition(juce::Point<float> newPos) = 0;
    virtual void updateBounds() = 0;

    //==================================================
    // Shared interaction

    virtual void mouseDown(const juce::MouseEvent& e);
    bool hitTest(juce::Point<float> p) const;
    //==================================================
    // Value choices

    void addChoice(float value,
                   const juce::String& label);
    void setSelectedIndex(int idx);
    float getSelectedValue() const;
    juce::String getSelectedLabel() const;

    //==================================================
    // Accessors

    const juce::String& getName() const noexcept
    {
        return name;
    }

    const std::vector<Terminal>&
    getTerminals() const noexcept
    {
        return terminals;
    }

    const juce::Rectangle<float>&
    getBounds() const noexcept
    {
        return bounds;
    }

    //==================================================
    // UI callback

    std::function<void(float)> onValueChanged;

protected:
    juce::String name;
    std::vector<Terminal> terminals;
    std::vector<ValueChoice> choices;
    juce::Rectangle<float> bounds;

    int selected = 0;
    void showPopupMenu(const juce::MouseEvent& e);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SchematicElement)
};

