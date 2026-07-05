#pragma once

#include "schematic/SchematicElement.h"

std::vector<Terminal>& getTriodeTerminals (Terminal center) ;
std::vector<Terminal>& getParallelTriodeTerminals (Terminal center) ;

//==============================================================================
/**
    TRIODE ELEMENT
 */
class TriodeElement :   public SchematicElement, 
                        public ParametrableElement,
                        public MonitoringElement,
                        public InspectableElement,
                        public SignalElement
{
public:
    TriodeElement (const juce::String& name,
                    Terminal center,
                    const int paramIndex,
                    int choiceIndex,
                    std::vector<ValueChoice> choices,
                    int monitorIndex,
                    const int signalPathMode = SIGNALPATH_MODE_NORMAL_FORWARD,
                    SignalPath* signalPathRef = nullptr,
                    juce::AttributedString descr = juce::AttributedString{}
                    )
    : SchematicElement (name , getTriodeTerminals(center)),
        ParametrableElement(paramIndex ,choiceIndex ,std::move(choices)) ,
        MonitoringElement(std::vector<int>{monitorIndex}),
        InspectableElement(std::move(descr)),
        SignalElement(signalPathRef, signalPathMode)
        {}

    TriodeElement (const juce::String& name,
                    Terminal center,
                    const int paramIndex,
                    int choiceIndex,
                    std::vector<ValueChoice> choices,
                    int monitorIndex,
                    const int signalPathMode = SIGNALPATH_MODE_NORMAL_FORWARD,
                    SignalPath* signalPathRef = nullptr,
                    SignalPath* gridSignalPathRef = nullptr,
                    juce::AttributedString descr = juce::AttributedString{}
                    )
    : SchematicElement (name , getTriodeTerminals(center)),
        ParametrableElement(paramIndex ,choiceIndex ,std::move(choices)) ,
        MonitoringElement(std::vector<int>{monitorIndex}),
        InspectableElement(std::move(descr)),
        SignalElement({signalPathRef, gridSignalPathRef}, {signalPathMode, SIGNALPATH_MODE_NORMAL_FORWARD})
        {}

    void draw (juce::Graphics& g) const override;
    void prepareToDraw () override;
    void createSignalPaths () override;
    void updateSignalPaths () override;
    void drawPower (juce::Graphics& g) const override;

    juce::AttributedString getInspectContent () override;
    juce::String getInspectValue () override;

    void addPointToTerminal(Terminal t, const int termIndex=0, const bool direction=false) override;

protected:
    juce::Path grid, gridHolder, plate, plateHolder, cathode, cathodeHolder, bulb, filament;
    std::vector<juce::Path> flow;
    juce::Point<float> labelCenter;

};

class ParallelTriodeElement : public TriodeElement
{
public:
    using TriodeElement::TriodeElement;

    void prepareToDraw () override;
    void createSignalPaths () override;

private:

    juce::Path lgrid, rgrid, lplate, rplate, lcathode, rcathode, gridConnect;
};