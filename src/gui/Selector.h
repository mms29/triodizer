#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "utils/Glow.h"


class GlowComboBoxLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawComboBox (juce::Graphics& g,
                    int width,
                    int height,
                    bool isButtonDown,
                    int buttonX,
                    int buttonY,
                    int buttonW,
                    int buttonH,
                    juce::ComboBox&) override;

    void positionComboBoxText (juce::ComboBox& box, juce::Label& label) override;
    void drawLabel (juce::Graphics& g, juce::Label& label) override;
    void drawButtonText (
    juce::Graphics& g,
    juce::TextButton& button,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown) override;

    void drawButtonBackground (
    juce::Graphics& g,
    juce::Button& button,
    const juce::Colour& backgroundColour,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown) override;

    void drawPopupMenuBackground (juce::Graphics& g,
                                int width,
                                int height) override;

    void drawPopupMenuItem (juce::Graphics& g,
                            const juce::Rectangle<int>& area,
                            bool isSeparator,
                            bool isActive,
                            bool isHighlighted,
                            bool isTicked,
                            bool hasSubMenu,
                            const juce::String& text,
                            const juce::String& shortcutKeyText,
                            const juce::Drawable* icon,
                            const juce::Colour* textColour) override;

    int getPopupMenuBorderSize() override;

    juce::Font getPopupMenuFont() override;

    void drawAlertBox (juce::Graphics& g,
                    juce::AlertWindow& alert,
                    const juce::Rectangle<int>& textArea,
                    juce::TextLayout& textLayout) override;

    int getAlertBoxWindowFlags() override;
    juce::Font getAlertWindowTitleFont() override;
    juce::Font getAlertWindowMessageFont() override;

    void fillTextEditorBackground (juce::Graphics& g,
                               int width,
                               int height,
                               juce::TextEditor& editor) override;

    void drawTextEditorOutline (juce::Graphics& g,
                                int width,
                                int height,
                                juce::TextEditor& editor) override;
};