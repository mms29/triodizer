#include "schematic/SchematicElement.h"

bool                BaseElement::isHighlighted() const noexcept  { return highlighted; }
void BaseElement::setHighlighted (bool should) noexcept { highlighted = should; }

bool BaseElement::hitTest (juce::Point<float> point) const
{
    for (const auto& terminal : terminals)
        if (terminal.getDistanceFrom (point) < 12.0f)
            return true;
    if (cachedBounds.expanded (6.0f).contains (point))
        return true;
    return false;
}

const std::vector<Terminal>& BaseElement::getTerminals() const noexcept
{
    return terminals;
}

const juce::String& SchematicElement::getName() const noexcept       { return name; }
void SchematicElement::drawLabel(juce::Graphics& g, Terminal center, juce::String value) const{
    juce::Font fontName = juce::FontOptions (FONT_SUB1);
    juce::Font fontValue = juce::FontOptions (FONT_SUB2);
    if (isHighlighted()){
        fontName = fontName.boldened();
        fontValue = fontValue.boldened();
    }

    // Name
    g.setFont (fontName);
    g.setColour (getColourHighlight());
    g.drawText (getName(),
                center.getX() - 40, center.getY() - 18, 80, 18,
                juce::Justification::centred, true);

    // Value
    g.setFont (fontValue);
    g.setColour (getColourNormal());
    g.drawText (value,
                center.getX() - 40, center.getY() + 2, 80, 18,
                juce::Justification::centred, true);

    // cachedBounds = cachedBounds.getUnion (juce::Rectangle<float> (center.getX() - 30, center.getY() - 14, 60, 16));
}


juce::String ParametrableElement::getChoiceLabel() const
{
    if (choices.size() > (size_t)choiceIndex){
        return choices[choiceIndex].label;
    }
    return {};
}

float ParametrableElement::getChoiceValue() const
{
    if (choices.size() > (size_t)choiceIndex){
        return choices[choiceIndex].value;
    }
    return 0.0f;
}

void WireElement::createSignalPath (const int ) { 
    setSignalPath(true);
    CachedPath cachedWirePath {wirePath};
    cachedWirePath.rebuildCache();
    signalPaths.push_back(cachedWirePath);
}
void  WireElement::prepareToDraw ()
{
    jassert (terminals.size() == 2);
    const auto& start = terminals[0];
    const auto& end = terminals[1];

    wirePath.startNewSubPath(start);
    wirePath.lineTo(end);

    if (isSigPath)
        createSignalPath(0);
}

void  WireElement::draw (juce::Graphics& g) const
{

    drawGlowPath(g, wirePath, 0.0f, getColourNormal(),getColourAmber(),false);

    for (auto& sigpath : signalPaths)
        drawSignalPath(g, sigpath);
}

void WireElement::updateSignalPath () {
    float t=0.0f;
    if (getNumMonitors()> 0)
        t = getRMSValue(0) * POWER_SCALING; 

    for (auto& cachedPath : signalPaths)
    {
        updateCachedPath(t, SchematicElement::getClock(), cachedPath);
    }

};