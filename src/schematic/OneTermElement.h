#pragma once

#include "schematic/SchematicElement.h"

inline const int JUSTIFY_LEFT = 0;
inline const int JUSTIFY_RIGHT = 1; 
inline const int JUSTIFY_TOP = 2;
inline const int JUSTIFY_BOTTOM = 3;

// ==============================================================================
// One-terminal elements (Ground, Junction, Voltage source, Voltmeter)

class GroundElement : public SchematicElement
{
public:
    GroundElement (Terminal termPosition);
    void draw (juce::Graphics& g) const override;
    void prepareToDraw () override;
};

class JunctionElement : public SchematicElement,
                        public MonitoringElement,
                        public InspectableElement
{
public:
    JunctionElement (Terminal termPosition);
    JunctionElement (Terminal termPosition,
                     const int monitorIndex);
    void draw (juce::Graphics& g) const override;
    void prepareToDraw () override;
    juce::AttributedString getInspectContent () override;
};

class JackElement : public SchematicElement
{
public:
    JackElement (juce::String name,Terminal termPosition, int justification = JUSTIFY_LEFT);
    void draw (juce::Graphics& g) const override;
    void prepareToDraw () override;
private:
    int justification;
    juce::Point<float> labelCenter;
};

class VoltageElement : public SchematicElement,
                       public SettableElement,
                       public MonitoringElement,
                       public InspectableElement,
                       public SignalElement
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
                    const int monitorIndex)
        : SchematicElement (name, std::vector<Terminal> { termPosition }),
          SettableElement (paramIndex),
          MonitoringElement (std::vector<int> { monitorIndex }) {}

    void prepareToDraw () override;
    void draw (juce::Graphics& g) const override;
    juce::String valueToLabel (float v) override;
    float labelToValue (const juce::String s) override;
    juce::AttributedString getInspectContent () override;
    juce::String getInspectValue () override {return label;};
    void drawPower (juce::Graphics& g) const override;
    void createSignalPaths () override;
    void updateSignalPaths () override;

private:
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