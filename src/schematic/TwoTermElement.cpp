#include "schematic/TwoTermElement.h"


void TwoTermElement::draw (juce::Graphics& g) const
{
    // Labels
    drawLabel(g, labelCenter, label);

}
void TwoTermElement::updateSignalPaths () {
    if (getNumMonitors()> 0){
        signalPaths[0].updateSignalPath(
            getSmoothedValue(0, MONITOR_PORT_I) * INTENSITY_SCALING,
            getSmoothedValue(0, MONITOR_PORT_V), 
            getRMSValue(0, MONITOR_PORT_I)*getRMSValue(0, MONITOR_PORT_V) *POWER_SCALING
        );
    }
};

void TwoTermElement::addPointToTerminal(Terminal t, const int termIndex, const bool direction) {
    SchematicElement::addPointToTerminal(t, termIndex, termIndex==0);
    if (signalPaths.size()>0){
        auto& p = signalPaths.front().getSignalPaths().front();
        p.path = path;
        p.rebuildCache(); 
    }
}


juce::AttributedString TwoTermElement::getInspectContent () 
{
    juce::AttributedString textContent;
    auto font = juce::Font (juce::FontOptions(FONT_SUB1));
    if (getNumMonitors() >0){
        float v = getSmoothedValue(0, MONITOR_PORT_V);
        float c = getSmoothedValue(0, MONITOR_PORT_I);
        float vac = getRMSValue(0, MONITOR_PORT_V);

        textContent.append ("Voltage : \n\t ", font, getColourNormal());
        textContent.append (formatVDCAC(v,vac), font, getColourElectrical());
        textContent.append ("\nCurrent : \n\t ", font, getColourNormal());
        textContent.append (formatCurrent(c), font, getColourHotRed());
        textContent.append ("\nPower : \n\t ", font, getColourNormal());
        textContent.append (formatPower(v * c), font, getColourAmber());
    
    }
    return textContent;
}


juce::String TwoTermElement::getInspectValue () 
{
    return label;
}


void ResistorElement::createSignalPaths () 
{
    signalPaths[0].addPath(path);
}

float ResistorElement::labelToValue (const juce::String s) const
{
    auto str = s.trim().toLowerCase();
    if (str.isEmpty()) return getValue(); // fallback to original

    float multiplier = 1.0;

    // handle suffixes
    if (str.endsWith ("kΩ") || str.endsWith ("k"))
    {
        multiplier = 1e3;
    }
    else if (str.endsWith ("mΩ") || str.endsWith ("meg")|| str.endsWith ("m")){
        multiplier = 1e6;
    }
    else if (str.endsWith ("r") || str.endsWith ("Ω"))
    {
        multiplier = 1.0;
    }
    
    // parse numeric part
    float value = str.getFloatValue();

    if (value == 0.0)
        return getValue();

    return (float) (value * multiplier);
}
juce::String ResistorElement::valueToLabel (float v) const
{
    if (v >= 1e6) return juce::String (v / 1e6, 0) + "MΩ";
    if (v >= 1e3) return juce::String (v / 1e3, 0) + "kΩ";
    if (v >= 1) return juce::String (v, 0) + "Ω";
    if (v < 1 ) return juce::String (v * 1e3, 2) + "mΩ";
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



float CapacitorElement:: labelToValue (const juce::String s) const
{
    auto str = s.trim().toLowerCase();

    if (str.isEmpty())
        return getValue(); // fallback

    double multiplier = 1.0;

    // suffix handling
    if (str.endsWith ("pf") || str.endsWith ("p") )
    {
        multiplier = 1e-12;
    }
    else if (str.endsWith ("n")  || str.endsWith ("nf"))
    {
        multiplier = 1e-9;
    }
    else if (str.endsWith ("u") || str.endsWith ("µ")  || str.endsWith ("uf") || str.endsWith ("µf"))
    {
        multiplier = 1e-6;
    }
    else if (str.endsWith ("m")|| str.endsWith ("mf"))
    {
        multiplier = 1e-3;
    }

    auto numeric = str.getFloatValue();
    if (numeric == 0.0) return getValue(); // fallback to previous valid value

    return (float) (numeric * multiplier);
}

juce::String CapacitorElement::valueToLabel (float v) const
{
    if (v >= 1e-3) return juce::String (v * 1e3, 0) + "mF";   // mF
    if (v >= 1e-6) return juce::String (v * 1e6, 0) + "µF";   // µF
    if (v >= 1e-9) return juce::String (v * 1e9, 0) + "nF";   // nF
    if (v >= 1e-12) return juce::String (v * 1e12, 0) + "pF";  // pF

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
    posPath.startNewSubPath (p0);
    posPath.lineTo   (a);
    posPlate.startNewSubPath (a - CAPACITOR_PLATE_WIDTH * 0.5f * v);
    posPlate.lineTo   (a + CAPACITOR_PLATE_WIDTH * 0.5f * v);
    negPlate.startNewSubPath   (b + CAPACITOR_PLATE_WIDTH * 0.5f * v);
    negPlate.lineTo (b - CAPACITOR_PLATE_WIDTH * 0.5f * v);
    negPath.startNewSubPath(b);
    negPath.lineTo(p1);
    path.addPath(posPath);
    path.addPath(negPath);
    path.addPath(posPlate);
    path.addPath(negPlate);

    // Labels
    const float labelOff = -42.0f;
    const juce::Point<float> m = (p0 + p1) * 0.5f;
    labelCenter = m + labelOff * v;
}

void CapacitorElement::createSignalPaths () 
{
    signalPaths[0].addPath(posPath, 0.0f, 0.0f);
    signalPaths[0].addPath(negPath, 1.0f, 1.0f);
    signalPaths[0].addPath(posPlate, 0.0f, 0.0f);
    signalPaths[0].addPath(negPlate, 1.0f, 1.0f);
}



void CapacitorElement::addPointToTerminal(Terminal t, const int termIndex, const bool ) {

    if (termIndex> 1) return;

    auto& tt = terminals[termIndex];
    juce::Path newPath;
    
    if (termIndex == 0){
        juce::Path tmp = posPath;
        newPath.startNewSubPath(t);
        newPath.lineTo(tt);
        posPath.clear();
        posPath.addPath(newPath);
        posPath.addPath(tmp);

    }else{
        newPath.startNewSubPath(tt);
        newPath.lineTo(t);
        negPath.addPath(newPath);
    }
    tt = t;
    path.addPath(newPath);

    

    if (signalPaths.size()>0){
        auto& p = signalPaths.front().getSignalPaths()[termIndex];
        p.path = termIndex == 0 ? posPath : negPath;
        p.rebuildCache(); 
    }
}

void ReverbTankElement::createSignalPaths () 
{
    signalPaths[0].addPath(springPath);
}


void ReverbTankElement::prepareToDraw (){

    const float REVERB_SPRING_LENGTH = 157.0f;
    const float REVERB_SPRING_WIDTH = 30.0f;
    const float REVERB_SPRING_HEIGHT = 50.0f;


    const auto& p0 = terminals[0];
    const auto& p1 = terminals[1];

    const juce::Point<float> d = p1-p0;
    const float length = p1.getDistanceFrom(p0);
    if (length < REVERB_SPRING_LENGTH) return;

    const juce::Point<float> u = d/length;
    const juce::Point<float> v {- u.getY(), u.getX()};
    
    const juce::Point<float> a = p0 + d*(length-REVERB_SPRING_LENGTH)/(2*length);
    const juce::Point<float> b = p1 - d*(length-REVERB_SPRING_LENGTH)/(2*length);

    cachedBounds = juce::Rectangle<float> (p0, p1);
    cachedBounds.expand(REVERB_SPRING_HEIGHT, REVERB_SPRING_HEIGHT);
    path.addRoundedRectangle(a.x, a.y- REVERB_SPRING_HEIGHT*0.5f, REVERB_SPRING_LENGTH, REVERB_SPRING_HEIGHT, 4.0f);



    int nloops = (int) REVERB_SPRING_LENGTH/(std::sqrt(2)*REVERB_SPRING_WIDTH/2);
    float height = REVERB_SPRING_HEIGHT*0.5f;
    float width = REVERB_SPRING_WIDTH;

    springPath.startNewSubPath (p0);
    springPath.lineTo (a);

    for (int i =0; i<nloops; i++){
        auto initAngle = -juce::MathConstants<float>::pi*3/4;
        auto endAngle = juce::MathConstants<float>::pi*3/4;
        if (i==0) initAngle = -juce::MathConstants<float>::pi/2;
        if (i==nloops-1) endAngle = juce::MathConstants<float>::pi/2;

        springPath.addArc (a.x + (std::sqrt(2)*width/2)*(i)  , a.y - height*.5f, width, height,
                initAngle,    
                endAngle,    
                true);                 
    }
    springPath.lineTo(p1);

    path.addPath(springPath);

    const float labelOff = -40.0f;
    const juce::Point<float> m = (p0 + p1) * 0.5f;
    labelCenter = m + labelOff * v;

}


void ReverbTankElement::draw (juce::Graphics& g) const
{
    // Labels
    drawLabel(g, labelCenter, "");

}

void ReverbTankElement::updateSignalPaths () {
    if (getNumMonitors()> 0){
        signalPaths[0].updateSignalPath(
            getSmoothedValue(0, MONITOR_PORT_I) * INTENSITY_SCALING,
            getSmoothedValue(0, MONITOR_PORT_V),
            getRMSValue(0, MONITOR_PORT_I)*getRMSValue(0, MONITOR_PORT_V) *POWER_SCALING
        );
    }
};


float GainElement::labelToValue (const juce::String s) const {
    auto str = s.trim().toLowerCase();
    if (str.isEmpty()) return getValue();

    float value_db = str.getFloatValue();
    float value_gain = std::pow (10.0f, value_db / 20.0f);

    return value_gain;

}


juce::String GainElement::valueToLabel (float v) const
{
    return juce::String (20.0f * std::log10 (v), 1) + " dB";
}



void GainElement::draw (juce::Graphics& g) const
{
    const auto v= value * controlValue /100.0f;
    drawLabel(g, labelCenter, valueToLabel(v));

}
void GainElement::prepareToDraw ()
{
    const auto& p0 = terminals[0];
    const auto& p1 = terminals[1];

    const juce::Point<float> d = p1-p0;
    const float length = p1.getDistanceFrom(p0);
    if (length < SCHEMATIC_GAIN_SIZE) return;

    const juce::Point<float> u = d/length;
    const juce::Point<float> v {- u.getY(), u.getX()};

    const juce::Point<float> a = p0 + d*(length-SCHEMATIC_GAIN_SIZE)/(2*length);
    const juce::Point<float> b = p1 - d*(length-SCHEMATIC_GAIN_SIZE)/(2*length);

    // Build cached bounds
    cachedBounds = juce::Rectangle<float> (p0, p1);
    cachedBounds.expand(1.0f + std::abs(v.x*SCHEMATIC_GAIN_SIZE/2.0f), 1.0f + std::abs(v.y*SCHEMATIC_GAIN_SIZE/2.0f));

    // Draw two parallel plates
    leftPath.startNewSubPath (p0);
    leftPath.lineTo   (a);
    leftPath.lineTo (a - SCHEMATIC_GAIN_SIZE/2.0f * v);
    leftPath.lineTo   (b);
    rightPath.startNewSubPath   (a);
    rightPath.lineTo (a +SCHEMATIC_GAIN_SIZE/2.0f * v);
    rightPath.lineTo(b);
    rightPath.lineTo(p1);
    path.addPath(leftPath);
    path.addPath(rightPath);

    // Labels
    const float labelOff = -42.0f;
    const juce::Point<float> m = (p0 + p1) * 0.5f;
    labelCenter = m + labelOff * v;
}


void GainElement::controlCallback(float value, SchematicPanelListener* listener)
{
    if (value <=0.1f) value = 0.1f;
    if (value >=99.9f) value = 99.9f;
    listener->setCircuitControl(getControlIndex(), value);
    controlValue = value;
    return;
}


void GainElement::updateSignalPaths () {
    signalPaths[0].updateSignalPath(0.0F,0.0F,0.0F);
};

void GainElement::createSignalPaths () 
{
    signalPaths[0].addPath(leftPath);
    signalPaths[0].addPath(rightPath);
}

juce::AttributedString DiodeElement::getInspectContent () 
{
    juce::AttributedString textContent;
    auto font = juce::Font (juce::FontOptions(FONT_SUB1));
    if (getNumMonitors() >0){
        float v = getSmoothedValue(0, MONITOR_PORT_V);
        float c = getSmoothedValue(0, MONITOR_PORT_I);
        float vac = getRMSValue(0, MONITOR_PORT_V);

        textContent.append ("Voltage : \n\t ", font, getColourNormal());
        textContent.append (formatVDCAC(v,vac), font, getColourElectrical());
        textContent.append ("\nCurrent : \n\t ", font, getColourNormal());
        textContent.append (formatCurrent(c), font, getColourHotRed());
        textContent.append ("\nPower : \n\t ", font, getColourNormal());
        textContent.append (formatPower(v * c), font, getColourAmber());
    
    }
    return textContent;
}

juce::String DiodeElement::getInspectValue () 
{
    return getChoiceLabel();
}


void DiodeElement::draw (juce::Graphics& g) const
{
    // Labels
    drawLabel(g, labelCenter, getChoiceLabel());

}
void DiodeElement::prepareToDraw ()
{
    const auto& p0 = terminals[0];
    const auto& p1 = terminals[1];

    const juce::Point<float> d = p1-p0;
    const float length = p1.getDistanceFrom(p0);
    if (length < SCHEMATIC_DIODE_SIZE*3) return;

    const juce::Point<float> u = d/length;
    const juce::Point<float> v {- u.getY(), u.getX()};

    const juce::Point<float> a = p0 + d*(length-SCHEMATIC_DIODE_SIZE)/(2*length);
    const juce::Point<float> b = p1 - d*(length-SCHEMATIC_DIODE_SIZE)/(2*length);

    // Build cached bounds
    cachedBounds = juce::Rectangle<float> (p0, p1);
    cachedBounds.expand(1.0f + std::abs(v.x*SCHEMATIC_DIODE_SIZE/2.0f), 1.0f + std::abs(v.y*SCHEMATIC_DIODE_SIZE/2.0f));

    leftPath.startNewSubPath (p0);
    leftPath.lineTo   (a);
    leftPath.lineTo (a - SCHEMATIC_DIODE_SIZE/2.0f * v);
    leftPath.lineTo   (b);
    rightPath.startNewSubPath   (a);
    rightPath.lineTo (a +SCHEMATIC_DIODE_SIZE/2.0f * v);
    rightPath.lineTo(b);
    rightPath.lineTo(p1);
    barPath.startNewSubPath(b +SCHEMATIC_DIODE_SIZE/2.0f * v);
    barPath.lineTo(b -SCHEMATIC_DIODE_SIZE/2.0f * v);

    path.addPath(leftPath);
    path.addPath(rightPath);
    path.addPath(barPath);

    // Labels
    const float labelOff = -42.0f;
    const juce::Point<float> m = (p0 + p1) * 0.5f;
    labelCenter = m + labelOff * v;
}

void DiodeElement::updateSignalPaths () {
    if (getNumMonitors()> 0){
        signalPaths[0].updateSignalPath(
            getSmoothedValue(0, MONITOR_PORT_I) * INTENSITY_SCALING,
            getSmoothedValue(0, MONITOR_PORT_V), 
            getRMSValue(0, MONITOR_PORT_I)*getRMSValue(0, MONITOR_PORT_V) *POWER_SCALING
        );
    }
};

void DiodeElement::createSignalPaths () 
{
    signalPaths[0].addPath(leftPath);
    signalPaths[0].addPath(rightPath);
}
