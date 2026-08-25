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
    if (dynamic_cast<juce::AlertWindow*> (label.getParentComponent()) != nullptr)
        return;
    g.setColour (juce::Colours::white.withAlpha(0.5f));
    g.setFont (label.getFont());

    g.drawText (label.getText(),
                label.getLocalBounds(),
                juce::Justification::centredLeft);
}
void GlowComboBoxLookAndFeel::drawButtonBackground (
    juce::Graphics& g,
    juce::Button& button,
    const juce::Colour&,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    auto area = button.getLocalBounds().toFloat();

    // Same outline as ComboBox
    juce::Path outline;
    outline.addRoundedRectangle (
        area.reduced (1.0f),
        6.0f);

    g.setColour (
        getColourGrey().withAlpha (0.5f));

    g.strokePath (
        outline,
        juce::PathStrokeType (
            0.5f,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded));

    // Same arrow as ComboBox
    juce::Path arrow;

    float cx = area.getRight() - 15.0f;
    float cy = area.getCentreY();

    arrow.startNewSubPath (cx - 5.0f, cy - 3.0f);
    arrow.lineTo         (cx,       cy + 3.0f);
    arrow.lineTo         (cx + 5.0f, cy - 3.0f);
    arrow.closeSubPath();

    drawGlowAndCorePath (
        g,
        arrow,
        0.05f,
        getColourNormal(),
        getColourHotRed(),
        false);
}
void GlowComboBoxLookAndFeel::drawButtonText (
    juce::Graphics& g,
    juce::TextButton& button,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    // Same font / colour as ComboBox label
    g.setColour (
        juce::Colours::white.withAlpha (0.5f));

    g.setFont (
        juce::FontOptions (
            FONT_SUB2,
            juce::Font::plain));

    // Leave room for the arrow
    auto textArea = button.getLocalBounds()
                           .reduced (10, 2);

    textArea.removeFromRight (20);

    g.drawText (
        button.getButtonText(),
        textArea,
        juce::Justification::centredLeft,
        true);
}

void GlowComboBoxLookAndFeel::drawPopupMenuBackground (juce::Graphics& g,
                                                        int width,
                                                        int height)
{
    auto area = juce::Rectangle<float> (0.0f, 0.0f,
                                        (float) width,
                                        (float) height);

    // Subtle outer glow
    juce::Path outline;
    g.setColour (getColourBackground());
    g.fillRect(area);
    outline.addRoundedRectangle (area, 6.0f);

    g.setColour (getColourGrey());
    g.strokePath (
        outline,
        juce::PathStrokeType (
            0.5f,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded));

}

void GlowComboBoxLookAndFeel::drawPopupMenuItem (
    juce::Graphics& g,
    const juce::Rectangle<int>& area,
    bool isSeparator,
    bool isActive,
    bool isHighlighted,
    bool isTicked,
    bool hasSubMenu,
    const juce::String& text,
    const juce::String& shortcutKeyText,
    const juce::Drawable* icon,
    const juce::Colour* textColour)
{
    if (isSeparator)
    {
        auto separatorArea = area.reduced (10, 0);

        g.setColour (getColourGrey().withAlpha (0.5f));

        g.fillRect (
            separatorArea.getX(),
            separatorArea.getCentreY(),
            separatorArea.getWidth(),
            1);
        
        return;
    }

    auto itemArea = area.toFloat();

    // Hover background
    if (isHighlighted && isActive)
    {
        auto highlightArea = itemArea.reduced (2.0f, 1.0f);

        g.setColour (getColourNormal().withAlpha (0.08f));
        g.fillRoundedRectangle (highlightArea, 4.0f);

        // Optional glow around hovered item
        juce::Path highlightPath;
        highlightPath.addRoundedRectangle (highlightArea, 4.0f);

        drawGlowAndCorePath (
            g,
            highlightPath,
            0.025f,
            getColourNormal(),
            getColourNormal(),
            false);
    }

    // Text
    auto colour = textColour != nullptr
                    ? *textColour
                    : getColourGrey();

    if (!isActive)
        colour = colour.withAlpha (0.25f);

    if (isHighlighted && isActive)
        colour = getColourHighlight();

    g.setColour (colour);

    g.setFont (
        juce::FontOptions (
            FONT_SUB2,
            isHighlighted ? juce::Font::bold
                          : juce::Font::plain));

    auto textArea = area.reduced (12, 0);

    // Leave room for tick/checkmark
    if (isTicked)
        textArea.removeFromLeft (18);

    g.drawText (
        text,
        textArea,
        juce::Justification::centredLeft,
        true);

    // Tick indicator
    if (isTicked)
    {
        juce::Path tick;

        float x = (float) area.getX() + 9.0f;
        float y = (float) area.getCentreY();

        tick.startNewSubPath (x - 3.0f, y);
        tick.lineTo         (x - 1.0f, y + 3.0f);
        tick.lineTo         (x + 4.0f, y - 4.0f);

        drawGlowAndCorePath (
            g,
            tick,
            0.04f,
            getColourNormal(),
            getColourHighlight(),
            false);
    }

    // Submenu arrow
    if (hasSubMenu)
    {
        juce::Path arrow;

        float x = (float) area.getRight() - 10.0f;
        float y = (float) area.getCentreY();

        arrow.startNewSubPath (x - 3.0f, y - 4.0f);
        arrow.lineTo         (x + 2.0f, y);
        arrow.lineTo         (x - 3.0f, y + 4.0f);

        drawGlowAndCorePath (
            g,
            arrow,
            0.04f,
            getColourNormal(),
            getColourHotRed(),
            false);
    }
}

int GlowComboBoxLookAndFeel::getPopupMenuBorderSize()
{
    return 1;
}

juce::Font GlowComboBoxLookAndFeel::getPopupMenuFont()
{
    return juce::FontOptions (FONT_SUB1, juce::Font::plain);
}

void GlowComboBoxLookAndFeel::drawAlertBox (
    juce::Graphics& g,
    juce::AlertWindow& alert,
    const juce::Rectangle<int>& textArea,
    juce::TextLayout& textLayout)
{
    auto area = alert.getLocalBounds().toFloat();

    // Background
    g.setColour (juce::Colour (0xff101010));
    g.fillRoundedRectangle (area.reduced (1.0f), 8.0f);

    // Outline
    juce::Path outline;
    outline.addRoundedRectangle (area.reduced (1.0f), 8.0f);

    g.setColour (getColourGrey().withAlpha (0.5f));

    g.strokePath (
        outline,
        juce::PathStrokeType (
            0.5f,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded));

    // Subtle glow
    drawGlowAndCorePath (
        g,
        outline,
        0.035f,
        getColourNormal(),
        getColourNormal(),
        false);

    // Message
    textLayout.draw (
        g,
        textArea.toFloat());
}
void GlowComboBoxLookAndFeel::drawTextEditorOutline (
    juce::Graphics& g,
    int width,
    int height,
    juce::TextEditor& editor)
{
    auto area = juce::Rectangle<float> (
        0.0f, 0.0f,
        (float) width,
        (float) height);

    juce::Path outline;
    outline.addRoundedRectangle (
        area.reduced (1.0f),
        5.0f);

    g.setColour (getColourGrey().withAlpha (0.5f));

    g.strokePath (
        outline,
        juce::PathStrokeType (
            0.5f,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded));

    if (editor.hasKeyboardFocus (true))
    {
        drawGlowAndCorePath (
            g,
            outline,
            0.035f,
            getColourNormal(),
            getColourNormal(),
            false);
    }
}
void GlowComboBoxLookAndFeel::fillTextEditorBackground (
    juce::Graphics& g,
    int width,
    int height,
    juce::TextEditor& editor)
{
    auto area = juce::Rectangle<float> (
        0.0f, 0.0f,
        (float) width,
        (float) height);

    g.setColour (juce::Colour (0xff101010));
    g.fillRoundedRectangle (
        area.reduced (1.0f),
        5.0f);
}
juce::Font GlowComboBoxLookAndFeel::getAlertWindowTitleFont()
{
    return juce::FontOptions (
        FONT_SUB2,
        juce::Font::bold);
}

juce::Font GlowComboBoxLookAndFeel::getAlertWindowMessageFont()
{
    return juce::FontOptions (
        FONT_SUB2,
        juce::Font::plain);
}
int GlowComboBoxLookAndFeel::getAlertBoxWindowFlags()
{
    return 0;
}




void InspectButtonLookAndFeel::drawButtonBackground (
    juce::Graphics& g,
    juce::Button& button,
    const juce::Colour& backgroundColour,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    auto area = button.getLocalBounds().toFloat();

    juce::Path outline;
    outline.addRoundedRectangle (area.reduced (10.0f), 6.0f);

    // Same outline as the ComboBox
    g.setColour (getColourGrey().withAlpha(0.5f));
    g.strokePath (
        outline,
        juce::PathStrokeType (
            0.5f,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded));

    // Optional glow when hovered/pressed
    if (shouldDrawButtonAsHighlighted || shouldDrawButtonAsDown)
    {

        drawGlowAndCorePath (g,
                      outline,
                      0.02f,
                      getColourNormal(),
                      getColourNormal(),
                      false);

        g.setColour(getColourNormal().withAlpha (0.1f));
        g.fillPath(outline);

    }
}

void InspectButtonLookAndFeel::drawButtonText (
    juce::Graphics& g,
    juce::TextButton& button,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{

    // Optional glow when hovered/pressed
    if (shouldDrawButtonAsHighlighted || shouldDrawButtonAsDown)
    {
    g.setColour (getColourHighlight());
    g.setFont (juce::FontOptions (FONT_SUB2, juce::Font::bold));
    }
    else{
    g.setColour (juce::Colours::white.withAlpha(0.5f));
    g.setFont (juce::FontOptions (FONT_SUB2, juce::Font::plain));
    }

    g.drawText (
        button.getButtonText(),
        button.getLocalBounds(),
        juce::Justification::centred);
    

}
