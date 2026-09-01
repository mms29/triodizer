#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <chowdsp_wdf/chowdsp_wdf.h>

using namespace chowdsp::wdft;

template <typename T, typename WDFType>
inline T power (const WDFType& wdf) noexcept
{
    return (wdf.wdf.a - wdf.wdf.b) * ((T) 0.5 * wdf.wdf.G) *1e-1;
}


inline const int MONITOR_TUBE_IP = 0;
inline const int MONITOR_TUBE_IK = 1;
inline const int MONITOR_TUBE_VG = 2;
inline const int MONITOR_TUBE_VK = 3;
inline const int MONITOR_TUBE_VP = 4;
inline const int MONITOR_TUBE_SC = 5;
inline const int MONITOR_TUBE_SP = 6;
inline const int MONITOR_TUBE_COUNT = 7;

inline const int MONITOR_PORT_I = 0;
inline const int MONITOR_PORT_V = 1;
inline const int MONITOR_PORT_COUNT = 2;

template <typename T>
struct MonitorValue{
    std::array<T, MONITOR_TUBE_COUNT> values{}; 
    uint8_t size = 0;  
};

using MonitorValuef = MonitorValue<float>;

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
    const MonitorValue<T>& getMonitoring(const int monitorIndex) {return monitors.at(monitorIndex);}

    void updateTubeMonitor(const int monitorIndex, const MonitorValue<T>& v){
        monitors[monitorIndex] = v;
    }
    template <typename WDFType>
    void updatePortMonitor(const int monitorIndex, const WDFType& wdf){
        monitors[monitorIndex].size = MONITOR_PORT_COUNT;
        monitors[monitorIndex].values[MONITOR_PORT_V] = voltage<T>(wdf);  
        monitors[monitorIndex].values[MONITOR_PORT_I] = current<T>(wdf);  
    }

    int getNumParam() const { return (int) params.size();}
    int getNumControl() const { return (int) controls.size();}
    int getNumMonitor() const { return (int) monitors.size();}
    float getOutputGain() const { return outputGain;}
    void setOutputGain(const float newGain) { outputGain= newGain;}

protected :
    std::vector<T> params;
    std::vector<T> controls;
    std::vector<MonitorValue<T>> monitors;

    float outputGain = 1.0f;
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

enum class PotType
{
    Linear,
    Log,
    ReverseLog
};

template <typename T>
struct PotRatios
{
    T plus;
    T minus;
};

template <typename T>
PotRatios<T> getPotRatios(T ratio, PotType type)
{
    T r = ratio;
    auto eps = 1e-5f;

    switch (type)
    {
        case PotType::Linear:
            r = ratio;
            break;

        case PotType::ReverseLog:
            r = std::pow(ratio, T(3));
            break;

        case PotType::Log:
            r = T(1) - std::pow(T(1) - ratio, T(3));
            break;
    }
    r = juce::jlimit(T(eps), T(1.0f - eps), r);

    return { T(1) - r, r };
}