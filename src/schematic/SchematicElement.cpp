#include "schematic/SchematicElement.h"

bool                BaseElement::isHighlighted() const noexcept  { return highlighted; }
void BaseElement::setHighlighted (bool should) noexcept { highlighted = should; }

bool BaseElement::hitTest (juce::Point<float> point) const
{
    for (const auto& terminal : terminals)
        if (terminal.getDistanceFrom (point) < 12.0f)
            return true;
    if (cachedBounds.expanded (6.0f).contains (point))
        return true;
    return false;
}

const std::vector<Terminal>& BaseElement::getTerminals() const noexcept
{
    return terminals;
}

const juce::String& SchematicElement::getName() const noexcept       { return name; }
void SchematicElement::drawLabel(juce::Graphics& g, Terminal center, juce::String value) const{
    juce::Font font (juce::FontOptions (18.0f));
    if (isHighlighted())
        font = font.boldened();
    g.setFont (font);

    // Name
    g.setColour (getColourHighlight());
    g.drawText (getName(),
                center.getX() - 40, center.getY() - 18, 80, 18,
                juce::Justification::centred, true);

    // Value
    g.setColour (getColourNormal());
    g.drawText (value,
                center.getX() - 40, center.getY() + 2, 80, 18,
                juce::Justification::centred, true);

    // cachedBounds = cachedBounds.getUnion (juce::Rectangle<float> (center.getX() - 30, center.getY() - 14, 60, 16));
}


juce::String ParametrableElement::getChoiceLabel() const
{
    if (choices.size() > (size_t)choiceIndex){
        return choices[choiceIndex].label;
    }
    return {};
}

float ParametrableElement::getChoiceValue() const
{
    if (choices.size() > (size_t)choiceIndex){
        return choices[choiceIndex].value;
    }
    return 0.0f;
}
