#include "utils/SignalPath.h"

juce::Point<float> CachedPath::getPoint (float t) const
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

void CachedPath::rebuildCache ()
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
void updateCachedPath (float intensity, int clockTick, CachedPath& cachedPath)
{
    intensity = juce::jmap (intensity, 0.0f, 1.0f, 0.0f, 1.0f);
    cachedPath.phase += clockTick * intensity * 0.001f;
}

void drawSignalPath (juce::Graphics& g, const CachedPath& cachedPath)
{
    constexpr float beadSpacingPx = 15;

    if (cachedPath.samples.size() < 2)
        return;

    float length = cachedPath.length;
    int numBeads = juce::jmax (1, (int) (length / beadSpacingPx));
    float spacing = 1.0f / numBeads;


    for (int i = 0; i < numBeads; ++i)
    {
        float offset = i * spacing;
        float t = offset + cachedPath.phase;
        t -= (int) t; // fast wrap [0..1]

        // use cached interpolation instead of Path queries
        juce::Point<float> p = cachedPath.getPoint (t);
        float beadSize = 2.0f ;

        g.setColour(getColourLaserGreen());
        // g.drawEllipse(p.x - beadSize * 0.5f,
        //                  p.y - beadSize * 0.5f,
        //                  beadSize,
        //                  beadSize, 1.0f);

        juce::Path bead;
        bead.addEllipse (p.x - beadSize * 0.5f,
                         p.y - beadSize * 0.5f,
                         beadSize,
                         beadSize);

        // g.strokePath(bead, juce::PathStrokeType(1.0f));
        drawGlowPath (g,
                      bead,
                      0.05f,
                      getColourLaserGreen(),
                      getColourLaserGreen(),
                      false);
    }
}