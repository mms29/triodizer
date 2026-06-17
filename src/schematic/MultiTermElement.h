#pragma once

#include "schematic/SchematicElement.h"


/* 
------------------------------------------------------------------------------------------------------------------------
    Potentiometer element
------------------------------------------------------------------------------------------------------------------------
*/
class PotElement :  public SchematicElement, 
                    public ControllableElement, 
                    public SettableElement,
                    public MonitoringElement
{
public:
    PotElement(const juce::String& name,
                    Terminal termA,
                    Terminal termB,
                    Terminal termC,
                    const int controlIndex,
                    const int paramIndex): 
        
        SchematicElement(name, std::vector<Terminal>{termA, termB, termC}),
        ControllableElement(controlIndex),
        SettableElement(paramIndex),
        MonitoringElement(std::vector<int> {})
        {};

    PotElement(const juce::String& name,
                    Terminal termA,
                    Terminal termB,
                    Terminal termC,
                    const int controlIndex,
                    const int paramIndex,
                    const int voltagePlusMonitorIndex,
                    const int currentPlusMonitorIndex,
                    const int voltageMinusMonitorIndex,
                    const int currentMinusMonitorIndex): 
        
        SchematicElement(name, std::vector<Terminal>{termA, termB, termC}),
        ControllableElement(controlIndex),
        SettableElement(paramIndex),
        MonitoringElement(std::vector<int> {voltagePlusMonitorIndex,
                                            currentPlusMonitorIndex,
                                            voltageMinusMonitorIndex,
                                            currentMinusMonitorIndex})
        {};
    void draw (juce::Graphics& g) const override;
    void controlCallback(float value, SchematicPanelListener* l) override;
    juce::String valueToLabel (float v) override;
    float labelToValue (const juce::String s) override;
    void prepareToDraw () override;
    void createSignalPath (const int signalPathMode) override;
    void updateSignalPath () override;

private:
    juce::Path zigzag;
    Terminal labelCenter, pp0, pp1, pp2, arrowDir;    
};


/* 
------------------------------------------------------------------------------------------------------------------------
    Transformer element
------------------------------------------------------------------------------------------------------------------------
*/

class TransformerElement : public SchematicElement,
                           public SettableElement
{
public:
    TransformerElement (const juce::String& name,
                        Terminal termAprim,
                        Terminal termBprim,
                        Terminal termAsec,
                        Terminal termBsec,
                        const int paramIndex)
        : SchematicElement (name, std::vector<Terminal> { termAprim, termBprim, termAsec, termBsec }),
          SettableElement (paramIndex) {}

    void draw (juce::Graphics& g) const override;
    void prepareToDraw () override;

private:
};