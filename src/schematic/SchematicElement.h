#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include <functional>
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
    ~SignalElement()
    {
    }

    virtual void updateSignalPaths (){}
    virtual void createSignalPaths (){}

    // Sig path
    SignalPath* getSignalPathPtr (const int index =0 ) noexcept {return &signalPaths[index];}
    int getNumSignals () const noexcept {return signalPaths.size();}

    void clearSignalReferences ();

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
    void drawLabel(juce::Graphics& g, Terminal center, juce::String labelValue, const int width = 80) const;

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
    (single choice param - superseded by MultiParamElement, kept until migration)
------------------------------------------------------------------------------------------------------------------------
*/

inline bool nearlyEqual (float a, float b) noexcept
{
    const float diff = std::abs (a - b);
    const float largest = std::max (std::abs (a), std::abs (b));
    return diff <= std::max (1e-6f * largest, 1e-12f);
}

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

};


/*
------------------------------------------------------------------------------------------------------------------------
    Multi-param element: a list of typed parameters (float values and/or choices)
------------------------------------------------------------------------------------------------------------------------
*/

enum class ParamType
{
    Float,
    Choice
};

struct ElementParam
{
    int paramIndex = -1;              // circuit param index; < 0 = UI-only, skipped by sync & commit
    juce::String id;                  // name shown in menus, e.g. "Feedback"
    ParamType type = ParamType::Float;

    // Float param
    float value = 0.0f;
    std::function<juce::String (float)> valueToLabel;           // empty -> plain number
    std::function<float (const juce::String&)> labelToValue;    // empty -> s.getFloatValue()

    // Choice param
    std::vector<ValueChoice> choices;
    int choiceIndex = 0;

    // Formats an arbitrary value with this param's formatter (plain number when unset)
    juce::String format (float v) const;

    // Current display label
    juce::String getLabel() const;

    // Current value (Float -> value, Choice -> selected choice's value, 0.0f when out of range)
    float getValue() const;

    // Set from a raw circuit value (Choice maps through the choice list, keeps current when no match)
    void setValue (float v);

    // Set from an edited label; returns false and keeps the current value on empty input
    bool setFromLabel (const juce::String& s);

    void setChoiceIndex (int newIndex) noexcept { choiceIndex = newIndex; }
    int getChoiceIndex() const noexcept { return choiceIndex; }
    void addChoice (const float v, const juce::String label) noexcept { choices.push_back (ValueChoice { label, v }); }
    const std::vector<ValueChoice>& getChoices() const noexcept { return choices; }

    // Index of the choice matching value (-1 when none)
    int getIndexChoiceFromValue (const float v) const;
};

class MultiParamElement
{
public:
    virtual ~MultiParamElement() = default;

    // The returned reference points into the param vector: configure it immediately,
    // never hold it across add* calls (the vector may reallocate).
    ElementParam& addFloatParam (int paramIndex, juce::String id,
                                 std::function<juce::String (float)> v2l = {},
                                 std::function<float (const juce::String&)> l2v = {})
    {
        ElementParam p;
        p.type = ParamType::Float;
        p.paramIndex = paramIndex;
        p.id = std::move (id);
        p.valueToLabel = std::move (v2l);
        p.labelToValue = std::move (l2v);
        params.push_back (std::move (p));
        return params.back();
    }

    ElementParam& addChoiceParam (int paramIndex, juce::String id,
                                  std::vector<ValueChoice> newChoices,
                                  int initialChoiceIndex = 0)
    {
        ElementParam p;
        p.type = ParamType::Choice;
        p.paramIndex = paramIndex;
        p.id = std::move (id);
        p.choices = std::move (newChoices);
        p.choiceIndex = initialChoiceIndex;
        params.push_back (std::move (p));
        return params.back();
    }

    int getNumParams() const noexcept { return (int) params.size(); }

    ElementParam& getParam (int index) noexcept { return params[(size_t) index]; }
    const ElementParam& getParam (int index) const noexcept { return params[(size_t) index]; }

protected:
    std::vector<ElementParam> params;
};


/*
------------------------------------------------------------------------------------------------------------------------
    Settable Element
    (single float param - superseded by MultiParamElement, kept until migration)
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

    virtual float labelToValue(const juce::String s) const { return s.getFloatValue();}
    virtual juce::String valueToLabel(const float v) const {return juce::String(v);}

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