#include "schematic/SchematicElement.h"

const juce::String& SchematicElement::getName() const noexcept       { return name; }
bool                SchematicElement::isHighlighted() const noexcept  { return highlighted; }
void SchematicElement::setHighlighted (bool should) noexcept { highlighted = should; }

bool SchematicElement::hitTest (juce::Point<float> point) const
{
    for (const auto& terminal : terminals)
        if (terminal.getDistanceFrom (point) < 12.0f)
            return true;
    if (cachedBounds.expanded (6.0f).contains (point))
        return true;
    return false;
}

const std::vector<Terminal>& SchematicElement::getTerminals() const noexcept
{
    return terminals;
}

void SchematicElement::drawLabel(juce::Graphics& g, Terminal center, juce::String value) const{
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
void PotElement::controlCallback(float value, SchematicPanelListener* listener)
{
    if (value <=0.1f) value = 0.1f;
    if (value >=99.9f) value = 99.9f;

    listener->setCircuitControl(getControlIndex(), value);

    controlValue = value;
    return;
}


void PotElement::draw (juce::Graphics& g) const
{
    float thickness = isHighlighted() ? STROKE_HIGHLIGHT : STROKE_NORMAL;
    g.setColour (isHighlighted() ? SCHEMATIC_HIGHLIGHT : SCHEMATIC_NORMAL);

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
    const juce::Point<float> ab = b-a;

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
    float ratio = (100.0f-controlValue)/100.0f ;
    g.strokePath (zigzag, juce::PathStrokeType (thickness));
    g.drawArrow(juce::Line(p0 + d*0.5f -v*zigzagLength, a + ab*ratio -v*zigzagAmplitude*1.3f), thickness, 10.0f, 10.0f);
    g.drawLine(juce::Line(p0 + d*0.5f -v*zigzagLength, p2), thickness);

    // Labels
    const float labelOff = 40.0f;
    const juce::Point<float> m = (p0 + p1) * 0.5f;
    const juce::Point<float> l = m + labelOff * v;
    drawLabel(g, l, label);

}



float PotElement::labelToValue (const juce::String s)
{
    auto str = s.trim().toLowerCase();
    if (str.isEmpty()) return getValue(); // fallback to original

    float multiplier = 1.0;

    // handle suffixes
    if (str.endsWith ("k"))
    {
        multiplier = 1e3;
        str = str.dropLastCharacters (1);
    }
    else if (str.endsWith ("m") ){
        multiplier = 1e6;
        str = str.dropLastCharacters (1);
    }
    else if (str.endsWith ("meg") ){
        multiplier = 1e6;
        str = str.dropLastCharacters (3);
    }
    else if (str.endsWith ("r"))
    {
        multiplier = 1.0;
        str = str.dropLastCharacters (1);
    }

    // parse numeric part
    float value = str.getFloatValue();

    if (value == 0.0)
        return getValue();

    return (float) (value * multiplier);
}
juce::String PotElement::valueToLabel (float v)
{
    if (v >= 1e6) return juce::String (v / 1e6, 0) + "M";
    if (v >= 1e3) return juce::String (v / 1e3, 0) + "k";
    if (v >= 1) return juce::String (v, 0) + "R";
    if (v < 1 ) return juce::String (v * 1e3, 2) + "m";
    return juce::String (v);
}