#include "schematic/SchematicPanel.h"

//==============================================================================
SchematicPanel::SchematicPanel(SchematicPanelListener* l) : listener(l)
{
    setInterceptsMouseClicks (true, true);
}

void SchematicPanel::addElement (std::unique_ptr<SchematicElement> element)
{
    jassert (element != nullptr);
    elements.push_back (std::move (element));
    
    if (element->isControlable())
    {
        std::unique_ptr<Knob> driveKnob = std::make_unique<Knob>(
            [this](float value) {return;},
            element->getName(),
            0.0f,
            100.0f,
            1.0f,
            " %");

        // controls.push_back (std::move (driveKnob));
    }
}

void SchematicPanel::addWire (juce::Point<float> start, juce::Point<float> end)
{
    wires.emplace_back (start, end);
}

int SchematicPanel::getNumElements() const noexcept
{
    return static_cast<int> (elements.size());
}

SchematicElement* SchematicPanel::getElement (juce::String name) const noexcept
{
    for (const auto& element : elements)
        if (element->getName() == name)
            return element.get();

    return nullptr;
}

void SchematicPanel::setMonitorVoltage (const juce::String& elementName, float voltage)
{
    if (auto* elem = getElement (elementName))
    {
        if (auto* voltmeter = dynamic_cast<VoltmeterElement*>(elem))
            {
                voltmeter->setMonitorValue(voltage);
                repaint();
            }
    }
}

//==============================================================================
void SchematicPanel::paint (juce::Graphics& g)
{
    // Background
    g.fillAll (juce::Colours::black);

    // Draw wires first (behind elements)
    g.setColour (juce::Colours::white);
    for (const auto& wire : wires)
    {
        g.drawLine (wire.start.x, wire.start.y,
                     wire.end.x,   wire.end.y, STROKE_NORMAL);
    }

    // Draw every element on top
    for (const auto& elem : elements)
    {
        jassert (elem != nullptr);
        elem->draw (g);
    }
}

void SchematicPanel::resized()
{
    // Elements use absolute positions, nothing to do here.
}

//==============================================================================
void SchematicPanel::mouseDown (const juce::MouseEvent& e)
{
    auto* hit = getElementAt (e.getPosition());
    if (hit != nullptr)
        showPopupMenuForElement (hit, e.getPosition());
}

void SchematicPanel::mouseMove (const juce::MouseEvent& e)
{
    auto* hit = getElementAt (e.getPosition());

    if (hit != hoveredElement)
    {
        if (hoveredElement != nullptr)
            hoveredElement->setHighlighted (false);

        hoveredElement = hit;

        if (hoveredElement != nullptr)
            hoveredElement->setHighlighted (true);

        repaint();
    }
}

void SchematicPanel::mouseExit (const juce::MouseEvent&)
{
    if (hoveredElement != nullptr)
    {
        hoveredElement->setHighlighted (false);
        hoveredElement = nullptr;
        repaint();
    }
}

//==============================================================================
SchematicElement* SchematicPanel::getElementAt (juce::Point<int> position) const
{
    // Search in reverse order so top-most elements are hit first.
    // Uses each element's own hitTest which checks both body and terminals.
    for (int i = static_cast<int> (elements.size()) - 1; i >= 0; --i)
    {
        const auto& elem = elements[static_cast<size_t> (i)];
        if (elem->hitTest (position.toFloat()))
            return elem.get();
    }
    return nullptr;
}

//==============================================================================
void SchematicPanel::showPopupMenuForElement (SchematicElement* element,
                                               juce::Point<int> pos)
{
    juce::PopupMenu menu;

    auto choices = element->getParameters();
    for (std::size_t i = 0; i < choices.size(); ++i)
    {
        menu.addItem ((int) i + 1, choices[i].label, true,
                      static_cast<int> (i) == element->getValueIndex());
    }

    menu.showMenuAsync (juce::PopupMenu::Options(),
        [element, this] (int result) mutable
        {
            if (result > 0)
            {
                auto chosenIndex = result - 1;
                auto choices = element->getParameters();
                if (chosenIndex >= 0 && chosenIndex < static_cast<int> (choices.size()))
                {
                    element->setValueIndex (static_cast<int> (chosenIndex));
                    repaint();

                    // Notify listener (e.g. PluginEditor) to update the DSP
                    if (listener != nullptr)
                    {
                        float newValue = element->getParamValue();
                        listener->schematicParameterChanged (element->getName(),
                                                             newValue);
                    }
                }
            }
        });
}



void buildCommonCathodeStage(SchematicPanel& schematic)
{

    //TRIODE
    schematic.addElement (std::make_unique<TriodeElement> (
        "Triode", 0,
        std::vector<ValueChoice>
        {
            { 0.0f, "12AX7" },
            { 0.0f, "12AT7" },
        },
        Terminal {600, 360}
    ));

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg",
        1,
        std::vector<ValueChoice>
        {
            { 4.7e3f, "4.7k" },
            { 10e3f,  "10k" },
            { 20e3f,  "20k" },
            { 47e3f,  "47k" },
        },
        std::vector<Terminal>
        {
            {  schematic.getElement("Triode")->getTerminals()[0]  + Terminal {-100.0f, 0.0f} },
            { schematic.getElement("Triode")->getTerminals()[0] }
        }
    ));

    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ci",
        2,
        std::vector<ValueChoice>
        {
            { 22.0e-9f, "22nF" },
            { 47.0e-9f, "47nF" },
            { 100.0e-9f, "100nF" },
        },
        std::vector<Terminal>
        {
            { schematic.getElement("Rg")->getTerminals()[0]  + Terminal {-150.0f, 0.0f} },
            { schematic.getElement("Rg")->getTerminals()[0] }
        }
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Ri",
        2,
        std::vector<ValueChoice>
        {
            { 0.22e6f, "220K" },
            { 0.47e6f, "470k" },
            { 1.0e6f,  "1M" },
            { 2.2e6f,  "2.2M" },
        },
        std::vector<Terminal>
        {
            { schematic.getElement("Rg")->getTerminals()[0] },
            { schematic.getElement("Rg")->getTerminals()[0] + Terminal {0.0f, 150.0f} }
        }
    ));



    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk",
        2,
        std::vector<ValueChoice>
        {
            { 820.0f, "820R" },
            { 1.0e3f, "1k" },
            { 1.5e3f,  "1.5k" },
            { 2.2e3f,  "2.2k" },
            { 4.7e3f,  "4.7k" },
            { 6.8e3f,  "6.8k" },
            { 10e3f,   "10k" },
        },
        std::vector<Terminal>
        {
            { schematic.getElement("Triode")->getTerminals()[2]+ Terminal { 0.0f, 150.0f} },
            { schematic.getElement("Triode")->getTerminals()[2]}
        }
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck",
        2,
        std::vector<ValueChoice>
        {
            { 4.7e-6f, "4.7uF" },
            { 10e-6f, "10uF" },
            { 22e-6f, "22uF" },
            { 47e-6f, "47uF" },
        },
        std::vector<Terminal>
        {
            { schematic.getElement("Rk")->getTerminals()[1] +  Terminal {50.0f, 0.0f} },
            { schematic.getElement("Rk")->getTerminals()[0] +  Terminal {50.0f, 0.0f} },
        }
    ));

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp",
        1,
        std::vector<ValueChoice>
        {
            { 47e3f, "47k" },
            { 100.0e3f, "100k" },
            { 220.0e3f, "220K" },
        },
        std::vector<Terminal>
        {
            { schematic.getElement("Triode")->getTerminals()[1]},
            { schematic.getElement("Triode")->getTerminals()[1] + Terminal { 0.0f, -150.0f} }
        }
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Co",
        1,
        std::vector<ValueChoice>
        {
            { 22.0e-9f, "22nF" },
            { 47.0e-9f, "47nF" },
            { 100.0e-9f, "100nF" },
        },
        std::vector<Terminal>
        {
            { schematic.getElement("Triode")->getTerminals()[1]},
            { schematic.getElement("Triode")->getTerminals()[1] + Terminal { 250.0f, 0.0f} }
        }
    ));


    schematic.addElement (std::make_unique<PotElement> (
        "Volume",
        2,
        std::vector<ValueChoice>
        {
            { 0.22e6f, "220K" },
            { 0.47e6f, "470k"},
            { 1.0e6f,  "1M" },
            { 2.2e6f,  "2.2M"},
        },
        std::vector<Terminal>
        {
            { schematic.getElement("Co")->getTerminals()[1]},
            { schematic.getElement("Co")->getTerminals()[1] + Terminal { 0.0f, 150.0f} },
            { schematic.getElement("Co")->getTerminals()[1] + Terminal { 100.0f, 75.0f} }
        }
    ));


    //Voltages
    schematic.addElement (std::make_unique<VoltageElement>(
        "B+", 0, 
        std::vector<ValueChoice>{
            { 250, "250V" },
            { 275, "275V" },
            { 300, "300V" },
            { 325, "325V" },
        },
        std::vector<Terminal>{schematic.getElement("Rp")->getTerminals()[1]}));

    // Grounds
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ri")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Volume")->getTerminals()[1]) );

    // Voltmeters (read-only monitor elements)
    // Grid voltage — near the grid terminal of the triode
    auto* triodeElem = schematic.getElement ("Triode");
    jassert (triodeElem != nullptr);
    const auto& triodeTerminals = triodeElem->getTerminals();

    // Plate voltage — near the plate terminal
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vp", triodeTerminals[1] + Terminal { -50.0f, 0.0f} ));

    // Cathode voltage — near the cathode terminal
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vk", triodeTerminals[2] + Terminal { -50.0f, 0.0f} ));


    // Wires
    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Ci")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Volume")->getTerminals()[2]) );
    schematic.addWire (
        schematic.getElement("Ck")->getTerminals()[0],
        schematic.getElement("Rk")->getTerminals()[1]
    );
    schematic.addWire (
         triodeTerminals[2] ,
          triodeTerminals[2] + Terminal { -50.0f, 0.0f}
    );
    schematic.addWire (
         triodeTerminals[1] ,
          triodeTerminals[1] + Terminal { -50.0f, 0.0f}
    );
}