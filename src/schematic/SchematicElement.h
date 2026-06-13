#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include "gui/Knob.h"
    
const float STROKE_NORMAL = 2.5f;
const float STROKE_HIGHLIGHT = 5.0f;

const juce::Colour COLOR_BACKGROUND = juce::Colour(10, 5, 0);
const juce::Colour COLOR_NORMAL = juce::Colour(195, 176, 123);
const juce::Colour COLOR_HIGHLIGHT = juce::Colour(252, 237, 173);
const juce::Colour COLOR_AMBER = juce::Colour(255, 166, 38);
const juce::Colour COLOR_ELECTRICAL = juce::Colour(80, 180, 255);
const juce::Colour COLOR_PURPLE = juce::Colour(180, 110, 255);
const juce::Colour COLOR_HOTRED = juce::Colour(255, 110, 60);

const juce::Font FONT_TITLE = juce::FontOptions(22.0f);
const juce::Font FONT_SUB1 = juce::FontOptions(18.0f);
const juce::Font FONT_SUB2 = juce::FontOptions(14.0f);

const float POWER_SCALING = 1.0e3f;
struct ValueChoice
{
    juce::String label;
    float value;
};

using Terminal = juce::Point<float> ;


void drawGlowPath(juce::Graphics& g,
                  const juce::Path& path,
                  float intensity, 
                  juce::Colour coreColour, 
                  juce::Colour glowColour, 
                  bool highlight
);
juce::Colour getSignalColour (float t);
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
                      : name (name), terminals (std::move (terminals)){};

    virtual ~SchematicElement() = default;

    // Accessors
    const juce::String& getName() const noexcept;
    const std::vector<Terminal>& getTerminals() const noexcept;
    bool isHighlighted() const noexcept;
    virtual void setHighlighted (bool shouldBeHighlighted) noexcept;

    // Templates
    virtual void draw (juce::Graphics& g) const = 0;
    virtual bool hitTest (juce::Point<float> point) const;

    // Helper function to display param label
    void drawLabel(juce::Graphics& g, Terminal center, juce::String labelValue) const;


protected:
    bool                               highlighted      = false;
    mutable juce::Rectangle<float>     cachedBounds;
    juce::String                       name;
    std::vector<Terminal>              terminals;
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
    }

    int getNumMonitors() const noexcept
    {
        return (int) circuitIndices.size();
    }

    void setMonitorValue(int monitorIndex, float v) noexcept
    {
        monitorValues[(size_t) monitorIndex] = v;
        smoothedValues[(size_t) monitorIndex] = lowPass(v, getSmoothedValue(monitorIndex));
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

private:
    float alpha=.05f;

    float lowPass(float x, float mean){
        return mean + alpha * (x - mean);
    }
    float variance(float x, float mean, float var){
        return var + alpha * (std::sqrt((x - mean)*(x - mean)) - var);
    }

    std::vector<float> monitorValues;
    std::vector<float> smoothedValues;
    std::vector<int> circuitIndices;
};
/* 
------------------------------------------------------------------------------------------------------------------------
    InspectableElement
------------------------------------------------------------------------------------------------------------------------
*/
class ComponentInspector : public juce::Component
{
public:
    ComponentInspector();

    void paint (juce::Graphics& g) override;
};

class InspectableElement
{
public:
    explicit InspectableElement()
    {
    }

protected:
    ComponentInspector inspector;
};
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
                    const int currentMonitorIndex): 
        
        SchematicElement(name, std::vector<Terminal>{termA, termB, termC}),
        ControllableElement(controlIndex),
        SettableElement(paramIndex),
        MonitoringElement(std::vector<int> {currentMonitorIndex})
        {};
    void draw (juce::Graphics& g) const override;
    void controlCallback(float value, SchematicPanelListener* l) override;
    juce::String valueToLabel (float v) override;
    float labelToValue (const juce::String s) override;

private:
    static constexpr int    zigzagCount    = 6;
    static constexpr float  zigzagAmplitude = 10.0f;
    static constexpr int    zigzagLength = 40.0f;
};