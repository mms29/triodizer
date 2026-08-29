
#include "schematic/OneTermElement.h"

GroundElement::GroundElement (Terminal termPosition)
    : SchematicElement ("", std::vector<Terminal> {termPosition})
{
    jassert (terminals.size() == 1);
}

void GroundElement::prepareToDraw () {
    float groundSize = SCHEMATIC_GROUND_SIZE;

    const auto& p0 = terminals[0];

    path.startNewSubPath(p0);
    path.lineTo( p0.x, p0.y + groundSize);
    path.startNewSubPath(p0.x - groundSize, p0.y + groundSize);
    path.lineTo(p0.x + groundSize, p0.y + groundSize);
    path.startNewSubPath(p0.x - groundSize*0.66f, p0.y + groundSize*1.25f);
    path.lineTo(p0.x + groundSize*0.66f, p0.y + groundSize*1.25f);
    path.startNewSubPath(p0.x - groundSize*0.33f, p0.y + groundSize*1.5f);
    path.lineTo(p0.x + groundSize*0.33f, p0.y + groundSize*1.5f);

    cachedBounds = juce::Rectangle<float>(p0.x-groundSize, p0.y , groundSize, groundSize).expanded(6);

}
void GroundElement::draw (juce::Graphics& g) const
{
}

JunctionElement::JunctionElement (Terminal termPosition)
    : SchematicElement ("JUNCTION",std::vector<Terminal> {termPosition}),
    MonitoringElement(std::vector<int>{}), 
    InspectableElement()
{}
JunctionElement::JunctionElement (Terminal termPosition,
                     const int voltageMonitorIndex)
    : SchematicElement ("JUNCTION",std::vector<Terminal> {termPosition}),
    MonitoringElement(std::vector<int>{voltageMonitorIndex}), 
    InspectableElement()
{}

juce::AttributedString JunctionElement::getInspectContent () 
{
    juce::AttributedString textContent;
    auto font = juce::Font (juce::FontOptions(FONT_SUB1));
    if (getNumMonitors() == 1){
        float v = getSmoothedValue(0, MONITOR_PORT_V);
        textContent.append ("Voltage : \n", font, getColourNormal());
        textContent.append ("\t"+ juce::String(v, 1)+" VDC\n", font, getColourElectrical());
        textContent.append ("\t"+ juce::String(getRMSValue(0, MONITOR_PORT_V), 1)+" VAC\n", font, getColourElectrical());
    }
    return textContent;
};


void JunctionElement::prepareToDraw () {

    const auto& p0 = terminals[0];

    float radius = SCHEMATIC_JUNC_SIZE;

    cachedBounds = juce::Rectangle<float> (p0, p0);
    cachedBounds.expand(radius*2, radius*2);

    path.addEllipse(p0.x - radius*0.5f, p0.y - radius*0.5f, radius, radius);
    path.addEllipse(p0.x - radius*0.25f, p0.y - radius*0.25f, radius*0.5f, radius*0.5f);

}

void JunctionElement::draw (juce::Graphics& g) const
{
}
JackElement::JackElement (juce::String name, Terminal termPosition, int justification)
    : SchematicElement (name, std::vector<Terminal> {termPosition}),
      justification (justification)
{
    jassert (terminals.size() == 1);
}

void JackElement::prepareToDraw () {

    const auto& p0 = terminals[0];

    float radius = SCHEMATIC_JACK_SIZE;
    float radiusInner = radius*0.6;

    auto center = p0;
    Terminal lineStart ;
    if (justification == JUSTIFY_LEFT){
        center = p0 + Terminal {radius, 0.0f};
        lineStart = center - Terminal {radius*0.5f, 0.0f};
    }
    else if (justification == JUSTIFY_RIGHT){
        center = p0 - Terminal {radius, 0.0f};
        lineStart = center + Terminal {radius*0.5f, 0.0f};
    }
    else if (justification == JUSTIFY_TOP){
        center = p0 + Terminal {0.0f, radius};
        lineStart = center - Terminal {0.0f, radius*0.5f};
    }
    else if (justification == JUSTIFY_BOTTOM){
        center = p0 - Terminal {0.0f, radius};
        lineStart = center + Terminal {0.0f, radius*0.5f};
    }

    path.addEllipse(center.x - radius*0.5f, center.y - radius*0.5f, radius, radius);
    path.addEllipse(center.x - radiusInner*0.5f, center.y - radiusInner*0.5f, radiusInner, radiusInner);
    path.startNewSubPath(lineStart);
    path.lineTo(p0);

    labelCenter = center;

    const float t= radius;
    const float r = t * 0.5f;

    const float left   = std::min(center.x - r, p0.x);
    const float right  = std::max(center.x + r, p0.x);
    const float top    = std::min(center.y - r, p0.y);
    const float bottom = std::max(center.y + r, p0.y);

    cachedBounds = juce::Rectangle<float>(
        left,
        top,
        right - left,
        bottom - top
    ).expanded(6);


}
void JackElement::draw (juce::Graphics& g) const
{
    float radius = SCHEMATIC_JACK_SIZE;
    float radiusInner = radius*0.6;

    g.setColour(getColourHighlight());
    g.setFont(juce::FontOptions(FONT_TITLE));
    g.drawText(getName(), juce::Rectangle<float>{labelCenter.x - radius*1.5f, labelCenter.y - radius*1.5f, radius*3, radius*3}, juce::Justification::centredTop);

}

juce::AttributedString VoltageElement::getInspectContent () 
{
    juce::AttributedString textContent;
    auto font = juce::Font (juce::FontOptions(FONT_SUB1));
    if (getNumMonitors() == 1){
        float v = getSmoothedValue(0, MONITOR_PORT_V);
        textContent.append ("Voltage : \n", font, getColourNormal());
        textContent.append ("\t"+ juce::String(v, 1)+" VDC\n", font, getColourElectrical());
        textContent.append ("\t"+ juce::String(getRMSValue(0, MONITOR_PORT_V), 1)+" VAC\n", font, getColourElectrical());
    }
    return textContent;
};


void VoltageElement::createSignalPaths () 
{
        signalPaths[0].addPath(leftpath);
        signalPaths[0].addPath(rightpath);
}

void VoltageElement::updateSignalPaths () {
    if (getNumMonitors()> 0){
        signalPaths[0].updateSignalPath(
            getSmoothedValue(0, MONITOR_PORT_I) * INTENSITY_SCALING,
            getSmoothedValue(0, MONITOR_PORT_V),
            getRMSValue(0, MONITOR_PORT_I)*getRMSValue(0, MONITOR_PORT_V) *POWER_SCALING
        );
    }
};
float VoltageElement:: labelToValue (const juce::String s) const
{
    auto str = s.trim().toLowerCase();

    if (str.isEmpty())
        return getValue(); // fallback

    double multiplier = 1.0;
    if (str.endsWith ("v"))
        str = str.dropLastCharacters (1);

    // suffix handling
    if (str.endsWith ("k") || str.endsWith ("kv"))
    {
        multiplier = 1e3;
        str = str.dropLastCharacters (1);
    }
    else if (str.endsWith ("m")|| str.endsWith ("mv"))
    {
        multiplier = 1e-3;
        str = str.dropLastCharacters (1);
    }

    auto numeric = str.getFloatValue();
    if (numeric == 0.0) return getValue(); // fallback to previous valid value

    return (float) (numeric * multiplier);
}

juce::String VoltageElement::valueToLabel (float v) const
{
    if (v >= 1e3) return juce::String (v / 1e3, 0) + "kV";
    if (v >= 1) return juce::String (v, 0) + "V";
    if (v < 1 ) return juce::String (v * 1e3, 2) + "mV";
    return juce::String (v);
}
void VoltageElement::prepareToDraw () {

    float groundSize =SCHEMATIC_GROUND_SIZE;

    const auto& p0 = terminals[0];


    leftpath.startNewSubPath(p0.x , p0.y - groundSize*2);
    leftpath.lineTo(p0.x + groundSize, p0.y - groundSize);
    leftpath.lineTo(p0.x, p0.y - groundSize);

    rightpath.startNewSubPath(p0.x , p0.y - groundSize*2);
    rightpath.lineTo(p0.x - groundSize, p0.y - groundSize);
    rightpath.lineTo(p0.x, p0.y - groundSize);

    rightpath.startNewSubPath(p0.x, p0.y - groundSize);
    rightpath.lineTo(p0.x, p0.y);

    path.addPath(leftpath);
    path.addPath(rightpath);
    cachedBounds = juce::Rectangle<float>(p0.x-groundSize, p0.y-groundSize*2 , groundSize*2, groundSize*2);

    labelCenter =  p0 - Terminal {0.0f, groundSize*3.4f};
}
void VoltageElement::draw (juce::Graphics& g) const
{
    drawLabel(g,labelCenter, label);
}

void VoltmeterElement::draw (juce::Graphics& g) const
{
    const auto& p0 = terminals[0];


    // Circle background
    g.setColour (juce::Colours::darkgrey);
    g.fillEllipse (p0.x - VOLTMETER_RADIUS * 2.0f, p0.y - VOLTMETER_RADIUS,
                   VOLTMETER_RADIUS * 4.0f, VOLTMETER_RADIUS * 2.0f);

    // Border
    g.setColour (getColourNormal());
    g.drawEllipse (p0.x - VOLTMETER_RADIUS * 2.0f, p0.y - VOLTMETER_RADIUS,
                   VOLTMETER_RADIUS * 4.0f, VOLTMETER_RADIUS * 2.0f, 2.0f);

    // Voltage text
    juce::Font font (juce::FontOptions (16.0f));
    g.setFont (font);
    g.setColour (juce::Colours::greenyellow);

    juce::String text = juce::String (getMonitorValue(0, MONITOR_PORT_V), 1) + " V";
    g.drawText (text,
                p0.x - VOLTMETER_RADIUS * 2.0f + 4,
                p0.y - VOLTMETER_RADIUS + 4,
                VOLTMETER_RADIUS * 4.0f - 8,
                VOLTMETER_RADIUS * 2.0f - 8,
                juce::Justification::centred, true);
}

bool VoltmeterElement::hitTest (juce::Point<float> point) const
{
    return point.getDistanceFrom (terminals[0]) < VOLTMETER_RADIUS;
}

