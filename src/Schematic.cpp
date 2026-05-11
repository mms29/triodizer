#include "Schematic.h"
#include <cmath>


//==============================================================================
//                          SchematicElement
//==============================================================================

ResistorElement::ResistorElement(Type t,
                                   const juce::String& cName,
                                   juce::Point<float> a,
                                   juce::Point<float> b)
    : type(t), name(cName), p0(a), p1(b), position((a + b) * 0.5f)
{
    // Two terminals for 2-port elements
    terminals.push_back(Terminal{ p0, juce::String() });
    terminals.push_back(Terminal{ p1, juce::String() });
    updateBounds();
}

SchematicElement::SchematicElement(juce::Point<float> centre)
    : type(Type::Triode), name("Triode"), position(centre)
{
    // Grid — left
    terminals.push_back(Terminal{ centre + juce::Point<float>(-50.0f, 0.0f),  "G" });
    // Cathode — bottom
    terminals.push_back(Terminal{ centre + juce::Point<float>(0.0f, 50.0f),   "K" });
    // Plate — top
    terminals.push_back(Terminal{ centre + juce::Point<float>(0.0f, -50.0f),  "P" });
    updateBounds();
}

SchematicElement::SchematicElement(juce::Point<float> s,
                                   juce::Point<float> e,
                                   float v)
    : type(Type::VoltageRail), name("Rail"), p0(s), p1(e), voltageValue(v)
{
    position = (s + e) * 0.5f;
    updateBounds();
}

void SchematicElement::updateBounds()
{
    if (type == Type::Triode)
    {
        bounds = juce::Rectangle<float>(position.x - 55.0f, position.y - 55.0f,
                                         110.0f, 110.0f);
    }
    else if (type == Type::VoltageRail)
    {
        float x0 = std::min(p0.x, p1.x) - 4.0f;
        float y0 = std::min(p0.y, p1.y) - 4.0f;
        float w  = std::abs(p1.x - p0.x) + 8.0f;
        float h  = std::abs(p1.y - p0.y) + 8.0f;
        bounds = { x0, y0, w, h };
    }
    else
    {
        float x0 = std::min(p0.x, p1.x) - 12.0f;
        float y0 = std::min(p0.y, p1.y) - 12.0f;
        float w  = std::abs(p1.x - p0.x) + 24.0f;
        float h  = std::abs(p1.y - p0.y) + 24.0f;
        bounds = { x0, y0, w, h };
    }
}

void SchematicElement::addChoice(float v, const juce::String& lbl)
{
    choices.push_back({ v, lbl });
}

void SchematicElement::setSelectedIndex(int idx)
{
    if (idx >= 0 && idx < (int)choices.size())
        selected = idx;
}

float SchematicElement::getSelectedValue() const
{
    if (choices.empty()) return 0.0f;
    return choices[selected].value;
}

juce::String SchematicElement::getSelectedLabel() const
{
    if (choices.empty()) return {};
    return choices[selected].label;
}

void SchematicElement::setPosition(juce::Point<float> newPos)
{
    juce::Point<float> delta = newPos - position;
    position = newPos;
    // Shift both terminals for 2-port elements
    if (type != Type::Triode && type != Type::VoltageRail)
    {
        p0 += delta;
        p1 += delta;
        terminals[0].pos = p0;
        terminals[1].pos = p1;
    }
    else if (type == Type::Triode)
    {
        terminals[0].pos = newPos + juce::Point<float>(-50.0f, 0.0f);
        terminals[1].pos = newPos + juce::Point<float>(  0.0f, 50.0f);
        terminals[2].pos = newPos + juce::Point<float>(  0.0f,-50.0f);
    }
    else if (type == Type::VoltageRail)
    {
        p0 += delta;
        p1 += delta;
    }
    updateBounds();
}

// ---------------------------------------------------------------------------
// Drawing helpers (static)
// ---------------------------------------------------------------------------

/** Draw a standard zigzag resistor between two points. */
void SchematicElement::drawZigzag(juce::Graphics& g,
                                   float x1, float y1, float x2, float y2,
                                   float amplitude)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len < 1.0f) return;
    float ux = dx / len, uy = dy / len;
    float nx = -uy, ny = ux;           // unit normal
    int   numZigs = juce::jmax(2, (int)(len / 12.0f));
    float stepX = dx / numZigs;
    float stepY = dy / numZigs;

    juce::Path p;
    p.startNewSubPath(x1, y1);
    for (int i = 0; i < numZigs; ++i)
    {
        float mx = x1 + stepX * (i + 0.5f);
        float my = y1 + stepY * (i + 0.5f);
        p.lineTo(mx + nx * amplitude, my + ny * amplitude);
        p.lineTo(x1 + stepX * (i + 1.0f), y1 + stepY * (i + 1.0f));
    }
    g.strokePath(p, juce::PathStrokeType(2.2f));
}

/** Draw two parallel capacitor plates. */
void SchematicElement::drawPlates(juce::Graphics& g,
                                  float x1, float y1, float x2, float y2,
                                  float plateLen)
{
    bool vert = std::abs(x1 - x2) < std::abs(y1 - y2);
    float midX = (x1 + x2) * 0.5f;
    float midY = (y1 + y2) * 0.5f;

    g.drawLine(x1, y1, midX, midY, 1.5f);          // lead in
    if (vert)
    {
        g.drawLine(midX - 5.0f, midY, midX + 5.0f, midY, 2.0f);
        g.drawLine(midX - 5.0f, midY + plateLen,
                   midX + 5.0f, midY + plateLen, 2.0f);
    }
    else
    {
        g.drawLine(midX, midY - 5.0f, midX, midY + 5.0f, 2.0f);
        g.drawLine(midX + plateLen, midY - 5.0f,
                   midX + plateLen, midY + 5.0f, 2.0f);
    }
    g.drawLine(midX, midY, x2, y2, 1.5f);            // lead out
}

/** Simple inductor coil (series of semi-circular bumps). */
void SchematicElement::drawCoil(juce::Graphics& g,
                                float x1, float y1, float x2, float y2,
                                int numBumps)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len < 1.0f) return;
    float ux = dx / len, uy = dy / len;
    float nx = -uy, ny = ux;
    float bump = len / (float)numBumps;
    float radius = bump * 0.35f;

    juce::Path p;
    p.startNewSubPath(x1, y1);
    for (int i = 0; i < numBumps; ++i)
    {
        float cx = x1 + ux * bump * ((float)i + 0.5f);
        float cy = y1 + uy * bump * ((float)i + 0.5f);
        float ax = cx - nx * radius;
        float ay = cy - ny * radius;
        float bx = cx + nx * radius;
        float by = cy + ny * radius;
        if (i % 2 == 0)
            p.cubicTo(ax, ay, bx, by,
                      x1 + ux * bump * (float)(i + 1),
                      y1 + uy * bump * (float)(i + 1));
        else
            p.cubicTo(bx, by, ax, ay,
                      x1 + ux * bump * (float)(i + 1),
                      y1 + uy * bump * (float)(i + 1));
    }
    g.strokePath(p, juce::PathStrokeType(2.2f));
}

void SchematicElement::drawGroundSymbol(juce::Graphics& g,
                                         float cx, float cy, float size)
{
    g.drawLine(cx, cy, cx, cy + size, 2.0f);
    float s = size * 0.45f;
    g.drawLine(cx - s, cy + size, cx + s, cy + size, 2.0f);
    s *= 0.6f;
    g.drawLine(cx - s, cy + size + s * 0.5f,
               cx + s, cy + size + s * 0.5f, 1.8f);
}

// ---------------------------------------------------------------------------
// Paint / interaction
// ---------------------------------------------------------------------------

void SchematicElement::paint(juce::Graphics& g)
{
    const float plateLen = 10.0f;

    switch (type)
    {
        case Type::Resistor:
        {
            g.setColour(juce::Colours::white);
            drawZigzag(g, p0.x, p0.y, p1.x, p1.y, 6.0f);
            // Label
            float mx = (p0.x + p1.x) * 0.5f;
            float my = (p0.y + p1.y) * 0.5f;
            juce::String lbl = name + ": " + getSelectedLabel();
            g.drawText(lbl, mx - 30.0f, my - 20.0f, 65.0f, 16.0f,
                       juce::Justification::centred);
            break;
        }

        case Type::Capacitor:
        {
            g.setColour(juce::Colours::cyan);
            drawPlates(g, p0.x, p0.y, p1.x, p1.y, plateLen);
            juce::String lbl = name + ": " + getSelectedLabel();
            float mx = (p0.x + p1.x) * 0.5f;
            float my = (p0.y + p1.y) * 0.5f;
            g.drawText(lbl, mx - 30.0f, my - 20.0f, 65.0f, 16.0f,
                       juce::Justification::centred);
            break;
        }

        case Type::Inductor:
        {
            g.setColour(juce::Colours::orange);
            drawCoil(g, p0.x, p0.y, p1.x, p1.y, 4);
            juce::String lbl = name + ": " + getSelectedLabel();
            float mx = (p0.x + p1.x) * 0.5f;
            float my = (p0.y + p1.y) * 0.5f;
            g.drawText(lbl, mx - 30.0f, my - 20.0f, 65.0f, 16.0f,
                       juce::Justification::centred);
            break;
        }

        case Type::Triode:
        {
            // Envelope circle
            g.setColour(juce::Colours::lightgreen);
            g.drawEllipse(position.x - 35.0f, position.y - 35.0f,
                          70.0f, 70.0f, 2.5f);

            // Heater symbol
            g.drawLine(position.x - 10, position.y - 8,
                       position.x + 10, position.y - 8, 1.5f);
            g.drawLine(position.x - 10, position.y + 8,
                       position.x + 10, position.y + 8, 1.5f);
            g.drawLine(position.x - 10, position.y - 8,
                       position.x - 10, position.y + 8, 1.5f);
            g.drawLine(position.x + 10, position.y - 8,
                       position.x + 10, position.y + 8, 1.5f);

            // Pins — Grid
            g.drawLine(terminals[0].pos.x, terminals[0].pos.y,
                       position.x - 35, position.y, 2.0f);
            g.drawText("G", terminals[0].pos.x - 14, terminals[0].pos.y - 8,
                       20.0f, 16.0f, juce::Justification::centred);

            // Cathode
            g.drawLine(position.x, position.y + 35,
                       terminals[1].pos.x, terminals[1].pos.y, 2.0f);
            g.drawText("K", terminals[1].pos.x - 8, terminals[1].pos.y + 4,
                       20.0f, 16.0f, juce::Justification::centred);

            // Plate
            g.drawLine(position.x, position.y - 35,
                       terminals[2].pos.x, terminals[2].pos.y, 2.0f);
            g.drawText("P", terminals[2].pos.x - 8, terminals[2].pos.y - 20,
                       20.0f, 16.0f, juce::Justification::centred);

            // "12AX7" label
            g.drawText("12AX7", position.x - 30, position.y - 10,
                       60.0f, 20.0f, juce::Justification::centred);
            break;
        }

        case Type::VoltageRail:
        {
            g.setColour(juce::Colours::red);
            g.drawLine(p0.x, p0.y, p1.x, p1.y, 2.5f);
            // "+V" label
            float mx = (p0.x + p1.x) * 0.5f;
            float my = (p0.y + p1.y) * 0.5f;
            juce::String lbl = juce::String(voltageValue) + " V";
            g.drawText(lbl, mx - 25, my - 16, 50, 14,
                       juce::Justification::centred);
            break;
        }
    }
}

void SchematicElement::mouseDown(const juce::MouseEvent& e)
{
    if (type == Type::Triode || type == Type::VoltageRail)
        return; // triode/rail have no value picker for now

    if (!choices.empty())
        showPopupMenu(e);
}

void SchematicElement::showPopupMenu(const juce::MouseEvent& e)
{
    juce::PopupMenu menu;
    for (int i = 0; i < (int)choices.size(); ++i)
        menu.addItem(i + 1, choices[i].label, true, i == selected);

    auto pt = e.getScreenPosition();
    menu.showMenuAsync (juce::PopupMenu::Options(),
                     [this] (int result)
                     {
                        setSelectedIndex(result-1);
                        std::cout << getSelectedValue() << std::endl;
                        if (onValueChanged)
                            onValueChanged(getSelectedValue());  
                     });
    // (juce::PopupMenu::Options()
    //     .withTargetScreenArea(juce::Rectangle<int>(pt.x, pt.y, 0, 0)));
    // if (result > 0)
    // {
    //     int idx = result - 1;
    //     setSelectedIndex(idx);
    //     if (onValueChanged)
    //         onValueChanged(getSelectedValue());
    // }
}


//==============================================================================
//                              SchematicPanel
//==============================================================================

SchematicPanel::SchematicPanel()
{
    setOpaque(true);
}

void SchematicPanel::paint(juce::Graphics& g)
{
    g.fillAll(backgroundColour);
    drawWires(g);
    drawNodes(g);

    for (auto& e : elements_)
        e->paint(g);
}

void SchematicPanel::resized()
{
    // Elements own their bounds; nothing to do here.
}

void SchematicPanel::mouseDown(const juce::MouseEvent& e)
{
    // Hit-test each element (front-to-back)
    for (int i = (int)elements_.size() - 1; i >= 0; --i)
    {
        auto& el = elements_[i];
        if (el->getBounds().contains(e.getPosition().toFloat()))
        {
            el->mouseDown(e);
            break;
        }
    }
}

// ---------------------------------------------------------------------------
// Building helpers
// ---------------------------------------------------------------------------

void SchematicPanel::addElement(std::unique_ptr<SchematicElement> elem)
{
    elements_.push_back(std::move(elem));
}

SchematicElement* SchematicPanel::addTriode(const juce::String& nm,
                                             juce::Point<float> centre)
{
    auto el = std::make_unique<SchematicElement>(centre);
    auto* ptr = el.get();
    ptr->setName(nm);
    elements_.push_back(std::move(el));
    return ptr;
}

SchematicElement* SchematicPanel::addVoltageRail(const juce::String& nm,
                                                  juce::Point<float> s,
                                                  juce::Point<float> e,
                                                  float v)
{
    auto el = std::make_unique<SchematicElement>(s, e, v);
    el->setName(nm);
    auto* ptr = el.get();
    elements_.push_back(std::move(el));
    return ptr;
}

void SchematicPanel::addWire(SchematicElement* e0, int t0,
                              SchematicElement* e1, int t1)
{
    wires_.push_back({ e0, t0, e1, t1 });
}

void SchematicPanel::clearAll()
{
    elements_.clear();
    wires_.clear();
}

// ---------------------------------------------------------------------------
// Internal drawing
// ---------------------------------------------------------------------------

void SchematicPanel::drawWires(juce::Graphics& g)
{
    g.setColour(wireColour);
    for (auto& w : wires_)
    {
        auto p0 = w.elemA->getTerminals()[w.termA].pos;
        auto p1 = w.elemB->getTerminals()[w.termB].pos;
        g.drawLine(p0.x, p0.y, p1.x, p1.y, 1.8f);
    }
}

void SchematicPanel::drawNodes(juce::Graphics& g)
{
    g.setColour(nodeColour);
    // Collect unique connection points and draw a small dot at each
    std::vector<juce::Point<float>> pts;
    auto addPt = [&](juce::Point<float> p)
    {
        for (auto& q : pts)
            if ((p - q).getDistanceFromOrigin() < 6.0f)
                return;
        pts.push_back(p);
    };

    for (auto& w : wires_)
    {
        addPt(w.elemA->getTerminals()[w.termA].pos);
        addPt(w.elemB->getTerminals()[w.termB].pos);
    }

    for (auto& p : pts)
        g.fillEllipse(p.x - 3.0f, p.y - 3.0f, 6.0f, 6.0f);
}


//==============================================================================
//                              Demo wiring
//
//  Helper that builds a common-cathode triode stage schematic.
//  You can call this from your editor constructor or replace it with your
//  own layout logic.
//==============================================================================

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