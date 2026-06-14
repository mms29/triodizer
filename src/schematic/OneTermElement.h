#pragma once

#include "schematic/SchematicElement.h"

// ==============================================================================
// One-terminal elements (Ground, Junction, Voltage source, Voltmeter)

class GroundElement : public SchematicElement
{
public:
    GroundElement (Terminal termPosition);
    void draw (juce::Graphics& g) const override;
    void prepareToDraw () override;

private:
    juce::Path groundPath;
};

class JunctionElement : public SchematicElement
{
public:
    JunctionElement (Terminal termPosition);
    void draw (juce::Graphics& g) const override;
};

class VoltageElement : public SchematicElement,
                       public SettableElement,
                       public MonitoringElement
{
public:
    VoltageElement (const juce::String& name,
                    Terminal termPosition,
                    const int paramIndex)
        : SchematicElement (name, std::vector<Terminal> { termPosition }),
          SettableElement (paramIndex),
          MonitoringElement (std::vector<int> {}) {}

    VoltageElement (const juce::String& name,
                    Terminal termPosition,
                    const int paramIndex,
                    const int currentMonitorIndex)
        : SchematicElement (name, std::vector<Terminal> { termPosition }),
          SettableElement (paramIndex),
          MonitoringElement (std::vector<int> { currentMonitorIndex }) {}

    void prepareToDraw () override;
    void draw (juce::Graphics& g) const override;
    juce::String valueToLabel (float v) override;
    float labelToValue (const juce::String s) override;
private:

    juce::Path votlagePath;
    Terminal labelCenter;
};

class VoltmeterElement : public SchematicElement,
                         public MonitoringElement
{
public:
    VoltmeterElement (const juce::String& name,
                     Terminal terminal,
                     const int monitorIndex)
        : SchematicElement (name, std::vector<Terminal> { terminal }),
          MonitoringElement (std::vector<int> { monitorIndex }) {}

    void draw (juce::Graphics& g) const override;
    bool hitTest (juce::Point<float> point) const override;
};