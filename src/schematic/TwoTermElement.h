#pragma once

#include "schematic/SchematicElement.h"

// ==============================================================================
// Two-terminal elements (Resistor, Capacitor, Inductor)
class TwoTermElement : public SchematicElement,
                        public SettableElement,
                        public MonitoringElement,
                        public InspectableElement
{
public:
    TwoTermElement (const juce::String& name,
                     Terminal termA,
                     Terminal termB,
                     const int paramIndex)
        : SchematicElement (name, std::vector<Terminal> { termA, termB }),
          SettableElement (paramIndex),
          MonitoringElement (std::vector<int> {}),
          InspectableElement(juce::AttributedString {})
    {
    }

    TwoTermElement (const juce::String& name,
                     Terminal termA,
                     Terminal termB,
                     const int paramIndex,
                     const int voltageMonitorIndex,
                     const int currentMonitorIndex,
                    juce::AttributedString descr)
        : SchematicElement (name, std::vector<Terminal> { termA, termB }),
          SettableElement (paramIndex),
          MonitoringElement (std::vector<int> {voltageMonitorIndex,  currentMonitorIndex }),
          InspectableElement(std::move(descr))
    {
    }
    void draw (juce::Graphics& g) const override;
    void updateSignalPath () override;

    juce::AttributedString getInspectContent () override;
    juce::String getInspectValue () override;


protected:
    juce::Path path;
    juce::Point<float> labelCenter;
};


class ResistorElement : public TwoTermElement
{
public:
    using TwoTermElement::TwoTermElement;

    juce::String valueToLabel (float v) override;
    float labelToValue (const juce::String s) override;
    void prepareToDraw() override;
    void createSignalPath (const int signalPathMode) override;
};

class CapacitorElement : public TwoTermElement
{
public:
    using TwoTermElement::TwoTermElement;

    juce::String valueToLabel (float v) override;
    float labelToValue (const juce::String s) override;
    void prepareToDraw() override;
    void createSignalPath (const int signalPathMode) override;
};
