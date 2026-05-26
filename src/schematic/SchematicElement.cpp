#include "schematic/SchematicElement.h"

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
bool                SchematicElement::isControlable() const           { return false;}

std::vector<ValueChoice> SchematicElement::getParameters() const{ return parameters; }

void SchematicElement::setHighlighted (bool should) noexcept { highlighted = should; }

juce::String SchematicElement::getParamLabel() const
{
    if (parameters.size() > (size_t)paramIndex){
        return parameters[paramIndex].label;
    }
    return {};
}

float SchematicElement::getParamValue() const
{
    if (parameters.size() > (size_t)paramIndex){
        return parameters[paramIndex].value;
    }
    return 0.0f;
}
const std::vector<Terminal>& SchematicElement::getTerminals() const noexcept
{
    return terminals;
}

bool SchematicElement::hitTest (juce::Point<float> point) const
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


void PotElement::draw (juce::Graphics& g) const
{
    float thickness = isHighlighted() ? STROKE_HIGHLIGHT : STROKE_NORMAL;
    g.setColour (juce::Colours::white);

    const auto& p0 = terminals[0];
    const auto& p1 = terminals[1];
    const auto& p2 = terminals[2];

    const juce::Point<float> d = p1-p0;
    const float length = p1.getDistanceFrom(p0);
    if (length < zigzagLength) return;

    const juce::Point<float> u = d/length;
    const juce::Point<float> v {- u.getY(), u.getX()};

    const float halfAmp = zigzagAmplitude;
    const float s = zigzagLength/(zigzagCount*2);

    const juce::Point<float> a = p0 + d*(length-zigzagLength)/(2*length);
    const juce::Point<float> b = p1 - d*(length-zigzagLength)/(2*length);

    juce::Path zigzag;
    zigzag.startNewSubPath (p0);

    cachedBounds = juce::Rectangle<float> (p0, p1);
    cachedBounds.expand(1.0f + std::abs(v.x*halfAmp), 1.0f + std::abs(v.y*halfAmp));

    zigzag.lineTo(a);
    juce::Point<float>  curr = a;
    for (int i = 0; i <= zigzagCount; ++i)
    {
        int sign = std::pow(-1, i);
        curr = curr + (halfAmp * v * sign) + (s * u);
        if (i != 0 && i!= zigzagCount){
            curr = curr + (halfAmp*v * sign) + (s*u);
        }
        zigzag.lineTo (curr);
    }
    zigzag.lineTo(p1);

    // Zigzag line
    g.strokePath (zigzag, juce::PathStrokeType (thickness));
    g.drawArrow(juce::Line(p0 + d/2 -v*50.0f, p0 + d/2 -v*zigzagAmplitude), thickness, 10.0f, 10.0f);
    g.drawLine(juce::Line(p0 + d/2 -v*50.0f, p2), thickness);

    // Labels
    const float labelOff = 40.0f;
    const juce::Point<float> m = (p0 + p1) * 0.5f;
    const juce::Point<float> l = m + labelOff * v;
    drawLabel(g, l);

}
