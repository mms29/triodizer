#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <dsp/Circuit.h>
/* 
------------------------------------------------------------------------------------------------------------------------
    Listener.
------------------------------------------------------------------------------------------------------------------------
*/
class SchematicPanelListener
{
public:
    virtual ~SchematicPanelListener() = default;
    virtual void setCircuitParam (const int index, float newValue) = 0;
    virtual void setCircuitControl (const int index, float newValue) = 0;
    virtual const MonitorValuef&  getCircuitMonitoring (const int index) = 0;
    virtual float getCircuitParam (const int index) = 0;
    virtual float getCircuitControl (const int index) = 0;
    virtual void updateCircuitMonitoring () = 0;
};

