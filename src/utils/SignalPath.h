#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "constants/SchematicConstants.h"
#include "utils/Glow.h"


inline const float SIGNAL_PATH_MAX_VOLTAGE = 450.0f;
inline const float POLARITY_FORWARD = false;
inline const float POLARITY_BACKWARD = true;
inline const float DIRECTION_FORWARD = false;
inline const float DIRECTION_BACKWARD = true;

struct CachedPath
{
    juce::Path path;
    float length = 0.0f;
    float phase = 0.0f;
    float rangeMin =0.0f;
    float rangeMax = 1.0f;
    std::vector<juce::Point<float>> samples;
    
    juce::Point<float> getPoint (float t) const;
    void rebuildCache ();
    void updateCachedPath (float intensity);
};




class SignalPath {
public:

    SignalPath() = default;
    ~SignalPath() = default;
    std::vector<CachedPath>& getSignalPaths () noexcept {return signalPaths;}
    float getRefVoltage () noexcept {return refVoltage;}
    float getDeltaVoltage () noexcept {return deltaVoltage;}
    void createSignalPath(std::vector<juce::Path>& paths, SignalPath* ref=nullptr, bool mode = SIGNALPATH_MODE_NORMAL_FORWARD);
    void createSignalPath(juce::Path& path, SignalPath* ref=nullptr, bool mode = SIGNALPATH_MODE_NORMAL_FORWARD);
    void setRef( SignalPath* ref);
    void setPolarity( bool pol );
    void setMode( const int mode );
    void setDirection( bool direction );
    void addPath(juce::Path& path, float rangeMin = 0.0f, float rangeMax = 1.0f);
    void updateSignalPath (float intensity, float voltage, float power = 0.0f) ;
    void draw(juce::Graphics& g);
    void shufflePhase();

private:
    std::vector<CachedPath> signalPaths;
    float refVoltage = 0.0f;
    float deltaVoltage = 0.0f;
    float power =0.0f;
    bool polarity = POLARITY_FORWARD; 
    bool direction = DIRECTION_FORWARD; 
    SignalPath* refNode;
};