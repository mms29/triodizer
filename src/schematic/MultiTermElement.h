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
                    public MonitoringElement,
                    public InspectableElement,
                    public SignalElement
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
                    const int monitorPlusIndex,
                    const int monitorMinusIndex,
                    const int signalPathMode = SIGNALPATH_MODE_NORMAL_FORWARD,
                    SignalPath* signalPathRef = nullptr
                ): 
        
        SchematicElement(name, std::vector<Terminal>{termA, termB, termC}),
        ControllableElement(controlIndex),
        SettableElement(paramIndex),
        MonitoringElement(std::vector<int> {monitorPlusIndex,
                                            monitorMinusIndex}),
        SignalElement(signalPathRef, signalPathMode)
        {};
    void draw (juce::Graphics& g) const override;
    void controlCallback(float value, SchematicPanelListener* l) override;
    juce::String valueToLabel (float v) const override;
    float labelToValue (const juce::String s) const override;
    void prepareToDraw () override;
    void createSignalPaths () override;
    void updateSignalPaths () override;
    juce::AttributedString getInspectContent () override; 
    juce::String getInspectValue () override;

    virtual void updateArrow ();

    protected:
    juce::Path zigzag, zigzagPlus, zigzagMinus, arrow;
    Terminal labelCenter, pp0, pp1, pp2, arrowDir;    
};



class VarResElement :  public PotElement{

public:

    VarResElement(const juce::String& name,
                    Terminal termA,
                    Terminal termB,
                    const int controlIndex,
                    const int paramIndex,
                    const int monitorIndex,
                    const int signalPathMode = SIGNALPATH_MODE_NORMAL_FORWARD,
                    SignalPath* signalPathRef = nullptr): 
        
        PotElement(name, 
            termA, termB, termA, 
            controlIndex, 
            paramIndex,
            monitorIndex,
            monitorIndex,
            signalPathMode,
            signalPathRef)
        {};
    void updateArrow () override;
};
/* 
------------------------------------------------------------------------------------------------------------------------
    Transformer element
------------------------------------------------------------------------------------------------------------------------
*/

class TransformerElement : public SchematicElement,
                           public SettableElement,
                        public MonitoringElement,
                        public InspectableElement,
                        public SignalElement
{
public:
    TransformerElement (const juce::String& name,
                        Terminal termAprim,
                        Terminal termBprim,
                        Terminal termAsec,
                        Terminal termBsec,
                        const int paramIndex,
                        const int primaryMonitorIndex,
                        const int secondaryMonitorIndex,
                        const int primarySigPathMode = SIGNALPATH_MODE_NORMAL_FORWARD,
                        const int secondarySigPathMode = SIGNALPATH_MODE_NORMAL_FORWARD,
                        SignalPath* primarySigPathRef = nullptr,
                        SignalPath* secondarySigPathRef = nullptr
                )
        : SchematicElement (name, std::vector<Terminal> { termAprim, termBprim, termAsec, termBsec }),
          SettableElement (paramIndex),
          MonitoringElement (std::vector<int> {primaryMonitorIndex, secondaryMonitorIndex}),
          SignalElement(std::vector<SignalPath*>{primarySigPathRef, secondarySigPathRef},
                        std::vector<int>{primarySigPathMode, secondarySigPathMode})
          {}

    void draw (juce::Graphics& g) const override;
    void prepareToDraw () override;
    juce::String valueToLabel (float v) const override;
    float labelToValue (const juce::String s) const override;
    void createSignalPaths () override;
    void updateSignalPaths () override;
    juce::AttributedString getInspectContent () override; 
    juce::String getInspectValue () override;

private:
    juce::Point<float> labelCenter;
    juce::Path primary, secondary, gapPath;
};