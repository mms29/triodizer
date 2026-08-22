#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "utils/Glow.h"

class Inspector : public juce::Component
{
public:
    Inspector() = default;

    void setContent (juce::String newName,
                    juce::String newValue,
                    juce::AttributedString newContent,
                     juce::AttributedString newDescription)
    {
        name = std::move (newName);
        value = std::move (newValue);
        content = std::move (newContent);
        description = std::move (newDescription);
        // repaint();
    }

    void clear()
    {
        name.clear();
        value.clear();
        content.clear();
        description.clear();
        // repaint();
    }

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        bounds.removeFromLeft(15);
        bounds.removeFromBottom(15);
        bounds.removeFromTop(15);
        // Border
        juce::Path borderPath;
        borderPath.addRoundedRectangle(bounds, 10.0f, 10.0f);
        drawGlowAndCorePath(g, borderPath, .1f, getColourNormal(), getColourNormal(), false);

        // Background
        g.setColour (getColourBackground().withAlpha(0.9f));
        g.fillRoundedRectangle (bounds, 10.0f);

        // Title
        // bounds.reduced (4.0f);
        bounds.removeFromLeft(10.0f);
        bounds.removeFromRight(10.0f);
        bounds.removeFromTop(5.0f);
        bounds.removeFromBottom(5.0f);

        juce::AttributedString textTitle;
        textTitle.append (name, juce::Font (juce::FontOptions(FONT_TITLE)).boldened(), getColourHighlight());
        textTitle.append (" - "+ value,juce::Font (juce::FontOptions(FONT_TITLE)), getColourNormal());
        textTitle.draw (g, bounds);

        // line
        g.setColour (getColourNormal());
        bounds.removeFromTop(30);
        g.drawLine (juce::Line(bounds.getTopLeft(), bounds.getTopRight()), 1.0f);

        // // Content    
        // bounds.removeFromTop(10);
        // content.draw (g, bounds);


        // // Description    
        // description.setJustification(juce::Justification::bottomLeft);
        // description.draw(g, bounds);

        juce::TextLayout contentLayout;
        contentLayout.createLayout (content, bounds.getWidth());

        auto contentHeight = contentLayout.getHeight();
        auto descriptionBounds = bounds;

        descriptionBounds.removeFromTop (10);
        descriptionBounds.removeFromTop (contentHeight);

        contentLayout.draw (g, bounds);
        description.draw (g, descriptionBounds);

    }

private:
    juce::String name;
    juce::String value;
    juce::AttributedString description;
    juce::AttributedString content;
};