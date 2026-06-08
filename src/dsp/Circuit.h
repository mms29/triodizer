#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

template <typename T>
class Circuit
{
public:
    virtual ~Circuit() = default;

    virtual void prepare(T sampleRate) = 0;
    virtual void reset() = 0;
    virtual T processSample(T x) = 0;

    virtual void setParam(const int index , T value) = 0;
    virtual void setControl(const int index , T value) = 0;
    virtual void updateMonitors() = 0;

    T getControl(const int index) const {return controls.at(index);}
    T getParam(const int index) const {return params.at(index);}
    T getMonitoring(const int index ) {return monitors.at(index);};

    int getNumParam() const { return (int) params.size();}
    int getNumControl() const { return (int) controls.size();}
    int getNumMonitor() const { return (int) monitors.size();}

protected :
    std::vector<T> params;
    std::vector<T> controls;
    std::vector<T> monitors;
};

template <typename T>
class DefaultCircuit : public Circuit<T>
{
public:
    void prepare(T) override {}
    void reset() override {}

    T processSample(T x) override {return x;}

    void setParam(int, T) override {}
    void setControl(int, T) override {}
    void updateMonitors() override {}
};