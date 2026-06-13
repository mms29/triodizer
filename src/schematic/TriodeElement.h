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
                    int currentMonitor,
                    int gridVoltageMonitor,
                    int cathodeVoltageMonitor,
                    int plateVoltageMonitor
                    )
    : SchematicElement (name , std::vector<Terminal>{
                        juce::Point<float>{center.x - TUBE_WIDTH/2, center.y},
                        juce::Point<float>{center.x, center.y - TUBE_HEIGHT/2},
                        juce::Point<float>{center.x - TUBE_WIDTH/4, center.y + TUBE_HEIGHT/2}}),
        ParametrableElement(paramIndex ,choiceIndex ,std::move(choices)) ,
        MonitoringElement(std::vector<int>{currentMonitor, gridVoltageMonitor, cathodeVoltageMonitor, plateVoltageMonitor}) 
        {prepareToDraw () ;}

    TriodeElement (const juce::String& name,
                    Terminal center,
                    const int paramIndex,
                    int choiceIndex,
                    std::vector<ValueChoice> choices,
                    int currentMonitor)
    : SchematicElement (name , std::vector<Terminal>{
                        juce::Point<float>{center.x - TUBE_WIDTH/2, center.y},
                        juce::Point<float>{center.x, center.y - TUBE_HEIGHT/2},
                        juce::Point<float>{center.x - TUBE_WIDTH/4, center.y + TUBE_HEIGHT/2}}),
        ParametrableElement(paramIndex ,choiceIndex ,std::move(choices)) ,
        MonitoringElement(std::vector<int>{currentMonitor}) {prepareToDraw () ;}

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
        MonitoringElement(std::vector<int>{}) {prepareToDraw () ;}

                    
    void draw (juce::Graphics& g) const override;
    void prepareToDraw () ;
    void drawInspector (juce::Graphics& g) const override;
    void createSignalPath (const int signalPathMode) override;

private:
    juce::Path grid, gridHolder, plate, plateHolder, cathode, cathodeHolder, bulb, filament;
    std::vector<juce::Path> flow, reverseflow;
    juce::Point<float> labelCenter;

};