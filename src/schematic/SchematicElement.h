#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include "gui/Knob.h"
    
const float STROKE_NORMAL = 2.5f;
const float STROKE_HIGHLIGHT = 5.0f;

const juce::Colour SCHEMATIC_BACKGROUND = juce::Colours::black;
const juce::Colour SCHEMATIC_NORMAL = juce::Colours::white;
const juce::Colour SCHEMATIC_HIGHLIGHT = juce::Colours::yellow;

struct ValueChoice
{
    float value;
    juce::String label;
};

using Terminal = juce::Point<float> ;

static constexpr float e12[] =
{
    1.0, 1.2, 1.5, 1.8, 2.2, 2.7, 3.3, 3.9, 4.7, 5.6, 6.8, 8.2
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
                      : name (name), terminals (std::move (terminals)){};

    virtual ~SchematicElement() = default;

    // Accessors
    const juce::String& getName() const noexcept;
    const std::vector<Terminal>& getTerminals() const noexcept;
    bool isHighlighted() const noexcept;
    void setHighlighted (bool shouldBeHighlighted) noexcept;

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
    void addChoice (const float value, const juce::String label) noexcept {choices.push_back(ValueChoice{value, label}) ;}

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
    MonitoringElement (const int monitorIndex): monitorIndex(monitorIndex) {};
    void  setMonitorValue (float v) noexcept  { monitorValue =v; }
    int  getMonitorIndex () noexcept {return monitorIndex;}

protected:
    float                              monitorValue   = 0.0f;
    int                              monitorIndex;
};

/* 
------------------------------------------------------------------------------------------------------------------------
    Potentiometer element
------------------------------------------------------------------------------------------------------------------------
*/
class PotElement :  public SchematicElement, 
                    public ControllableElement, 
                    public ParametrableElement
{
public:
    PotElement(const juce::String& name,
                    std::vector<Terminal> terminals,
                    const int controlIndex,
                    const int paramIndex,
                    int choiceIndex,
                    std::vector<ValueChoice> choices): 
        
        SchematicElement(name, terminals),
        ControllableElement(controlIndex),
        ParametrableElement(paramIndex, choiceIndex, choices)
        {};
    void draw (juce::Graphics& g) const override;
    void controlCallback(float value, SchematicPanelListener* l) override;

private:
    static constexpr int    zigzagCount    = 6;
    static constexpr float  zigzagAmplitude = 10.0f;
    static constexpr int    zigzagLength = 40.0f;
};