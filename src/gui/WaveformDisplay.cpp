#include "gui/WaveformDisplay.h"

void WaveformDisplay::paint (juce::Graphics& g)
{
    processor.getWaveformInputBuffer().getLastBlock (inputBuffer.data(), displaySize);
    processor.getWaveformOutputBuffer().getLastBlock (outputBuffer.data(), displaySize);

    g.fillAll (backgroundColour);

    auto bounds = getLocalBounds().toFloat();
    auto centerY = bounds.getHeight() / 2.0f;
    auto width = bounds.getWidth();
    auto height = bounds.getHeight();

    // Draw grid lines
    g.setColour (juce::Colours::darkgrey);
    for (int i = 0; i <= 4; ++i)
    {
        auto y = bounds.getY() + (height * i / 4.0f);
        g.drawLine (bounds.getX(), y, bounds.getRight(), y, 1.0f);
    }

    // Draw input waveform (brighter)
    if (displaySize > 0)
    {
        auto numPoints = displaySize;
        auto xStep = width / (float) displaySize;

        juce::Path inputPath;
        for (int i = 0; i < numPoints; ++i)
        {
            auto x = bounds.getX() + i * xStep;
            auto y = centerY - inputBuffer[i] * amplitudeScale;
            if (i == 0)
                inputPath.startNewSubPath (x, y);
            else
                inputPath.lineTo (x, y);
        }
        g.setColour (waveformColour);
        g.strokePath (inputPath, juce::PathStrokeType (1.5f));

        // Draw output waveform (offset down slightly)
        juce::Path outputPath;
        for (int i = 0; i < numPoints; ++i)
        {
            auto x = bounds.getX() + i * xStep;
            auto y = centerY - outputBuffer[i] * amplitudeScale;
            if (i == 0)
                outputPath.startNewSubPath (x, y);
            else
                outputPath.lineTo (x, y);
        }
        g.setColour (juce::Colours::lightblue);
        g.strokePath (outputPath, juce::PathStrokeType (1.0f));
    }
}