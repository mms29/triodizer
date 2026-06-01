#include "gui/WaveformDisplay.h"

void WaveformDisplay::paint (juce::Graphics& g)
{
    processor.getWaveformInputBuffer().getLastBlock (inputBuffer.data(), displaySize);
    processor.getWaveformOutputBuffer().getLastBlock (outputBuffer.data(), displaySize);

    g.fillAll (backgroundColour);

    auto bounds = getLocalBounds().toFloat();

    auto halfWidth = bounds.getWidth() * 0.5f;

    auto inputArea  = bounds.removeFromLeft (halfWidth);
    auto outputArea = bounds;

    auto drawGrid = [&g](juce::Rectangle<float> area)
    {
        g.setColour (juce::Colours::darkgrey);

        for (int i = 0; i <= 4; ++i)
        {
            auto y = area.getY() + (area.getHeight() * i / 4.0f);
            g.drawLine (area.getX(), y, area.getRight(), y, 1.0f);
        }
    };
    g.setColour (juce::Colours::darkgrey);
    g.drawLine (juce::Line(inputArea.getTopRight() , inputArea.getBottomRight()), 1.5f);

    // GRID + MID LINE
    drawGrid (inputArea);
    drawGrid (outputArea);

    // =========================
    // INPUT (LEFT)
    // =========================
    if (displaySize > 0)
    {
        auto centerY = inputArea.getCentreY();
        auto width   = inputArea.getWidth();
        auto xStep   = width / (float) displaySize;

        juce::Path inputPath;

        for (int i = 0; i < displaySize; ++i)
        {
            auto x = inputArea.getX() + i * xStep;
            auto y = centerY - inputBuffer[i] * amplitudeScale;

            if (i == 0)
                inputPath.startNewSubPath (x, y);
            else
                inputPath.lineTo (x, y);
        }

        g.setColour (waveformColour);
        g.strokePath (inputPath, juce::PathStrokeType (1.5f));
    }

    // =========================
    // OUTPUT (RIGHT)
    // =========================
    if (displaySize > 0)
    {
        auto centerY = outputArea.getCentreY();
        auto width   = outputArea.getWidth();
        auto xStep   = width / (float) displaySize;

        juce::Path outputPath;

        for (int i = 0; i < displaySize; ++i)
        {
            auto x = outputArea.getX() + i * xStep;
            auto y = centerY - outputBuffer[i] * amplitudeScale;

            if (i == 0)
                outputPath.startNewSubPath (x, y);
            else
                outputPath.lineTo (x, y);
        }

        g.setColour (waveformColour);
        g.strokePath (outputPath, juce::PathStrokeType (1.0f));
    }
}