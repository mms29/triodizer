#include "schematic/SchematicPanel.h"
#include "dsp/TriodeGainStage.h"
#include "dsp/BassmanPreamp.h"
#include "dsp/FullBassmanPreamp.h"
#include "dsp/DualRectifierPreamp.h"

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
    for (const auto& element : elements){

        if (auto* paramElem = dynamic_cast<ParametrableElement*>(element.get()))
        {
            float v = listener->getCircuitParam(paramElem->getParamIndex());
            int index = paramElem->getIndexChoiceFromValue(v);
            if (index< 0) {
                paramElem->addChoice(v, juce::String(v, 2));
                paramElem->setChoiceIndex(paramElem->getChoices().size()-1);
            }
            else{
                paramElem->setChoiceIndex(index);
            }

        }
        if (auto* ctrlElem = dynamic_cast<ControllableElement*>(element.get()))
        {
            float v = listener->getCircuitControl(ctrlElem->getControlIndex());
            for (const auto& control : controls){
                if (control->getName() == element->getName()){
                    control->getSlider().setValue(v);
                    break;
                }
            }
            ctrlElem->controlCallback(v, listener);
        }   
        if (auto* setElem = dynamic_cast<SettableElement*>(element.get()))
        {
            float v = listener->getCircuitParam(setElem->getParamIndex());
            setElem->setValue(v);
        } 
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

void SchematicPanel::updateMonitoring ()
{
    for (const auto& element : elements){
        if (auto* monitor = dynamic_cast<MonitoringElement*>(element.get())){

            for (int i = 0; i < monitor->getNumMonitors(); ++i)
            {
                monitor->setMonitorValue(
                    i,
                    listener->getCircuitMonitoring(monitor->getCircuitIndex(i))
                );
            }
        }
    }
    repaint();
}
//==============================================================================
void SchematicPanel::paint (juce::Graphics& g)
{
    // Background
    g.fillAll (SCHEMATIC_BACKGROUND);

    // Apply zoom and pan transform
    g.addTransform(juce::AffineTransform::scale(zoomFactor).followedBy(
        juce::AffineTransform::translation(viewOffset.x, viewOffset.y)));

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
void SchematicPanel::setZoom (float newZoom)
{
    zoomFactor = juce::jlimit(0.2f, 5.0f, newZoom);
    repaint();
}

void SchematicPanel::zoomIn()
{
    setZoom(zoomFactor * 1.2f);
}

void SchematicPanel::zoomOut()
{
    setZoom(zoomFactor / 1.2f);
}

void SchematicPanel::resetView()
{
    viewOffset = {0.0f, 0.0f};
    zoomFactor = 1.0f;
    initOffset();
    repaint();
}

void SchematicPanel::setOffset (juce::Point<float> newOffset)
{
    viewOffset = newOffset;
    repaint();
}

void SchematicPanel::initOffset()
{
    float minx = std::numeric_limits<float>::max();
    float miny = std::numeric_limits<float>::max();

    float maxx = std::numeric_limits<float>::lowest();
    float maxy = std::numeric_limits<float>::lowest();

    float meanx = 0.0f;
    float meany = 0.0f;
    int count = 0;

    for (auto& element : elements)
    {
        for (const auto& t : element->getTerminals())
        {
            minx = std::min(minx, t.x);
            miny = std::min(miny, t.y);

            maxx = std::max(maxx, t.x);
            maxy = std::max(maxy, t.y);

            meanx += t.x;
            meany += t.y;
            ++count;
        }
    }

    if (count == 0)
        return;

    const float centerX = meanx / (float) count;
    const float centerY = meany / (float) count;

    const float circuitWidth  = maxx - minx;
    const float circuitHeight = maxy - miny;

    constexpr float margin = 100.0f; // pixels

    const float availableWidth  = getWidth()  - 2.0f * margin;
    const float availableHeight = getHeight() - 2.0f * margin;

    const float zoomX = availableWidth  / circuitWidth;
    const float zoomY = availableHeight / circuitHeight;


    zoomFactor = std::min(zoomX, zoomY);

    // Optional clamp
    zoomFactor = juce::jlimit(0.2f, 5.0f, zoomFactor);

    // Center circuit in window
    viewOffset.x = getWidth()  * 0.5f - centerX * zoomFactor;
    viewOffset.y = getHeight() * 0.5f - centerY * zoomFactor;
}

// juce::Point<float> SchematicPanel::getViewPosition (juce::Point<float> worldPos) const
// {
//     return { (worldPos.x - viewOffset.x) / zoomFactor,
//              (worldPos.y - viewOffset.y) / zoomFactor };
// }

//==============================================================================
void SchematicPanel::mouseDown (const juce::MouseEvent& e)
{
    auto* hit = getElementAt (e.getPosition());
    if (hit != nullptr)
        showPopupMenuForElement (hit, e.getPosition());

    dragStartMouse  = e.position;
    dragStartOffset = viewOffset;

}

void SchematicPanel::mouseDrag (const juce::MouseEvent& e)
{
    const auto delta = e.position - dragStartMouse;

    viewOffset = dragStartOffset + delta;

    repaint();
}

void SchematicPanel::mouseUp (const juce::MouseEvent& e)
{
}

void SchematicPanel::mouseDoubleClick (const juce::MouseEvent& e)
{
}

//==============================================================================
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

void SchematicPanel::mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
    if (wheel.deltaY != 0)
    {
        auto oldZoom = zoomFactor;
        // Conventional: scroll up (negative deltaY) zooms in, scroll down zooms out
        zoomFactor = juce::jlimit(0.2f, 5.0f, wheel.deltaY < 0 ? zoomFactor * 1.1f : zoomFactor / 1.1f);

        // Zoom towards mouse position - adjust offset so mouse stays over same world point
        auto mousePos = e.position.toFloat();
        viewOffset.x = mousePos.x + (viewOffset.x - mousePos.x) * (zoomFactor / oldZoom);
        viewOffset.y = mousePos.y + (viewOffset.y - mousePos.y) * (zoomFactor / oldZoom);
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
    // Convert screen position to world coordinates (inverse of the view transform)
    // The transform applied in paint() is: scale(zoomFactor) then translate(viewOffset)
    // So to go from screen to world: (screenPos - viewOffset) / zoomFactor
    juce::Point<float> worldPos = {
        (position.x - viewOffset.x) / zoomFactor,
        (position.y - viewOffset.y) / zoomFactor
    };

    // Search in reverse order so top-most elements are hit first.
    // Uses each element's own hitTest which checks both body and terminals.
    for (int i = static_cast<int> (elements.size()) - 1; i >= 0; --i)
    {
        const auto& elem = elements[static_cast<size_t> (i)];
        if (elem->hitTest (worldPos))
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
    if (auto* setElem = dynamic_cast<SettableElement*>(element) )
    {
        auto* window = new juce::AlertWindow (element->getName(),
                                      "Enter value:",
                                      juce::AlertWindow::NoIcon);

        window->addTextEditor ("text", setElem->getLabel());
        auto* editor = window->getTextEditor("text");
        if (editor != nullptr)
        {
            editor->setJustification (juce::Justification::centred);
        }

        window->addButton ("OK", 1);
        window->addButton ("Cancel", 0);

        // window->setSize (320, 140); 

        window->enterModalState (
            true,
            juce::ModalCallbackFunction::create (
                [this, window, setElem] (int result)
                {
                    if (result == 1)
                    {
                        auto textValue = window->getTextEditorContents ("text");

                        setElem->setLabel(textValue);
                        listener->setCircuitParam (setElem->getParamIndex(), setElem->getValue());

                    }

                    window->exitModalState (0);
                    delete window;
                }));
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
void SchematicBuilder::buildDefault(SchematicPanel& schematic)
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
void SchematicBuilder::buildBassmanToneStack(SchematicPanel& schematic)
{
    float d=170.0f;
    auto toneStackPosition = Terminal {500, 100};

    schematic.addElement (std::make_unique<ResistorElement> (
        "R4",
        toneStackPosition + Terminal {0.0f, d} ,
        toneStackPosition,
        (int) BassmanToneStackCircuit::Param::R4
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C1",
        toneStackPosition,
        toneStackPosition+Terminal {d, 0.0f} ,
        (int) BassmanToneStackCircuit::Param::C1
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C2",
        toneStackPosition + Terminal {0.0f, d},
        toneStackPosition + Terminal {d, d} ,
        (int) BassmanToneStackCircuit::Param::C2
    ));

    schematic.addElement (std::make_unique<PotElement> (
        "Bass",
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1] + Terminal {0.0f, d},
        schematic.getElement("C2")->getTerminals()[1] + Terminal {0.5f*d, 0.5f*d},
        (int) BassmanToneStackCircuit::Control::Bass,
        (int) BassmanToneStackCircuit::Param::RBass
    ));

    schematic.addElement (std::make_unique<PotElement> (
        "Treble",
        schematic.getElement("C1")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C1")->getTerminals()[1] + Terminal {0.25f*d, .5f*d} ,
        (int) BassmanToneStackCircuit::Control::Treble,
        (int) BassmanToneStackCircuit::Param::RTreble
    ));
    schematic.addElement (std::make_unique<PotElement> (
        "Mid",     
        schematic.getElement("Bass")->getTerminals()[1]  + Terminal {0.0f, d},
        schematic.getElement("Bass")->getTerminals()[1],
        schematic.getElement("Bass")->getTerminals()[1]  + Terminal {-d*0.25f, .5f*d},
        (int) BassmanToneStackCircuit::Control::Mid,
        (int) BassmanToneStackCircuit::Param::RMid
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C3",
        schematic.getElement("Mid")->getTerminals()[2]  + Terminal {-d*0.5f, 0.0f},
        schematic.getElement("Mid")->getTerminals()[2]  ,
        (int) BassmanToneStackCircuit::Param::C3
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
        schematic.getElement("Treble")->getTerminals()[2] ,
        schematic.getElement("Treble")->getTerminals()[2] + Terminal {d, 0.0f}
    );
    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("C1")->getTerminals()[0] + Terminal {-d*0.5f, 0.0f}));
    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Treble")->getTerminals()[2] + Terminal {d, 0.0f}));

}

// ===================================================================================================================
// COMMON CATHODE STAGE
// ===================================================================================================================
// void buildCommonCathodeStage(SchematicPanel& schematic)
// {
//     //TRIODE
//     schematic.addElement (std::make_unique<TriodeElement> (
//         "Triode", 
//         Terminal {600, 360},
//         (int) TriodeGainStage::Param::Triode,
//         0, std::vector<ValueChoice>{{ 0.0f, "12AX7" },{ 0.0f, "12AT7" }}
//     ));

//     auto triodeTerms = schematic.getElement("Triode")->getTerminals();

//     schematic.addElement (std::make_unique<ResistorElement> (
//         "Rg",
//         triodeTerms[0]  + Terminal {-100.0f, 0.0f} ,
//         triodeTerms[0] ,
//         (int) TriodeGainStage::Param::Rg
//     ));

//     schematic.addElement (std::make_unique<CapacitorElement> (
//         "Ci",
//         schematic.getElement("Rg")->getTerminals()[0]  + Terminal {-150.0f, 0.0f},
//         schematic.getElement("Rg")->getTerminals()[0],
//         (int) TriodeGainStage::Param::Ci
//     ));
//     schematic.addElement (std::make_unique<ResistorElement> (
//         "Ri",
//         schematic.getElement("Rg")->getTerminals()[0] ,
//         schematic.getElement("Rg")->getTerminals()[0] + Terminal {0.0f, 150.0f},
//         (int) TriodeGainStage::Param::Ri
//     ));

//     schematic.addElement (std::make_unique<ResistorElement> (
//         "Rk",
//         triodeTerms[2]+ Terminal { 0.0f, 150.0f},
//         triodeTerms[2],
//         (int) TriodeGainStage::Param::Rk
//     ));
//     schematic.addElement (std::make_unique<CapacitorElement> (
//         "Ck",
//         schematic.getElement("Rk")->getTerminals()[1] +  Terminal {50.0f, 0.0f},
//         schematic.getElement("Rk")->getTerminals()[0] +  Terminal {50.0f, 0.0f},
//         (int) TriodeGainStage::Param::Ck
//     ));

//     schematic.addElement (std::make_unique<ResistorElement> (
//         "Rp",
//         triodeTerms[1],
//         triodeTerms[1] + Terminal { 0.0f, -150.0f} ,
//         (int) TriodeGainStage::Param::Rp
//     ));
//     schematic.addElement (std::make_unique<CapacitorElement> (
//         "Co",
//         std::vector<Terminal>
//         {
//             { triodeTerms[1]},
//             { triodeTerms[1] + Terminal { 250.0f, 0.0f} }
//         },
//         (int) TriodeGainStage::Param::Co
//     ));


//     schematic.addElement (std::make_unique<PotElement> (
//         "Volume",
//         std::vector<Terminal>
//         {
//             { schematic.getElement("Co")->getTerminals()[1]},
//             { schematic.getElement("Co")->getTerminals()[1] + Terminal { 0.0f, 150.0f} },
//             { schematic.getElement("Co")->getTerminals()[1] + Terminal { 100.0f, 75.0f} }
//         },
//         (int) TriodeGainStage::Control::Volume,
//         (int) TriodeGainStage::Param::Volume,
//         2,
//         std::vector<ValueChoice>
//         {
//             { 0.22e6f, "220K" },
//             { 0.47e6f, "470k"},
//             { 1.0e6f,  "1M" },
//             { 2.2e6f,  "2.2M"},
//         }
//     ));


//     //Voltages
//     schematic.addElement (std::make_unique<VoltageElement>(
//         "B+",
//         std::vector<Terminal>{schematic.getElement("Rp")->getTerminals()[1]}, 
//         (int) TriodeGainStage::Param::E
//     ));

//     // Grounds
//     schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ri")->getTerminals()[1]) );
//     schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk")->getTerminals()[0]) );
//     schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck")->getTerminals()[1]) );
//     schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Volume")->getTerminals()[1]) );

//     // Grid voltage — near the grid terminal of the triode
//     auto* triodeElem = schematic.getElement ("Triode");
//     jassert (triodeElem != nullptr);
//     const auto& triodeTerminals = triodeElem->getTerminals();

//     // Plate voltage — near the plate terminal
//     schematic.addElement (std::make_unique<VoltmeterElement> (
//         "Vp", 
//         triodeTerminals[1] + Terminal { -50.0f, 0.0f},
//         (int) TriodeGainStage::Monitoring::Vp
//     ));

//     // Cathode voltage — near the cathode terminal
//     schematic.addElement (std::make_unique<VoltmeterElement> (
//         "Vk", 
//         triodeTerminals[2] + Terminal { -50.0f, 0.0f},
//         (int) TriodeGainStage::Monitoring::Vk
//     ));


//     // Wires
//     schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Ci")->getTerminals()[0]) );
//     schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Volume")->getTerminals()[2]) );
//     schematic.addWire (
//         schematic.getElement("Ck")->getTerminals()[0],
//         schematic.getElement("Rk")->getTerminals()[1]
//     );
//     schematic.addWire (
//          triodeTerminals[2] ,
//           triodeTerminals[2] + Terminal { -50.0f, 0.0f}
//     );
//     schematic.addWire (
//          triodeTerminals[1] ,
//           triodeTerminals[1] + Terminal { -50.0f, 0.0f}
//     );
// }



// ===================================================================================================================
// BASSMAN PREAMP
// ===================================================================================================================
void SchematicBuilder::buildBassmanPreampSmall(SchematicPanel& schematic)
{
    using Param         =  BassmanPreampCircuit::Param;
    using Control       =  BassmanPreampCircuit::Control;
    using Monitoring    =  BassmanPreampCircuit::Monitoring;

    //TRIODE
    schematic.addElement (std::make_unique<TriodeElement> (
        "Triode", 
        Terminal {450, 350},
        (int) Param::Triode,
        0, std::vector<ValueChoice>{{ 0.0f, "12AX7" },{ 0.0f, "12AT7" } }
    ));
    auto triodeTerms = schematic.getElement("Triode")->getTerminals();

    auto toneStackPosition = triodeTerms[1] + rightXL;

    // Grid circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg",
        triodeTerms[0]  + leftM,
        triodeTerms[0] ,
        (int) Param::Rg 
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ci",
        schematic.getElement("Rg")->getTerminals()[0] + leftL,
        schematic.getElement("Rg")->getTerminals()[0],
        (int) Param::Ci
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Ri",
        schematic.getElement("Rg")->getTerminals()[0],
        schematic.getElement("Rg")->getTerminals()[0] + bottomL,
        (int) Param::Ri
    ));


    // Cathode Circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk",
        triodeTerms[2]+ bottomL,
        triodeTerms[2],
        (int) Param::Rk
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck",
        schematic.getElement("Rk")->getTerminals()[1] +  rightXS,
        schematic.getElement("Rk")->getTerminals()[0] +  rightXS,
        (int) Param::Ck
    ));

    // Plate Circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp",
        triodeTerms[1],
        triodeTerms[1] + topL,
        (int) Param::Rp
    ));
    schematic.addElement (std::make_unique<VoltageElement>(
        "B+",
        schematic.getElement("Rp")->getTerminals()[1], 
        (int) Param::E
    ));

    // Tone Stack
    schematic.addElement (std::make_unique<ResistorElement> (
        "R4",
        toneStackPosition + bottomL,
        toneStackPosition,
        (int) Param::R4
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C1",
        toneStackPosition,
        toneStackPosition + rightL,
        (int) Param::C1
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C2",
        toneStackPosition + bottomL,
        toneStackPosition + bottomL + rightL,
        (int) Param::C2
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C3",
        toneStackPosition + bottomL*2.0f + rightL*0.25f,
        toneStackPosition + bottomL*2.0f + rightL*0.75f,
        (int) Param::C3
    ));

    schematic.addElement (std::make_unique<PotElement> (
        "Treble",
        schematic.getElement("C1")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C1")->getTerminals()[1] + rightXS + bottomL*0.5f ,
        (int) Control::Treble,
        (int) Param::RTreble
    ));
    schematic.addElement (std::make_unique<PotElement> (
        "Bass",
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1] + bottomM,
        schematic.getElement("C2")->getTerminals()[1] + bottomM*0.5f + rightXS,
        (int) Control::Bass,
        (int) Param::RBass
    ));
    schematic.addElement (std::make_unique<PotElement> (
        "Mid",
        schematic.getElement("Bass")->getTerminals()[1]  + bottomM,
        schematic.getElement("Bass")->getTerminals()[1] ,
        schematic.getElement("Bass")->getTerminals()[1]  + bottomM*0.5f + leftXS,
        (int) Control::Mid,
        (int) Param::RMid
    ));
    // Grounds
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Mid")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ri")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck")->getTerminals()[1]) );

    // Monitors
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vp", 
        triodeTerms[1] + leftXS,
        (int) Monitoring::Vp
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vk", 
        triodeTerms[2] + leftXS,
        (int) Monitoring::Vk
    ));


    // Wires
    schematic.addWire (
        schematic.getElement("Ck")->getTerminals()[0],
        schematic.getElement("Rk")->getTerminals()[1]
    );
    schematic.addWire (
        schematic.getElement("C3")->getTerminals()[0] ,
        schematic.getElement("C3")->getTerminals()[0] + leftL*0.25f
    );
    schematic.addWire (
        schematic.getElement("C3")->getTerminals()[0]+ leftL*0.25f,
        schematic.getElement("C2")->getTerminals()[0]
    );
    schematic.addWire (
        schematic.getElement("Bass")->getTerminals()[2],
        schematic.getElement("C2")->getTerminals()[1] + rightXS
    );
    schematic.addWire (
        schematic.getElement("C2")->getTerminals()[1] ,
        schematic.getElement("C2")->getTerminals()[1] + rightXS
    );
    schematic.addWire (
        schematic.getElement("Treble")->getTerminals()[2] ,
        schematic.getElement("Treble")->getTerminals()[2] + rightL
    );
    schematic.addWire (
        triodeTerms[1] ,
        schematic.getElement("C1")->getTerminals()[0] 
    );

    // I/O
    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Treble")->getTerminals()[2] + rightL));
    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Ci")->getTerminals()[0]) );

}













// ===================================================================================================================
// DUAL RECTIFIER
// ===================================================================================================================
void SchematicBuilder::buildDualRectifierPreamp(SchematicPanel& schematic)
{
    using Param         =  DualRectifierPreampCircuit::Param;
    using Control       =  DualRectifierPreampCircuit::Control;
    using Monitoring    =  DualRectifierPreampCircuit::Monitoring;

    //V1
    schematic.addElement (std::make_unique<TriodeElement> (
        "V1", 
        Terminal {0, 0},
        (int) Param::V1,
        0, std::vector<ValueChoice>{{ 0.0f, "12AX7" },{ 0.0f, "12AT7" } },
        (int) Monitoring::Ik1
    ));
    auto V1pos = schematic.getElement("V1")->getTerminals();


    // Grid circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg1",
        V1pos[0]  + leftL,
        V1pos[0] ,
        (int) Param::Rg1 
    ));

    // Cathode Circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk1",
        V1pos[2]+ bottomL,
        V1pos[2],
        (int) Param::Rk1
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck1",
        schematic.getElement("Rk1")->getTerminals()[1] +  rightXS,
        schematic.getElement("Rk1")->getTerminals()[0] +  rightXS,
        (int) Param::Ck1
    ));

    // Plate Circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp1",
        V1pos[1],
        V1pos[1] + topL,
        (int) Param::Rp1
    ));
    schematic.addElement (std::make_unique<VoltageElement>(
        "E1",
        schematic.getElement("Rp1")->getTerminals()[1], 
        (int) Param::E1
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cp1",
        V1pos[1],
        V1pos[1] + rightXL,
        (int) Param::Cp1
    ));

    // Tone stack 1
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ca1",
        schematic.getElement("Cp1")->getTerminals()[1],
        schematic.getElement("Cp1")->getTerminals()[1] + rightM,
        (int) Param::Ca1
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Ra1",
        schematic.getElement("Ca1")->getTerminals()[1] + bottomXS,
        schematic.getElement("Ca1")->getTerminals()[0] + bottomXS,
        (int) Param::Ra1
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cb1",
        schematic.getElement("Ra1")->getTerminals()[1] + bottomL,
        schematic.getElement("Ra1")->getTerminals()[0] + bottomL,
        (int) Param::Cb1
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rb1",
        schematic.getElement("Cb1")->getTerminals()[1] + bottomXS,
        schematic.getElement("Cb1")->getTerminals()[0] + bottomXS,
        (int) Param::Rb1
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rc1",
        schematic.getElement("Rb1")->getTerminals()[1] + bottomM,
        schematic.getElement("Rb1")->getTerminals()[1],
        (int) Param::Rb1
    ));

    schematic.addElement (std::make_unique<PotElement> (
        "Volume",
        schematic.getElement("Ca1")->getTerminals()[1] + rightM,
        schematic.getElement("Ca1")->getTerminals()[1] + rightM + bottomXL,
        schematic.getElement("Ca1")->getTerminals()[1] + rightM + rightS + bottomXL*0.5f ,
        (int) Control::Volume,
        (int) Param::RVol
    ));

    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cbright",
        schematic.getElement("Volume")->getTerminals()[0] +bottomXS,
        schematic.getElement("Volume")->getTerminals()[0] + rightS+bottomXS,
        (int) Param::Cbright
    ));

    // V2
    schematic.addElement (std::make_unique<TriodeElement> (
        "V2", 
        schematic.getElement("Volume")->getTerminals()[2]+ rightL,
        (int) Param::V2,
        0, std::vector<ValueChoice>{{ 0.0f, "12AX7" },{ 0.0f, "12AT7" } },
        (int) Monitoring::Ik2
    ));
    auto V2pos = schematic.getElement("V2")->getTerminals();
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg2",
        schematic.getElement("Volume")->getTerminals()[2],
        V2pos[0],
        (int) Param::Rg2
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk2",
        V2pos[2]+ bottomL,
        V2pos[2],
        (int) Param::Rk2
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp2",
        V2pos[1],
        V2pos[1] + topL,
        (int) Param::Rp2
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck2",
        schematic.getElement("Rk2")->getTerminals()[1] +  rightXS,
        schematic.getElement("Rk2")->getTerminals()[0] +  rightXS,
        (int) Param::Ck2
    ));
    schematic.addElement (std::make_unique<VoltageElement>(
        "E2",
        schematic.getElement("Rp2")->getTerminals()[1], 
        (int) Param::E2
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cp2",
        V2pos[1],
        V2pos[1] + rightXL,
        (int) Param::Cp2
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Ra2",
        schematic.getElement("Cp2")->getTerminals()[1],
        schematic.getElement("Cp2")->getTerminals()[1] + bottomM,
        (int) Param::Ra2
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rb2",
        schematic.getElement("Ra2")->getTerminals()[1],
        schematic.getElement("Ra2")->getTerminals()[1] + bottomXL,
        (int) Param::Rb2
    ));

    // V3
    schematic.addElement (std::make_unique<TriodeElement> (
        "V3", 
        schematic.getElement("Ra2")->getTerminals()[1] +rightL,
        (int) Param::V3,
        0, std::vector<ValueChoice>{{ 0.0f, "12AX7" },{ 0.0f, "12AT7" } },
        (int) Monitoring::Ik3
    ));
    auto V3pos = schematic.getElement("V3")->getTerminals();

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk3",
        V3pos[2]+ bottomL,
        V3pos[2],
        (int) Param::Rk3
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp3",
        V3pos[1],
        V3pos[1] + topL,
        (int) Param::Rp3
    ));
    schematic.addElement (std::make_unique<VoltageElement>(
        "E3",
        schematic.getElement("Rp3")->getTerminals()[1], 
        (int) Param::E3
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cp3",
        V3pos[1],
        V3pos[1] + rightL,
        (int) Param::Cp3
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Ra3",
        schematic.getElement("Cp3")->getTerminals()[1]+ bottomM,
        schematic.getElement("Cp3")->getTerminals()[1]+ bottomM + bottomXL,
        (int) Param::Ra3
    ));


    // V4
    schematic.addElement (std::make_unique<TriodeElement> (
        "V4", 
        schematic.getElement("Ra3")->getTerminals()[0] +rightXL,
        (int) Param::V4,
        0, std::vector<ValueChoice>{{ 0.0f, "12AX7" },{ 0.0f, "12AT7" } },
        (int) Monitoring::Ik4
    ));
    auto V4pos = schematic.getElement("V4")->getTerminals();
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg4",
        V4pos[0]  + leftM,
        V4pos[0] ,
        (int) Param::Rg4
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk4",
        V4pos[2]+ bottomL,
        V4pos[2],
        (int) Param::Rk4
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp4",
        V4pos[1],
        V4pos[1] + topL,
        (int) Param::Rp4
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck4",
        schematic.getElement("Rk4")->getTerminals()[1] +  rightXS,
        schematic.getElement("Rk4")->getTerminals()[0] +  rightXS,
        (int) Param::Ck4
    ));
    schematic.addElement (std::make_unique<VoltageElement>(
        "E4",
        schematic.getElement("Rp4")->getTerminals()[1], 
        (int) Param::E4
    ));

    // V5
    schematic.addElement (std::make_unique<TriodeElement> (
        "V5", 
        V4pos[1] +rightXL,
        (int) Param::V5,
        0, std::vector<ValueChoice>{{ 0.0f, "12AX7" },{ 0.0f, "12AT7" } },
        (int) Monitoring::Ik5
    ));
    auto V5pos = schematic.getElement("V5")->getTerminals();
    schematic.addElement (std::make_unique<VoltageElement>(
        "E5",
        V5pos[1] + topXS, 
        (int) Param::E5
    ));

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk5",
        V5pos[2]+ bottomXL,
        V5pos[2],
        (int) Param::Rk5
    ));

    auto toneStackPosition = V5pos[2] + rightM;

    // Tone Stack
    schematic.addElement (std::make_unique<ResistorElement> (
        "R4",
        toneStackPosition + bottomL,
        toneStackPosition,
        (int) Param::R4
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C1",
        toneStackPosition,
        toneStackPosition + rightL,
        (int) Param::C1
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C2",
        toneStackPosition + bottomL,
        toneStackPosition + bottomL + rightL,
        (int) Param::C2
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C3",
        toneStackPosition + bottomL*2.0f + rightL*0.25f,
        toneStackPosition + bottomL*2.0f + rightL*0.75f,
        (int) Param::C3
    ));

    schematic.addElement (std::make_unique<PotElement> (
        "Treble",
        schematic.getElement("C1")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C1")->getTerminals()[1] + rightXS + bottomL*0.5f ,
        (int) Control::Treble,
        (int) Param::RTreble
    ));
    schematic.addElement (std::make_unique<PotElement> (
        "Bass",
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1] + bottomM,
        schematic.getElement("C2")->getTerminals()[1] + bottomM*0.5f + rightXS,
        (int) Control::Bass,
        (int) Param::RBass
    ));
    schematic.addElement (std::make_unique<PotElement> (
        "Mid",
        schematic.getElement("Bass")->getTerminals()[1]  + bottomM,
        schematic.getElement("Bass")->getTerminals()[1] ,
        schematic.getElement("Bass")->getTerminals()[1]  + bottomM*0.5f + leftXS,
        (int) Control::Mid,
        (int) Param::RMid
    ));

    // Grounds
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk1")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck1")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rc1")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk2")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck2")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Volume")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rb2")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk3")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ra3")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk4")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck4")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk5")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Mid")->getTerminals()[0]) );

    // Monitors
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vp1", 
        V1pos[1] + leftXS,
        (int) Monitoring::VDCp1
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vk1", 
        V1pos[2] + leftXS,
        (int) Monitoring::VDCk1
    ));

    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vp2", 
        V2pos[1] + leftXS,
        (int) Monitoring::VDCp2
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vk2", 
        V2pos[2] + leftXS,
        (int) Monitoring::VDCk2
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vp3", 
        V3pos[1] + leftXS,
        (int) Monitoring::VDCp3
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vk3", 
        V3pos[2] + leftXS,
        (int) Monitoring::VDCk3
    ));

    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vp4", 
        V4pos[1] + leftXS,
        (int) Monitoring::VDCp4
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vk4", 
        V4pos[2] + leftXS,
        (int) Monitoring::VDCk4
    ));

    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vp5", 
        V5pos[1] + leftXS,
        (int) Monitoring::VDCp5
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vk5", 
        V5pos[2] + leftXS,
        (int) Monitoring::VDCk5
    ));

    // Wires
    schematic.addWire (
        schematic.getElement("Ck1")->getTerminals()[0],
        schematic.getElement("Rk1")->getTerminals()[1]
    );
    schematic.addWire (
        schematic.getElement("Ck2")->getTerminals()[0],
        schematic.getElement("Rk2")->getTerminals()[1]
    );
    schematic.addWire (
        schematic.getElement("Rb1")->getTerminals()[0],
        schematic.getElement("Ca1")->getTerminals()[1]
    );
    schematic.addWire (
        schematic.getElement("Rb1")->getTerminals()[1],
        schematic.getElement("Ca1")->getTerminals()[0]
    );
    schematic.addWire (
        schematic.getElement("Ca1")->getTerminals()[1],
        schematic.getElement("Volume")->getTerminals()[0]
    );
    schematic.addWire (
        schematic.getElement("Cbright")->getTerminals()[1],
        schematic.getElement("Volume")->getTerminals()[2]
    );
    schematic.addWire (
        schematic.getElement("Cp3")->getTerminals()[1],
        schematic.getElement("Ra3")->getTerminals()[0]
    );
    schematic.addWire (
        schematic.getElement("Ck4")->getTerminals()[0],
        schematic.getElement("Rk4")->getTerminals()[1]
    );
    schematic.addWire (
        V3pos[0],
        schematic.getElement("Ra2")->getTerminals()[1]
    );
    schematic.addWire (
        V4pos[1],
        V5pos[0]
    );
    schematic.addWire (
        schematic.getElement("E5")->getTerminals()[0],
        V5pos[1]
    );
    schematic.addWire (
        schematic.getElement("Ck1")->getTerminals()[0],
        schematic.getElement("Rk1")->getTerminals()[1]
    );
    schematic.addWire (
        schematic.getElement("C3")->getTerminals()[0] ,
        schematic.getElement("C3")->getTerminals()[0] + leftL*0.25f
    );
    schematic.addWire (
        schematic.getElement("C3")->getTerminals()[0]+ leftL*0.25f,
        schematic.getElement("C2")->getTerminals()[0]
    );
    schematic.addWire (
        schematic.getElement("Bass")->getTerminals()[2],
        schematic.getElement("C2")->getTerminals()[1] + rightXS
    );
    schematic.addWire (
        schematic.getElement("C2")->getTerminals()[1] ,
        schematic.getElement("C2")->getTerminals()[1] + rightXS
    );
    schematic.addWire (
        schematic.getElement("Treble")->getTerminals()[2] ,
        schematic.getElement("Treble")->getTerminals()[2] + rightS
    );
    schematic.addWire (
        V5pos[2] ,
        schematic.getElement("C1")->getTerminals()[0] 
    );
    schematic.addWire (
        schematic.getElement("Rg4")->getTerminals()[0] ,
        schematic.getElement("Ra3")->getTerminals()[0] 
    );

    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Treble")->getTerminals()[2] + rightS));
    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Rg1")->getTerminals()[0]) );
}



// ===================================================================================================================
// BASSMAN PREAMP
// ===================================================================================================================
void SchematicBuilder::buildBassmanPreamp(SchematicPanel& schematic)
{
    using Param         =  FullBassmanPreampCircuit::Param;
    using Control       =  FullBassmanPreampCircuit::Control;
    using Monitoring    =  FullBassmanPreampCircuit::Monitoring;

    //V1
    schematic.addElement (std::make_unique<TriodeElement> (
        "V1", 
        Terminal {250, 350},
        (int) Param::V1,
        0, std::vector<ValueChoice>{{ 0.0f, "12AX7" },{ 0.0f, "12AT7" } },
        (int) Monitoring::Ik1
    ));
    auto V1pos = schematic.getElement("V1")->getTerminals();


    // Grid circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg1",
        V1pos[0]  + leftM,
        V1pos[0] ,
        (int) Param::Rg1 
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ci1",
        schematic.getElement("Rg1")->getTerminals()[0] + leftL,
        schematic.getElement("Rg1")->getTerminals()[0],
        (int) Param::Ci1
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Ri1",
        schematic.getElement("Rg1")->getTerminals()[0],
        schematic.getElement("Rg1")->getTerminals()[0] + bottomL,
        (int) Param::Ri1
    ));

    // Cathode Circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk1",
        V1pos[2]+ bottomL,
        V1pos[2],
        (int) Param::Rk1
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck1",
        schematic.getElement("Rk1")->getTerminals()[1] +  rightXS,
        schematic.getElement("Rk1")->getTerminals()[0] +  rightXS,
        (int) Param::Ck1
    ));

    // Plate Circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp1",
        V1pos[1],
        V1pos[1] + topL,
        (int) Param::Rp1
    ));
    schematic.addElement (std::make_unique<VoltageElement>(
        "E1",
        schematic.getElement("Rp1")->getTerminals()[1], 
        (int) Param::E1
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cp1",
        V1pos[1],
        V1pos[1] + rightXL,
        (int) Param::Cp1
    ));


    schematic.addElement (std::make_unique<PotElement> (
        "Volume",
        schematic.getElement("Cp1")->getTerminals()[1],
        schematic.getElement("Cp1")->getTerminals()[1] + bottomXL,
        schematic.getElement("Cp1")->getTerminals()[1] + rightXS + bottomXL*0.5f ,
        (int) Control::Volume,
        (int) Param::RVol
    ));

    // V2
    schematic.addElement (std::make_unique<TriodeElement> (
        "V2", 
        schematic.getElement("Volume")->getTerminals()[2]+ rightL,
        (int) Param::V2,
        0, std::vector<ValueChoice>{{ 0.0f, "12AX7" },{ 0.0f, "12AT7" } },
        (int) Monitoring::Ik2
    ));
    auto V2pos = schematic.getElement("V2")->getTerminals();
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg2",
        schematic.getElement("Volume")->getTerminals()[2],
        V2pos[0],
        (int) Param::Rg2
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk2",
        V2pos[2]+ bottomL,
        V2pos[2],
        (int) Param::Rk2
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp2",
        V2pos[1],
        V2pos[1] + topL,
        (int) Param::Rp2
    ));
    schematic.addElement (std::make_unique<VoltageElement>(
        "E2",
        schematic.getElement("Rp2")->getTerminals()[1], 
        (int) Param::E2
    ));


    // V3
    schematic.addElement (std::make_unique<TriodeElement> (
        "V3", 
        V2pos[1]+ rightL*2.0f,
        (int) Param::V3,
        0, std::vector<ValueChoice>{{ 0.0f, "12AX7" },{ 0.0f, "12AT7" } },
        (int) Monitoring::Ik3
    ));
    auto V3pos = schematic.getElement("V3")->getTerminals();
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk3",
        V3pos[2]+ bottomM*2.0f,
        V3pos[2],
        (int) Param::Rk3
    ));
    schematic.addElement (std::make_unique<VoltageElement>(
        "E3",
        V3pos[1], 
        (int) Param::E3
    ));


    auto toneStackPosition = V3pos[2] + rightXL;

    // Tone Stack
    schematic.addElement (std::make_unique<ResistorElement> (
        "R4",
        toneStackPosition + bottomL,
        toneStackPosition,
        (int) Param::R4
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C1",
        toneStackPosition,
        toneStackPosition + rightL,
        (int) Param::C1
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C2",
        toneStackPosition + bottomL,
        toneStackPosition + bottomL + rightL,
        (int) Param::C2
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C3",
        toneStackPosition + bottomL*2.0f + rightL*0.25f,
        toneStackPosition + bottomL*2.0f + rightL*0.75f,
        (int) Param::C3
    ));

    schematic.addElement (std::make_unique<PotElement> (
        "Treble",
        schematic.getElement("C1")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C1")->getTerminals()[1] + rightXS + bottomL*0.5f ,
        (int) Control::Treble,
        (int) Param::RTreble
    ));
    schematic.addElement (std::make_unique<PotElement> (
        "Bass",
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1] + bottomM,
        schematic.getElement("C2")->getTerminals()[1] + bottomM*0.5f + rightXS,
        (int) Control::Bass,
        (int) Param::RBass
    ));
    schematic.addElement (std::make_unique<PotElement> (
        "Mid",
        schematic.getElement("Bass")->getTerminals()[1]  + bottomM,
        schematic.getElement("Bass")->getTerminals()[1] ,
        schematic.getElement("Bass")->getTerminals()[1]  + bottomM*0.5f + leftXS,
        (int) Control::Mid,
        (int) Param::RMid
    ));
    // Grounds
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Mid")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ri1")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk1")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck1")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk2")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk3")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Volume")->getTerminals()[1]) );

    // Monitors
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vp1", 
        V1pos[1] + leftXS,
        (int) Monitoring::VDCp1
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vk1", 
        V1pos[2] + leftXS,
        (int) Monitoring::VDCk1
    ));

    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vp2", 
        V2pos[1] + leftXS,
        (int) Monitoring::VDCp2
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vk2", 
        V2pos[2] + leftXS,
        (int) Monitoring::VDCk2
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vp3", 
        V3pos[1] + leftXS,
        (int) Monitoring::VDCp3
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vk3", 
        V3pos[2] + leftXS,
        (int) Monitoring::VDCk3
    ));


    // Wires
    schematic.addWire (
        schematic.getElement("Ck1")->getTerminals()[0],
        schematic.getElement("Rk1")->getTerminals()[1]
    );
    schematic.addWire (
        schematic.getElement("C3")->getTerminals()[0] ,
        schematic.getElement("C3")->getTerminals()[0] + leftL*0.25f
    );
    schematic.addWire (
        schematic.getElement("C3")->getTerminals()[0]+ leftL*0.25f,
        schematic.getElement("C2")->getTerminals()[0]
    );
    schematic.addWire (
        schematic.getElement("Bass")->getTerminals()[2],
        schematic.getElement("C2")->getTerminals()[1] + rightXS
    );
    schematic.addWire (
        schematic.getElement("C2")->getTerminals()[1] ,
        schematic.getElement("C2")->getTerminals()[1] + rightXS
    );
    schematic.addWire (
        schematic.getElement("Treble")->getTerminals()[2] ,
        schematic.getElement("Treble")->getTerminals()[2] + rightL
    );
    schematic.addWire (
        V3pos[2] ,
        schematic.getElement("C1")->getTerminals()[0] 
    );
    schematic.addWire (
        V3pos[0] ,
        V2pos[1]
    );

    // I/O
    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Treble")->getTerminals()[2] + rightL));
    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Ci1")->getTerminals()[0]) );

}