#include "Schematic.h"

//==============================================================================
SchematicPanel::SchematicPanel()
{
    setOpaque(true);
}

//==============================================================================
void SchematicPanel::paint(juce::Graphics& g)
{
    g.fillAll(backgroundColour);

    drawWires(g);

    for (auto& e : elements)
        e->paint(g);

    drawNodes(g);
}

//==============================================================================
void SchematicPanel::mouseDown(const juce::MouseEvent& e)
{
    auto pos = e.position;

    for (auto it = elements.rbegin(); it != elements.rend(); ++it)
    {
        auto& elem = *it;

        if (elem->hitTest(pos))
        {
            elem->mouseDown(e);
            repaint();
            return;
        }
    }
}

//==============================================================================
// Element creation
//==============================================================================
template<typename ElementType, typename... Args>
ElementType* SchematicPanel::addElement(Args&&... args)
{
    auto element = std::make_unique<ElementType>(
        std::forward<Args>(args)...);

    auto* ptr = element.get();
    elements.push_back(std::move(element));

    return ptr;
}

// IMPORTANT: template must be explicitly instantiated if used across TU boundaries
// (or keep implementation in header — JUCE style often prefers header-only)

//==============================================================================
void SchematicPanel::connect(SchematicElement* a, int terminalA,
                             SchematicElement* b, int terminalB)
{
    wires.push_back({ a, terminalA, b, terminalB });
}

//==============================================================================
void SchematicPanel::clear()
{
    elements.clear();
    wires.clear();
    repaint();
}

//==============================================================================
const std::vector<std::unique_ptr<SchematicElement>>&
SchematicPanel::getElements() const noexcept
{
    return elements;
}

//==============================================================================
// Drawing
//==============================================================================
void SchematicPanel::drawWires(juce::Graphics& g)
{
    // g.setColour(wireColour);

    // for (const auto& w : wires)
    // {
    //     auto p0 = w.elemA->getTerminal(w.terminalA);
    //     auto p1 = w.elemB->getTerminal(w.terminalB);

    //     g.drawLine({ p0.x, p0.y, p1.x, p1.y }, 2.0f);
    // }
}

//==============================================================================
void SchematicPanel::drawNodes(juce::Graphics& g)
{
    g.setColour(nodeColour);

    constexpr float r = 3.0f;

    for (const auto& w : wires)
    {
        auto p0 = w.elemA->getTerminal(w.terminalA);
        auto p1 = w.elemB->getTerminal(w.terminalB);

        g.fillEllipse(p0.x - r, p0.y - r, 2*r, 2*r);
        g.fillEllipse(p1.x - r, p1.y - r, 2*r, 2*r);
    }
}
void buildCommonCathodeStage(SchematicPanel& panel)
{
    const float cx = 300.0f;
    const float cy = 250.0f;

    // // --- Triode ---
    // auto* tri = panel.addTriode("V1", { cx, cy });

    // --- Plate supply rail (top) ---
    auto* rail = panel.addVoltageRail("B+",
        { 80.0f, cy - 120.0f }, { 520.0f, cy - 120.0f }, 250.0f);

    // --- Plate resistor Rp ---
    auto* rp = panel.addElement(SchematicElement::Type::Resistor, "Rp",
        { cx, cy - 50 }, { cx + 200.0f, cy - 150.0f });
    rp->addChoice(47000.0f,  "47 kOhms");
    rp->addChoice(68000.0f,  "68 kOhms");
    rp->addChoice(100000.0f, "100 kOhms");
    rp->addChoice(150000.0f, "150 kOhms");
    rp->addChoice(220000.0f, "220 kOhms");
    rp->setSelectedIndex(2); // 100k default

    // --- Plate coupling cap Co ---
    auto* co = panel.addElement(SchematicElement::Type::Capacitor, "Co",
        { cx + 200.0f, cy - 150.0f }, { cx + 300.0f, cy - 50.0f });
    co->addChoice(100.0e-9f,  "100 nF");
    co->addChoice(220.0e-9f,  "220 nF");
    co->addChoice(470.0e-9f,  "470 nF");
    co->addChoice(1.0e-6f,    "1 μF");
    co->setSelectedIndex(0);

    // // --- Plate load (output) resistor Ro ---
    // auto* ro = panel.addElement(SchematicElement::Type::Resistor, "Ro",
    //     { cx + 300.0f, cy - 50.0f }, { cx + 450.0f, cy - 50.0f });
    // ro->addChoice(100000.0f,  "100 kΩ");
    // ro->addChoice(220000.0f,  "220 kΩ");
    // ro->addChoice(470000.0f,  "470 kΩ");
    // ro->addChoice(1000000.0f, "1 MΩ");
    // ro->setSelectedIndex(0);

    // // --- Parasitic plate-to-grid cap Cp ---
    // auto* cp = panel.addElement(SchematicElement::Type::Capacitor, "Cp",
    //     { cx + 200.0f, cy - 150.0f }, { cx, cy - 50.0f });  // feedback path
    // cp->addChoice(5.0e-12f,  "5 pF");
    // cp->addChoice(10.0e-12f, "10 pF");
    // cp->addChoice(22.0e-12f, "22 pF");
    // cp->addChoice(47.0e-12f, "47 pF");
    // cp->setSelectedIndex(1);

    // // --- Grid resistor Rg (input to grid) ---
    // auto* rg = panel.addElement(SchematicElement::Type::Resistor, "Rg",
    //     { cx - 200.0f, cy - 50.0f }, { cx - 50.0f, cy });
    // rg->addChoice(68000.0f,  "68 kΩ");
    // rg->addChoice(100000.0f, "100 kΩ");
    // rg->addChoice(220000.0f, "220 kΩ");
    // rg->addChoice(470000.0f, "470 kΩ");
    // rg->setSelectedIndex(1);

    // // --- Input coupling cap Ci ---
    // auto* ci = panel.addElement(SchematicElement::Type::Capacitor, "Ci",
    //     { cx - 350.0f, cy - 50.0f }, { cx - 200.0f, cy - 50.0f });
    // ci->addChoice(10.0e-9f,  "10 nF");
    // ci->addChoice(22.0e-9f,  "22 nF");
    // ci->addChoice(47.0e-9f,  "47 nF");
    // ci->addChoice(100.0e-9f, "100 nF");
    // ci->setSelectedIndex(2);

    // // --- Grid-leak / input resistor Ri ---
    // auto* ri = panel.addElement(SchematicElement::Type::Resistor, "Ri",
    //     { cx - 100.0f, cy + 20.0f }, { cx - 100.0f, cy + 150.0f });
    // ri->addChoice(470000.0f,  "470 kΩ");
    // ri->addChoice(1000000.0f, "1 MΩ");
    // ri->addChoice(2200000.0f, "2.2 MΩ");
    // ri->addChoice(4700000.0f, "4.7 MΩ");
    // ri->setSelectedIndex(1);

    // // --- Cathode resistor Rk ---
    // auto* rk = panel.addElement(SchematicElement::Type::Resistor, "Rk",
    //     { cx, cy + 50 }, { cx, cy + 170.0f });
    // rk->addChoice(330.0f,    "330 Ω");
    // rk->addChoice(680.0f,    "680 Ω");
    // rk->addChoice(1000.0f,   "1 kΩ");
    // rk->addChoice(3300.0f,   "3.3 kΩ");
    // rk->addChoice(10000.0f,  "10 kΩ");
    // rk->setSelectedIndex(2);

    // // --- Cathode bypass cap Ck ---
    // auto* ck = panel.addElement(SchematicElement::Type::Capacitor, "Ck",
    //     { cx + 30.0f, cy + 120.0f }, { cx + 30.0f, cy + 200.0f });
    // ck->addChoice(1.0e-6f,   "1 μF");
    // ck->addChoice(10.0e-6f,  "10 μF");
    // ck->addChoice(22.0e-6f,  "22 μF");
    // ck->addChoice(47.0e-6f,  "47 μF");
    // ck->addChoice(100.0e-6f, "100 μF");
    // ck->setSelectedIndex(2);

    // // --- Cathode bypass wire (Ck to cathode terminal) ---
    // // --- Ground return from bottom of Ck/Rk ---
    // auto* ground = panel.addElement(SchematicElement::Type::Resistor, "GND",
    //     { cx, cy + 200.0f }, { cx, cy + 260.0f });  // drawn as a stub
    // ground->addChoice(0.0f, "(GND)");

    // // === Wire connections ===

    // // Rail + to Rp top
    // panel.addWire(rail, 0, rp, 0);

    // // Rp bottom -> Co -> Ro -> output (Ro p1)
    // panel.addWire(rp, 1, co, 0);
    // panel.addWire(co, 1, ro, 0);

    // // Ro output -> Cp feedback (Cp p0)
    // panel.addWire(ro, 1, cp, 0);

    // // Cp other end -> plate (triode terminal 2)
    // panel.addWire(cp, 1, tri, 2);

    // // Rp top also -> plate
    // panel.addWire(rp, 0, tri, 2);

    // // Ci -> Rg -> grid (triode terminal 0)
    // panel.addWire(ci, 1, rg, 0);
    // panel.addWire(rg, 1, tri, 0);

    // // Grid leak Ri: from grid node down to cathode area
    // panel.addWire(rg, 0, ri, 0);
    // // Ri bottom tied to cathode via wire
    // panel.addWire(ri, 1, rk, 0);
    // // Rk bottom to ground stub
    // panel.addWire(rk, 1, ground, 0);

    // // Cathode bypass: Ck across Rk
    // panel.addWire(rk, 0, ck, 0);
    // panel.addWire(rk, 1, ck, 1);

    // // Cathode terminal
    // panel.addWire(rk, 0, tri, 1);
}