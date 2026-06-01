#pragma once

#include "schematic/SchematicElement.h"


const float TUBE_WIDTH = 180.0f;
const float TUBE_HEIGHT = 200.0f;

//==============================================================================
/**
    TRIODE ELEMENT
 */
class TriodeElement : public SchematicElement, public ParametrableElement
{
public:
    TriodeElement (const juce::String& name,
                    Terminal center,
                    const int paramIndex,
                    int choiceIndex,
                    std::vector<ValueChoice> choices 
                    )
    : SchematicElement (name ,
      std::vector<Terminal>{
            juce::Point<float>{center.x - TUBE_WIDTH/2, center.y},
            juce::Point<float>{center.x, center.y - TUBE_HEIGHT/2},
            juce::Point<float>{center.x, center.y + TUBE_HEIGHT/2}
        }),
        ParametrableElement(
     paramIndex ,
      choiceIndex ,
      std::move(choices) )
{
}

                    
    void draw (juce::Graphics& g) const override;

private:
    //==========================================================================
    juce::Array<juce::String> triodeValues; 
};