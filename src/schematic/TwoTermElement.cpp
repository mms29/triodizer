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

void ResistorElement::prepareToDraw (){
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


    cachedBounds = juce::Rectangle<float> (p0, p1);
    cachedBounds.expand(1.0f + std::abs(v.x*halfAmp), 1.0f + std::abs(v.y*halfAmp));

    path.startNewSubPath (p0);

    path.lineTo(a);
    juce::Point<float>  curr = a;
    for (int i = 0; i <= zigzagCount; ++i)
    {
        int sign = std::pow(-1, i);
        curr = curr + (halfAmp * v * sign) + (s * u);
        if (i != 0 && i!= zigzagCount){
            curr = curr + (halfAmp*v * sign) + (s*u);
        }
        path.lineTo (curr);
    }
    path.lineTo(p1);

    const float labelOff = -40.0f;
    const juce::Point<float> m = (p0 + p1) * 0.5f;
    labelCenter = m + labelOff * v;

}


void ResistorElement::draw (juce::Graphics& g) const
{
    // Zigzag line
    float t=0.0f;
    if (getNumMonitors()> 0)
        t = getRMSValue(0) * POWER_SCALING; 
    drawGlowPath(g, path, t,COLOR_NORMAL,COLOR_AMBER, isHighlighted());

    if (isSignalPath()){
        auto& cachedPath = signalPaths.back();
        drawSignalPath(g, cachedPath, t, getClock());
    }
    // Labels
    drawLabel(g, labelCenter, label);

}

void ResistorElement::createSignalPath (const int signalPathMode) 
{
    setSignalPath(true);
    CachedPath cachedPath;
    cachedPath.path = path;
    cachedPath.rebuildCache();
    signalPaths.push_back (cachedPath);
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
    if (v > 1e-3) return juce::String (v * 1e3, 0) + "m";   // mF
    if (v > 1e-6) return juce::String (v * 1e6, 0) + "u";   // µF
    if (v > 1e-9) return juce::String (v * 1e9, 0) + "n";   // nF
    if (v > 1e-12) return juce::String (v * 1e12, 0) + "p";  // pF

    return juce::String (v);
}


void CapacitorElement::prepareToDraw ()
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
    path.startNewSubPath (p0);
    path.lineTo   (a);
    path.startNewSubPath (a - plateWidth * 0.5f * v);
    path.lineTo   (a + plateWidth * 0.5f * v);
    path.startNewSubPath (b - plateWidth * 0.5f * v);
    path.lineTo   (b + plateWidth * 0.5f * v);
    path.startNewSubPath (p1);
    path.lineTo   (b);

    // Labels
    const float labelOff = -42.0f;
    const juce::Point<float> m = (p0 + p1) * 0.5f;
    labelCenter = m + labelOff * v;
}

void CapacitorElement::createSignalPath (const int signalPathMode) 
{
    setSignalPath(true);

    const auto& p0 = terminals[0];
    const auto& p1 = terminals[1];
    juce::Path sigpath;
    sigpath.startNewSubPath (p0);
    sigpath.lineTo   (p1);
    CachedPath cachedPath;
    cachedPath.path = sigpath;
    cachedPath.rebuildCache();
    signalPaths.push_back (cachedPath);
}
void CapacitorElement::draw (juce::Graphics& g) const
{
    float t=0.0f;
    if (getNumMonitors()> 0)
        t = getRMSValue(0) * POWER_SCALING; 
    drawGlowPath(g, path, t, COLOR_NORMAL,COLOR_AMBER, isHighlighted());

 
    if (isSignalPath()){
        auto& cachedPath = signalPaths.back();
        drawSignalPath(g, cachedPath, t, getClock());
    }

    drawLabel(g, labelCenter, label);
}



void TransformerElement::draw (juce::Graphics& g) const
{
    g.setColour (isHighlighted() ? COLOR_HIGHLIGHT : COLOR_NORMAL);
    float thickness = isHighlighted() ? STROKE_HIGHLIGHT : STROKE_NORMAL;

    const auto& p0 = terminals[0];
    const auto& p1 = terminals[1];
    const auto& p2 = terminals[2];
    const auto& p3 = terminals[3];

    const juce::Point<float> d1 = p1-p0;
    const float length1 = p1.getDistanceFrom(p0);
    const juce::Point<float> d2 = p1-p0;
    const float length2 = p3.getDistanceFrom(p2);
    if (length1 < coilLength) return;
    if (length2 < coilLength) return;

    const juce::Point<float> u = d1/length1;
    const juce::Point<float> v {- u.getY(), u.getX()};

    const juce::Point<float> a = p0 + d1*(length1-coilLength)/(2*length1);
    const juce::Point<float> b = p1 - d1*(length1-coilLength)/(2*length1);
    const juce::Point<float> c = p2 + d2*(length2-coilLength)/(2*length2);
    const juce::Point<float> d = p3 - d2*(length2-coilLength)/(2*length2);

    // Build cached bounds
    cachedBounds = juce::Rectangle<float> (p0, p3);
    // cachedBounds.expand(1.0f + std::abs(v.x*plateWidth/2.0f), 1.0f + std::abs(v.y*plateWidth/2.0f));
    juce::Path primary, secondary;

    juce::Rectangle<float> bounds (50.0f, 50.0f, 200.0f, 200.0f);

    primary.startNewSubPath(p0);
    primary.lineTo(a);
    primary.startNewSubPath(p2);
    primary.lineTo(c);

    int ncoil = (int) coilLength/coilWidth;
    for (int i =0; i<ncoil; i++){

        primary.addArc (a.x - coilWidth*0.5F, a.y - coilWidth*(i+1), coilWidth, coilWidth,
                juce::MathConstants<float>::pi,          // start angle
                0.0f,                                    // end angle
                true);                                   // connect to centre (false for arc only)
        
        secondary.addArc (c.x - coilWidth*0.5F, c.y - coilWidth*(i+1), coilWidth, coilWidth,
                -juce::MathConstants<float>::pi,                                   // start angle
                0.0f,         // end angle
                true);                                   // connect to centre (false for arc only)
        
    }
    primary.lineTo(p1);
    secondary.lineTo(p3);
    g.strokePath (primary,  juce::PathStrokeType (thickness));
    g.strokePath (secondary,  juce::PathStrokeType (thickness));

    float coilGap = p3.x - p1.x;

    g.drawLine(juce::Line(a+v*coilGap*0.4f, b+v*coilGap*0.4f), thickness);
    g.drawLine(juce::Line(a+v*coilGap*0.6f, b+v*coilGap*0.6f), thickness);

    // Labels
    const float labelOff = -42.0f;
    const juce::Point<float> m = (p0 + p1) * 0.5f;
    const juce::Point<float> l = m + labelOff * v;
    drawLabel(g, l, label);

}