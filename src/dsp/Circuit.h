#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class Circuit
{
public:
    virtual ~Circuit() = default;

    virtual void prepare(double sampleRate) = 0;
    virtual void reset() = 0;
    virtual float processSample(float x) = 0;

    virtual void setParam(const int index , float value) = 0;
    virtual void setControl(const int index , float value) = 0;
    virtual float getMonitoring(const int index ) = 0;

    int getNumParam() const { return (int) params.size();}
    int getNumControl() const { return (int) controls.size();}
    float getControl(const int index) const {return controls.at(index);}
    float getParam(const int index) const {return params.at(index);}

    juce::ValueTree saveState() const
    {
        juce::ValueTree t ("Circuit");

        for (int i = 0; i < getNumParam(); ++i){
            t.setProperty ("P" + juce::String(i), getParam(i), nullptr);
        }

        for (int i = 0; i < getNumControl(); ++i){
            t.setProperty ("C" + juce::String(i), getControl(i), nullptr);
        }

        return t;
    }

    void loadState (const juce::ValueTree& t)
    {

        for (int i = 0; i < getNumParam(); ++i)
        {
            auto name = "P" + juce::String(i);
            if (t.hasProperty(name)) setParam(i, (float)t[name]);
        }
        for (int i = 0; i < getNumControl(); ++i)
        {
            auto name = "C" + juce::String(i);
            if (t.hasProperty(name)) setControl(i, (float)t[name]);
        }
    }


protected :
    std::vector<float> params;
    std::vector<float> controls;
};

class DefaultCircuit : public Circuit
{
public:
    void prepare(double) override {}
    void reset() override {}

    float processSample(float x) override {return x;}

    void setParam(int, float) override {}
    void setControl(int, float) override {}

    float getMonitoring(int) override{ return 0.0f;}
};