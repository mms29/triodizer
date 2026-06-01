#pragma once
#include "schematic/SchematicElement.h"

class ResistorElement : public SchematicElement, public ParametrableElement
{
public:
    ResistorElement(const juce::String& name,
                    std::vector<Terminal> terminals,
                    const int paramIndex,
                    int choiceIndex,
                    std::vector<ValueChoice> choices): 
        
        SchematicElement(name, terminals),
        ParametrableElement(paramIndex, choiceIndex, choices)
        {};
    void draw (juce::Graphics& g) const override;

private:
    //==========================================================================
    static constexpr int    zigzagCount    = 6;
    static constexpr float  zigzagAmplitude = 10.0f;
    static constexpr int    zigzagLength = 40.0f;
};


class CapacitorElement : public SchematicElement, public ParametrableElement
{
public:
    CapacitorElement(const juce::String& name,
                    std::vector<Terminal> terminals,
                    const int paramIndex,
                    int choiceIndex,
                    std::vector<ValueChoice> choices): 
        
        SchematicElement(name, terminals),
        ParametrableElement(paramIndex, choiceIndex, choices)
        {};
    void draw (juce::Graphics& g) const override;
private:
    //==========================================================================
    static constexpr int    plateWidth    = 35;
    static constexpr float  plateGap      = 10.0f;
};