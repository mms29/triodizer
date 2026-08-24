#include "schematic/TriodeElement.h"

std::vector<Terminal>& getTriodeTerminals (Terminal center) {
    static std::vector<Terminal> terminals;
    terminals = std::vector<Terminal>{
                        juce::Point<float>{center.x - TUBE_WIDTH/2, center.y},
                        juce::Point<float>{center.x, center.y - TUBE_HEIGHT/2},
                        juce::Point<float>{center.x - TUBE_WIDTH/4, center.y + TUBE_HEIGHT/2}
                    };
    return terminals;
}
std::vector<Terminal>& getParallelTriodeTerminals (Terminal center) {
    static std::vector<Terminal> terminals;
    terminals = std::vector<Terminal>{
                        juce::Point<float>{center.x - TUBE_WIDTH, center.y},
                        juce::Point<float>{center.x, center.y - TUBE_HEIGHT/2},
                        juce::Point<float>{center.x, center.y + TUBE_HEIGHT/2}
                    };
    return terminals;
}


void TriodeElement::prepareToDraw () 
{
    const auto& p0 = terminals[0];
    const auto& p1 = terminals[1];
    const auto& p2 = terminals[2];
    float tubeScaling = 0.7;
    const juce::Point<float> center = Terminal{0.0f, (p2.y-p1.y)*0.5f} + p1;

    //PLATE
    float tubePlatesHeight =  TUBE_HEIGHT/3.2f;
    plateHolder.startNewSubPath (p1);
    plateHolder.lineTo (p1 + Terminal {0.0f, tubePlatesHeight});
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
    cathode.startNewSubPath (p2 - Terminal {-TUBE_WIDTH*2/4, cathodeHeight-2*cathodeBend});
    cathode.lineTo (p2 - Terminal {-TUBE_WIDTH*2/4, cathodeHeight-cathodeBend});
    cathode.lineTo (p2 - Terminal {-TUBE_WIDTH*2/4+TUBE_WIDTH/8, cathodeHeight});
    cathode.lineTo (p2 - Terminal {-TUBE_WIDTH/4, cathodeHeight});
    cathode.lineTo (p2 - Terminal {-TUBE_WIDTH/8, cathodeHeight});
    cathode.lineTo (p2 - Terminal {0.0f, cathodeHeight-cathodeBend});
    cathode.lineTo (p2 - Terminal {0.0f, cathodeHeight-2*cathodeBend});

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
        float flowX = p0.x + flowGrid+flowDist*(i*2 +1);
        juce::Path _flow;
        _flow.startNewSubPath(flowX, p0.y -flowMax);
        _flow.lineTo(flowX,  p0.y +flowMin);
        flow.push_back(_flow);
    }

    path.addPath(plateHolder);
    path.addPath(plate);
    path.addPath(gridHolder);
    path.addPath(grid);
    path.addPath(cathodeHolder);
    path.addPath(cathode);
    path.addPath(bulb);

    // Update cached bounds
    cachedBounds = juce::Rectangle<float> (center.x - TUBE_WIDTH*0.5f, center.y - TUBE_HEIGHT*0.5f, TUBE_WIDTH, TUBE_HEIGHT);

    // Labels
    labelCenter = center - Terminal {50.0f, 100.0f};
    
}
void TriodeElement::createSignalPaths ()
{
    signalPaths[0].addPath(plateHolder, 0.0f, 0.0f);
    signalPaths[0].addPath(cathodeHolder, 1.0f, 1.0f);
    signalPaths[0].addPath(plate, 0.0f, 0.0f);
    signalPaths[0].addPath(cathode, 1.0f, 1.0f);
    for (auto& p : flow)
        signalPaths[0].addPath(p);
    signalPaths[0].shufflePhase();

    if (signalPaths.size()>1){
        signalPaths[1].addPath(gridHolder, 0.0f, 0.0f);
        // signalPaths[1].addPath(grid, 0.0f, 0.0f);

    }

}



void TriodeElement::addPointToTerminal(Terminal t, const int termIndex, const bool ) {

    if (termIndex> 0) return;

    auto& tt = terminals[termIndex];
    juce::Path newPath;
    juce::Path tmp = gridHolder;
    newPath.startNewSubPath(t);
    newPath.lineTo(tt);
    gridHolder.clear();
    gridHolder.addPath(newPath);
    gridHolder.addPath(tmp);
    
    tt = t;
    path.addPath(newPath);

    if (signalPaths.size()>1){
        auto& p = signalPaths[1].getSignalPaths()[0];
        p.path = gridHolder;
        p.rebuildCache(); 
    }
}

void TriodeElement::updateSignalPaths () {
    if (getNumMonitors()> 0){

        signalPaths[0].updateSignalPath(
            getSmoothedValue(0, MONITOR_TUBE_IK) * INTENSITY_SCALING,
            getSmoothedValue(0, MONITOR_TUBE_VP) -getSmoothedValue(0, MONITOR_TUBE_VK) ,
            getRMSValue(0, MONITOR_TUBE_IP)*std::abs(getRMSValue(0, MONITOR_TUBE_VP)-getRMSValue(0, MONITOR_TUBE_VK)) * POWER_SCALING
        );
        if (signalPaths.size()>1)
            signalPaths[1].updateSignalPath(
                0.0f,
                getSmoothedValue(0, MONITOR_TUBE_VG)
            );
    }
};
void TriodeElement::draw (juce::Graphics& g) const
{
    if (getNumMonitors() > 0){
        // drawGlowPath(g, plate, getColourElectrical(),  getSmoothedValue(0, MONITOR_TUBE_SP));
        // drawGlowPath(g, grid,getColourPurple(), intensity);
        // drawGlowAndCorePath(g, bulb,  getSmoothedValue(0, MONITOR_TUBE_SC),getColourNormal(),getColourNormal(), isHighlighted());
        // float intensity = getSmoothedValue(0, MONITOR_TUBE_IP) *.2e3f;
        // drawGlowPath(g, cathode, getColourHotRed(), intensity);
    
    }
    drawGlowAndCorePath(g, filament, 0.1f,getColourHotRed(),getColourHotRed(), isHighlighted());

    juce::Font fontName = juce::FontOptions (FONT_TITLE);
    juce::Font fontValue = juce::FontOptions (FONT_SUB1);
    if (isHighlighted()){
        fontName = fontName.boldened();
        fontValue = fontValue.boldened();
    }

    // Name
    g.setFont (fontName);
    g.setColour (getColourHighlight());
    g.drawText (getName(),
                labelCenter.getX() - 40, labelCenter.getY() - 18, 80, 18,
                juce::Justification::centred, true);

    // Value
    g.setFont (fontValue);
    g.setColour (getColourNormal());
    g.drawText (getChoiceLabel(),
                labelCenter.getX() - 40, labelCenter.getY() + 2, 80, 18,
                juce::Justification::centred, true);


}

juce::AttributedString TriodeElement::getInspectContent () 
{
    juce::AttributedString textContent;
    auto font = juce::Font (juce::FontOptions(FONT_SUB1));
    if (getNumMonitors()>0){
        textContent.append ("Plate voltage : \n\t ", font, getColourNormal());
        textContent.append (formatVDCAC(getSmoothedValue(0, MONITOR_TUBE_VP),getRMSValue(0, MONITOR_TUBE_VP)), font, getColourElectrical());
        textContent.append ("\nGrid voltage : \n\t ", font, getColourNormal());
        textContent.append (formatVDCAC(getSmoothedValue(0, MONITOR_TUBE_VG),getRMSValue(0, MONITOR_TUBE_VG)), font, getColourElectrical());
        textContent.append ("\nCathode voltage : \n\t ", font, getColourNormal());
        textContent.append (formatVDCAC(getSmoothedValue(0, MONITOR_TUBE_VK),getRMSValue(0, MONITOR_TUBE_VK)), font, getColourElectrical());
        textContent.append ("\nCurrent : \n\t ", font, getColourNormal());
        textContent.append (formatCurrent(getSmoothedValue(0, MONITOR_TUBE_IP)), font, getColourHotRed());
        textContent.append ("\nPlate dissipation : \n\t ", font, getColourNormal());
        textContent.append (formatPower(getSmoothedValue(0, MONITOR_TUBE_IP)*(getSmoothedValue(0, MONITOR_TUBE_VP)-getSmoothedValue(0, MONITOR_TUBE_VK))), font, getColourAmber());
    
        textContent.append ("\nPlate saturation : \n\t ", font, getColourNormal());
        textContent.append (juce::String(getSmoothedValue(0, MONITOR_TUBE_SC)*100.0F, 1) + " %", font, getColourLaserGreen());
        textContent.append ("\nTube cutoff : \n\t", font, getColourNormal());
        textContent.append (juce::String(getSmoothedValue(0, MONITOR_TUBE_SP)*100.0F, 1) + " %", font, getColourLaserGreen());
    }
    return textContent;
}

juce::String TriodeElement::getInspectValue () 
{
    return getChoiceLabel();
}



void ParallelTriodeElement::prepareToDraw () 
{
    // Terminals are mutable which is convenient hre but is a hack. Should be refactored to have a better design.
    terminals[0] = Terminal{terminals[0].x - TUBE_WIDTH/2, terminals[0].y};
    terminals[2] = Terminal{terminals[2].x + TUBE_WIDTH/4, terminals[2].y};


    const auto& p0 = terminals[0];
    const auto& p1 = terminals[1];
    const auto& p2 = terminals[2];
    float tubeScaling = 0.7;
    const juce::Point<float> center = Terminal{0.0f, (p2.y-p1.y)*0.5f} + p1;

    //PLATE
    float tubePlatesHeight =  TUBE_HEIGHT/3.2f;
    plateHolder.startNewSubPath (p1);
    plateHolder.lineTo (p1 + Terminal {0.0f, tubePlatesHeight*0.7f});

    lplate.startNewSubPath (p1 + Terminal {0.0f, tubePlatesHeight*0.7f });
    lplate.lineTo (p1 + Terminal {-TUBE_WIDTH/4, tubePlatesHeight*0.7f });
    lplate.lineTo (p1 + Terminal {-TUBE_WIDTH/4, tubePlatesHeight});
    lplate.lineTo (p1 + Terminal {-TUBE_WIDTH/4 - TUBE_WIDTH*3/8, tubePlatesHeight});
    lplate.lineTo (p1 + Terminal {+TUBE_WIDTH/4 - TUBE_WIDTH*3/8, tubePlatesHeight});
    lplate.lineTo (p1 + Terminal {+TUBE_WIDTH/4 - TUBE_WIDTH*3/8, tubePlatesHeight+2.0f});
    lplate.lineTo (p1 + Terminal {-TUBE_WIDTH/4 - TUBE_WIDTH*3/8, tubePlatesHeight+2.0f});
    lplate.lineTo (p1 + Terminal {-TUBE_WIDTH/4 - TUBE_WIDTH*3/8, tubePlatesHeight});


    rplate.startNewSubPath (p1 + Terminal {0.0f, tubePlatesHeight*0.7f });
    rplate.lineTo (p1 + Terminal {+TUBE_WIDTH/4, tubePlatesHeight*0.7f });
    rplate.lineTo (p1 + Terminal {+TUBE_WIDTH/4, tubePlatesHeight});
    rplate.lineTo (p1 + Terminal {-TUBE_WIDTH/4 + TUBE_WIDTH*3/8, tubePlatesHeight});
    rplate.lineTo (p1 + Terminal {+TUBE_WIDTH/4 + TUBE_WIDTH*3/8, tubePlatesHeight});
    rplate.lineTo (p1 + Terminal {+TUBE_WIDTH/4 + TUBE_WIDTH*3/8, tubePlatesHeight+2.0f});
    rplate.lineTo (p1 + Terminal {-TUBE_WIDTH/4 + TUBE_WIDTH*3/8, tubePlatesHeight+2.0f});
    rplate.lineTo (p1 + Terminal {-TUBE_WIDTH/4 + TUBE_WIDTH*3/8, tubePlatesHeight});
    rplate.lineTo (p1 + Terminal {-TUBE_WIDTH/4 + TUBE_WIDTH*3/8, tubePlatesHeight});

    plate.addPath(lplate);
    plate.addPath(rplate);

    // CATHODE
    float cathodeHeight =  TUBE_HEIGHT/2.8f;
    float cathodeBend =  TUBE_HEIGHT/16.0f;
    cathodeHolder.startNewSubPath (p2 - Terminal {0.0f, cathodeHeight-2*cathodeBend});
    cathodeHolder.lineTo (p2);

    lcathode.startNewSubPath (p2 - Terminal {-TUBE_WIDTH*2/4 - TUBE_WIDTH/8, cathodeHeight-2*cathodeBend});
    lcathode.lineTo (p2 - Terminal {-TUBE_WIDTH*2/4 - TUBE_WIDTH/8, cathodeHeight-cathodeBend});
    lcathode.lineTo (p2 - Terminal {-TUBE_WIDTH*2/4, cathodeHeight});
    lcathode.lineTo (p2 - Terminal {-TUBE_WIDTH/4- TUBE_WIDTH/8, cathodeHeight});
    lcathode.lineTo (p2 - Terminal {-2*TUBE_WIDTH/8, cathodeHeight});
    lcathode.lineTo (p2 - Terminal {- TUBE_WIDTH/8, cathodeHeight-cathodeBend});
    lcathode.lineTo (p2 - Terminal {- TUBE_WIDTH/8, cathodeHeight-2*cathodeBend});
    lcathode.lineTo (p2 - Terminal {0.0f, cathodeHeight-2*cathodeBend});

    rcathode.startNewSubPath (p2 - Terminal {TUBE_WIDTH*2/4 + TUBE_WIDTH/8, cathodeHeight-2*cathodeBend});
    rcathode.lineTo (p2 - Terminal {TUBE_WIDTH*2/4 + TUBE_WIDTH/8, cathodeHeight-cathodeBend});
    rcathode.lineTo (p2 - Terminal {TUBE_WIDTH*2/4-TUBE_WIDTH/8 + TUBE_WIDTH/8, cathodeHeight});
    rcathode.lineTo (p2 - Terminal {TUBE_WIDTH/4 + TUBE_WIDTH/8, cathodeHeight});
    rcathode.lineTo (p2 - Terminal {TUBE_WIDTH/8 + TUBE_WIDTH/8, cathodeHeight});
    rcathode.lineTo (p2 - Terminal {0.0f+ TUBE_WIDTH/8, cathodeHeight-cathodeBend});
    rcathode.lineTo (p2 - Terminal {0.0f+ TUBE_WIDTH/8, cathodeHeight-2*cathodeBend});
    rcathode.lineTo (p2 - Terminal {0.0f, cathodeHeight-2*cathodeBend});

    cathode.addPath(lcathode);
    cathode.addPath(rcathode);

    // grid
    float start = TUBE_WIDTH/4.0f;
    float segment = TUBE_WIDTH/8.0f *0.6f;
    float dash = TUBE_WIDTH/8.0f *0.2f;

    auto m = p0 ;
    gridHolder.startNewSubPath(m);
    m+= Terminal { TUBE_WIDTH/8, 0.0f};
    gridHolder.lineTo(m+Terminal {start-dash, 0.0f});
    grid.startNewSubPath(m+Terminal {start+dash, 0.0f});
    grid.lineTo(m+Terminal {start+dash+segment, 0.0f});
    grid.startNewSubPath(m+Terminal {start+dash+segment+2*dash, 0.0f});
    grid.lineTo(m+Terminal {start+dash+segment+2*dash +segment, 0.0f});
    grid.startNewSubPath(m+Terminal {start+dash+segment+2*dash +segment +2*dash, 0.0f});
    grid.lineTo(m+Terminal {start+dash+segment+2*dash +segment +2*dash +segment, 0.0f});
    grid.startNewSubPath(m+Terminal {start+dash+segment+2*dash +segment +2*dash +segment +2*dash, 0.0f});
    grid.lineTo(m+Terminal {start+dash+segment+2*dash +segment +2*dash +segment +2*dash + segment, 0.0f});

    m = p0 + Terminal {TUBE_WIDTH - TUBE_WIDTH/8 +dash, 0.0f};
    gridConnect.startNewSubPath(m);
    gridConnect.lineTo(m+Terminal {start-dash, 0.0f});
    grid.startNewSubPath(m+Terminal {start+dash, 0.0f});
    grid.lineTo(m+Terminal {start+dash+segment, 0.0f});
    grid.startNewSubPath(m+Terminal {start+dash+segment+2*dash, 0.0f});
    grid.lineTo(m+Terminal {start+dash+segment+2*dash +segment, 0.0f});
    grid.startNewSubPath(m+Terminal {start+dash+segment+2*dash +segment +2*dash, 0.0f});
    grid.lineTo(m+Terminal {start+dash+segment+2*dash +segment +2*dash +segment, 0.0f});
    grid.startNewSubPath(m+Terminal {start+dash+segment+2*dash +segment +2*dash +segment +2*dash, 0.0f});
    grid.lineTo(m+Terminal {start+dash+segment+2*dash +segment +2*dash +segment +2*dash + segment, 0.0f});

    //filament
    float filamentHeight =  TUBE_HEIGHT/4.0f;
    filament.startNewSubPath(p2- Terminal {-TUBE_WIDTH*1/8 - TUBE_WIDTH/8, filamentHeight*0.5f});
    filament.lineTo(p2- Terminal {-TUBE_WIDTH*1/8 -TUBE_WIDTH/8, filamentHeight});
    filament.lineTo(p2- Terminal {-TUBE_WIDTH/4 -TUBE_WIDTH/8, filamentHeight*1.2f});
    filament.lineTo(p2- Terminal {-TUBE_WIDTH*3/8 -TUBE_WIDTH/8, filamentHeight});
    filament.lineTo(p2- Terminal {-TUBE_WIDTH*3/8 -TUBE_WIDTH/8, filamentHeight*0.5f});
    
    filament.startNewSubPath(p2- Terminal {TUBE_WIDTH*1/8 + TUBE_WIDTH/8, filamentHeight*0.5f});
    filament.lineTo(p2- Terminal {TUBE_WIDTH*1/8 +TUBE_WIDTH/8, filamentHeight});
    filament.lineTo(p2- Terminal {TUBE_WIDTH/4 +TUBE_WIDTH/8, filamentHeight*1.2f});
    filament.lineTo(p2- Terminal {TUBE_WIDTH*3/8 +TUBE_WIDTH/8, filamentHeight});
    filament.lineTo(p2- Terminal {TUBE_WIDTH*3/8 +TUBE_WIDTH/8, filamentHeight*0.5f});

    // BULB
    float x = center.x - (TUBE_WIDTH + TUBE_WIDTH/16)  * tubeScaling;
    float y = center.y - TUBE_HEIGHT * 0.5f * tubeScaling;
    float w = (TUBE_WIDTH + TUBE_WIDTH/16) * tubeScaling*2;
    float h = TUBE_HEIGHT * tubeScaling;
    float r = h * 0.5f; // very round ends
    bulb.startNewSubPath(x+w, y + h - r*0.5f);
    bulb.lineTo(x+w, y +  r*0.5f);
    bulb.addArc(x + w*.5f, y, w*.5f, r,
             juce::MathConstants<float>::pi/2,
             0.0,
             true);
    bulb.lineTo(x+ w*0.25f, y);
    bulb.addArc(x, y, w*.5f, r,
             0.0,
             -juce::MathConstants<float>::pi/2,
             true);
    bulb.lineTo(x, y + h - r*0.5f);
    bulb.addArc(x +  w*.5f, y + h - r,  w*.5f, r,
             juce::MathConstants<float>::pi/2,
             juce::MathConstants<float>::pi,
             true);
    bulb.lineTo(x+ w*0.25f, y +h);
    bulb.addArc(x, y + h - r,  w*.5f, r,
             juce::MathConstants<float>::pi,
             juce::MathConstants<float>::pi*3/2,
             true);

    float flowDist = TUBE_WIDTH/16.0f;
    float flowMin = TUBE_HEIGHT/8.0f;
    float flowMax = TUBE_HEIGHT/6.0f;
    float flowGrid = TUBE_WIDTH/4.0f + TUBE_WIDTH/8.0f;

    for (int i = 0; i<4; i++){
        float flowX = p0.x + flowGrid+flowDist*(i*2 +1);
        juce::Path _flow;
        _flow.startNewSubPath(flowX, p0.y -flowMax);
        _flow.lineTo(flowX,  p0.y +flowMin);
        flow.push_back(_flow);
    }
    m = p0 + Terminal {TUBE_WIDTH - TUBE_WIDTH/4 +dash, 0.0f};
    for (int i = 0; i<4; i++){
        float flowX = m.x + flowGrid+flowDist*(i*2 +1);
        juce::Path _flow;
        _flow.startNewSubPath(flowX, m.y -flowMax);
        _flow.lineTo(flowX,  m.y +flowMin);
        flow.push_back(_flow);
    }

    path.addPath(plateHolder);
    path.addPath(plate);
    path.addPath(gridHolder);
    path.addPath(gridConnect);
    path.addPath(grid);
    path.addPath(cathodeHolder);
    path.addPath(cathode);
    path.addPath(bulb);

    // Update cached bounds
    cachedBounds = juce::Rectangle<float> (center.x - TUBE_WIDTH*0.5f, center.y - TUBE_HEIGHT*0.5f, TUBE_WIDTH, TUBE_HEIGHT);

    // Labels
    labelCenter = center - Terminal {50.0f, 100.0f};
    
}
void ParallelTriodeElement::createSignalPaths ()
{
    signalPaths[0].addPath(plateHolder, 0.0f, 0.0f);
    signalPaths[0].addPath(cathodeHolder, 1.0f, 1.0f);
    signalPaths[0].addPath(lplate, 0.0f, 0.0f);
    signalPaths[0].addPath(rplate, 0.0f, 0.0f);
    signalPaths[0].addPath(lcathode, 1.0f, 1.0f);
    signalPaths[0].addPath(rcathode, 1.0f, 1.0f);
    for (auto& p : flow)
        signalPaths[0].addPath(p);
    signalPaths[0].shufflePhase();

    if (signalPaths.size()>1){
        signalPaths[1].addPath(gridHolder, 0.0f, 0.0f);
        signalPaths[1].addPath(gridConnect, 0.0f, 0.0f);
    }

}
