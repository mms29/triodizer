#include "SchematicElement.h"

SchematicElement::SchematicElement (const juce::String& name,
                                    int paramIndex,
                                    std::vector<ValueChoice> parameters,
                                    std::vector<Terminal> terminals)
    : name (name),
      paramIndex (paramIndex),
      parameters (std::move (parameters)),
      terminals (std::move (terminals))
{
}

const juce::String& SchematicElement::getName() const noexcept       { return name; }
void                SchematicElement::setName (const juce::String& n) { name = n; }
int                 SchematicElement::getValueIndex() const noexcept  { return paramIndex; }
void                SchematicElement::setValueIndex (int n) noexcept  { paramIndex = n; }
bool                SchematicElement::isHighlighted() const noexcept  { return highlighted; }

std::vector<ValueChoice> SchematicElement::getParameters() const{ return parameters; }

void SchematicElement::setHighlighted (bool should) noexcept { highlighted = should; }

juce::String SchematicElement::getParamLabel() const
{
    if (parameters.size() > paramIndex){
        return parameters[paramIndex].label;
    }
}

float SchematicElement::getParamValue() const
{
    if (parameters.size() >paramIndex){
        return parameters[paramIndex].value;
    }
}

const std::vector<Terminal>& SchematicElement::getTerminals() const noexcept
{
    return terminals;
}

bool SchematicElement::hitTest (juce::Point<float> point) const noexcept
{
    for (const auto& terminal : terminals)
        if (terminal.getDistanceFrom (point) < 12.0f)
            return true;
    if (cachedBounds.expanded (6.0f).contains (point))
        return true;
    return false;
}
void SchematicElement::drawLabel(juce::Graphics& g, Terminal center) const noexcept{
    juce::Font font (juce::FontOptions (18.0f));
    if (isHighlighted())
        font = font.boldened();
    g.setFont (font);

    // Name
    g.setColour (juce::Colours::yellow);
    g.drawText (getName(),
                center.getX() - 40, center.getY() - 18, 80, 18,
                juce::Justification::centred, true);

    // Value
    g.setColour (juce::Colours::lightblue);
    g.drawText (getParamLabel(),
                center.getX() - 40, center.getY() + 2, 80, 18,
                juce::Justification::centred, true);

    // cachedBounds = cachedBounds.getUnion (juce::Rectangle<float> (center.getX() - 30, center.getY() - 14, 60, 16));
}

GroundElement::GroundElement (Terminal termPosition)
    : SchematicElement ("",
                        0,
                        {},   
                        std::vector<Terminal> {termPosition})
{
    jassert (terminals.size() == 1);
}


void GroundElement::draw (juce::Graphics& g) const
{
    float thickness = STROKE_NORMAL;
    g.setColour (juce::Colours::white);

    float groundSize = 20.0f;

    const auto& p0 = terminals[0];

    g.drawLine(juce::Line( p0.x, p0.y, p0.x, p0.y + groundSize), thickness);
    g.drawLine(juce::Line(p0.x - groundSize, p0.y + groundSize, p0.x + groundSize, p0.y + groundSize), thickness);
    g.drawLine(juce::Line(p0.x - groundSize*0.66f, p0.y + groundSize*1.25f, p0.x + groundSize*0.66f, p0.y + groundSize*1.25f), thickness);
    g.drawLine(juce::Line(p0.x - groundSize*0.33f, p0.y + groundSize*1.5f, p0.x + groundSize*0.33f, p0.y + groundSize*1.5f), thickness);

}

JunctionElement::JunctionElement (Terminal termPosition)
    : SchematicElement ("",
                        0,
                        {},   
                        std::vector<Terminal> {termPosition})
{
    jassert (terminals.size() == 1);
}
void JunctionElement::draw (juce::Graphics& g) const
{
    g.setColour (juce::Colours::white);
    const auto& p0 = terminals[0];

    float radius = 8.0f;

    g.fillEllipse(p0.x - radius*0.5f, p0.y - radius*0.5f, radius, radius);

}
void VoltageElement::draw (juce::Graphics& g) const
{
    float thickness = isHighlighted() ? STROKE_HIGHLIGHT : STROKE_NORMAL;
    g.setColour (juce::Colours::white);

    float groundSize = 20.0f;

    const auto& p0 = terminals[0];

    g.drawLine(juce::Line( p0.x, p0.y, p0.x, p0.y - groundSize), thickness);
    g.drawLine(juce::Line(p0.x - groundSize, p0.y - groundSize, p0.x + groundSize, p0.y - groundSize), thickness);
    g.drawLine(juce::Line(p0.x - groundSize, p0.y - groundSize, p0.x , p0.y - groundSize*2), thickness);
    g.drawLine(juce::Line(p0.x + groundSize, p0.y - groundSize, p0.x , p0.y - groundSize*2), thickness);

    cachedBounds = juce::Rectangle<float>(p0.x-groundSize, p0.y-groundSize*2 , groundSize*2, groundSize*2);

    drawLabel(g, p0 - Terminal {0.0f, groundSize*3.4f});


}
