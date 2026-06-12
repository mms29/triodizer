#include "schematic/TriodeElement.h"

void TriodeElement::draw (juce::Graphics& g) const
{
    const auto& p0 = terminals[0];
    const auto& p1 = terminals[1];
    const auto& p2 = terminals[2];
    float tubeScaling = 0.8;
    const juce::Point<float> center = (p2-p1)*0.5f + p1;


    // if (getNumMonitors()>0){


    //     float current = getMonitorValue(0);
    //     float intensity = juce::jlimit (0.0f, 1.0f, current*0.5e3f);
    //     juce::ColourGradient gradient (
    //         juce::Colours::white.withBrightness(intensity),
    //         center.x, center.y,
    //         juce::Colours::black,
    //         center.x - TUBE_WIDTH * 0.5f * tubeScaling*1.5 ,center.y - TUBE_WIDTH * 0.5f * tubeScaling*1.5,
    //         true
    //     );
    //     gradient.addColour (0.2*intensity, juce::Colours::yellow.withBrightness(intensity));
    //     gradient.addColour (0.4*intensity, juce::Colour (255, 140, 0).withBrightness(intensity)); // orange
    //     gradient.addColour (0.6*intensity, juce::Colours::red.withBrightness(intensity));
    //     gradient.addColour (0.8*intensity, juce::Colours::darkred.withBrightness(intensity));
    //     gradient.addColour (1.0*intensity, juce::Colours::black);

    //     // g.setOpacity (current*0.5e3f);
    //     g.setGradientFill (gradient);
    //     g.fillEllipse (
    //         center.x - TUBE_WIDTH*2 * 0.5f * tubeScaling,
    //         center.y - TUBE_HEIGHT*2 * 0.5f * tubeScaling,
    //         TUBE_WIDTH * tubeScaling*2,
    //         TUBE_HEIGHT * tubeScaling*2
    //     );    
    // }


    // colors
    float thickness = isHighlighted() ? STROKE_HIGHLIGHT : STROKE_NORMAL;
    g.setColour (isHighlighted() ? SCHEMATIC_HIGHLIGHT : SCHEMATIC_NORMAL);

    g.drawEllipse (
            center.x - TUBE_WIDTH * 0.5f * tubeScaling,
            center.y - TUBE_HEIGHT * 0.5f * tubeScaling,
            TUBE_WIDTH * tubeScaling,
            TUBE_HEIGHT * tubeScaling, thickness
        );    
    //PLATE
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
    g.strokePath (cathode, juce::PathStrokeType (thickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded ));

    const float dashes[] = { 6.0f, 6.0f };
    g.drawLine(juce::Line<float>(p0,p0+Terminal {TUBE_WIDTH/4.0f, 0.0f}), thickness);
    g.drawDashedLine(juce::Line<float>(
        p0+Terminal {TUBE_WIDTH/4.0f, 0.0f},
        p0+Terminal {TUBE_WIDTH*3.0f/4.0f, 0.0f}), dashes,2, thickness);

    // Update cached bounds
    cachedBounds = juce::Rectangle<float> (center.x - TUBE_WIDTH*0.5f, center.y - TUBE_HEIGHT*0.5f, TUBE_WIDTH, TUBE_HEIGHT);

    // Labels
    Terminal labelcenter = center + Terminal {100.0f, -50.0f};
    drawLabel(g, labelcenter, getChoiceLabel());

    //Monitor
    if (getNumMonitors()>0){
        Terminal monitor = center + Terminal {100.0f, -30.0f};
        g.setColour (juce::Colours::red);
        g.drawText (juce::String(getMonitorValue(0)*1e3f, 2) + " mA",
                    center.getX() - 40, center.getY() + 2, 80, 18,
                    juce::Justification::centred, true);
    }
}
