#pragma once

#include "schematic/SchematicElement.h"

// ==============================================================================
// Two-terminal elements (Resistor, Capacitor, Transformer)

class ResistorElement : public SchematicElement,
                        public SettableElement,
                        public MonitoringElement
{
public:
    ResistorElement (const juce::String& name,
                     Terminal termA,
                     Terminal termB,
                     const int paramIndex)
        : SchematicElement (name, std::vector<Terminal> { termA, termB }),
          SettableElement (paramIndex),
          MonitoringElement (std::vector<int> {})
    {
    }

    ResistorElement (const juce::String& name,
                     Terminal termA,
                     Terminal termB,
                     const int paramIndex,
                     const int currentMonitorIndex)
        : SchematicElement (name, std::vector<Terminal> { termA, termB }),
          SettableElement (paramIndex),
          MonitoringElement (std::vector<int> { currentMonitorIndex })
    {
    }

    void draw (juce::Graphics& g) const override;

    juce::String valueToLabel (float v) override;
    float labelToValue (const juce::String s) override;

    void prepareToDraw() override;
    void createSignalPath (const int signalPathMode) override;

private:
    juce::Path path;
    juce::Point<float> labelCenter;
};

class CapacitorElement : public SchematicElement,
                         public SettableElement,
                         public MonitoringElement
{
public:
    CapacitorElement (const juce::String& name,
                       Terminal termA,
                       Terminal termB,
                       const int paramIndex)
        : SchematicElement (name, std::vector<Terminal> { termA, termB }),
          SettableElement (paramIndex),
          MonitoringElement (std::vector<int> {})
    {
    }

    CapacitorElement (const juce::String& name,
                       Terminal termA,
                       Terminal termB,
                       const int paramIndex,
                       const int currentMonitorIndex)
        : SchematicElement (name, std::vector<Terminal> { termA, termB }),
          SettableElement (paramIndex),
          MonitoringElement (std::vector<int> { currentMonitorIndex })
    {
    }

    void draw (juce::Graphics& g) const override;

    juce::String valueToLabel (float v) override;
    float labelToValue (const juce::String s) override;

    void prepareToDraw() override;
    void createSignalPath (const int signalPathMode) override;

private:
    juce::Path path;
    juce::Point<float> labelCenter;
};
