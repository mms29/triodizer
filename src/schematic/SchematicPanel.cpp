#include "schematic/SchematicPanel.h"
#include "dsp/TriodeGainStage.h"
#include "dsp/BassmanPreamp.h"

#include <format>
//==============================================================================
SchematicPanel::SchematicPanel(SchematicPanelListener* l) : listener(l)
{
    setInterceptsMouseClicks (true, true);
}

void SchematicPanel::addElement (std::unique_ptr<SchematicElement> element)
{
    jassert (element != nullptr);
    
    if (auto* ctrlElem = dynamic_cast<ControllableElement*>(element.get()))
    {
        std::unique_ptr<Knob> controlKnob = std::make_unique<Knob>(
            [this, ctrlElem](float value) {
                ctrlElem->controlCallback(value, listener);
            },
            element->getName(),
            0.0f,
            100.0f,
            1.0f,
            " %");

        addAndMakeVisible(*controlKnob);
        controls.push_back (std::move (controlKnob));
    }
    elements.push_back (std::move (element));

}

void SchematicPanel::syncSchematicToCircuit(){
    std::cout << "Sync ..."<< std::endl;
    for (const auto& element : elements){

        if (auto* paramElem = dynamic_cast<ParametrableElement*>(element.get()))
        {
            float v = listener->getCircuitParam(paramElem->getParamIndex());
            std::cout <<"Searching for param element "<< element->getName()<<" value in choices ..."<< std::endl;
            int index = paramElem->getIndexChoiceFromValue(v);
            if (index< 0) {
                paramElem->addChoice(v, juce::String(v, 2));
                paramElem->setChoiceIndex(paramElem->getChoices().size()-1);
                std::cout <<"Not Found! "<< std::endl;
            }
            else{
                std::cout <<"Found close value at  "<< index<<std::endl;
                paramElem->setChoiceIndex(index);
            }

        }
        if (auto* ctrlElem = dynamic_cast<ControllableElement*>(element.get()))
        {
            std::cout <<"Searching for control element "<< element->getName()<<"..."<< std::endl;
            float v = listener->getCircuitControl(ctrlElem->getControlIndex());
            for (const auto& control : controls){
                if (control->getName() == element->getName()){
                    control->getSlider().setValue(v);
                    std::cout <<"Found control "<< control->getName()<< " and setting value " <<v<<std::endl;
                    break;
                }
            }
            ctrlElem->controlCallback(v, listener);
        }    
    }
    std::cout << "Done."<< std::endl;

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

void SchematicPanel::updateMonitoring ()
{
    for (const auto& element : elements){
        if (auto* voltmeter = dynamic_cast<VoltmeterElement*>(element.get())){
            voltmeter->setMonitorValue(listener->getCircuitMonitoring(voltmeter->getMonitorIndex()));
        }
    }
    repaint();
}
//==============================================================================
void SchematicPanel::paint (juce::Graphics& g)
{
    // Background
    g.fillAll (SCHEMATIC_BACKGROUND);

    // Draw wires first (behind elements)
    g.setColour (SCHEMATIC_NORMAL);
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
    auto area = getLocalBounds().reduced(4);
    auto right = area.removeFromRight(120);

    for (const auto& control : controls){
        control->setBounds(right.removeFromTop(120));
        right.removeFromTop(20);
    }

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
    if (auto* paramElem = dynamic_cast<ParametrableElement*>(element) )
    {

        juce::PopupMenu menu;
        auto choices = paramElem->getChoices();
        for (std::size_t i = 0; i < choices.size(); ++i)
        {
            menu.addItem ((int) i + 1, choices[i].label, true,
                        static_cast<int> (i) == paramElem->getChoiceIndex());
        }

        menu.showMenuAsync (juce::PopupMenu::Options(),
            [paramElem, this] (int result) mutable
            {
                if (result > 0)
                {
                    auto chosenIndex = result - 1;
                    auto choices = paramElem->getChoices();
                    if (chosenIndex >= 0 && chosenIndex < static_cast<int> (choices.size()))
                    {
                        paramElem->setChoiceIndex (static_cast<int> (chosenIndex));
                        repaint();

                        // Notify listener (e.g. PluginEditor) to update the DSP
                        if (listener != nullptr)
                        {
                            float newValue = paramElem->getChoiceValue();
                            listener->setCircuitParam (paramElem->getParamIndex(), newValue);
                        }
                    }
                }
            });
    }
}

void SchematicPanel::clear()
{
    elements.clear();
    controls.clear();
    wires.clear();
}


// ===================================================================================================================
// DEFAULT
// ===================================================================================================================
void buildDefault(SchematicPanel& schematic)
{
    auto t1 = Terminal {600, 300};
    auto t2 = Terminal {700, 300};
    schematic.addElement (std::make_unique<JunctionElement>(t1));
    schematic.addElement (std::make_unique<JunctionElement>(t2));
    schematic.addWire (t1, t2);
}


// ===================================================================================================================
// BASSMAN TONE STACK
// ===================================================================================================================
void buildBassmanToneStack(SchematicPanel& schematic)
{
    float d=170.0f;
    auto t1 = Terminal {500, 100};

    schematic.addElement (std::make_unique<ResistorElement> (
        "R4",
        std::vector<Terminal>
        {
            { t1  + Terminal {0.0f, d} },
            { t1 }
        },
        (int) BassmanToneStackCircuit::Param::R4,
        0,
        std::vector<ValueChoice>
        {
            { 56e3f, "56k" },
        }
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C1",
        std::vector<Terminal>
        {
            { t1  },
            { t1 + Terminal {d, 0.0f} }
        },
        (int) BassmanToneStackCircuit::Param::C1,
        0,
        std::vector<ValueChoice>
        {
            { 0.25e-9f, "250pF" },
        }
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C2",
        std::vector<Terminal>
        {
            { t1 + Terminal {0.0f, d} },
            { t1 + Terminal {d, d} }
        },
        (int) BassmanToneStackCircuit::Param::C2,
        0,
        std::vector<ValueChoice>
        {
            { 22e-9f, "22nF" },
        }
    ));

    schematic.addElement (std::make_unique<PotElement> (
        "Bass",
        std::vector<Terminal>
        {
            { schematic.getElement("C2")->getTerminals()[1] },
            { schematic.getElement("C2")->getTerminals()[1] + Terminal {0.0f, d} },
            { schematic.getElement("C2")->getTerminals()[1] + Terminal {0.5f*d, 0.5f*d} }
        },
        (int) BassmanToneStackCircuit::Control::Bass,
        (int) BassmanToneStackCircuit::Param::RBass,
        0,
        std::vector<ValueChoice>
        {
            { 250e3, "250K" },
        }
    ));

    schematic.addElement (std::make_unique<PotElement> (
        "Trebble",
        std::vector<Terminal>
        {
            { schematic.getElement("C1")->getTerminals()[1]},
            { schematic.getElement("C2")->getTerminals()[1] },
            { schematic.getElement("C1")->getTerminals()[1] + Terminal {0.25f*d, .5f*d} }
        },
        (int) BassmanToneStackCircuit::Control::Trebble,
        (int) BassmanToneStackCircuit::Param::RTrebble,
        0,
        std::vector<ValueChoice>
        {
            { 250e3, "250K" },
        }
    ));
    schematic.addElement (std::make_unique<PotElement> (
        "Mid",
        std::vector<Terminal>
        {
            { schematic.getElement("Bass")->getTerminals()[1]  + Terminal {0.0f, d} },
            { schematic.getElement("Bass")->getTerminals()[1]  },
            { schematic.getElement("Bass")->getTerminals()[1]  + Terminal {-d*0.25f, .5f*d} }
        },
        (int) BassmanToneStackCircuit::Control::Mid,
        (int) BassmanToneStackCircuit::Param::RMid,
        0,
        std::vector<ValueChoice>
        {
            { 10e3, "10K" },
        }
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C3",
        std::vector<Terminal>
        {
            { schematic.getElement("Mid")->getTerminals()[2]  + Terminal {-d*0.5f, 0.0f} },
            { schematic.getElement("Mid")->getTerminals()[2]  }
        },
        (int) BassmanToneStackCircuit::Param::C3,
        0,
        std::vector<ValueChoice>
        {
            { 22e-9f, "22nF" },
        }
    ));
    // Grounds
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Mid")->getTerminals()[0]) );

    schematic.addWire (
        schematic.getElement("C3")->getTerminals()[0] - Terminal{.25f*d, 0.0f},
        schematic.getElement("C3")->getTerminals()[0]
    );

    schematic.addWire (
        schematic.getElement("C3")->getTerminals()[0] - Terminal{.25f*d, 0.0f},
        schematic.getElement("C2")->getTerminals()[0]
    );
    schematic.addWire (
        schematic.getElement("Bass")->getTerminals()[2],
        schematic.getElement("C2")->getTerminals()[1] + Terminal {d*0.5f, 0.0f}
    );
    schematic.addWire (
        schematic.getElement("C2")->getTerminals()[1] ,
        schematic.getElement("C2")->getTerminals()[1] + Terminal {d*0.5f, 0.0f}
    );
    schematic.addWire (
        schematic.getElement("C1")->getTerminals()[0] ,
        schematic.getElement("C1")->getTerminals()[0] + Terminal {-d*0.5f, 0.0f}
    );
    schematic.addWire (
        schematic.getElement("Trebble")->getTerminals()[2] ,
        schematic.getElement("Trebble")->getTerminals()[2] + Terminal {d, 0.0f}
    );
    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("C1")->getTerminals()[0] + Terminal {-d*0.5f, 0.0f}));
    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Trebble")->getTerminals()[2] + Terminal {d, 0.0f}));

}

// ===================================================================================================================
// COMMON CATHODE STAGE
// ===================================================================================================================
void buildCommonCathodeStage(SchematicPanel& schematic)
{
    //TRIODE
    schematic.addElement (std::make_unique<TriodeElement> (
        "Triode", 
        Terminal {600, 360},
        (int) TriodeGainStage::Param::Triode,
        0,
        std::vector<ValueChoice>
        {
            { 0.0f, "12AX7" },
            { 0.0f, "12AT7" },
        }
    ));

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg",
        std::vector<Terminal>
        {
            {  schematic.getElement("Triode")->getTerminals()[0]  + Terminal {-100.0f, 0.0f} },
            { schematic.getElement("Triode")->getTerminals()[0] }
        },
        (int) TriodeGainStage::Param::Rg,
        1,
        std::vector<ValueChoice>
        {
            { 4.7e3f, "4.7k" },
            { 10e3f,  "10k" },
            { 20e3f,  "20k" },
            { 47e3f,  "47k" },
        }
    ));

    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ci",
        std::vector<Terminal>
        {
            { schematic.getElement("Rg")->getTerminals()[0]  + Terminal {-150.0f, 0.0f} },
            { schematic.getElement("Rg")->getTerminals()[0] }
        },
        (int) TriodeGainStage::Param::Ci,
        2,
        std::vector<ValueChoice>
        {
            { 22.0e-9f, "22nF" },
            { 47.0e-9f, "47nF" },
            { 100.0e-9f, "100nF" },
        }
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Ri",
        std::vector<Terminal>
        {
            { schematic.getElement("Rg")->getTerminals()[0] },
            { schematic.getElement("Rg")->getTerminals()[0] + Terminal {0.0f, 150.0f} }
        },
        (int) TriodeGainStage::Param::Ri,
        2,
        std::vector<ValueChoice>
        {
            { 0.22e6f, "220K" },
            { 0.47e6f, "470k" },
            { 1.0e6f,  "1M" },
            { 2.2e6f,  "2.2M" },
        }
    ));



    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk",
        std::vector<Terminal>
        {
            { schematic.getElement("Triode")->getTerminals()[2]+ Terminal { 0.0f, 150.0f} },
            { schematic.getElement("Triode")->getTerminals()[2]}
        },
        (int) TriodeGainStage::Param::Rk,
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
        }
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck",
        std::vector<Terminal>
        {
            { schematic.getElement("Rk")->getTerminals()[1] +  Terminal {50.0f, 0.0f} },
            { schematic.getElement("Rk")->getTerminals()[0] +  Terminal {50.0f, 0.0f} },
        },
        (int) TriodeGainStage::Param::Ck,
        2,
        std::vector<ValueChoice>
        {
            { 4.7e-6f, "4.7uF" },
            { 10e-6f, "10uF" },
            { 22e-6f, "22uF" },
            { 47e-6f, "47uF" },
        }
    ));

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp",
        std::vector<Terminal>
        {
            { schematic.getElement("Triode")->getTerminals()[1]},
            { schematic.getElement("Triode")->getTerminals()[1] + Terminal { 0.0f, -150.0f} }
        },
        (int) TriodeGainStage::Param::Rp,
        1,
        std::vector<ValueChoice>
        {
            { 47e3f, "47k" },
            { 100.0e3f, "100k" },
            { 220.0e3f, "220K" },
        }
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Co",
        std::vector<Terminal>
        {
            { schematic.getElement("Triode")->getTerminals()[1]},
            { schematic.getElement("Triode")->getTerminals()[1] + Terminal { 250.0f, 0.0f} }
        },
        (int) TriodeGainStage::Param::Co,
        1,
        std::vector<ValueChoice>
        {
            { 22.0e-9f, "22nF" },
            { 47.0e-9f, "47nF" },
            { 100.0e-9f, "100nF" },
        }
    ));


    schematic.addElement (std::make_unique<PotElement> (
        "Volume",
        std::vector<Terminal>
        {
            { schematic.getElement("Co")->getTerminals()[1]},
            { schematic.getElement("Co")->getTerminals()[1] + Terminal { 0.0f, 150.0f} },
            { schematic.getElement("Co")->getTerminals()[1] + Terminal { 100.0f, 75.0f} }
        },
        (int) TriodeGainStage::Control::Volume,
        (int) TriodeGainStage::Param::Volume,
        2,
        std::vector<ValueChoice>
        {
            { 0.22e6f, "220K" },
            { 0.47e6f, "470k"},
            { 1.0e6f,  "1M" },
            { 2.2e6f,  "2.2M"},
        }
    ));


    //Voltages
    schematic.addElement (std::make_unique<VoltageElement>(
        "B+",
        std::vector<Terminal>{schematic.getElement("Rp")->getTerminals()[1]}, 
        (int) TriodeGainStage::Param::E,
        0, 
        std::vector<ValueChoice>{
            { 250, "250V" },
            { 275, "275V" },
            { 300, "300V" },
            { 325, "325V" },
        }
    ));

    // Grounds
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ri")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Volume")->getTerminals()[1]) );

    // Grid voltage — near the grid terminal of the triode
    auto* triodeElem = schematic.getElement ("Triode");
    jassert (triodeElem != nullptr);
    const auto& triodeTerminals = triodeElem->getTerminals();

    // Plate voltage — near the plate terminal
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vp", 
        triodeTerminals[1] + Terminal { -50.0f, 0.0f},
        (int) TriodeGainStage::Monitoring::Vp
    ));

    // Cathode voltage — near the cathode terminal
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vk", 
        triodeTerminals[2] + Terminal { -50.0f, 0.0f},
        (int) TriodeGainStage::Monitoring::Vk
    ));


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









// ===================================================================================================================
// BASSMAN PREAMP
// ===================================================================================================================
void buildBassmanPreamp(SchematicPanel& schematic)
{
    using Param         =  BassmanPreampCircuit::Param;
    using Control       =  BassmanPreampCircuit::Control;
    using Monitoring    =  BassmanPreampCircuit::Monitoring;

    //TRIODE
    schematic.addElement (std::make_unique<TriodeElement> (
        "Triode", 
        Terminal {450, 350},
        (int) Param::Triode,
        0,
        std::vector<ValueChoice>
        {
            { 0.0f, "12AX7" },
            { 0.0f, "12AT7" },
        }
    ));

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg",
        std::vector<Terminal>
        {
            {  schematic.getElement("Triode")->getTerminals()[0]  + Terminal {-100.0f, 0.0f} },
            { schematic.getElement("Triode")->getTerminals()[0] }
        },
        (int) Param::Rg,
        1,
        std::vector<ValueChoice>
        {
            { 4.7e3f, "4.7k" },
            { 10e3f,  "10k" },
            { 20e3f,  "20k" },
            { 47e3f,  "47k" },
        }
    ));

    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ci",
        std::vector<Terminal>
        {
            { schematic.getElement("Rg")->getTerminals()[0]  + Terminal {-150.0f, 0.0f} },
            { schematic.getElement("Rg")->getTerminals()[0] }
        },
        (int) Param::Ci,
        2,
        std::vector<ValueChoice>
        {
            { 22.0e-9f, "22nF" },
            { 47.0e-9f, "47nF" },
            { 100.0e-9f, "100nF" },
        }
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Ri",
        std::vector<Terminal>
        {
            { schematic.getElement("Rg")->getTerminals()[0] },
            { schematic.getElement("Rg")->getTerminals()[0] + Terminal {0.0f, 150.0f} }
        },
        (int) Param::Ri,
        2,
        std::vector<ValueChoice>
        {
            { 0.22e6f, "220K" },
            { 0.47e6f, "470k" },
            { 1.0e6f,  "1M" },
            { 2.2e6f,  "2.2M" },
        }
    ));



    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk",
        std::vector<Terminal>
        {
            { schematic.getElement("Triode")->getTerminals()[2]+ Terminal { 0.0f, 150.0f} },
            { schematic.getElement("Triode")->getTerminals()[2]}
        },
        (int) Param::Rk,
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
        }
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck",
        std::vector<Terminal>
        {
            { schematic.getElement("Rk")->getTerminals()[1] +  Terminal {50.0f, 0.0f} },
            { schematic.getElement("Rk")->getTerminals()[0] +  Terminal {50.0f, 0.0f} },
        },
        (int) Param::Ck,
        2,
        std::vector<ValueChoice>
        {
            { 4.7e-6f, "4.7uF" },
            { 10e-6f, "10uF" },
            { 22e-6f, "22uF" },
            { 47e-6f, "47uF" },
        }
    ));

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp",
        std::vector<Terminal>
        {
            { schematic.getElement("Triode")->getTerminals()[1]},
            { schematic.getElement("Triode")->getTerminals()[1] + Terminal { 0.0f, -150.0f} }
        },
        (int) Param::Rp,
        1,
        std::vector<ValueChoice>
        {
            { 47e3f, "47k" },
            { 100.0e3f, "100k" },
            { 220.0e3f, "220K" },
        }
    ));

    //Voltages
    schematic.addElement (std::make_unique<VoltageElement>(
        "B+",
        std::vector<Terminal>{schematic.getElement("Rp")->getTerminals()[1]}, 
        (int) Param::E,
        0, 
        std::vector<ValueChoice>{
            { 250, "250V" },
            { 275, "275V" },
            { 300, "300V" },
            { 325, "325V" },
        }
    ));

    float d=160.0f;
    auto t1 = schematic.getElement("Triode")->getTerminals()[1] + Terminal {d*1.5f, 0.0f};

    schematic.addElement (std::make_unique<ResistorElement> (
        "R4",
        std::vector<Terminal>
        {
            { t1  + Terminal {0.0f, d} },
            { t1 }
        },
        (int) Param::R4,
        0,
        std::vector<ValueChoice>
        {
            { 56e3f, "56k" },
        }
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C1",
        std::vector<Terminal>
        {
            { t1  },
            { t1 + Terminal {d, 0.0f} }
        },
        (int) Param::C1,
        0,
        std::vector<ValueChoice>
        {
            { 0.25e-9f, "250pF" },
        }
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C2",
        std::vector<Terminal>
        {
            { t1 + Terminal {0.0f, d} },
            { t1 + Terminal {d, d} }
        },
        (int) Param::C2,
        0,
        std::vector<ValueChoice>
        {
            { 22e-9f, "22nF" },
        }
    ));

    schematic.addElement (std::make_unique<PotElement> (
        "Bass",
        std::vector<Terminal>
        {
            { schematic.getElement("C2")->getTerminals()[1] },
            { schematic.getElement("C2")->getTerminals()[1] + Terminal {0.0f, d*0.6f} },
            { schematic.getElement("C2")->getTerminals()[1] + Terminal {0.25f*d, 0.3f*d} }
        },
        (int) Control::Bass,
        (int) Param::RBass,
        0,
        std::vector<ValueChoice>
        {
            { 250e3, "250K" },
        }
    ));

    schematic.addElement (std::make_unique<PotElement> (
        "Trebble",
        std::vector<Terminal>
        {
            { schematic.getElement("C1")->getTerminals()[1]},
            { schematic.getElement("C2")->getTerminals()[1] },
            { schematic.getElement("C1")->getTerminals()[1] + Terminal {0.25f*d, .5f*d} }
        },
        (int) Control::Trebble,
        (int) Param::RTrebble,
        0,
        std::vector<ValueChoice>
        {
            { 250e3, "250K" },
        }
    ));
    schematic.addElement (std::make_unique<PotElement> (
        "Mid",
        std::vector<Terminal>
        {
            { schematic.getElement("Bass")->getTerminals()[1]  + Terminal {0.0f, d*0.6f} },
            { schematic.getElement("Bass")->getTerminals()[1]  },
            { schematic.getElement("Bass")->getTerminals()[1]  + Terminal {-d*0.25f, .3f*d} }
        },
        (int) Control::Mid,
        (int) Param::RMid,
        0,
        std::vector<ValueChoice>
        {
            { 10e3, "10K" },
        }
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C3",
        std::vector<Terminal>
        {
            { schematic.getElement("Mid")->getTerminals()[2]  + Terminal {-d*0.5f, 0.0f} },
            { schematic.getElement("Mid")->getTerminals()[2]  }
        },
        (int) Param::C3,
        0,
        std::vector<ValueChoice>
        {
            { 22e-9f, "22nF" },
        }
    ));
    // Grounds
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Mid")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ri")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck")->getTerminals()[1]) );

    auto* triodeElem = schematic.getElement ("Triode");
    jassert (triodeElem != nullptr);
    const auto& triodeTerminals = triodeElem->getTerminals();

    // Plate voltage — near the plate terminal
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vp", 
        triodeTerminals[1] + Terminal { -50.0f, 0.0f},
        (int) Monitoring::Vp
    ));

    // Cathode voltage — near the cathode terminal
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vk", 
        triodeTerminals[2] + Terminal { -50.0f, 0.0f},
        (int) Monitoring::Vk
    ));


    // Wires
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
        schematic.addWire (
        schematic.getElement("C3")->getTerminals()[0] - Terminal{.25f*d, 0.0f},
        schematic.getElement("C3")->getTerminals()[0]
    );

    schematic.addWire (
        schematic.getElement("C3")->getTerminals()[0] - Terminal{.25f*d, 0.0f},
        schematic.getElement("C2")->getTerminals()[0]
    );
    schematic.addWire (
        schematic.getElement("Bass")->getTerminals()[2],
        schematic.getElement("C2")->getTerminals()[1] + Terminal {d*0.25f, 0.0f}
    );
    schematic.addWire (
        schematic.getElement("C2")->getTerminals()[1] ,
        schematic.getElement("C2")->getTerminals()[1] + Terminal {d*0.25f, 0.0f}
    );
    schematic.addWire (
        schematic.getElement("C1")->getTerminals()[0] ,
        schematic.getElement("C1")->getTerminals()[0] + Terminal {-d*0.5f, 0.0f}
    );
    schematic.addWire (
        schematic.getElement("Trebble")->getTerminals()[2] ,
        schematic.getElement("Trebble")->getTerminals()[2] + Terminal {d, 0.0f}
    );
    schematic.addWire (
        schematic.getElement("Triode")->getTerminals()[1] ,
        schematic.getElement("C1")->getTerminals()[0] 
    );
    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Trebble")->getTerminals()[2] + Terminal {d, 0.0f}));
    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Ci")->getTerminals()[0]) );

}