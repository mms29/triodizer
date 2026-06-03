
#include "schematic/OneTermElement.h"

GroundElement::GroundElement (Terminal termPosition)
    : SchematicElement ("", std::vector<Terminal> {termPosition})
{
    jassert (terminals.size() == 1);
}


void GroundElement::draw (juce::Graphics& g) const
{
    float thickness = STROKE_NORMAL;
    g.setColour (SCHEMATIC_NORMAL);

    float groundSize = 20.0f;

    const auto& p0 = terminals[0];

    g.drawLine(juce::Line( p0.x, p0.y, p0.x, p0.y + groundSize), thickness);
    g.drawLine(juce::Line(p0.x - groundSize, p0.y + groundSize, p0.x + groundSize, p0.y + groundSize), thickness);
    g.drawLine(juce::Line(p0.x - groundSize*0.66f, p0.y + groundSize*1.25f, p0.x + groundSize*0.66f, p0.y + groundSize*1.25f), thickness);
    g.drawLine(juce::Line(p0.x - groundSize*0.33f, p0.y + groundSize*1.5f, p0.x + groundSize*0.33f, p0.y + groundSize*1.5f), thickness);

}

JunctionElement::JunctionElement (Terminal termPosition)
    : SchematicElement ("",std::vector<Terminal> {termPosition})
{
    jassert (terminals.size() == 1);
}
void JunctionElement::draw (juce::Graphics& g) const
{
    g.setColour (SCHEMATIC_NORMAL);
    const auto& p0 = terminals[0];

    float radius = 10.0f;

    g.fillEllipse(p0.x - radius*0.5f, p0.y - radius*0.5f, radius, radius);
    g.setColour (SCHEMATIC_BACKGROUND);
    g.fillEllipse(p0.x - radius*0.25f, p0.y - radius*0.25f, radius*0.5f, radius*0.5f);

}

float VoltageElement:: labelToValue (const juce::String s)
{
    auto str = s.trim().toLowerCase();

    if (str.isEmpty())
        return getValue(); // fallback

    double multiplier = 1.0;
    if (str.endsWith ("v"))
        str = str.dropLastCharacters (1);

    // suffix handling
    if (str.endsWith ("k"))
    {
        multiplier = 1e3;
        str = str.dropLastCharacters (1);
    }
    else if (str.endsWith ("m"))
    {
        multiplier = 1e-3;
        str = str.dropLastCharacters (1);
    }

    auto numeric = str.getFloatValue();
    if (numeric == 0.0) return getValue(); // fallback to previous valid value

    return (float) (numeric * multiplier);
}

juce::String VoltageElement::valueToLabel (float v)
{
    if (v >= 1e3) return juce::String (v / 1e3, 0) + "kV";
    if (v >= 1) return juce::String (v, 0) + "V";
    if (v < 1 ) return juce::String (v * 1e3, 2) + "mV";
    return juce::String (v);
}

void VoltageElement::draw (juce::Graphics& g) const
{
    float thickness = isHighlighted() ? STROKE_HIGHLIGHT : STROKE_NORMAL;
    g.setColour (isHighlighted() ? SCHEMATIC_HIGHLIGHT : SCHEMATIC_NORMAL);

    float groundSize = 20.0f;

    const auto& p0 = terminals[0];

    g.drawLine(juce::Line( p0.x, p0.y, p0.x, p0.y - groundSize), thickness);
    g.drawLine(juce::Line(p0.x - groundSize, p0.y - groundSize, p0.x + groundSize, p0.y - groundSize), thickness);
    g.drawLine(juce::Line(p0.x - groundSize, p0.y - groundSize, p0.x , p0.y - groundSize*2), thickness);
    g.drawLine(juce::Line(p0.x + groundSize, p0.y - groundSize, p0.x , p0.y - groundSize*2), thickness);

    cachedBounds = juce::Rectangle<float>(p0.x-groundSize, p0.y-groundSize*2 , groundSize*2, groundSize*2);

    drawLabel(g, p0 - Terminal {0.0f, groundSize*3.4f}, label);


}

void VoltmeterElement::draw (juce::Graphics& g) const
{
    const auto& p0 = terminals[0];


    // Circle background
    g.setColour (juce::Colours::darkgrey);
    g.fillEllipse (p0.x - METER_RADIUS * 2.0f, p0.y - METER_RADIUS,
                   METER_RADIUS * 4.0f, METER_RADIUS * 2.0f);

    // Border
    g.setColour (SCHEMATIC_NORMAL);
    g.drawEllipse (p0.x - METER_RADIUS * 2.0f, p0.y - METER_RADIUS,
                   METER_RADIUS * 4.0f, METER_RADIUS * 2.0f, 2.0f);

    // Voltage text
    juce::Font font (juce::FontOptions (16.0f));
    g.setFont (font);
    g.setColour (juce::Colours::greenyellow);

    juce::String text = juce::String (getMonitorValue(0), 1) + " V";
    g.drawText (text,
                p0.x - METER_RADIUS * 2.0f + 4,
                p0.y - METER_RADIUS + 4,
                METER_RADIUS * 4.0f - 8,
                METER_RADIUS * 2.0f - 8,
                juce::Justification::centred, true);
}

bool VoltmeterElement::hitTest (juce::Point<float> point) const
{
    return point.getDistanceFrom (terminals[0]) < METER_RADIUS;
}

