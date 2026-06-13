#include "schematic/TriodeElement.h"


void TriodeElement::draw (juce::Graphics& g) const
{
    const auto& p0 = terminals[0];
    const auto& p1 = terminals[1];
    const auto& p2 = terminals[2];
    float tubeScaling = 0.7;
    const juce::Point<float> center = Terminal{0.0f, (p2.y-p1.y)*0.5f} + p1;
    float intensity = 0.0f;
    if (getNumMonitors()>0){
        intensity = getSmoothedValue(0) *.2e3f;
    }

    //PLATE
    float tubePlatesHeight =  TUBE_HEIGHT/3.2f;
    juce::Path plateHolder;
    plateHolder.startNewSubPath (p1);
    plateHolder.lineTo (p1 + Terminal {0.0f, tubePlatesHeight});
    drawGlowPath(g, plateHolder, 0.1f,COLOR_NORMAL,COLOR_AMBER, isHighlighted());
    juce::Path plate;
    plate.startNewSubPath (p1 + Terminal {0.0f, tubePlatesHeight});
    plate.lineTo (p1 + Terminal {-TUBE_WIDTH/4, tubePlatesHeight});
    plate.lineTo (p1 + Terminal {+TUBE_WIDTH/4, tubePlatesHeight});
    plate.lineTo (p1 + Terminal {+TUBE_WIDTH/4, tubePlatesHeight+2.0f});
    plate.lineTo (p1 + Terminal {-TUBE_WIDTH/4, tubePlatesHeight+2.0f});
    plate.lineTo (p1 + Terminal {-TUBE_WIDTH/4, tubePlatesHeight});

    drawGlowPath(g, plate, intensity,COLOR_NORMAL,COLOR_ELECTRICAL, isHighlighted());

    // CATHODE
    float cathodeHeight =  TUBE_HEIGHT/2.8f;
    float cathodeBend =  TUBE_HEIGHT/16.0f;
    juce::Path cathode;
    cathode.startNewSubPath (p2);
    cathode.lineTo (p2 - Terminal {0.0f, cathodeHeight-cathodeBend});
    cathode.lineTo (p2 - Terminal {-TUBE_WIDTH/8, cathodeHeight});
    cathode.lineTo (p2 - Terminal {-TUBE_WIDTH/4, cathodeHeight});
    cathode.lineTo (p2 - Terminal {-TUBE_WIDTH*2/4+TUBE_WIDTH/8, cathodeHeight});
    cathode.lineTo (p2 - Terminal {-TUBE_WIDTH*2/4, cathodeHeight-cathodeBend});
    cathode.lineTo (p2 - Terminal {-TUBE_WIDTH*2/4, cathodeHeight-2*cathodeBend});
    
    drawGlowPath(g, cathode, intensity, COLOR_NORMAL,COLOR_HOTRED, isHighlighted());


    // grid
    float start = TUBE_WIDTH/4.0f;
    float segment = TUBE_WIDTH/8.0f *0.6f;
    float dash = TUBE_WIDTH/8.0f *0.2f;
    juce::Path gridHolder;
    gridHolder.startNewSubPath(p0);
    gridHolder.lineTo(p0+Terminal {start-dash, 0.0f});
    drawGlowPath(g, gridHolder, 0.1f,COLOR_NORMAL,COLOR_AMBER, isHighlighted());
    juce::Path grid;
    grid.startNewSubPath(p0+Terminal {start+dash, 0.0f});
    grid.lineTo(p0+Terminal {start+dash+segment, 0.0f});
    grid.startNewSubPath(p0+Terminal {start+dash+segment+2*dash, 0.0f});
    grid.lineTo(p0+Terminal {start+dash+segment+2*dash +segment, 0.0f});
    grid.startNewSubPath(p0+Terminal {start+dash+segment+2*dash +segment +2*dash, 0.0f});
    grid.lineTo(p0+Terminal {start+dash+segment+2*dash +segment +2*dash +segment, 0.0f});
    grid.startNewSubPath(p0+Terminal {start+dash+segment+2*dash +segment +2*dash +segment +2*dash, 0.0f});
    grid.lineTo(p0+Terminal {start+dash+segment+2*dash +segment +2*dash +segment +2*dash + segment, 0.0f});

    drawGlowPath(g, grid, 0.1f,COLOR_PURPLE,COLOR_PURPLE, isHighlighted());

    //filament
    juce::Path filament;
    float filamentHeight =  TUBE_HEIGHT/4.0f;

    filament.startNewSubPath(p2- Terminal {-TUBE_WIDTH*1/8, filamentHeight*0.5f});
    filament.lineTo(p2- Terminal {-TUBE_WIDTH*1/8, filamentHeight});
    filament.lineTo(p2- Terminal {-TUBE_WIDTH/4, filamentHeight*1.2f});
    filament.lineTo(p2- Terminal {-TUBE_WIDTH*3/8, filamentHeight});
    filament.lineTo(p2- Terminal {-TUBE_WIDTH*3/8, filamentHeight*0.5f});

    drawGlowPath(g, filament, 0.0f,COLOR_HOTRED,COLOR_HOTRED, isHighlighted());

    // BULB
    juce::Path p;
    float x = center.x - TUBE_WIDTH * 0.5f * tubeScaling;
    float y = center.y - TUBE_HEIGHT * 0.5f * tubeScaling;
    float w = TUBE_WIDTH * tubeScaling;
    float h = TUBE_HEIGHT * tubeScaling;
    float r = h * 0.5f; // very round ends
    p.startNewSubPath(x+w, y + h - r*0.5f);
    p.lineTo(x+w, y +  r*0.5f);
    p.addArc(x, y, w, r,
             juce::MathConstants<float>::pi/2,
             -juce::MathConstants<float>::pi/2,
             true);
    p.lineTo(x, y + h - r*0.5f);
    p.addArc(x, y + h - r, w, r,
             juce::MathConstants<float>::pi/2,
             juce::MathConstants<float>::pi*3/2,
             true);

    drawGlowPath(g, p, 0.1f,COLOR_NORMAL,COLOR_NORMAL, isHighlighted());

    // Update cached bounds
    cachedBounds = juce::Rectangle<float> (center.x - TUBE_WIDTH*0.5f, center.y - TUBE_HEIGHT*0.5f, TUBE_WIDTH, TUBE_HEIGHT);

    // Labels
    Terminal labelcenter = center + Terminal {100.0f, -50.0f};
    drawLabel(g, labelcenter, getChoiceLabel());

}


void TriodeElement::drawInspector (juce::Graphics& g) const
{
    const auto& p1 = terminals[1];
    const auto& p2 = terminals[2];
    const juce::Point<float> center = Terminal{0.0f, (p2.y-p1.y)*0.5f} + p1 ;

    Terminal inspecTopLeft = center + Terminal {100.0f, -50.0f};

    juce::Rectangle<float> bounds { inspecTopLeft.x, inspecTopLeft.y, 200.0f, 300.0f};

    // Background
    g.setColour (COLOR_BACKGROUND);
    g.fillRoundedRectangle (bounds, 10.0f);

    // Border
    g.setColour (COLOR_NORMAL);
    g.drawRoundedRectangle (bounds.reduced (1.0f), 10.0f, 2.0f);

    // Title
    g.setColour (COLOR_HIGHLIGHT);
    g.setFont (FONT_TITLE);

    g.drawText (getName() + " - " + getChoiceLabel(),bounds,
                juce::Justification::topLeft);

    bounds.removeFromTop(20);
    g.drawLine (juce::Line(bounds.getTopLeft(), bounds.getTopRight()), 1.0f);
    // Dummy values
    g.setFont (FONT_SUB1);

    g.drawText ("Power : 2.20 mW",
                bounds.removeFromTop(20),
                juce::Justification::left);
}