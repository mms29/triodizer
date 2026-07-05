#include "schematic/MultiTermElement.h"


juce::String PotElement::getInspectValue () 
{
    return label;
}

juce::AttributedString PotElement::getInspectContent () 
{
    juce::AttributedString textContent;
    auto font = juce::Font (juce::FontOptions(FONT_SUB1));
    if (getNumMonitors() > 0){
        float vp = getSmoothedValue(0, MONITOR_PORT_V);
        float cp = getSmoothedValue(0, MONITOR_PORT_I);
        float vm = getSmoothedValue(1, MONITOR_PORT_V);
        float cm = getSmoothedValue(1, MONITOR_PORT_I);
        float vp_rms = getRMSValue(0, MONITOR_PORT_V);
        float cp_rms = getRMSValue(0, MONITOR_PORT_I);
        float vm_rms = getRMSValue(1, MONITOR_PORT_V);
        float cm_rms = getRMSValue(1, MONITOR_PORT_I);
        textContent.append ("Resistor A: ", font, getColourNormal());
        textContent.append ("\n\t");
        textContent.append (formatVDCAC(vp, vp_rms), font, getColourElectrical());
        textContent.append ("\n\t");
        textContent.append (formatCurrent(cp), font, getColourAmber());
        textContent.append ("\n");
        textContent.append ("Resistor B: ", font, getColourNormal());
        textContent.append ("\n\t");
        textContent.append (formatVDCAC(vm, vm_rms), font, getColourElectrical());
        textContent.append ("\n\t");
        textContent.append (formatCurrent(cm), font, getColourAmber());
        textContent.append ("\n\t");
        
        

    }
    return textContent;
}

void PotElement::updateArrow()
{
    const auto& p2 = terminals[2];
    // Arrow
    float ratio = (100.0f-controlValue)/100.0f ;
    arrow.clear();
    arrow.startNewSubPath(p2);
    arrow.lineTo(pp2);
    arrow.addArrow(juce::Line(pp2, pp0 + (pp1-pp0)*ratio -arrowDir*RESISTOR_ZIGZAG_AMPLITUDE*1.3f), 1.0f, 10.0f, 10.0f);
    

    if (signalPaths.size()>0){
        auto& p = signalPaths[0].getSignalPaths()[2];
        p.path = arrow;
        p.rebuildCache();
    }
    
}


void PotElement::controlCallback(float value, SchematicPanelListener* listener)
{
    if (value <=0.1f) value = 0.1f;
    if (value >=99.9f) value = 99.9f;
    listener->setCircuitControl(getControlIndex(), value);
    controlValue = value;

    updateArrow();

    path.clear();
    path.addPath(arrow);
    path.addPath(zigzag);
    
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

    zigzagPlus.startNewSubPath (p0);
    zigzagPlus.lineTo(a);
    juce::Point<float>  curr = a;
    for (int i = 0; i < RESISTOR_ZIGZAG_COUNT/2; ++i)
    {
        int sign = std::pow(-1, i);
        curr = curr + (halfAmp * v * sign) + (s * u);
        if (i != 0 && i!= RESISTOR_ZIGZAG_COUNT){
            curr = curr + (halfAmp*v * sign) + (s*u);
        }
        zigzagPlus.lineTo (curr);
    }

    zigzagMinus.startNewSubPath (curr);
    for (int i = RESISTOR_ZIGZAG_COUNT/2; i <= RESISTOR_ZIGZAG_COUNT; ++i)
    {
        int sign = std::pow(-1, i);
        curr = curr + (halfAmp * v * sign) + (s * u);
        if (i != 0 && i!= RESISTOR_ZIGZAG_COUNT){
            curr = curr + (halfAmp*v * sign) + (s*u);
        }
        zigzagMinus.lineTo (curr);
    }
    zigzagMinus.lineTo(p1);

    zigzag.addPath(zigzagPlus);
    zigzag.addPath(zigzagMinus);

    pp0=a;
    pp1=b;

    float sign =1.0f;
    if ( p2.getDistanceFrom(p0 + d*0.5f +v*RESISTOR_ZIGZAG_LENGTH) >=
         p2.getDistanceFrom(p0 + d*0.5f -v*RESISTOR_ZIGZAG_LENGTH)   )
         sign = -1.0f;
    pp2= p0 + d*0.5f + sign*v*RESISTOR_ZIGZAG_LENGTH;
    arrowDir = -sign*v;


    float ratio = (100.0f-controlValue)/100.0f ;
    arrow.startNewSubPath(p2);
    arrow.lineTo(pp2.x, p2.y);
    arrow.lineTo(pp2);
    arrow.addArrow(juce::Line(pp2, pp0 + (pp1-pp0)*ratio -arrowDir*RESISTOR_ZIGZAG_AMPLITUDE*1.3f), 1.0f, 10.0f, 10.0f);

    const float labelOff = 40.0f;
    const juce::Point<float> m = (p0 + p1) * 0.5f;
    labelCenter = m + labelOff * -sign* v;
}
void PotElement::draw (juce::Graphics& g) const
{
    drawLabel(g, labelCenter, label);
}

void PotElement::drawPower (juce::Graphics& g) const
{
    if (getNumMonitors() > 0){
        float pPlus = getRMSValue(0, MONITOR_PORT_I)*getRMSValue(0, MONITOR_PORT_V) * POWER_SCALING; 
        float pMinus = getRMSValue(1, MONITOR_PORT_I)*getRMSValue(1, MONITOR_PORT_V) * POWER_SCALING; 
        float pOut = std::abs((getRMSValue(0, MONITOR_PORT_I)-getRMSValue(1, MONITOR_PORT_I)) * getRMSValue(1, MONITOR_PORT_V)) * POWER_SCALING; 
        drawPowerGlowPath(g, zigzagPlus, pPlus);
        drawPowerGlowPath(g, zigzagMinus, pMinus);
        drawPowerGlowPath(g, arrow,pOut);
    }
}


void PotElement::createSignalPaths () 
{
    signalPaths[0].addPath(zigzagPlus, 0.0f, 0.5f);
    signalPaths[0].addPath(zigzagMinus, 0.5f, 1.0f);
    signalPaths[0].addPath(arrow, 0.5f, 0.5f);
}

void PotElement::updateSignalPaths () {
    if (getNumMonitors()> 0){
        signalPaths[0].updateSignalPath(
            getSmoothedValue(0, MONITOR_PORT_I) * POWER_SCALING,
            getSmoothedValue(0, MONITOR_PORT_V)
        );
    }
};

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


void VarResElement::updateArrow()
{
    const auto& p2 = terminals[2];
    // Arrow
    float ratio = (100.0f-controlValue)/100.0f ;
    arrow.clear();
    arrow.startNewSubPath(p2);
    arrow.lineTo(pp2.x, p2.y);
    arrow.lineTo(pp2);
    arrow.addArrow(juce::Line(pp2, pp0 + (pp1-pp0)*ratio -arrowDir*RESISTOR_ZIGZAG_AMPLITUDE*1.3f), 1.0f, 10.0f, 10.0f);

    if (signalPaths.size()>0){
        auto& p = signalPaths[0].getSignalPaths()[2];
        p.path = arrow;
        p.rebuildCache();
    }
    
}

void TransformerElement::prepareToDraw () 
{
    const auto& p0 = terminals[0];
    const auto& p1 = terminals[1];
    const auto& p2 = terminals[2];
    const auto& p3 = terminals[3];

    const juce::Point<float> d1 = p1-p0;
    const float length1 = p1.getDistanceFrom(p0);
    const juce::Point<float> d2 = p3-p2;
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


    juce::Rectangle<float> bounds (50.0f, 50.0f, 200.0f, 200.0f);

    primary.startNewSubPath(p0);
    primary.lineTo(a);
    secondary.startNewSubPath(p2);
    secondary.lineTo(c);

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
    

    float coilGap = p3.x - p1.x;
    gapPath.startNewSubPath( a+v*coilGap*0.4f);
    gapPath.lineTo(b+v*coilGap*0.4f);
    gapPath.startNewSubPath( a+v*coilGap*0.6f);
    gapPath.lineTo(b+v*coilGap*0.6f);


    path.addPath(primary);
    path.addPath(secondary);
    path.addPath(gapPath);

    // Labels
    const float labelOff = -42.0f;
    const juce::Point<float> m = (p0 + p1) * 0.5f;
    labelCenter = m + labelOff * v;
}
void TransformerElement::draw (juce::Graphics& g) const
{
    drawLabel(g, labelCenter, label);

}

juce::String TransformerElement::valueToLabel (float v)
{
    return "TR="+ juce::String ((int) v);
}


float TransformerElement::labelToValue (const juce::String s)
{
    auto str = s.trim().toLowerCase();
    if (str.isEmpty()) return getValue(); // fallback to original

    // parse numeric part
    float value = str.getFloatValue();

    if (value == 0.0)
        return getValue();

    return (float) (value );
}



void TransformerElement::createSignalPaths () 
{
    signalPaths[0].addPath(primary);
    signalPaths[1].addPath(secondary);
}

void TransformerElement::updateSignalPaths () {
    if (getNumMonitors()> 1){
        signalPaths[0].updateSignalPath(
            getSmoothedValue(0, MONITOR_PORT_I) * POWER_SCALING,
            getSmoothedValue(0, MONITOR_PORT_V)
        );
        signalPaths[1].updateSignalPath(
            getSmoothedValue(1, MONITOR_PORT_I) * POWER_SCALING,
            getSmoothedValue(1, MONITOR_PORT_V)
        );
    }
};

void TransformerElement::drawPower (juce::Graphics& g) const
{
    if (getNumMonitors() > 1){
        float pprim = getRMSValue(0, MONITOR_PORT_I)*getRMSValue(0, MONITOR_PORT_V) * POWER_SCALING; 
        float psec = getRMSValue(1, MONITOR_PORT_I)*getRMSValue(1, MONITOR_PORT_V) * POWER_SCALING; 
        drawPowerGlowPath(g, primary, pprim);
        drawPowerGlowPath(g, secondary, psec);
    }
}


juce::String TransformerElement::getInspectValue () 
{
    return label;
}

juce::AttributedString TransformerElement::getInspectContent () 
{
    juce::AttributedString textContent;
    auto font = juce::Font (juce::FontOptions(FONT_SUB1));
    if (getNumMonitors() > 1){
        float vp = getSmoothedValue(0, MONITOR_PORT_V);
        float cp = getSmoothedValue(0, MONITOR_PORT_I);
        float vs = getSmoothedValue(1, MONITOR_PORT_V);
        float cs = getSmoothedValue(1, MONITOR_PORT_I);
        float vp_rms = getRMSValue(0, MONITOR_PORT_V);
        float vs_rms = getRMSValue(1, MONITOR_PORT_V);
        textContent.append ("Primary : ", font, getColourNormal());
        textContent.append ("\n\t");
        textContent.append (formatVDCAC(vp, vp_rms), font, getColourElectrical());
        textContent.append ("\n\t");
        textContent.append (formatCurrent(cp), font, getColourAmber());
        textContent.append ("\n");
        textContent.append ("Secondary : ", font, getColourNormal());
        textContent.append ("\n\t");
        textContent.append (formatVDCAC(vs, vs_rms), font, getColourElectrical());
        textContent.append ("\n\t");
        textContent.append (formatCurrent(cs), font, getColourAmber());
        textContent.append ("\n\t");
        

    }
    return textContent;
}