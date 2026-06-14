#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include "gui/Knob.h"
#include "utils/SignalPath.h"
#include "constants/SchematicConstants.h"
#include "utils/Glow.h"


using Terminal = juce::Point<float> ;

/* 
------------------------------------------------------------------------------------------------------------------------
    Base class for Elements in the schematic
------------------------------------------------------------------------------------------------------------------------
*/
class BaseElement 
{
public:
    BaseElement (std::vector<Terminal> terminals)
                 : terminals (std::move (terminals)){};

    virtual ~BaseElement() = default;

    // Accessors
    const std::vector<Terminal>& getTerminals() const noexcept;
    bool isHighlighted() const noexcept;
    virtual void setHighlighted (bool shouldBeHighlighted) noexcept;
    bool isSignalPath() const noexcept {return isSigPath;};
    virtual void setSignalPath (bool value) noexcept {isSigPath = value;};

    // Templates
    virtual void draw (juce::Graphics& g) const = 0;
    virtual void prepareToDraw () {}
    virtual bool hitTest (juce::Point<float> point) const;

    // clock helpers
    static int getClock() {return clock;}
    static void incrementClock (){clock++;}
    virtual void createSignalPath (const int signalPathMode) { setSignalPath(true);}

protected:
    bool                               highlighted      = false;
    mutable juce::Rectangle<float>     cachedBounds;
    std::vector<Terminal>              terminals;
    static inline int                  clock=0;
    bool isSigPath = false;
    std::vector<CachedPath> signalPaths;

};
class SchematicElement : public BaseElement
{
public:
    SchematicElement (const juce::String& name,
                      std::vector<Terminal> terminals)
                      : name (name), BaseElement(terminals){};

    virtual ~SchematicElement() = default;

    // Accessors
    const juce::String& getName() const noexcept;

    // Helper function to display param label
    void drawLabel(juce::Graphics& g, Terminal center, juce::String labelValue) const;

protected:
    juce::String                       name;

};

/* 
------------------------------------------------------------------------------------------------------------------------
    Listener. TODO: Move to separate file
------------------------------------------------------------------------------------------------------------------------
*/
class SchematicPanelListener
{
public:
    virtual ~SchematicPanelListener() = default;
    virtual void setCircuitParam (const int index, float newValue) = 0;
    virtual void setCircuitControl (const int index, float newValue) = 0;
    virtual float getCircuitMonitoring (const int index) = 0;
    virtual float getCircuitParam (const int index) = 0;
    virtual float getCircuitControl (const int index) = 0;
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
        monitorValues.resize(circuitIndices.size(), 0.0f);
        smoothedValues.resize(circuitIndices.size(), 0.0f);
        rmsValues.resize(circuitIndices.size(), 0.0f);
    }

    int getNumMonitors() const noexcept
    {
        return (int) circuitIndices.size();
    }

    void setMonitorValue(int monitorIndex, float v) noexcept
    {
        monitorValues[(size_t) monitorIndex] = v;
        auto s = getSmoothedValue(monitorIndex);
        smoothedValues[(size_t) monitorIndex] = smooth(v, s);
        rmsValues[(size_t) monitorIndex] = rms(v, s, getRMSValue(monitorIndex));
    }

    int getCircuitIndex(int monitorIndex) const noexcept
    {
        return circuitIndices[(size_t) monitorIndex];
    }

    float getMonitorValue(int monitorIndex) const noexcept
    {
        return monitorValues[(size_t) monitorIndex];
    }
    float getSmoothedValue(int monitorIndex) const noexcept
    {
        return smoothedValues[(size_t) monitorIndex];
    }
    float getRMSValue(int monitorIndex) const noexcept
    {
        return rmsValues[(size_t) monitorIndex];
    }

private:
    float alpha=.05f;

    float smooth(float x, float mean){
        return mean + alpha * (x - mean);
    }
    float rms(float x, float mean, float var){
        return var + alpha * (std::sqrt((x - mean)*(x - mean)) - var);
    }

    std::vector<float> monitorValues;
    std::vector<float> smoothedValues;
    std::vector<float> rmsValues;
    std::vector<int> circuitIndices;
};


/* 
------------------------------------------------------------------------------------------------------------------------
    Wire element
------------------------------------------------------------------------------------------------------------------------
*/
class WireElement : public BaseElement, public MonitoringElement
{
public:
    WireElement(std::vector<Terminal> terminals,
                    bool isSignalPath = false,
                    int monitoringIndex = -1)
            : BaseElement(terminals),
            MonitoringElement(
                monitoringIndex >= 0
                    ? std::vector<int>{monitoringIndex}
                    : std::vector<int>{})
                    
    {
        setSignalPath(isSignalPath);
    };
    void createSignalPath (const int ) override { 
        setSignalPath(true);
        CachedPath cachedWirePath {wirePath};
        cachedWirePath.rebuildCache();
        signalPaths.push_back(cachedWirePath);
    }
    void prepareToDraw () override
    {
        jassert (terminals.size() == 2);
        const auto& start = terminals[0];
        const auto& end = terminals[1];

        wirePath.startNewSubPath(start);
        wirePath.lineTo(end);

        if (isSigPath)
            createSignalPath(0);
    }

    void draw (juce::Graphics& g) const override
    {

        drawGlowPath(g, wirePath, 0.0f, getColourNormal(),getColourAmber(),false);

        for (const auto& sigpath : signalPaths)
            drawSignalPath(g, sigpath, 0.0f, getClock());
    }
private:
    juce::Path wirePath;
};

/* 
------------------------------------------------------------------------------------------------------------------------
    InspectableElement
------------------------------------------------------------------------------------------------------------------------
*/
class InspectableElement
{
public:
    InspectableElement () = default;
    virtual ~InspectableElement() = default;
    virtual void drawInspector (juce::Graphics& g) const =0;
};