#include "TriodeElement.h"


const float TUBE_WIDTH = 180.0f;
const float TUBE_HEIGHT = 200.0f;

TriodeElement::TriodeElement (const juce::String& name,
                                    int paramIndex,
                                    std::vector<ValueChoice> parameters,
                                    Terminal center)
    : SchematicElement (name ,
      paramIndex ,
      parameters ,
      std::vector<Terminal>{
            juce::Point<float>{center.x - TUBE_WIDTH/2, center.y},
            juce::Point<float>{center.x, center.y - TUBE_HEIGHT/2},
            juce::Point<float>{center.x, center.y + TUBE_HEIGHT/2}
        })
{
}

void TriodeElement::draw (juce::Graphics& g) const
{
    const auto& p0 = terminals[0];
    const auto& p1 = terminals[1];
    const auto& p2 = terminals[2];

    float thickness = isHighlighted() ? STROKE_HIGHLIGHT : STROKE_NORMAL;
    g.setColour (juce::Colours::white);

    const juce::Point<float> center = (p2-p1)*0.5f + p1;

    float tubeScaling = 0.8;
    g.drawEllipse((center.x - TUBE_WIDTH*0.5f*tubeScaling), (center.y - TUBE_HEIGHT*0.5f*tubeScaling), TUBE_WIDTH*tubeScaling, TUBE_HEIGHT*tubeScaling, thickness);

    float tubePlatesHeight =  TUBE_HEIGHT/2.8f;
    juce::Path plate;
    plate.startNewSubPath (p1);
    plate.lineTo (p1 + Terminal {0.0f, tubePlatesHeight});
    g.strokePath (plate, juce::PathStrokeType (thickness));
    g.drawLine(juce::Line<float>(
        p1 + Terminal {-TUBE_WIDTH/4, tubePlatesHeight},
        p1 + Terminal {+TUBE_WIDTH/4, tubePlatesHeight}), thickness +2.0f);

    juce::Path cathode;
    cathode.startNewSubPath (p2);
    cathode.lineTo (p2 - Terminal {0.0f, TUBE_HEIGHT/4});
    cathode.lineTo (p2 - Terminal {+TUBE_WIDTH/4, TUBE_HEIGHT/4});
    cathode.lineTo (p2 - Terminal {+TUBE_WIDTH/4, tubePlatesHeight});
    cathode.lineTo (p2 - Terminal {-TUBE_WIDTH/4, tubePlatesHeight});
    cathode.lineTo (p2 - Terminal {-TUBE_WIDTH/4, tubePlatesHeight});
    g.strokePath (cathode, juce::PathStrokeType (thickness));

    const float dashes[] = { 6.0f, 6.0f };
    g.drawLine(juce::Line<float>(p0,p0+Terminal {TUBE_WIDTH/4.0f, 0.0f}), thickness);
    g.drawDashedLine(juce::Line<float>(
        p0+Terminal {TUBE_WIDTH/4.0f, 0.0f},
        p0+Terminal {TUBE_WIDTH*3.0f/4.0f, 0.0f}), dashes,2, thickness);

    // Update cached bounds
    cachedBounds = juce::Rectangle<float> (center.x - TUBE_WIDTH*0.5f, center.y - TUBE_HEIGHT*0.5f, TUBE_WIDTH, TUBE_HEIGHT);

    // Labels
    Terminal labelcenter = center + Terminal {115.0f, 0.0f};
    drawLabel(g, labelcenter);
}
