#include <gui/Button.h>

void PathToggleButton::paintButton (juce::Graphics& g,
                    bool,
                    bool) 
{
    auto bounds = getLocalBounds().reduced(0,0).toFloat();
    std::cout << "bounds: " << bounds.toString() << std::endl;

    // split area: icon + text
    auto textArea = bounds;
    textArea.removeFromRight (bounds.getHeight()*0.75f);
    auto iconArea = bounds.removeFromRight (bounds.getHeight());

    std::cout << "iconArea: " << iconArea.toString() << std::endl;
    std::cout << "textArea: " << textArea.toString() << std::endl;

    // ===== PATH =====
    juce::Path p = shape;

    auto b = p.getBounds();

    auto scale = std::min (iconArea.getWidth()  / b.getWidth(),
                        iconArea.getHeight() / b.getHeight()) * 0.25f;

    p.applyTransform (
        juce::AffineTransform::scale (scale, scale,
                                    b.getCentreX(),
                                    b.getCentreY())
        .translated (
            iconArea.getCentreX() - b.getCentreX(),
            iconArea.getCentreY() - b.getCentreY()));

    // ===== COLOR =====
    juce::Colour c = getToggleState() ? on : off;

    if (getToggleState())
    {
        drawGlowAndCorePath (g,
                        p,
                        0.2f,
                        c,
                        c,
                        false);
    }
    else
    {
        g.setColour (c.withAlpha (0.6f));
        g.strokePath (p, juce::PathStrokeType (2.0f));
    }

    // ===== LABEL =====
    g.setColour (c.withAlpha (getToggleState() ? 1.0f : 0.6f));

    g.setFont (juce::FontOptions (FONT_SUB2));

    g.drawText (text,
                textArea,
                juce::Justification::right);
}


juce::Path createSineWavePath (juce::Rectangle<float> bounds,
                               float cycles,
                               float amplitudeRatio)
{
    juce::Path p;

    const float w = bounds.getWidth();
    const float h = bounds.getHeight();

    const float midY = bounds.getCentreY();
    const float amplitude = h * amplitudeRatio;

    const int samples = 100; // smoothness

    for (int i = 0; i <= samples; ++i)
    {
        float t = (float) i / (float) samples;

        float x = bounds.getX() + t * w;

        float angle = t * cycles * juce::MathConstants<float>::twoPi;
        float y = midY + std::sin (angle) * amplitude;

        if (i == 0)
            p.startNewSubPath (x, y);
        else
            p.lineTo (x, y);
    }

    return p;
}