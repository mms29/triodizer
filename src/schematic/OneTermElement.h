#pragma once

#include "schematic/SchematicElement.h"

class GroundElement : public SchematicElement
{
public:
    GroundElement (Terminal termPosition);
    void draw (juce::Graphics& g) const override;
};

class JunctionElement : public SchematicElement
{
public:
    JunctionElement (Terminal termPosition);
    void draw (juce::Graphics& g) const override;
};

class VoltageElement : public SchematicElement,
                            public ParametrableElement
{
public:
    VoltageElement(const juce::String& name,
                    std::vector<Terminal> terminals,
                    const int paramIndex,
                    int choiceIndex,
                    std::vector<ValueChoice> choices): 
        
        SchematicElement(name, terminals),
        ParametrableElement(paramIndex, choiceIndex, choices)
        {};
    void draw (juce::Graphics& g) const override;
};


class VoltmeterElement : 
    public SchematicElement,
    public MonitoringElement
{
public:
    VoltmeterElement(const juce::String& name,
                    Terminal terminal,
                    const int monitorIndex): 
        
        SchematicElement(name, std::vector<Terminal> {terminal}),
        MonitoringElement(monitorIndex)
        {}; 
    void draw (juce::Graphics& g) const override;
    bool hitTest (juce::Point<float> point) const override;

private:
    static constexpr float METER_RADIUS = 18.0f;
};