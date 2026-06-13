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
    g.setColour (COLOR_HIGHLIGHT);
    g.drawText (getName(),
                center.getX() - 40, center.getY() - 18, 80, 18,
                juce::Justification::centred, true);

    // Value
    g.setColour (COLOR_NORMAL);
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

void PotElement::prepareToDraw ()
{
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


    cachedBounds = juce::Rectangle<float> (p0, p1);
    cachedBounds.expand(1.0f + std::abs(v.x*halfAmp), 1.0f + std::abs(v.y*halfAmp));

    zigzag.startNewSubPath (p0);
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
    const float labelOff = 40.0f;
    const juce::Point<float> m = (p0 + p1) * 0.5f;
    labelCenter = m + labelOff * v;

    // Arrow
    float ratio = (100.0f-controlValue)/100.0f ;
    arrow.startNewSubPath(p2);
    arrow.lineTo(p0 + d*0.5f -v*zigzagLength);
    arrow.addArrow(juce::Line(p0 + d*0.5f -v*zigzagLength, a + ab*ratio -v*zigzagAmplitude*1.3f), 1.0f, 10.0f, 10.0f);

    pp0=a;
    pp1=b;
    pp2= p0 + d*0.5f -v*zigzagLength;
}
void PotElement::draw (juce::Graphics& g) const
{

    float t=0.0f;
    if (getNumMonitors()> 0)
        t = getRMSValue(0) * POWER_SCALING; 
    drawGlowPath(g, zigzag, t, COLOR_NORMAL, COLOR_AMBER, isHighlighted());
    drawGlowPath(g, arrow, 0.0f, COLOR_NORMAL, COLOR_AMBER, isHighlighted());

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
void drawGlowPath(juce::Graphics& g,
                  const juce::Path& path,
                  float intensity, 
                  juce::Colour coreColour, 
                  juce::Colour glowColour, 
                  bool highlight
)
{

    intensity = juce::jlimit (0.0f, 1.0f, intensity);
    float coreIntensity = 0.7f + intensity*0.3f;

    auto white = juce::Colours::white;

    juce::PathStrokeType::JointStyle joinStyle = juce::PathStrokeType::curved;
    juce::PathStrokeType::EndCapStyle endStyle = juce::PathStrokeType::square;

    // if(intensity>0.0f){}

    int init = 5-intensity*5;
    for (int i = init; i<9; i++){
        g.setColour(glowColour.withAlpha(0.006f*(i+1)*(i+1) * intensity));
        g.strokePath(path, juce::PathStrokeType(6* (10-i), joinStyle, endStyle));
    }
    // // contrast
    g.setColour(COLOR_BACKGROUND.withAlpha(0.5f * coreIntensity));
    g.strokePath(path, juce::PathStrokeType(5.5f, joinStyle, endStyle));

    // Bright plasma
    g.setColour(coreColour.withAlpha(0.6f * coreIntensity));
    g.strokePath(path, juce::PathStrokeType(4.0f, joinStyle, endStyle));
    g.setColour(coreColour.withAlpha(0.7f * coreIntensity));
    g.strokePath(path, juce::PathStrokeType(3.0f, joinStyle, endStyle));

    // White hot core
    g.setColour(white.withAlpha(coreIntensity));
    g.strokePath(path, juce::PathStrokeType(1.5f, joinStyle, endStyle));

    if (highlight){

        g.setColour(white.withAlpha(coreIntensity));
        g.strokePath(path, juce::PathStrokeType(STROKE_HIGHLIGHT, joinStyle, endStyle));
    }
}


void drawSignalPath (juce::Graphics& g,
                     const CachedPath& cachedPath,
                     float intensity,
                     int clockTick)
{
    constexpr float beadSpacingPx = 15;

    if (cachedPath.samples.size() < 2)
        return;

    float speed = juce::jmap (intensity, 0.0f, 1.0f, 0.05f, 3.0f);

    float length = cachedPath.length;
    int numBeads = juce::jmax (1, (int) (length / beadSpacingPx));
    float spacing = 1.0f / numBeads;

    for (int i = 0; i < numBeads; ++i)
    {
        float phase = i * spacing;
        float t = phase + clockTick * speed * 0.01f;
        t -= (int) t; // fast wrap [0..1]

        // use cached interpolation instead of Path queries
        juce::Point<float> p = cachedPath.getPoint (t);
        float beadSize = 2.0f ;

        g.setColour(COLOR_LASERGREEN);
        // g.drawEllipse(p.x - beadSize * 0.5f,
        //                  p.y - beadSize * 0.5f,
        //                  beadSize,
        //                  beadSize, 1.0f);

        juce::Path bead;
        bead.addEllipse (p.x - beadSize * 0.5f,
                         p.y - beadSize * 0.5f,
                         beadSize,
                         beadSize);

        // g.strokePath(bead, juce::PathStrokeType(1.0f));
        drawGlowPath (g,
                      bead,
                      0.05f,
                      COLOR_LASERGREEN,
                      COLOR_LASERGREEN,
                      false);
    }
}
// void drawSignalPath (juce::Graphics& g,
//                      juce::Point<float> A,
//                      juce::Point<float> B,
//                      float intensity,
//                      int clockTick)
// {
//     // FIXED distance between beads (tweak this)
//     constexpr float beadSpacingPx = 14.0f;

//     float speed = juce::jmap (intensity, 0.0f, 1.0f,
//                               0.2f, 3.0f);

//     juce::Point<float> dir = B - A;
//     float length = dir.getDistanceFromOrigin();

//     if (length <= 0.0001f)
//         return;

//     dir /= length;

//     // number of beads depends on distance
//     int numBeads = (int) (length / beadSpacingPx);

//     if (numBeads < 1)
//         numBeads = 1;

//     float spacing = 1.0f / numBeads;

//     for (int i = 0; i < numBeads; ++i)
//     {
//         float phase = i * spacing;

//         float t = std::fmod (phase + clockTick * speed * 0.01f, 1.0f);

//         juce::Point<float> p = A + dir * (t * length);

//         float beadSize = 2.0f;

//         juce::Path bead;
//         bead.addEllipse (p.x - beadSize * 0.5f,
//                          p.y - beadSize * 0.5f,
//                          beadSize,
//                          beadSize);

//         drawGlowPath (g,
//                       bead,
//                       0.05f,
//                       COLOR_LASERGREEN,
//                       COLOR_LASERGREEN,
//                       false);
//     }
// }
