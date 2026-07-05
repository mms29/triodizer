#include <gui/Selector.h>


void GlowComboBoxLookAndFeel::drawComboBox (juce::Graphics& g,
                    int width,
                    int height,
                    bool isButtonDown,
                    int buttonX,
                    int buttonY,
                    int buttonW,
                    int buttonH,
                    juce::ComboBox&)
    {
        juce::Rectangle<float> area (0.0f, 0.0f, (float) width, (float) height);

        // Background outline path
        juce::Path outline;
        outline.addRoundedRectangle (area.reduced (1.0f), 6.0f);

        g.setColour(getColourGrey().withAlpha(0.5f));
        g.strokePath(outline, juce::PathStrokeType(0.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        // drawGlowPath (g,
        //               outline,
        //               0.0f,
        //               getColourNormal(),
        //               getColourAmber(),
        //               false);

        // Arrow indicator (simple triangle)
        juce::Path arrow;
        float cx = area.getRight() - 15.0f;
        float cy = area.getCentreY();

        arrow.startNewSubPath (cx - 5.0f, cy - 3.0f);
        arrow.lineTo (cx, cy + 3.0f);
        arrow.lineTo (cx + 5.0f, cy - 3.0f);
        arrow.closeSubPath();

        drawGlowAndCorePath (g,
                      arrow,
                      0.05f,
                      getColourNormal(),
                      getColourHotRed(),
                      false);
    }


void GlowComboBoxLookAndFeel::positionComboBoxText (juce::ComboBox& box,
                            juce::Label& label) 
{
    label.setBounds (box.getLocalBounds().reduced (10, 2));
    label.setFont (juce::FontOptions (FONT_SUB2, juce::Font::plain));
    label.setJustificationType (juce::Justification::centred);

}

void GlowComboBoxLookAndFeel::drawLabel (juce::Graphics& g, juce::Label& label) 
{
    g.setColour (juce::Colours::white.withAlpha(0.5f));
    g.setFont (label.getFont());

    g.drawText (label.getText(),
                label.getLocalBounds(),
                juce::Justification::centredLeft);
}