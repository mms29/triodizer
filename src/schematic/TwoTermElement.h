#pragma once
#include "schematic/SchematicElement.h"

class ResistorElement : public SchematicElement, public SettableElement
{
public:
    ResistorElement(const juce::String& name,
                    std::vector<Terminal> terminals,
                    const int paramIndex): 
        
        SchematicElement(name, terminals),
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
                    std::vector<Terminal> terminals,
                    const int paramIndex): 
        
        SchematicElement(name, terminals),
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