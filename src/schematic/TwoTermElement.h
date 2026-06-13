#pragma once
#include "schematic/SchematicElement.h"

class ResistorElement : public SchematicElement, public SettableElement, public MonitoringElement
{
public:
    ResistorElement(const juce::String& name,
                    Terminal termA, Terminal termB,
                    const int paramIndex): 
        
        SchematicElement(name, std::vector<Terminal>{termA, termB}),
        SettableElement(paramIndex), MonitoringElement(std::vector<int> {})
        {prepareToDraw();};
    ResistorElement(const juce::String& name,
                    Terminal termA, Terminal termB,
                    const int paramIndex, const int currentMonitorIndex): 
        
        SchematicElement(name, std::vector<Terminal>{termA, termB}),
        SettableElement(paramIndex), 
        MonitoringElement(std::vector<int> {currentMonitorIndex})
        {prepareToDraw();};
    void draw (juce::Graphics& g) const override;

    juce::String valueToLabel (float v) override;
    float labelToValue (const juce::String s) override;
    void prepareToDraw ();
    void createSignalPath (const int signalPathMode) override;

private:
    //==========================================================================
    static constexpr int    zigzagCount    = 6;
    static constexpr float  zigzagAmplitude = 10.0f;
    static constexpr int    zigzagLength = 40.0f;

    juce::Path path;
    juce::Point<float> labelCenter;
};


class CapacitorElement : public SchematicElement, public SettableElement, public MonitoringElement
{
public:
    CapacitorElement(const juce::String& name,
                    Terminal termA, Terminal termB,
                    const int paramIndex): 
        
        SchematicElement(name, std::vector<Terminal>{termA, termB}),
        SettableElement(paramIndex), MonitoringElement(std::vector<int> {})
        {prepareToDraw ();};
    CapacitorElement(const juce::String& name,
                    Terminal termA, Terminal termB,
                    const int paramIndex, const int currentMonitorIndex): 
        
        SchematicElement(name, std::vector<Terminal>{termA, termB}),
        SettableElement(paramIndex), 
        MonitoringElement(std::vector<int> {currentMonitorIndex})
        {prepareToDraw ();};
    void draw (juce::Graphics& g) const override;
    juce::String valueToLabel (float v) override;
    float labelToValue (const juce::String s) override;
    void prepareToDraw ();
    void createSignalPath (const int signalPathMode) override;

private:
    //==========================================================================
    static constexpr int    plateWidth    = 35;
    static constexpr float  plateGap      = 10.0f;

    juce::Path path;
    juce::Point<float> labelCenter;
};


class TransformerElement : public SchematicElement, public SettableElement
{
public:
    TransformerElement(const juce::String& name,
                    Terminal termAprim, Terminal termBprim,
                    Terminal termAsec, Terminal termBsec,
                    const int paramIndex): 
        
        SchematicElement(name, std::vector<Terminal>{termAprim, termBprim, termAsec, termBsec}),
        SettableElement(paramIndex)
        {};
    void draw (juce::Graphics& g) const override;
    // juce::String valueToLabel (float v) override;
    // float labelToValue (const juce::String s) override;
private:
    //==========================================================================
    static constexpr float  coilLength    = 100.0f;
    static constexpr float  coilWidth     = 20.0f;
};