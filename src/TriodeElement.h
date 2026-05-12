#pragma once

#include "SchematicElement.h"

//==============================================================================
/**
    TRIODE ELEMENT
 */
class TriodeElement : public SchematicElement
{
public:
    TriodeElement (const juce::String& name,
                    int paramIndex,
                    std::vector<ValueChoice> parameters,
                    Terminal center);
                    
    void draw (juce::Graphics& g) const override;

private:
    //==========================================================================
    juce::Array<juce::String> triodeValues; 
};