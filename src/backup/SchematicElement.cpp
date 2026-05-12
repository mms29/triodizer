#include "SchematicElement.h"

SchematicElement::SchematicElement(
    const juce::String& elementName)
    : name(elementName)
{
}

void SchematicElement::addChoice(
    float value,
    const juce::String& label)
{
    choices.push_back({ value, label });
}

void SchematicElement::setSelectedIndex(int idx)
{
    if (idx >= 0 &&
        idx < (int)choices.size())
    {
        selected = idx;
    }
}

float SchematicElement::getSelectedValue() const
{
    if (choices.empty())
        return 0.0f;

    return choices[selected].value;
}

juce::String
SchematicElement::getSelectedLabel() const
{
    if (choices.empty())
        return {};

    return choices[selected].label;
}

void SchematicElement::mouseDown(
    const juce::MouseEvent& e)
{
    if (!choices.empty())
        showPopupMenu(e);
}

void SchematicElement::showPopupMenu(
    const juce::MouseEvent&)
{
    juce::PopupMenu menu;

    for (int i = 0; i < choices.size(); ++i)
    {
        menu.addItem(
            i + 1,
            choices[i].label,
            true,
            i == selected);
    }

    juce::Component::SafePointer<SchematicElement>
        safeThis(this);

    menu.showMenuAsync(
        {},
        [safeThis](int result)
        {
            if (safeThis == nullptr)
                return;

            if (result <= 0)
                return;

            safeThis->setSelectedIndex(
                result - 1);

            if (safeThis->onValueChanged)
            {
                safeThis->onValueChanged(
                    safeThis->getSelectedValue());
            }
        });
}