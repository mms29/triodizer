#pragma once

#include "schematic/SchematicElement.h"


const float TUBE_WIDTH = 150.0f;
const float TUBE_HEIGHT = 200.0f;

//==============================================================================
/**
    TRIODE ELEMENT
 */
class TriodeElement :   public SchematicElement, 
                        public ParametrableElement,
                        public MonitoringElement,
                        public InspectableElement
{
public:
    TriodeElement (const juce::String& name,
                    Terminal center,
                    const int paramIndex,
                    int choiceIndex,
                    std::vector<ValueChoice> choices,
                    int currentMonitor
                    // int plateDCMonitor,
                    // int plateACMonitor,
                    // int cathodeDCMonitor,
                    // int cathodeACMonitor
                    )
    : SchematicElement (name , std::vector<Terminal>{
                        juce::Point<float>{center.x - TUBE_WIDTH/2, center.y},
                        juce::Point<float>{center.x, center.y - TUBE_HEIGHT/2},
                        juce::Point<float>{center.x - TUBE_WIDTH/4, center.y + TUBE_HEIGHT/2}}),
        ParametrableElement(paramIndex ,choiceIndex ,std::move(choices)) ,
        MonitoringElement(std::vector<int>{currentMonitor}) {
{
}
        }

    TriodeElement (const juce::String& name,
                    Terminal center,
                    const int paramIndex,
                    int choiceIndex,
                    std::vector<ValueChoice> choices
                    )
    : SchematicElement (name , std::vector<Terminal>{
                        juce::Point<float>{center.x - TUBE_WIDTH/2, center.y},
                        juce::Point<float>{center.x, center.y - TUBE_HEIGHT/2},
                        juce::Point<float>{center.x, center.y + TUBE_HEIGHT/2}}),
        ParametrableElement(paramIndex ,choiceIndex ,std::move(choices)) ,
        MonitoringElement(std::vector<int>{}) {
        }

                    
    void draw (juce::Graphics& g) const override;

private:
    //==========================================================================
    juce::Array<juce::String> triodeValues; 

    juce::Colour warmColour (float t) const
    {
        t = juce::jlimit (0.0f, 1.0f, t);

        juce::ColourGradient g;
        g.addColour (0.00, juce::Colours::white);
        g.addColour (0.25, juce::Colour (255, 255, 180)); // pale yellow
        g.addColour (0.50, juce::Colours::yellow);
        g.addColour (0.75, juce::Colour (255, 165, 0));   // orange
        g.addColour (1.00, juce::Colours::red);

        return g.getColourAtPosition (t);
    };
};