#include "TwoTermElement.h"

void ResistorElement::draw (juce::Graphics& g) const
{
    const auto& p0 = terminals[0];
    const auto& p1 = terminals[1];

    const juce::Point<float> d = p1-p0;
    const float length = p1.getDistanceFrom(p0);
    if (length < zigzagLength) return;

    const juce::Point<float> u = d/length;
    const juce::Point<float> v {- u.getY(), u.getX()};

    const float halfAmp = zigzagAmplitude;
    const float s = zigzagLength/(zigzagCount*2);

    const juce::Point<float> a = p0 + d*(length-zigzagLength)/(2*length);
    const juce::Point<float> b = p1 - d*(length-zigzagLength)/(2*length);

    juce::Path zigzag;
    zigzag.startNewSubPath (p0);

    cachedBounds = juce::Rectangle<float> (p0, p1);
    cachedBounds.expand(1.0f + std::abs(v.x*halfAmp), 1.0f + std::abs(v.y*halfAmp));

    zigzag.lineTo(a);
    juce::Point<float>  curr = a;
    for (int i = 0; i <= zigzagCount; ++i)
    {
        int sign = std::pow(-1, i);
        curr = curr + (halfAmp * v * sign) + (s * u);
        if (i != 0 && i!= zigzagCount){
            curr = curr + (halfAmp*v * sign) + (s*u);
        }
        zigzag.lineTo (curr);
    }
    zigzag.lineTo(p1);

    // Zigzag line
    g.setColour (juce::Colours::white);
    if (isHighlighted())
        g.strokePath (zigzag, juce::PathStrokeType (3.0f));
    else
        g.strokePath (zigzag, juce::PathStrokeType (2.0f));

    // Labels
    const float labelOff = -40.0f;
    const juce::Point<float> m = (p0 + p1) * 0.5f;
    const juce::Point<float> l = m + labelOff * v;
    drawLabel(g, l);

}


void CapacitorElement::draw (juce::Graphics& g) const
{
    const auto& p0 = terminals[0];
    const auto& p1 = terminals[1];

    const juce::Point<float> d = p1-p0;
    const float length = p1.getDistanceFrom(p0);
    if (length < plateGap) return;

    const juce::Point<float> u = d/length;
    const juce::Point<float> v {- u.getY(), u.getX()};

    const juce::Point<float> a = p0 + d*(length-plateGap)/(2*length);
    const juce::Point<float> b = p1 - d*(length-plateGap)/(2*length);

    // Build cached bounds
    cachedBounds = juce::Rectangle<float> (p0, p1);
    cachedBounds.expand(1.0f + std::abs(v.x*plateWidth/2.0f), 1.0f + std::abs(v.y*plateWidth/2.0f));

    // Draw two parallel plates
    juce::Path leftPlate, rightPlate, positivePath, negativePath;
    leftPlate.startNewSubPath (a - plateWidth * 0.5f * v);
    leftPlate.lineTo   (a + plateWidth * 0.5f * v);
    rightPlate.startNewSubPath (b - plateWidth * 0.5f * v);
    rightPlate.lineTo   (b + plateWidth * 0.5f * v);
    positivePath.startNewSubPath (p0);
    positivePath.lineTo   (a);
    negativePath.startNewSubPath (p1);
    negativePath.lineTo   (b);

    g.setColour (juce::Colours::white);
    if (!isHighlighted()){
        g.strokePath (leftPlate,  juce::PathStrokeType (STROKE_NORMAL));
        g.strokePath (rightPlate, juce::PathStrokeType (STROKE_NORMAL));
        g.strokePath (positivePath,  juce::PathStrokeType (STROKE_NORMAL));
        g.strokePath (negativePath, juce::PathStrokeType (STROKE_NORMAL));
    }else{
        g.strokePath (leftPlate,  juce::PathStrokeType (STROKE_HIGHLIGHT));
        g.strokePath (rightPlate, juce::PathStrokeType (STROKE_HIGHLIGHT));
        g.strokePath (positivePath,  juce::PathStrokeType (STROKE_HIGHLIGHT));
        g.strokePath (negativePath, juce::PathStrokeType (STROKE_HIGHLIGHT));
    }

    // Labels
    const float labelOff = -42.0f;
    const juce::Point<float> m = (p0 + p1) * 0.5f;
    const juce::Point<float> l = m + labelOff * v;
    drawLabel(g, l);

}