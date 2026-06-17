#include "schematic/TwoTermElement.h"


void TwoTermElement::draw (juce::Graphics& g) const
{
    float t=0.0f;
    if (getNumMonitors()> 1)
        t = getRMSValue(1) * POWER_SCALING; 
    drawGlowPath(g, path, t,getColourNormal(),getColourAmber(), isHighlighted());

    // Labels
    drawLabel(g, labelCenter, label);

}

void TwoTermElement::updateSignalPath () {
    float t=0.0f;
    if (getNumMonitors()> 1)
        t = getRMSValue(1) * POWER_SCALING; 

    for (auto& cachedPath : signalPaths)
    {
        updateCachedPath(t, SchematicElement::getClock(), cachedPath);
    }

};

juce::AttributedString TwoTermElement::getInspectContent () 
{
    juce::AttributedString textContent;
    auto font = juce::Font (juce::FontOptions(FONT_SUB1));
    if (getNumMonitors() == 2){
        float v = getSmoothedValue(0);
        float c = getSmoothedValue(1);
        textContent.append ("Voltage : \n", font, getColourNormal());
        textContent.append ("\t"+ juce::String(v, 1)+" VDC\n", font, getColourElectrical());
        textContent.append ("\t"+ juce::String(getRMSValue(0), 1)+" VAC\n", font, getColourElectrical());
        textContent.append ("Current : \n", font, getColourNormal());
        textContent.append ("\t"+ juce::String(c*1e3f, 1)+" mA\n", font, getColourAmber());
        textContent.append ("Power : \n", font, getColourNormal());
        textContent.append ("\t"+ juce::String(v*c, 1)+" W\n", font, getColourPurple());

    }
    return textContent;
}

juce::String TwoTermElement::getInspectValue () 
{
    return label;
}


void ResistorElement::createSignalPath (const int signalPathMode) 
{
    setSignalPath(true);
    CachedPath cachedPath;
    cachedPath.path = path;
    cachedPath.rebuildCache();
    signalPaths.push_back (cachedPath);
}

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
    if (length < RESISTOR_ZIGZAG_LENGTH) return;

    const juce::Point<float> u = d/length;
    const juce::Point<float> v {- u.getY(), u.getX()};

    const float halfAmp = RESISTOR_ZIGZAG_AMPLITUDE;
    const float s = RESISTOR_ZIGZAG_LENGTH/(RESISTOR_ZIGZAG_COUNT*2);

    const juce::Point<float> a = p0 + d*(length-RESISTOR_ZIGZAG_LENGTH)/(2*length);
    const juce::Point<float> b = p1 - d*(length-RESISTOR_ZIGZAG_LENGTH)/(2*length);


    cachedBounds = juce::Rectangle<float> (p0, p1);
    cachedBounds.expand(1.0f + std::abs(v.x*halfAmp), 1.0f + std::abs(v.y*halfAmp));

    path.startNewSubPath (p0);

    path.lineTo(a);
    juce::Point<float>  curr = a;
    for (int i = 0; i <= RESISTOR_ZIGZAG_COUNT; ++i)
    {
        int sign = std::pow(-1, i);
        curr = curr + (halfAmp * v * sign) + (s * u);
        if (i != 0 && i!= RESISTOR_ZIGZAG_COUNT){
            curr = curr + (halfAmp*v * sign) + (s*u);
        }
        path.lineTo (curr);
    }
    path.lineTo(p1);

    const float labelOff = -40.0f;
    const juce::Point<float> m = (p0 + p1) * 0.5f;
    labelCenter = m + labelOff * v;

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
    if (length < CAPACITOR_PLATE_GAP) return;

    const juce::Point<float> u = d/length;
    const juce::Point<float> v {- u.getY(), u.getX()};

    const juce::Point<float> a = p0 + d*(length-CAPACITOR_PLATE_GAP)/(2*length);
    const juce::Point<float> b = p1 - d*(length-CAPACITOR_PLATE_GAP)/(2*length);

    // Build cached bounds
    cachedBounds = juce::Rectangle<float> (p0, p1);
    cachedBounds.expand(1.0f + std::abs(v.x*CAPACITOR_PLATE_WIDTH/2.0f), 1.0f + std::abs(v.y*CAPACITOR_PLATE_WIDTH/2.0f));

    // Draw two parallel plates
    path.startNewSubPath (p0);
    path.lineTo   (a);
    path.startNewSubPath (a - CAPACITOR_PLATE_WIDTH * 0.5f * v);
    path.lineTo   (a + CAPACITOR_PLATE_WIDTH * 0.5f * v);
    path.startNewSubPath (b - CAPACITOR_PLATE_WIDTH * 0.5f * v);
    path.lineTo   (b + CAPACITOR_PLATE_WIDTH * 0.5f * v);
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