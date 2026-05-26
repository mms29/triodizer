#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include "gui/Knob.h"

    
const float STROKE_NORMAL = 2.0f;
const float STROKE_HIGHLIGHT = 4.0f;

struct ValueChoice
{
    float value;
    juce::String label;
};

using Terminal = juce::Point<float> ;


class SchematicElement
{
public:
    SchematicElement (const juce::String& name,
                      int paramIndex,
                      std::vector<ValueChoice> ,
                      std::vector<Terminal> terminals);

    virtual ~SchematicElement() = default;

    const juce::String& getName() const noexcept;
    void setName (const juce::String& newName);
    int getValueIndex() const noexcept;
    void setValueIndex (int newIndex) noexcept;
    juce::String getParamLabel() const;
    float getParamValue() const;
    const std::vector<Terminal>& getTerminals() const noexcept;

    std::vector<ValueChoice> getParameters() const;

    virtual void draw (juce::Graphics& g) const = 0;

    virtual bool hitTest (juce::Point<float> point) const;

    bool isHighlighted() const noexcept;
    virtual bool isControlable() const;
    void setHighlighted (bool shouldBeHighlighted) noexcept;

    void drawLabel(juce::Graphics& g, Terminal center) const noexcept;

protected:
    bool                               highlighted      = false;
    mutable juce::Rectangle<float>     cachedBounds;
    juce::String                       name;
    int                                paramIndex       = 0;
    std::vector<Terminal>              terminals;
    std::vector<ValueChoice>           parameters;
};

class PotElement : public SchematicElement
{
public:
    using SchematicElement::SchematicElement;
    void draw (juce::Graphics& g) const override;
    bool isControlable() const override { return true;}
private:
    //==========================================================================
    std::unique_ptr<Knob>   controlKnob;
    static constexpr int    zigzagCount    = 6;
    static constexpr float  zigzagAmplitude = 10.0f;
    static constexpr int    zigzagLength = 40.0f;
};