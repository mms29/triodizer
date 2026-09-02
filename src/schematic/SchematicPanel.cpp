#include "schematic/SchematicPanel.h"
#include <format>
//==============================================================================
SchematicPanel::SchematicPanel(SchematicPanelListener* l) : listener(l)
{
    setInterceptsMouseClicks (true, true);

    addAndMakeVisible(inspector);
    inspector.setAlwaysOnTop(true);
    
}

void SchematicPanel::addElement (std::unique_ptr<SchematicElement> element)
{
    jassert (element != nullptr);
    element->prepareToDraw();

    if (auto* sigElem = dynamic_cast<SignalElement*>(element.get()))
        sigElem->createSignalPaths();


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

        if (auto* multiParamElem = dynamic_cast<MultiParamElement*>(element.get()))
        {
            for (int i = 0; i < multiParamElem->getNumParams(); ++i)
            {
                auto& p = multiParamElem->getParam(i);
                if (p.paramIndex < 0)
                    continue;

                float v = listener->getCircuitParam(p.paramIndex);
                if (p.type == ParamType::Choice)
                {
                    int index = p.getIndexChoiceFromValue(v);
                    if (index < 0)
                    {
                        p.addChoice(v, juce::String(v, 2));
                        p.setChoiceIndex((int) p.getChoices().size() - 1);
                    }
                    else
                        p.setChoiceIndex(index);
                }
                else
                    p.setValue(v);
            }
            continue;
        }

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
    addWireElem(std::make_unique<WireElement>(std::vector<Terminal>{start, end}));
}

void SchematicPanel::addWireElem (std::unique_ptr<WireElement> wire)
{
    wire->prepareToDraw();
    elements.push_back (std::move (wire));
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
    listener->updateCircuitMonitoring();
    
    for (const auto& element : elements){
        if (auto* monitor = dynamic_cast<MonitoringElement*>(element.get())){

            for (int i = 0; i < monitor->getNumMonitors(); ++i)
            {
                auto index = monitor->getCircuitIndex(i);
                if (index >= 0)
                    monitor->setMonitorValue(
                        i,
                        listener->getCircuitMonitoring(index)
                    );
            }
        }
        if (signalPathActivated)
            if (auto* sig = dynamic_cast<SignalElement*>(element.get()))
                sig->updateSignalPaths();

    }
    repaint();
}
void SchematicPanel::updateInspect ()
{
    if (hoveredElement != nullptr){
        if (auto* inspectElem = dynamic_cast<InspectableElement*>(hoveredElement) )
        {
            inspector.setContent(
                hoveredElement->getName(),
                inspectElem->getInspectValue(),
                inspectElem->getInspectContent(),
                inspectElem->getInspectDescr()
            );
        }
    }

}
//==============================================================================
void SchematicPanel::paint (juce::Graphics& g)
{

    // Background
    g.fillAll (getColourBackground());
    g.saveState();

    // Apply zoom and pan transform
    g.addTransform(juce::AffineTransform::scale(zoomFactor).followedBy(
        juce::AffineTransform::translation(viewOffset.x, viewOffset.y)));


    // Render labels, glow etc
    for (const auto& elem : elements)
        elem->draw (g);

    // Render Main Path
    juce::Path mainPath ; 
    for (const auto& elem : elements)
        mainPath.addPath(elem->getPath());
    drawCorePath(g, mainPath, getColourNormal(), false);



    // Render sig path and highlight on top
    for (const auto& elem : elements)
    {
        if (elem->isHighlighted())
            drawSolidCorePath(g, elem->getPath(), true);
        if (signalPathActivated){
            if (auto* sigElem = dynamic_cast<SignalElement*>(elem.get()) ){
                for (int i = 0; i < sigElem->getNumSignals(); ++i)
                    sigElem->getSignalPathPtr(i)->draw(g);
            }

        }
    }    
    
    g.restoreState();

    juce::Path borderPath;
    borderPath.addRoundedRectangle(controlsBounds, 10.0f, 9.0f);
    drawGlowAndCorePath(g, borderPath, .1f, getColourNormal(), getColourAmber(), false);

    // Background
    g.setColour (getColourBackground().withAlpha(0.9f));
    g.fillRoundedRectangle (controlsBounds.expanded(1), 10.0f);
}


void SchematicPanel::resized()
{
    auto area = getLocalBounds().reduced(4);

    float offset = (area.getWidth() -  ((SCHEMATIC_BUTTON_SIZE+20) * controls.size()))*0.5f;


    auto bot = area;
    bot= bot.removeFromBottom(SCHEMATIC_BUTTON_SIZE);
    bot.removeFromLeft(offset);
    auto botArea = bot;

    for (const auto& control : controls){
        bot.removeFromLeft(10);
        control->setBounds(bot.removeFromLeft(SCHEMATIC_BUTTON_SIZE));
        bot.removeFromLeft(10);
    }
    botArea.setWidth(((SCHEMATIC_BUTTON_SIZE+20) * controls.size()) );
    botArea.setY(botArea.getY() - 10);
    botArea.expand(0, 10);
    botArea.reduce(10, 0);
    controlsBounds = botArea.reduced(10).toFloat();
    inspector.setBounds(area.removeFromRight(SCHEMATIC_INSPECTOR_SIZE));
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

            if (auto* inspectElem = dynamic_cast<InspectableElement*>(hoveredElement) )
            {
                inspector.setContent(
                    hoveredElement->getName(),
                    inspectElem->getInspectValue(),
                    inspectElem->getInspectContent(),
                    inspectElem->getInspectDescr()
                );
            }
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
    if (auto* multiParamElem = dynamic_cast<MultiParamElement*>(element))
    {
        showMultiParamMenu (element, multiParamElem);
        return;
    }

    if (auto* paramElem = dynamic_cast<ParametrableElement*>(element) )
    {

        juce::PopupMenu menu;
        auto choices = paramElem->getChoices();
        for (std::size_t i = 0; i < choices.size(); ++i)
        {
            menu.addItem ((int) i + 1, choices[i].label, true,
                        static_cast<int> (i) == paramElem->getChoiceIndex());
        }

        menu.setLookAndFeel (&glowLookAndFeel);
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
        showLabelEditor (element->getName(), setElem->getLabel(),
            [this, setElem] (const juce::String& textValue)
            {
                setElem->setLabel (textValue);
                listener->setCircuitParam (setElem->getParamIndex(), setElem->getValue());
            });
    }
}

void SchematicPanel::showLabelEditor (const juce::String& title,
                                      const juce::String& initialText,
                                      std::function<void (const juce::String&)> onOk)
{
    auto* window = new juce::AlertWindow (title,
                                  "Enter value:",
                                  juce::AlertWindow::NoIcon);

    window->setLookAndFeel (&glowLookAndFeel);
    window->addTextEditor ("text", initialText);
    auto* editor = window->getTextEditor ("text");
    if (editor != nullptr)
    {
        editor->setJustification (juce::Justification::centred);
    }

    window->addButton ("OK", 1);
    window->addButton ("Cancel", 0);

    window->enterModalState (
        true,
        juce::ModalCallbackFunction::create (
            [window, onOk = std::move (onOk)] (int result)
            {
                if (result == 1)
                    onOk (window->getTextEditorContents ("text"));

                window->exitModalState (0);
                delete window;
            }));
}

void SchematicPanel::showMultiParamMenu (SchematicElement* element, MultiParamElement* mp)
{
    const int numParams = mp->getNumParams();
    if (numParams <= 0)
        return;

    juce::Component::SafePointer<SchematicPanel> safeThis { this };

    // Single param: edit it directly, same UX as the legacy elements
    if (numParams == 1)
    {
        auto& p = mp->getParam (0);

        if (p.type == ParamType::Choice)
        {
            juce::PopupMenu menu;
            const auto& choices = p.getChoices();
            for (std::size_t i = 0; i < choices.size(); ++i)
                menu.addItem ((int) i + 1, choices[i].label, true,
                              (int) i == p.getChoiceIndex());

            menu.setLookAndFeel (&glowLookAndFeel);
            menu.showMenuAsync (juce::PopupMenu::Options(),
                [safeThis, mp] (int result) mutable
                {
                    if (result <= 0 || safeThis == nullptr)
                        return;

                    auto& p = mp->getParam (0);
                    const int chosenIndex = result - 1;
                    if (chosenIndex >= 0 && chosenIndex < (int) p.getChoices().size())
                    {
                        p.setChoiceIndex (chosenIndex);
                        safeThis->repaint();

                        if (p.paramIndex >= 0 && safeThis->listener != nullptr)
                            safeThis->listener->setCircuitParam (p.paramIndex, p.getValue());
                    }
                });
        }
        else
        {
            showLabelEditor (element->getName(), p.getLabel(),
                [safeThis, mp] (const juce::String& textValue) mutable
                {
                    if (safeThis == nullptr)
                        return;

                    auto& p = mp->getParam (0);
                    if (! p.setFromLabel (textValue))
                        return;

                    safeThis->repaint();

                    if (p.paramIndex >= 0 && safeThis->listener != nullptr)
                        safeThis->listener->setCircuitParam (p.paramIndex, p.getValue());
                });
        }
        return;
    }

    // Multiple params: one submenu per param
    juce::PopupMenu menu;
    for (int slot = 0; slot < numParams; ++slot)
    {
        auto& p = mp->getParam (slot);

        if (p.type == ParamType::Choice)
        {
            juce::PopupMenu subMenu;
            const auto& choices = p.getChoices();
            for (std::size_t i = 0; i < choices.size(); ++i)
                subMenu.addItem (slot * 100 + 2 + (int) i, choices[i].label, true,
                                 (int) i == p.getChoiceIndex());

            menu.addSubMenu (p.id + ": " + p.getLabel(), subMenu);
        }
        else
        {
            menu.addItem (slot * 100 + 1, p.id + ": " + p.getLabel(), true, false);
        }
    }

    menu.setLookAndFeel (&glowLookAndFeel);
    menu.showMenuAsync (juce::PopupMenu::Options(),
        [safeThis, mp] (int result) mutable
        {
            if (result <= 0 || safeThis == nullptr)
                return;

            const int slot = result / 100;
            const int k    = result % 100;
            auto& p = mp->getParam (slot);

            if (p.type == ParamType::Float && k == 1)
            {
                safeThis->showLabelEditor (p.id, p.getLabel(),
                    [safeThis, mp, slot] (const juce::String& textValue) mutable
                    {
                        if (safeThis == nullptr)
                            return;

                        auto& p = mp->getParam (slot);
                        if (! p.setFromLabel (textValue))
                            return;

                        safeThis->repaint();

                        if (p.paramIndex >= 0 && safeThis->listener != nullptr)
                            safeThis->listener->setCircuitParam (p.paramIndex, p.getValue());
                    });
                return;
            }

            if (p.type == ParamType::Choice && k >= 2)
            {
                const int chosenIndex = k - 2;
                if (chosenIndex < (int) p.getChoices().size())
                    p.setChoiceIndex (chosenIndex);
            }

            safeThis->repaint();

            if (p.paramIndex >= 0 && safeThis->listener != nullptr)
                safeThis->listener->setCircuitParam (p.paramIndex, p.getValue());
        });
}


// void SchematicPanel::addTubeJunctions()
// {
//     for (const auto& element : elements){

//         if (auto* triodeElem = dynamic_cast<TriodeElement*>(element.get()))
//         {
//             auto terms = triodeElem->getTerminals();

//             if (triodeElem->getNumMonitors() == 4){
//                 addElement (std::make_unique<JunctionElement>(
//                     terms[0], triodeElem->getCircuitIndex((int) TriodeElement::Monitoring::Vg)
//                 ) );

//                 addElement (std::make_unique<JunctionElement>(
//                     terms[2], triodeElem->getCircuitIndex((int) TriodeElement::Monitoring::Vk)
//                 ) );

//                 addElement (std::make_unique<JunctionElement>(
//                     terms[1], triodeElem->getCircuitIndex((int) TriodeElement::Monitoring::Vp)
//                 ) );
//             }
//         }
//     }
// }


void SchematicPanel::clear()
{
    hoveredElement = nullptr;

    // Break all SignalPath references
    for (auto& element : elements)
        if (auto* sigElem = dynamic_cast<SignalElement*>(element.get()))
            sigElem->clearSignalReferences();

    elements.clear();
    controls.clear();
}


