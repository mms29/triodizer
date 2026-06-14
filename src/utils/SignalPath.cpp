#include "utils/SignalPath.h"


void drawSignalPath (juce::Graphics& g,
                     const CachedPath& cachedPath,
                     float intensity,
                     int clockTick)
{
    constexpr float beadSpacingPx = 15;

    if (cachedPath.samples.size() < 2)
        return;

    float speed = juce::jmap (intensity, 0.0f, 1.0f, 0.05f, 3.0f);

    float length = cachedPath.length;
    int numBeads = juce::jmax (1, (int) (length / beadSpacingPx));
    float spacing = 1.0f / numBeads;

    for (int i = 0; i < numBeads; ++i)
    {
        float phase = i * spacing;
        float t = phase + clockTick * speed * 0.01f;
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