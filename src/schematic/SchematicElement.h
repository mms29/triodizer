#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include "gui/Knob.h"
#include "utils/SignalPath.h"
#include "constants/SchematicConstants.h"
#include "utils/Glow.h"
#include "schematic/SchematicPanelListener.h"
#include "utils/Format.h"
#include "dsp/Circuit.h"


using Terminal = juce::Point<float> ;


/* 
------------------------------------------------------------------------------------------------------------------------
    Signl Path Element 
------------------------------------------------------------------------------------------------------------------------
*/
class SignalElement 
{
public:
    SignalElement (std::vector<SignalPath*> signalPathRefs, const std::vector<int> signalPathModes){
        jassert(signalPathRefs.size() == signalPathModes.size());
        for (size_t i=0; i<signalPathRefs.size(); i++){
            SignalPath signalPath;
            signalPath.setRef(signalPathRefs[i]);
            signalPath.setMode(signalPathModes[i]);
            signalPaths.push_back(signalPath);
        }
    };
    SignalElement (SignalPath* signalPathRef = nullptr, const int signalPathMode= SIGNALPATH_MODE_NORMAL_FORWARD){
        SignalPath signalPath;
        signalPath.setRef(signalPathRef);
        signalPath.setMode(signalPathMode);
        signalPaths.push_back(signalPath);
    };
    ~SignalElement() = default;

    virtual void updateSignalPaths (){}
    virtual void createSignalPaths (){}

    // Sig path
    SignalPath* getSignalPathPtr (const int index =0 ) noexcept {return &signalPaths[index];}
    int getNumSignals () const noexcept {return signalPaths.size();}

protected:
    std::vector<SignalPath> signalPaths;
};

/* 
------------------------------------------------------------------------------------------------------------------------
    Base class for Elements in the schematic
------------------------------------------------------------------------------------------------------------------------
*/
class SchematicElement 
{
public:
    SchematicElement (const juce::String& name,
                        std::vector<Terminal> terminals)
                 : name (name), terminals (std::move (terminals))
                {};

    virtual ~SchematicElement() = default;

    // Accessors
    const std::vector<Terminal>& getTerminals() const noexcept;
    const juce::Path& getPath() const noexcept { return path;}
    bool isHighlighted() const noexcept;
    virtual void setHighlighted (bool shouldBeHighlighted) noexcept;
    const juce::String& getName() const noexcept;

    // Helper function to display param label
    void drawLabel(juce::Graphics& g, Terminal center, juce::String labelValue) const;

    // Templates
    virtual void draw (juce::Graphics& g) const = 0;
    virtual void prepareToDraw () {}
    virtual bool hitTest (juce::Point<float> point) const;

    SignalPath* getSignalPath(const int index =0) noexcept
    {
        if (auto* s = dynamic_cast<SignalElement*>(this))
            return s->getSignalPathPtr(index);

        return nullptr;
    }
    virtual void addPointToTerminal(Terminal t, const int termIndex=0, const bool direction=false);


protected:
    juce::String                       name;
    bool                               highlighted      = false;
    mutable juce::Rectangle<float>     cachedBounds;
    std::vector<Terminal>              terminals;
    juce::Path path;

};


/* 
------------------------------------------------------------------------------------------------------------------------
    Controlable Element with a Knob 
------------------------------------------------------------------------------------------------------------------------
*/
class ControllableElement 
{
public:
    ControllableElement (const int controlIndex): controlIndex(controlIndex) {};
    virtual ~ControllableElement() = default;
    virtual void controlCallback(float value, SchematicPanelListener* l){}
    virtual int getControlIndex(){return controlIndex;}

protected:
    const int controlIndex;
    float controlValue = 50.0f;
};


/* 
------------------------------------------------------------------------------------------------------------------------
    Parametrable element with a popup menu 
------------------------------------------------------------------------------------------------------------------------
*/

struct ValueChoice
{
    juce::String label;
    float value;
};
class ParametrableElement
{
public:
    ParametrableElement (const int paramIndex,
                        int choiceIndex,
                        std::vector<ValueChoice> choices): 
        paramIndex(paramIndex), 
        choiceIndex(choiceIndex), 
        choices(std::move(choices))
        {};
    virtual ~ParametrableElement() = default;

    // Accessors Param
    const int getParamIndex() const {return paramIndex;};

    // Accessor Choice popup menu
    juce::String getChoiceLabel() const;
    float getChoiceValue() const;
    std::vector<ValueChoice> getChoices() const {return choices;};
    int getChoiceIndex() const noexcept {return choiceIndex;};
    void setChoiceIndex (int newIndex) noexcept {choiceIndex = newIndex;};
    void addChoice (const float value, const juce::String label) noexcept {choices.push_back(ValueChoice{label, value}) ;}

    const int getIndexChoiceFromValue(const float value) const{

        int index = 0;
        for (const auto& choice : choices){
            if (nearlyEqual(choice.value, value)) 
                return index;
            index++;
        }
        return -1;
    }

protected:
    const int paramIndex;
    int choiceIndex ;    
    std::vector<ValueChoice> choices;

private:

    bool nearlyEqual(float a, float b) const
    {
        float diff = std::abs(a - b);

        float absA = std::abs(a);
        float absB = std::abs(b);

        float largest = std::max(absA, absB);

        return diff <= std::max(1e-6f * largest, 1e-12f);
    }

};


/* 
------------------------------------------------------------------------------------------------------------------------
    Settable Element
------------------------------------------------------------------------------------------------------------------------
*/
class SettableElement
{
public:
    SettableElement (const int paramIndex): 
        paramIndex(paramIndex)
        {};
    virtual ~SettableElement() = default;

    // Accessors
    const int getParamIndex() const {return paramIndex;};
    void setValue(const float v) {
        value=v;
        label = valueToLabel(v);
    }
    void setLabel(const juce::String s) {
        setValue(labelToValue(s));
    }
    juce::String getLabel() const {return label;}
    float getValue() const {return value;}

    virtual float labelToValue(const juce::String s) { return s.getFloatValue();}
    virtual juce::String valueToLabel(const float v) {return juce::String(v);}

protected:
    const int paramIndex;
    juce::String label = "";
    float value = 0.0f;
};
/* 
------------------------------------------------------------------------------------------------------------------------
    Monitoring element
------------------------------------------------------------------------------------------------------------------------
*/
class MonitoringElement
{
public:
    explicit MonitoringElement(std::vector<int> indices)
        : circuitIndices(std::move(indices))
    {
        monitorValues.resize(circuitIndices.size());
        smoothedValues.resize(circuitIndices.size());
        rmsValues.resize(circuitIndices.size());
    }

    int getNumMonitors() const noexcept
    {
        return (int) circuitIndices.size();
    }

    void setMonitorValue(int monitorIndex, MonitorValuef v) noexcept
    {
        monitorValues[(size_t) monitorIndex] = v;
        for (int i=0; i<v.size; i++){
            auto s = getSmoothedValue(monitorIndex, i);
            smoothedValues[(size_t) monitorIndex].values[i] = smooth(v.values[i], s);
            rmsValues[(size_t) monitorIndex].values[i] = rms(v.values[i], s, getRMSValue(monitorIndex, i));
        }
    }

    int getCircuitIndex(int monitorIndex) const noexcept
    {
        return circuitIndices[(size_t) monitorIndex];
    }

    float getMonitorValue(int monitorIndex, int valueIndex) const noexcept
    {
        return monitorValues[(size_t) monitorIndex].values[valueIndex];
    }
    float getSmoothedValue(int monitorIndex, int valueIndex) const noexcept
    {
        return smoothedValues[(size_t) monitorIndex].values[valueIndex];
    }
    float getRMSValue(int monitorIndex, int valueIndex) const noexcept
    {
        return rmsValues[(size_t) monitorIndex].values[valueIndex];
    }
    virtual void drawPower (juce::Graphics& g) const {};

private:
    static float smooth(float x, float mean) noexcept{
        return mean + MONITORING_SMOOTHING_ALPHA * (x - mean);
    }
    static float rms(float x, float mean, float var) noexcept{
        return var + MONITORING_SMOOTHING_ALPHA * (std::sqrt((x - mean)*(x - mean)) - var);
    }

    std::vector<MonitorValuef> monitorValues;
    std::vector<MonitorValuef> smoothedValues;
    std::vector<MonitorValuef> rmsValues;
    std::vector<int> circuitIndices;
};


/* 
------------------------------------------------------------------------------------------------------------------------
    Wire element
------------------------------------------------------------------------------------------------------------------------
*/
class WireElement : public SchematicElement, 
                    public MonitoringElement,
                    public SignalElement
{
public:
    WireElement(std::vector<Terminal> terminals)
            : SchematicElement("WIRE", terminals),
            MonitoringElement(std::vector<int>{}){}

    WireElement(std::vector<Terminal> terminals,
                     const int monitorIndex,
                    const int signalPathMode = SIGNALPATH_MODE_NORMAL_FORWARD,
                    SignalPath* signalPathRef = nullptr)
            : SchematicElement("WIRE", terminals),
            MonitoringElement(std::vector<int>{monitorIndex}),
            SignalElement(signalPathRef, signalPathMode)
    {
    };
    void createSignalPaths () override;
    void updateSignalPaths () override;
    void prepareToDraw () override;
    void draw (juce::Graphics& g) const override;
    void drawPower (juce::Graphics& g) const override;

};

/* 
------------------------------------------------------------------------------------------------------------------------
    InspectableElement
------------------------------------------------------------------------------------------------------------------------
*/
class InspectableElement
{
public:
    InspectableElement (juce::AttributedString descr=juce::AttributedString {}) : description(std::move(descr)){};
    virtual ~InspectableElement() = default;
    virtual juce::AttributedString getInspectContent () =0;
    virtual juce::AttributedString getInspectDescr () {return description;};
    virtual juce::String getInspectValue () {return value;};

protected:
    juce::AttributedString description;
    juce::String value;
};