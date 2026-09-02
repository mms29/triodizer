#include "schematic/SchematicElement.h"

bool SchematicElement::isHighlighted() const noexcept  { return highlighted; }
void SchematicElement::setHighlighted (bool should) noexcept { highlighted = should; }

void SignalElement::clearSignalReferences (){
    for (auto& s : signalPaths)
        s.clearReference();
}

bool SchematicElement::hitTest (juce::Point<float> point) const
{
    for (const auto& terminal : terminals)
        if (terminal.getDistanceFrom (point) < 12.0f)
            return true;
    if (cachedBounds.expanded (6.0f).contains (point))
        return true;
    return false;
}

const std::vector<Terminal>& SchematicElement::getTerminals() const noexcept
{
    return terminals;
}

const juce::String& SchematicElement::getName() const noexcept       { return name; }
void SchematicElement::drawLabel(juce::Graphics& g, Terminal center, juce::String value, const int width) const{
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
                center.getX() - width/2, center.getY() - 18, width, 18,
                juce::Justification::centred, true);

    // Value
    g.setFont (fontValue);
    g.setColour (getColourNormal());
    g.drawText (value,
                center.getX() - width/2, center.getY() + 2, width, 18,
                juce::Justification::centred, true);

    // cachedBounds = cachedBounds.getUnion (juce::Rectangle<float> (center.getX() - 30, center.getY() - 14, 60, 16));
}

void SchematicElement::addPointToTerminal(Terminal t, const int termIndex,  const bool direction)
{
    auto& tt = terminals[termIndex];
    juce::Path newPath;
    
    if (direction){
        juce::Path tmpPath = path;
        newPath.startNewSubPath(t);
        newPath.lineTo(tt);
        path.clear();
        path.addPath(newPath);
        path.addPath(tmpPath);

    }else{
        newPath.startNewSubPath(tt);
        newPath.lineTo(t);
        path.addPath(newPath);
    }
    tt = t;
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

juce::String ElementParam::format (const float v) const
{
    if (valueToLabel != nullptr)
        return valueToLabel (v);
    return juce::String (v);
}

juce::String ElementParam::getLabel() const
{
    if (type == ParamType::Choice)
    {
        if (choices.size() > (size_t) choiceIndex)
            return choices[(size_t) choiceIndex].label;
        return {};
    }
    return format (value);
}

float ElementParam::getValue() const
{
    if (type == ParamType::Choice)
    {
        if (choices.size() > (size_t) choiceIndex)
            return choices[(size_t) choiceIndex].value;
        return 0.0f;
    }
    return value;
}

void ElementParam::setValue (const float v)
{
    if (type == ParamType::Choice)
    {
        const int index = getIndexChoiceFromValue (v);
        if (index >= 0)
            choiceIndex = index;
        return;
    }
    value = v;
}

bool ElementParam::setFromLabel (const juce::String& s)
{
    if (s.trim().isEmpty())
        return false;

    if (type == ParamType::Choice)
    {
        // Match by label first, then fall through to value parsing
        for (int i = 0; i < (int) choices.size(); ++i)
            if (choices[(size_t) i].label == s)
            {
                choiceIndex = i;
                return true;
            }
    }

    const auto parse = labelToValue != nullptr
        ? labelToValue
        : [] (const juce::String& str) { return str.getFloatValue(); };

    setValue (parse (s));
    return true;
}

int ElementParam::getIndexChoiceFromValue (const float v) const
{
    int index = 0;
    for (const auto& choice : choices)
    {
        if (nearlyEqual (choice.value, v))
            return index;
        index++;
    }
    return -1;
}

void WireElement::createSignalPaths () { 
    signalPaths[0].addPath(path, 0.0f, 0.0f);

}
void  WireElement::prepareToDraw ()
{
    path.startNewSubPath(terminals[0]);
    for (int i=1; i< terminals.size(); i++){
        path.lineTo(terminals[i]);
    }

}

void  WireElement::draw (juce::Graphics& g) const
{
}



void WireElement::updateSignalPaths () {
    if (getNumMonitors()> 0){
        signalPaths[0].updateSignalPath(
            getSmoothedValue(0, MONITOR_PORT_I) * INTENSITY_SCALING,
            getSmoothedValue(0, MONITOR_PORT_V),
            getRMSValue(0, MONITOR_PORT_I)*getRMSValue(0, MONITOR_PORT_V) *POWER_SCALING
        );
    }
};
