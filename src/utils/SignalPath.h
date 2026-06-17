#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "constants/SchematicConstants.h"
#include "utils/Glow.h"

constexpr float sampleSpacing = 4.0f; // pixels


struct CachedPath
{
    juce::Path path;
    float length = 0.0f;
    float phase = 0.0f;
    std::vector<juce::Point<float>> samples;
    
    juce::Point<float> getPoint (float t) const;
    void rebuildCache ();
};

void updateCachedPath (float intensity, int clockTick, CachedPath& cachedPath);

void drawSignalPath (juce::Graphics& g, const CachedPath& cachedPath);

