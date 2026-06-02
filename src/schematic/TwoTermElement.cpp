#include "schematic/TwoTermElement.h"

float ResistorElement::labelToValue (const juce::String s)
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
juce::String ResistorElement::valueToLabel (float v)
{
    if (v >= 1e6) return juce::String (v / 1e6, 0) + "M";
    if (v >= 1e3) return juce::String (v / 1e3, 0) + "k";
    if (v >= 1) return juce::String (v, 0) + "R";
    if (v < 1 ) return juce::String (v * 1e3, 2) + "m";
    return juce::String (v);
}

void ResistorElement::draw (juce::Graphics& g) const
{
    const auto& p0 = terminals[0];
    const auto& p1 = terminals[1];

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
    g.setColour (isHighlighted() ? SCHEMATIC_HIGHLIGHT : SCHEMATIC_NORMAL);
    float thickness = isHighlighted() ? STROKE_HIGHLIGHT : STROKE_NORMAL;
    g.strokePath (zigzag, juce::PathStrokeType (thickness));

    // Labels
    const float labelOff = -40.0f;
    const juce::Point<float> m = (p0 + p1) * 0.5f;
    const juce::Point<float> l = m + labelOff * v;
    drawLabel(g, l, label);

}

float CapacitorElement:: labelToValue (const juce::String s)
{
    auto str = s.trim().toLowerCase();

    if (str.isEmpty())
        return getValue(); // fallback

    double multiplier = 1.0;
    if (str.endsWith ("f")) str = str.dropLastCharacters (1);

    // suffix handling
    if (str.endsWith ("p"))
    {
        multiplier = 1e-12;
        str = str.dropLastCharacters (1);
    }
    else if (str.endsWith ("n"))
    {
        multiplier = 1e-9;
        str = str.dropLastCharacters (1);
    }
    else if (str.endsWith ("u") || str.endsWith ("µ"))
    {
        multiplier = 1e-6;
        str = str.dropLastCharacters (1);
    }
    else if (str.endsWith ("m"))
    {
        multiplier = 1e-3;
        str = str.dropLastCharacters (1);
    }
    else if (str.endsWith ("f"))
    {
        multiplier = 1.0;
        str = str.dropLastCharacters (1);
    }

    auto numeric = str.getFloatValue();
    if (numeric == 0.0) return getValue(); // fallback to previous valid value

    return (float) (numeric * multiplier);
}

juce::String CapacitorElement::valueToLabel (float v)
{
    if (v >= 1e-3) return juce::String (v * 1e3, 0) + "m";   // mF
    if (v >= 1e-6) return juce::String (v * 1e6, 0) + "u";   // µF
    if (v >= 1e-9) return juce::String (v * 1e9, 0) + "n";   // nF
    if (v >= 1e-12) return juce::String (v * 1e12, 0) + "p";  // pF

    return juce::String (v);
}

void CapacitorElement::draw (juce::Graphics& g) const
{
    const auto& p0 = terminals[0];
    const auto& p1 = terminals[1];

    const juce::Point<float> d = p1-p0;
    const float length = p1.getDistanceFrom(p0);
    if (length < plateGap) return;

    const juce::Point<float> u = d/length;
    const juce::Point<float> v {- u.getY(), u.getX()};

    const juce::Point<float> a = p0 + d*(length-plateGap)/(2*length);
    const juce::Point<float> b = p1 - d*(length-plateGap)/(2*length);

    // Build cached bounds
    cachedBounds = juce::Rectangle<float> (p0, p1);
    cachedBounds.expand(1.0f + std::abs(v.x*plateWidth/2.0f), 1.0f + std::abs(v.y*plateWidth/2.0f));

    // Draw two parallel plates
    juce::Path leftPlate, rightPlate, positivePath, negativePath;
    leftPlate.startNewSubPath (a - plateWidth * 0.5f * v);
    leftPlate.lineTo   (a + plateWidth * 0.5f * v);
    rightPlate.startNewSubPath (b - plateWidth * 0.5f * v);
    rightPlate.lineTo   (b + plateWidth * 0.5f * v);
    positivePath.startNewSubPath (p0);
    positivePath.lineTo   (a);
    negativePath.startNewSubPath (p1);
    negativePath.lineTo   (b);

    g.setColour (isHighlighted() ? SCHEMATIC_HIGHLIGHT : SCHEMATIC_NORMAL);
    float thickness = isHighlighted() ? STROKE_HIGHLIGHT : STROKE_NORMAL;
    g.strokePath (leftPlate,  juce::PathStrokeType (thickness));
    g.strokePath (rightPlate, juce::PathStrokeType (thickness));
    g.strokePath (positivePath,  juce::PathStrokeType (thickness));
    g.strokePath (negativePath, juce::PathStrokeType (thickness));

    // Labels
    const float labelOff = -42.0f;
    const juce::Point<float> m = (p0 + p1) * 0.5f;
    const juce::Point<float> l = m + labelOff * v;
    drawLabel(g, l, label);

}