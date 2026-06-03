#pragma once

#include "schematic/SchematicElement.h"


const float TUBE_WIDTH = 180.0f;
const float TUBE_HEIGHT = 200.0f;

//==============================================================================
/**
    TRIODE ELEMENT
 */
class TriodeElement :   public SchematicElement, 
                        public ParametrableElement,
                        public MonitoringElement
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
                        juce::Point<float>{center.x, center.y + TUBE_HEIGHT/2}}),
        ParametrableElement(paramIndex ,choiceIndex ,std::move(choices)) ,
        MonitoringElement(std::vector<int>{currentMonitor}) {}

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
        MonitoringElement(std::vector<int>{}) {}

                    
    void draw (juce::Graphics& g) const override;

private:
    //==========================================================================
    juce::Array<juce::String> triodeValues; 
};