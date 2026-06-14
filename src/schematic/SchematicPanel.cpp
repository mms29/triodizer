#include "schematic/SchematicPanel.h"
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
    wires.push_back (std::make_unique<WireElement>(std::vector<Terminal>{start, end}));
}

void SchematicPanel::addWireElem (std::unique_ptr<WireElement> wire)
{
    wires.push_back (std::move (wire));
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
    SchematicElement::incrementClock();
    repaint();
}
//==============================================================================
void SchematicPanel::paint (juce::Graphics& g)
{
    // Background
    g.fillAll (COLOR_BACKGROUND);

    // Apply zoom and pan transform
    g.addTransform(juce::AffineTransform::scale(zoomFactor).followedBy(
        juce::AffineTransform::translation(viewOffset.x, viewOffset.y)));

    // Draw wires first (behind elements)
    for (const auto& wire : wires)
    {
        jassert (wire != nullptr);
        wire->draw (g);
    }

    // Draw every element on top
    for (const auto& elem : elements)
    {
        jassert (elem != nullptr);
        elem->draw (g);
    }

    // Draw Inspectors. at the end
    for (const auto& elem : elements)
    {
        jassert (elem != nullptr);
        if (elem->isHighlighted()){
            if (auto* inspectElem = dynamic_cast<InspectableElement*>(elem.get()) )
            {
                inspectElem->drawInspector(g);
            }
        }
    }
}


void SchematicPanel::resized()
{
    // Elements use absolute positions, nothing to do here.
    auto area = getLocalBounds().reduced(4);


    float butSizeX = 120;
    float butSizeY = 120;
    float offset = area.getWidth()*.5f -  (butSizeX * controls.size())*0.5f - butSizeX*0.5f +10 ;


    auto bot = area.removeFromBottom(butSizeY);
    bot.removeFromLeft(offset);
    for (const auto& control : controls){
        bot.removeFromLeft(10);
        control->setBounds(bot.removeFromLeft(butSizeX));
        bot.removeFromLeft(10);
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

        if (hoveredElement != nullptr){
            hoveredElement->setHighlighted (true);
        }

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


