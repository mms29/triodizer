#pragma once

#include <JuceHeader.h>
#include "SchematicElement.h"
#include <memory>
#include <vector>

//==============================================================================
class SchematicPanel : public juce::Component
{
public:
    SchematicPanel();
    ~SchematicPanel() override = default;

    //==========================================================================
    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;

    //==========================================================================
    template<typename ElementType, typename... Args>
    ElementType* addElement(Args&&... args);

    void connect(SchematicElement* a, int terminalA,
                 SchematicElement* b, int terminalB);

    void clear();

    const std::vector<std::unique_ptr<SchematicElement>>& getElements() const noexcept;

private:
    //==========================================================================
    struct Wire
    {
        SchematicElement* elemA = nullptr;
        int terminalA = 0;

        SchematicElement* elemB = nullptr;
        int terminalB = 0;
    };

    std::vector<std::unique_ptr<SchematicElement>> elements;
    std::vector<Wire> wires;

    //==========================================================================
    void drawWires(juce::Graphics& g);
    void drawNodes(juce::Graphics& g);

    //==========================================================================
    juce::Colour backgroundColour { 0xff101010 };
    juce::Colour wireColour       { juce::Colours::lightgrey };
    juce::Colour nodeColour       { juce::Colours::white };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SchematicPanel)
};