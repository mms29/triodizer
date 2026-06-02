#pragma once
#include "schematic/SchematicElement.h"

class ResistorElement : public SchematicElement, public SettableElement
{
public:
    ResistorElement(const juce::String& name,
                    Terminal termA, Terminal termB,
                    const int paramIndex): 
        
        SchematicElement(name, std::vector<Terminal>{termA, termB}),
        SettableElement(paramIndex)
        {};
    void draw (juce::Graphics& g) const override;

    juce::String valueToLabel (float v) override;
    float labelToValue (const juce::String s) override;


private:
    //==========================================================================
    static constexpr int    zigzagCount    = 6;
    static constexpr float  zigzagAmplitude = 10.0f;
    static constexpr int    zigzagLength = 40.0f;
};


class CapacitorElement : public SchematicElement, public SettableElement
{
public:
    CapacitorElement(const juce::String& name,
                    Terminal termA, Terminal termB,
                    const int paramIndex): 
        
        SchematicElement(name, std::vector<Terminal>{termA, termB}),
        SettableElement(paramIndex)
        {};
    void draw (juce::Graphics& g) const override;
    juce::String valueToLabel (float v) override;
    float labelToValue (const juce::String s) override;
private:
    //==========================================================================
    static constexpr int    plateWidth    = 35;
    static constexpr float  plateGap      = 10.0f;
};