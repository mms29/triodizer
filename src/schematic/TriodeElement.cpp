#include "schematic/TriodeElement.h"

void TriodeElement::prepareToDraw () 
{
    const auto& p0 = terminals[0];
    const auto& p1 = terminals[1];
    const auto& p2 = terminals[2];
    float tubeScaling = 0.7;
    const juce::Point<float> center = Terminal{0.0f, (p2.y-p1.y)*0.5f} + p1;

    //PLATE
    float tubePlatesHeight =  TUBE_HEIGHT/3.2f;
    plateHolder.startNewSubPath (p1 + Terminal {0.0f, tubePlatesHeight});
    plateHolder.lineTo (p1);
    plate.startNewSubPath (p1 + Terminal {0.0f, tubePlatesHeight});
    plate.lineTo (p1 + Terminal {-TUBE_WIDTH/4, tubePlatesHeight});
    plate.lineTo (p1 + Terminal {+TUBE_WIDTH/4, tubePlatesHeight});
    plate.lineTo (p1 + Terminal {+TUBE_WIDTH/4, tubePlatesHeight+2.0f});
    plate.lineTo (p1 + Terminal {-TUBE_WIDTH/4, tubePlatesHeight+2.0f});
    plate.lineTo (p1 + Terminal {-TUBE_WIDTH/4, tubePlatesHeight});

    // CATHODE
    float cathodeHeight =  TUBE_HEIGHT/2.8f;
    float cathodeBend =  TUBE_HEIGHT/16.0f;
    cathodeHolder.startNewSubPath (p2 - Terminal {0.0f, cathodeHeight-2*cathodeBend});
    cathodeHolder.lineTo (p2);
    cathode.startNewSubPath (p2 - Terminal {0.0f, cathodeHeight-2*cathodeBend});
    cathode.lineTo (p2 - Terminal {0.0f, cathodeHeight-cathodeBend});
    cathode.lineTo (p2 - Terminal {-TUBE_WIDTH/8, cathodeHeight});
    cathode.lineTo (p2 - Terminal {-TUBE_WIDTH/4, cathodeHeight});
    cathode.lineTo (p2 - Terminal {-TUBE_WIDTH*2/4+TUBE_WIDTH/8, cathodeHeight});
    cathode.lineTo (p2 - Terminal {-TUBE_WIDTH*2/4, cathodeHeight-cathodeBend});
    cathode.lineTo (p2 - Terminal {-TUBE_WIDTH*2/4, cathodeHeight-2*cathodeBend});

    // grid
    float start = TUBE_WIDTH/4.0f;
    float segment = TUBE_WIDTH/8.0f *0.6f;
    float dash = TUBE_WIDTH/8.0f *0.2f;
    gridHolder.startNewSubPath(p0);
    gridHolder.lineTo(p0+Terminal {start-dash, 0.0f});
    grid.startNewSubPath(p0+Terminal {start+dash, 0.0f});
    grid.lineTo(p0+Terminal {start+dash+segment, 0.0f});
    grid.startNewSubPath(p0+Terminal {start+dash+segment+2*dash, 0.0f});
    grid.lineTo(p0+Terminal {start+dash+segment+2*dash +segment, 0.0f});
    grid.startNewSubPath(p0+Terminal {start+dash+segment+2*dash +segment +2*dash, 0.0f});
    grid.lineTo(p0+Terminal {start+dash+segment+2*dash +segment +2*dash +segment, 0.0f});
    grid.startNewSubPath(p0+Terminal {start+dash+segment+2*dash +segment +2*dash +segment +2*dash, 0.0f});
    grid.lineTo(p0+Terminal {start+dash+segment+2*dash +segment +2*dash +segment +2*dash + segment, 0.0f});

    //filament
    float filamentHeight =  TUBE_HEIGHT/4.0f;
    filament.startNewSubPath(p2- Terminal {-TUBE_WIDTH*1/8, filamentHeight*0.5f});
    filament.lineTo(p2- Terminal {-TUBE_WIDTH*1/8, filamentHeight});
    filament.lineTo(p2- Terminal {-TUBE_WIDTH/4, filamentHeight*1.2f});
    filament.lineTo(p2- Terminal {-TUBE_WIDTH*3/8, filamentHeight});
    filament.lineTo(p2- Terminal {-TUBE_WIDTH*3/8, filamentHeight*0.5f});

    // BULB
    float x = center.x - TUBE_WIDTH * 0.5f * tubeScaling;
    float y = center.y - TUBE_HEIGHT * 0.5f * tubeScaling;
    float w = TUBE_WIDTH * tubeScaling;
    float h = TUBE_HEIGHT * tubeScaling;
    float r = h * 0.5f; // very round ends
    bulb.startNewSubPath(x+w, y + h - r*0.5f);
    bulb.lineTo(x+w, y +  r*0.5f);
    bulb.addArc(x, y, w, r,
             juce::MathConstants<float>::pi/2,
             -juce::MathConstants<float>::pi/2,
             true);
    bulb.lineTo(x, y + h - r*0.5f);
    bulb.addArc(x, y + h - r, w, r,
             juce::MathConstants<float>::pi/2,
             juce::MathConstants<float>::pi*3/2,
             true);

    float flowDist = TUBE_WIDTH/16.0f;
    float flowMin = TUBE_HEIGHT/8.0f;
    float flowMax = TUBE_HEIGHT/6.0f;
    float flowGrid = TUBE_WIDTH/4.0f;

    for (int i = 0; i<4; i++){
        float flowX = flowDist*(i*2 +1);
        juce::Path _flow, _rflow;
        _flow.startNewSubPath(p0.x +flowGrid, p0.y);
        _rflow.startNewSubPath(p0.x +flowGrid, p0.y);
        _flow.addArc(p0.x +flowGrid - flowX, p0.y,flowX*2, flowMin*2, 0.0f, juce::MathConstants<float>::pi/2);
        _rflow.addArc(p0.x +flowGrid - flowX, p0.y-2*flowMax,flowX*2, flowMax*2,  -juce::MathConstants<float>::pi, -juce::MathConstants<float>::pi*3/2);
        // _flow.startNewSubPath(center + juce::Point<float>{flowX, flowMin+i});
        // _flow.lineTo(center + juce::Point<float>{flowX, flowMax+i});
        // _rflow.startNewSubPath(center + juce::Point<float>{flowX, flowMax+i});
        // _rflow.lineTo(center + juce::Point<float>{flowX, flowMin+i});
        flow.push_back(_flow);
        reverseflow.push_back(_rflow);
    }

    // Update cached bounds
    cachedBounds = juce::Rectangle<float> (center.x - TUBE_WIDTH*0.5f, center.y - TUBE_HEIGHT*0.5f, TUBE_WIDTH, TUBE_HEIGHT);

    // Labels
    labelCenter = center + Terminal {100.0f, -50.0f};
    
}
void TriodeElement::createSignalPath (const int signalPathMode)
{
    setSignalPath(true);

    if (signalPathMode == 0){
        signalPaths.push_back(CachedPath{plateHolder});
        signalPaths.push_back(CachedPath{gridHolder});
        for (auto& p : reverseflow)
            signalPaths.push_back(CachedPath{p});
    }
    else{
        signalPaths.push_back(CachedPath{cathodeHolder});
        signalPaths.push_back(CachedPath{gridHolder});
        for (auto& p : flow)
            signalPaths.push_back(CachedPath{p});
    }
    for (auto& p : signalPaths)
        p.rebuildCache();
}
void TriodeElement::draw (juce::Graphics& g) const
{
    float intensity = 0.0f;
    float var=0.0f;
    if (getNumMonitors()>0){
        intensity = getSmoothedValue(0) *.2e3f;
        var = getRMSValue(0) * POWER_SCALING; 
    }

    drawGlowPath(g, plateHolder, var,getColourNormal(),getColourAmber(), isHighlighted());
    drawGlowPath(g, plate, intensity,getColourNormal(),getColourElectrical(), isHighlighted());

    drawGlowPath(g, gridHolder, 0.1f,getColourNormal(),getColourAmber(), isHighlighted());
    drawGlowPath(g, grid, 0.1f,getColourPurple(),getColourPurple(), isHighlighted());

    drawGlowPath(g, cathodeHolder, var, getColourNormal(),getColourAmber(), isHighlighted());
    drawGlowPath(g, cathode, intensity, getColourNormal(),getColourHotRed(), isHighlighted());

    drawGlowPath(g, filament, 0.0f,getColourHotRed(),getColourHotRed(), isHighlighted());
    drawGlowPath(g, bulb, 0.1f,getColourNormal(),getColourNormal(), isHighlighted());

    for (auto& p : signalPaths)
        drawSignalPath(g, p, 1.0f, getClock());
    drawLabel(g, labelCenter, getChoiceLabel());

}


void TriodeElement::drawInspector (juce::Graphics& g) const
{
    const auto& p1 = terminals[1];
    const auto& p2 = terminals[2];
    const juce::Point<float> center = Terminal{0.0f, (p2.y-p1.y)*0.5f} + p1 ;

    Terminal inspecTopLeft = center + Terminal {100.0f, -50.0f};

    juce::Rectangle<float> bounds { inspecTopLeft.x, inspecTopLeft.y, 200.0f, 300.0f};

    // Border
    juce::Path borderPath;
    borderPath.addRoundedRectangle(bounds, 10.0f, 10.0f);
    drawGlowPath(g, borderPath, .1f, getColourNormal(), getColourNormal(), false);

    // Background
    g.setColour (getColourBackground().withAlpha(0.9f));
    g.fillRoundedRectangle (bounds, 10.0f);

    // Title
    bounds.reduced (4.0f);
    bounds.removeFromLeft(10.0f);
    bounds.removeFromRight(10.0f);
    bounds.removeFromTop(5.0f);

    juce::AttributedString textTitle;
    textTitle.append (getName(), juce::Font (juce::FontOptions(FONT_TITLE)).boldened(), getColourHighlight());
    textTitle.append (" - "+ getChoiceLabel(),juce::Font (juce::FontOptions(FONT_TITLE)), getColourNormal());
    textTitle.draw (g, bounds.toFloat());

    // line
    g.setColour (getColourNormal());
    bounds.removeFromTop(30);
    g.drawLine (juce::Line(bounds.getTopLeft(), bounds.getTopRight()), 1.0f);
    
    // Content    
    bounds.removeFromTop(10);
    juce::AttributedString textContent;
    auto font = juce::Font (juce::FontOptions(FONT_SUB1));
    if (getNumMonitors()>1){
        textContent.append ("Plate voltage : \n", font, getColourNormal());
        textContent.append ("\t"+ juce::String(getSmoothedValue(3), 1)+" VDC\n", font, getColourElectrical());
        textContent.append ("\t"+ juce::String(getRMSValue(3), 1)+" VAC\n", font, getColourElectrical());
        textContent.append ("Grid voltage : \n", font, getColourNormal());
        textContent.append ("\t"+ juce::String(getSmoothedValue(1), 1)+" VDC\n", font, getColourPurple());
        textContent.append ("\t"+ juce::String(getRMSValue(1), 1)+" VAC\n", font, getColourPurple());
        textContent.append ("Cathode voltage : \n", font, getColourNormal());
        textContent.append ("\t"+ juce::String(getSmoothedValue(2), 1)+" VDC\n", font, getColourHotRed());
        textContent.append ("\t"+ juce::String(getRMSValue(2), 1)+" VAC\n", font, getColourHotRed());

    }
    if (getNumMonitors()>0){
        textContent.append ("Current : \n", font, getColourNormal());
        textContent.append ("\t"+ juce::String(getSmoothedValue(0)*1e3f, 2)+" mA\n", font, getColourAmber());
    }
    textContent.draw (g, bounds.toFloat());

}
