#pragma once

#include "SchematicElement.h"

class ResistorElement : public SchematicElement
{
public:
    ResistorElement(const juce::String& name,
                    juce::Point<float> start,
                    juce::Point<float> end);

    void paint(juce::Graphics& g) override;

    void setPosition(juce::Point<float> newPos) override;

    void updateBounds() override;

private:
    juce::Point<float> p0;
    juce::Point<float> p1;
    juce::Point<float> centre;

    void drawZigzag(juce::Graphics& g);
};

