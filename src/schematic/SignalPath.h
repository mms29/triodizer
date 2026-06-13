#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

const juce::Colour COLOR_LASERGREEN = juce::Colour(51, 255, 102);

constexpr float sampleSpacing = 4.0f; // pixels

struct CachedPath
{
    juce::Path path;
    float length = 0.0f;
    std::vector<juce::Point<float>> samples;

    juce::Point<float> getPoint (float t) const
    {
        if (samples.size() < 2)
            return {};

        // IMPORTANT: clamp instead of wrap
        t = juce::jlimit (0.0f, 1.0f, t);

        float pos = t * (samples.size() - 1);

        int i0 = (int) pos;
        int i1 = std::min (i0 + 1, (int) samples.size() - 1);

        float frac = pos - i0;

        return samples[i0]
            + (samples[i1] - samples[i0]) * frac;
    }
    void rebuildCache ()
    {
        length = path.getLength();

        int numSamples = std::max(
            2,
            (int) std::ceil(length / sampleSpacing)
        );
        samples.clear();
        samples.reserve(numSamples);

        for (int i = 0; i < numSamples; ++i)
        {
            float t = (float) i / (numSamples - 1);

            samples.push_back(
                path.getPointAlongPath(t * length));
        }
    }
};



