#pragma once
#include "SchematicElement.h"

class ResistorElement : public SchematicElement
{
public:
    using SchematicElement::SchematicElement;
    void draw (juce::Graphics& g) const override;

private:
    //==========================================================================
    static constexpr int    zigzagCount    = 6;
    static constexpr float  zigzagAmplitude = 10.0f;
    static constexpr int    zigzagLength = 40.0f;
};


class CapacitorElement : public SchematicElement
{
public:
    using SchematicElement::SchematicElement;
    void draw (juce::Graphics& g) const override;
private:
    //==========================================================================
    static constexpr int    plateWidth    = 35;
    static constexpr float  plateGap      = 10.0f;
};