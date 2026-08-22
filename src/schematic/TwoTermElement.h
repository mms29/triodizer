#pragma once

#include "schematic/SchematicElement.h"

// ==============================================================================
// Two-terminal elements (Resistor, Capacitor, Inductor)
class TwoTermElement : public SchematicElement,
                        public SettableElement,
                        public MonitoringElement,
                        public InspectableElement,
                        public SignalElement
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
                     const int monitorIndex,
                    const int signalPathMode = SIGNALPATH_MODE_NORMAL_FORWARD,
                    SignalPath* signalPathRef = nullptr,
                    juce::AttributedString descr=juce::AttributedString {})
        : SchematicElement (name, std::vector<Terminal> { termA, termB }),
          SettableElement (paramIndex),
          MonitoringElement (std::vector<int> {monitorIndex }),
          InspectableElement(std::move(descr)),
          SignalElement(signalPathRef, signalPathMode)
    {
    }
    void draw (juce::Graphics& g) const override;
    void updateSignalPaths () override;
    void drawPower (juce::Graphics& g) const override;

    juce::AttributedString getInspectContent () override;
    juce::String getInspectValue () override;

    void addPointToTerminal(Terminal t, const int termIndex=0, const bool direction=false) override;


protected:
    juce::Point<float> labelCenter;
};


class ResistorElement : public TwoTermElement
{
public:
    using TwoTermElement::TwoTermElement;

    juce::String valueToLabel (float v) override;
    float labelToValue (const juce::String s) override;
    void prepareToDraw() override;
    void createSignalPaths () override;
};

class CapacitorElement : public TwoTermElement
{
public:
    using TwoTermElement::TwoTermElement;

    juce::Path posPath, negPath, posPlate, negPlate;

    juce::String valueToLabel (float v) override;
    float labelToValue (const juce::String s) override;
    void prepareToDraw() override;
    void createSignalPaths () override;

    void addPointToTerminal(Terminal t, const int termIndex=0, const bool direction=false) override;

};

// ==============================================================================
// GainElement
class GainElement : public SchematicElement,
                    public ControllableElement
{
public:
    GainElement (const juce::String& name,
                     Terminal termA,
                     Terminal termB,
                     const int controlIndex)
        : SchematicElement (name, std::vector<Terminal> { termA, termB }),
          ControllableElement (controlIndex)
    {
    }
    void draw (juce::Graphics& g) const override;
    void prepareToDraw() override;
    void controlCallback(float value, SchematicPanelListener* l) override;

protected:
    juce::Point<float> labelCenter;
    juce::Path leftPath, rightPath;

};


// ==============================================================================
// DiodeElement
class DiodeElement : public SchematicElement,
                        public ParametrableElement,
                        public MonitoringElement,
                        public SignalElement
{
public:
    DiodeElement (const juce::String& name,
                     Terminal termA,
                     Terminal termB,
                     const int paramIndex,
                    int choiceIndex,
                    std::vector<ValueChoice> choices,
                     const int monitorIndex,
                    const int signalPathMode = SIGNALPATH_MODE_NORMAL_FORWARD,
                    SignalPath* signalPathRef = nullptr
                )
        : SchematicElement (name, std::vector<Terminal> { termA, termB }),
        ParametrableElement(paramIndex ,choiceIndex ,std::move(choices)) ,
          MonitoringElement (std::vector<int> {monitorIndex }),
          SignalElement(signalPathRef, signalPathMode)
    {
    }

    void draw (juce::Graphics& g) const override;
    void prepareToDraw() override;
    void updateSignalPaths ()  override;
    void createSignalPaths () override;
protected:
    juce::Point<float> labelCenter;
    juce::Path leftPath, rightPath, barPath;

};



// ==============================================================================
// ReverbTankElement
class ReverbTankElement : public SchematicElement,
                        public MonitoringElement,
                        public SignalElement
{
public:
    ReverbTankElement (const juce::String& name,
                     Terminal termA,
                     Terminal termB,
                     const int monitorIndex,
                    const int signalPathMode = SIGNALPATH_MODE_NORMAL_FORWARD,
                    SignalPath* signalPathRef = nullptr)
        : SchematicElement (name, std::vector<Terminal> { termA, termB }),
          MonitoringElement (std::vector<int> {monitorIndex }),
          SignalElement(signalPathRef, signalPathMode)
    {
    }
    void draw (juce::Graphics& g) const override;
    void updateSignalPaths () override;
    void drawPower (juce::Graphics& g) const override;
    void prepareToDraw() override;
    void createSignalPaths () override;
protected:
    juce::Point<float> labelCenter;
    juce::Path springPath;

};
