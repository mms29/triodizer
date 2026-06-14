#include "schematic/MultiTermElement.h"



void PotElement::controlCallback(float value, SchematicPanelListener* listener)
{
    if (value <=0.1f) value = 0.1f;
    if (value >=99.9f) value = 99.9f;

    listener->setCircuitControl(getControlIndex(), value);

    controlValue = value;
    return;
}

void PotElement::prepareToDraw ()
{
    const auto& p0 = terminals[0];
    const auto& p1 = terminals[1];
    const auto& p2 = terminals[2];

    const juce::Point<float> d = p1-p0;
    const float length = p1.getDistanceFrom(p0);
    if (length < RESISTOR_ZIGZAG_LENGTH) return;

    const juce::Point<float> u = d/length;
    const juce::Point<float> v {- u.getY(), u.getX()};

    const float halfAmp = RESISTOR_ZIGZAG_AMPLITUDE;
    const float s = RESISTOR_ZIGZAG_LENGTH/(RESISTOR_ZIGZAG_COUNT*2);

    const juce::Point<float> a = p0 + d*(length-RESISTOR_ZIGZAG_LENGTH)/(2*length);
    const juce::Point<float> b = p1 - d*(length-RESISTOR_ZIGZAG_LENGTH)/(2*length);
    const juce::Point<float> ab = b-a;


    cachedBounds = juce::Rectangle<float> (p0, p1);
    cachedBounds.expand(1.0f + std::abs(v.x*halfAmp), 1.0f + std::abs(v.y*halfAmp));

    zigzag.startNewSubPath (p0);
    zigzag.lineTo(a);
    juce::Point<float>  curr = a;
    for (int i = 0; i <= RESISTOR_ZIGZAG_COUNT; ++i)
    {
        int sign = std::pow(-1, i);
        curr = curr + (halfAmp * v * sign) + (s * u);
        if (i != 0 && i!= RESISTOR_ZIGZAG_COUNT){
            curr = curr + (halfAmp*v * sign) + (s*u);
        }
        zigzag.lineTo (curr);
    }
    zigzag.lineTo(p1);
    const float labelOff = 40.0f;
    const juce::Point<float> m = (p0 + p1) * 0.5f;
    labelCenter = m + labelOff * v;

    pp0=a;
    pp1=b;
    pp2= p0 + d*0.5f -v*RESISTOR_ZIGZAG_LENGTH;
    arrowDir = v;
}
void PotElement::draw (juce::Graphics& g) const
{
    const auto& p2 = terminals[2];
    // Arrow
    float ratio = (100.0f-controlValue)/100.0f ;
    juce::Path arrow;
    arrow.startNewSubPath(p2);
    arrow.lineTo(pp2);
    arrow.addArrow(juce::Line(pp2, pp0 + (pp1-pp0)*ratio -arrowDir*RESISTOR_ZIGZAG_AMPLITUDE*1.3f), 1.0f, 10.0f, 10.0f);

    float t=0.0f;
    if (getNumMonitors()> 0)
        t = getRMSValue(0) * POWER_SCALING; 
    drawGlowPath(g, zigzag, t, getColourNormal(), getColourAmber(), isHighlighted());
    drawGlowPath(g, arrow, 0.0f, getColourNormal(), getColourAmber(), isHighlighted());

    for (auto& p : signalPaths)
        drawSignalPath(g, p, t, getClock());
    // Labels
    drawLabel(g, labelCenter, label);

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

void PotElement::createSignalPath (const int signalPathMode) 
{
    const auto& p0 = terminals[0];
    const auto& p1 = terminals[1];
    const auto& p2 = terminals[2];
    
    setSignalPath(true);

    juce::Path sigpath;
    if (signalPathMode == 0){
        sigpath.startNewSubPath(p0);
        sigpath.lineTo(pp0);
        sigpath.lineTo(pp2);
        sigpath.lineTo(p2);
    }
    CachedPath cachedPath;
    cachedPath.path = sigpath;
    cachedPath.rebuildCache();
    signalPaths.push_back (cachedPath);
}



void TransformerElement::prepareToDraw () 
{
}
void TransformerElement::draw (juce::Graphics& g) const
{
    g.setColour (isHighlighted() ? getColourHighlight() : getColourNormal());
    float thickness = isHighlighted() ? STROKE_HIGHLIGHT : STROKE_NORMAL;

    const auto& p0 = terminals[0];
    const auto& p1 = terminals[1];
    const auto& p2 = terminals[2];
    const auto& p3 = terminals[3];

    const juce::Point<float> d1 = p1-p0;
    const float length1 = p1.getDistanceFrom(p0);
    const juce::Point<float> d2 = p1-p0;
    const float length2 = p3.getDistanceFrom(p2);
    if (length1 < TRANSFORMER_COIL_LENGTH) return;
    if (length2 < TRANSFORMER_COIL_LENGTH) return;

    const juce::Point<float> u = d1/length1;
    const juce::Point<float> v {- u.getY(), u.getX()};

    const juce::Point<float> a = p0 + d1*(length1-TRANSFORMER_COIL_LENGTH)/(2*length1);
    const juce::Point<float> b = p1 - d1*(length1-TRANSFORMER_COIL_LENGTH)/(2*length1);
    const juce::Point<float> c = p2 + d2*(length2-TRANSFORMER_COIL_LENGTH)/(2*length2);
    const juce::Point<float> d = p3 - d2*(length2-TRANSFORMER_COIL_LENGTH)/(2*length2);

    // Build cached bounds
    cachedBounds = juce::Rectangle<float> (p0, p3);
    // cachedBounds.expand(1.0f + std::abs(v.x*plateWidth/2.0f), 1.0f + std::abs(v.y*plateWidth/2.0f));
    juce::Path primary, secondary;

    juce::Rectangle<float> bounds (50.0f, 50.0f, 200.0f, 200.0f);

    primary.startNewSubPath(p0);
    primary.lineTo(a);
    primary.startNewSubPath(p2);
    primary.lineTo(c);

    int ncoil = (int) TRANSFORMER_COIL_LENGTH/TRANSFORMER_COIL_WIDTH;
    for (int i =0; i<ncoil; i++){

        primary.addArc (a.x - TRANSFORMER_COIL_WIDTH*0.5F, a.y - TRANSFORMER_COIL_WIDTH*(i+1), TRANSFORMER_COIL_WIDTH, TRANSFORMER_COIL_WIDTH,
                juce::MathConstants<float>::pi,          // start angle
                0.0f,                                    // end angle
                true);                                   // connect to centre (false for arc only)
        
        secondary.addArc (c.x - TRANSFORMER_COIL_WIDTH*0.5F, c.y - TRANSFORMER_COIL_WIDTH*(i+1), TRANSFORMER_COIL_WIDTH, TRANSFORMER_COIL_WIDTH,
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