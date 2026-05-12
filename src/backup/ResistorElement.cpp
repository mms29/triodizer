#include "ResistorElement.h"

ResistorElement::ResistorElement(
    const juce::String& elementName,
    juce::Point<float> start,
    juce::Point<float> end)
    : SchematicElement(elementName),
      p0(start),
      p1(end)
{
    centre = (p0 + p1) * 0.5f;

    terminals.push_back({ p0, "" });
    terminals.push_back({ p1, "" });

    updateBounds();
}

void ResistorElement::setPosition(
    juce::Point<float> newPos)
{
    auto delta = newPos - centre;

    centre = newPos;

    p0 += delta;
    p1 += delta;

    terminals[0].pos = p0;
    terminals[1].pos = p1;

    updateBounds();
}

void ResistorElement::updateBounds()
{
    float x = std::min(p0.x, p1.x) - 12.0f;
    float y = std::min(p0.y, p1.y) - 12.0f;

    float w = std::abs(p1.x - p0.x) + 24.0f;
    float h = std::abs(p1.y - p0.y) + 24.0f;

    bounds = { x, y, w, h };
}

void ResistorElement::paint(
    juce::Graphics& g)
{
    g.setColour(juce::Colours::white);

    drawZigzag(g);

    g.drawText(
        name + ": " + getSelectedLabel(),
        bounds.toNearestInt(),
        juce::Justification::centredTop);
}

void ResistorElement::drawZigzag(
    juce::Graphics& g)
{
    juce::Path path;

    path.startNewSubPath(p0);

    float dx = (p1.x - p0.x) / 6.0f;
    float dy = (p1.y - p0.y) / 6.0f;

    for (int i = 1; i <= 6; ++i)
    {
        float sign = (i % 2) ? 1.0f : -1.0f;

        path.lineTo(
            p0.x + dx * i,
            p0.y + dy * i + sign * 6.0f);
    }

    g.strokePath(path,
        juce::PathStrokeType(2.0f));
}