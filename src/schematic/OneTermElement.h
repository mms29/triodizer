#pragma once

#include "schematic/SchematicElement.h"

class GroundElement : public SchematicElement
{
public:
    GroundElement (Terminal termPosition);
    void draw (juce::Graphics& g) const override;
};

class JunctionElement : public SchematicElement
{
public:
    JunctionElement (Terminal termPosition);
    void draw (juce::Graphics& g) const override;
};

class VoltageElement : public SchematicElement
{
public:
    using SchematicElement::SchematicElement;
    void draw (juce::Graphics& g) const override;
};


class VoltmeterElement : public SchematicElement
{
public:
    VoltmeterElement (const juce::String& name, Terminal center);

    void draw (juce::Graphics& g) const override;
    bool hitTest (juce::Point<float> point) const override;
    // Monitor / voltmeter support
    void  setMonitorValue (float v) noexcept;
    float getMonitorValue() const noexcept;
    void  enableMonitor (bool shouldEnable) noexcept;
    bool  isMonitorEnabled() const noexcept;
private:
    float                              displayVoltage   = 0.0f;
    bool                               monitorEnabled   = false;
    static constexpr float METER_RADIUS = 18.0f;
};