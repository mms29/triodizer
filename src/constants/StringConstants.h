#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <constants/SchematicConstants.h>


inline const juce::AttributedString DESCR_PLATE_RESISTOR(){
    juce::AttributedString text;
    auto fontTitle = juce::Font (juce::FontOptions(FONT_TITLE)).boldened();
    auto fontContent = juce::Font (juce::FontOptions(FONT_SUB1)).italicised();
    auto fontContentBold = juce::Font (juce::FontOptions(FONT_SUB1)).boldened();
    auto col = getColourGrey();
    text.append ("Plate voltage \n", fontTitle, col);
    text.append ("Determines gain and operating point. \n", fontContent,col);
    
    text.append ("Sound impact: \n", fontContentBold,col);
    text.append ("Higher values increase gain but reduce headroom and can sound more compressed or vintage saggy."
         "Lower values reduce gain and can sound cleaner, tighter, and more linear. \n", fontContent,col);

    text.append ("Value effects: \n", fontContentBold,col);
    text.append ("Higher: more gain, earlier clipping, softer highs \n", fontContent,col);
    text.append ("Lower: more headroom, lower gain, more clarity \n", fontContent,col);
    
    return text;
}

inline const juce::AttributedString DESCR_CATHODE_RESISTOR()
{
    juce::AttributedString text;

    auto fontTitle = juce::Font(juce::FontOptions(FONT_TITLE)).boldened();
    auto fontContent = juce::Font(juce::FontOptions(FONT_SUB1)).italicised();
    auto fontContentBold = juce::Font(juce::FontOptions(FONT_SUB1)).boldened();
    auto col = getColourGrey();

    text.append("Cathode resistor \n", fontTitle, col);

    text.append("Sets the bias of the tube via self-biasing (voltage developed across the cathode). \n",
                fontContent, col);

    text.append("Sound impact: \n", fontContentBold, col);

    text.append(
        "Controls operating point and linearity. "
        "Strongly affects warmth, compression, and headroom. "
        "Affects how early or late the stage enters distortion. \n",
        fontContent, col);

    text.append("Value effects: \n", fontContentBold, col);

    text.append("Higher values: cooler bias, less current, softer attack, earlier breakup, looser feel \n",
                fontContent, col);

    text.append("Lower values: hotter bias, more current, tighter response, more headroom, firmer low end \n",
                fontContent, col);

    return text;
}
inline const juce::AttributedString DESCR_CATHODE_BYPASS_CAP()
{
    juce::AttributedString text;

    auto fontTitle = juce::Font(juce::FontOptions(FONT_TITLE)).boldened();
    auto fontContent = juce::Font(juce::FontOptions(FONT_SUB1)).italicised();
    auto fontContentBold = juce::Font(juce::FontOptions(FONT_SUB1)).boldened();
    auto col = getColourGrey();

    text.append("Cathode bypass capacitor \n", fontTitle, col);

    text.append("Placed in parallel with the cathode resistor to increase AC gain by bypassing part of the cathode degeneration. \n",
                fontContent, col);

    text.append("Sound impact: \n", fontContentBold, col);

    text.append(
        "One of the most important tone-shaping components in a gain stage. "
        "Controls how much low-frequency content is amplified. "
        "Directly affects perceived gain, fullness, and tightness of the stage. \n",
        fontContent, col);

    text.append("Value effects: \n", fontContentBold, col);

    text.append("Large values: full-frequency bypass, more gain, thicker low end, more saturation \n",
                fontContent, col);

    text.append("Small values: partial bypass, reduced bass gain, tighter response, more mid focus \n",
                fontContent, col);

    text.append("No capacitor: no bypassing, lower gain, very linear response, lean and clean tone \n",
                fontContent, col);

    return text;
}
inline const juce::AttributedString DESCR_GRID_STOPPER()
{
    juce::AttributedString text;

    auto fontTitle = juce::Font(juce::FontOptions(FONT_TITLE)).boldened();
    auto fontContent = juce::Font(juce::FontOptions(FONT_SUB1)).italicised();
    auto fontContentBold = juce::Font(juce::FontOptions(FONT_SUB1)).boldened();
    auto col = getColourGrey();

    text.append("Grid stopper resistor \n", fontTitle, col);

    text.append("Small resistor placed directly at the tube grid input, typically right at the socket pin. \n",
                fontContent, col);

    text.append("Sound impact: \n", fontContentBold, col);

    text.append(
        "Primarily used for stability and high-frequency control. "
        "Prevents parasitic oscillations and taming of ultrasonic oscillations. "
        "Can subtly shape top-end harshness when values are increased. \n",
        fontContent, col);

    text.append("Value effects: \n", fontContentBold, col);

    text.append("Small values (1k–10k): mostly transparent, stability only, minimal tone change \n",
                fontContent, col);

    text.append("Medium values (10k–33k): smoother high end, reduced edge, slightly softer attack \n",
                fontContent, col);

    text.append("Large values (33k–68k+): noticeably smoother highs, earlier treble roll-off, more controlled distortion \n",
                fontContent, col);

    return text;
}

inline const juce::AttributedString DESCR_GRID_LEAK_RESISTOR()
{
    juce::AttributedString text;

    auto fontTitle = juce::Font(juce::FontOptions(FONT_TITLE)).boldened();
    auto fontContent = juce::Font(juce::FontOptions(FONT_SUB1)).italicised();
    auto fontContentBold = juce::Font(juce::FontOptions(FONT_SUB1)).boldened();
    auto col = getColourGrey();

    text.append("Grid leak resistor \n", fontTitle, col);

    text.append("Provides a DC reference path for the control grid, ensuring it has a defined bias point. \n",
                fontContent, col);

    text.append("Sound impact: \n", fontContentBold, col);

    text.append(
        "Usually transparent in normal operation, but it strongly influences input impedance and how the stage interacts with preceding circuitry. "
        "Extremely large values can make the stage feel more open but also more noise-prone and sensitive to leakage currents. \n",
        fontContent, col);

    text.append("Value effects: \n", fontContentBold, col);

    text.append("Higher values (1M–5M): higher input impedance, more sensitivity, slightly more gain perception, increased noise susceptibility \n",
                fontContent, col);

    text.append("Lower values (220k–1M): more loading on previous stage, reduced sensitivity, slightly darker and more stable behavior \n",
                fontContent, col);

    return text;
}


inline const juce::AttributedString DESCR_TRIODE_COMMON_CATHODE()
{
    juce::AttributedString text;

    auto fontTitle = juce::Font(juce::FontOptions(FONT_TITLE)).boldened();
    auto fontContent = juce::Font(juce::FontOptions(FONT_SUB1)).italicised();
    auto fontContentBold = juce::Font(juce::FontOptions(FONT_SUB1)).boldened();
    auto col = getColourGrey();

    text.append("Triode – Common cathode amplifier \n", fontTitle, col);

    text.append("The standard voltage amplification configuration for a triode tube stage. Input is applied to the grid, output is taken from the plate, with the cathode referenced to ground via a resistor (and optionally bypass capacitor). \n",
                fontContent, col);

    text.append("Sound impact: \n", fontContentBold, col);

    text.append(
        "Primary gain stage topology in most tube amps. "
        "Produces voltage gain with strong harmonic character, typically rich in even-order harmonics when driven. "
        "Adds warmth, musical distortion, and natural compression when pushed into non-linearity. \n",
        fontContent, col);

    text.append("Behavior: \n", fontContentBold, col);

    text.append(
        "Inverts phase, provides moderate-to-high voltage gain depending on biasing and load. "
        "Highly sensitive to plate resistor, cathode resistor, and bypass capacitor values. \n",
        fontContent, col);

    text.append("Key feel: \n", fontContentBold, col);

    text.append("Open, dynamic, and harmonically rich amplification stage; core building block of tube amp tone shaping. \n",
                fontContent, col);

    return text;
}

inline const juce::AttributedString DESCR_CATHODE_FOLLOWER()
{
    juce::AttributedString text;

    auto fontTitle = juce::Font(juce::FontOptions(FONT_TITLE)).boldened();
    auto fontContent = juce::Font(juce::FontOptions(FONT_SUB1)).italicised();
    auto fontContentBold = juce::Font(juce::FontOptions(FONT_SUB1)).boldened();
    auto col = getColourGrey();

    text.append("Triode – Cathode follower \n", fontTitle, col);

    text.append("A buffer configuration where the output is taken from the cathode instead of the plate. The stage provides approximately unity voltage gain but significantly lowers output impedance. \n",
                fontContent, col);

    text.append("Sound impact: \n", fontContentBold, col);

    text.append(
        "Extremely transparent in most operating conditions. "
        "Does not significantly amplify voltage, but greatly improves current drive and impedance matching between stages. "
        "Often perceived as tightening the response and increasing clarity in downstream circuits. \n",
        fontContent, col);

    text.append("Behavior: \n", fontContentBold, col);

    text.append(
        "Non-inverting stage with near unity gain. "
        "Strongly reduces loading effects on tone stacks or following gain stages. "
        "Highly dependent on correct biasing and sufficient current through the cathode resistor. \n",
        fontContent, col);

    text.append("Key feel: \n", fontContentBold, col);

    text.append("Clean buffer stage that preserves tone while improving drive capability, commonly used for tone stacks and output buffering. \n",
                fontContent, col);

    return text;
}

inline const juce::AttributedString DESCR_VOLUME_POT()
{
    juce::AttributedString text;

    auto fontTitle = juce::Font(juce::FontOptions(FONT_TITLE)).boldened();
    auto fontContent = juce::Font(juce::FontOptions(FONT_SUB1)).italicised();
    auto fontContentBold = juce::Font(juce::FontOptions(FONT_SUB1)).boldened();
    auto col = getColourGrey();

    text.append("Volume potentiometer \n", fontTitle, col);

    text.append("Variable resistor used to control signal level by attenuating the input or interstage signal. It does not amplify, only reduces signal amplitude depending on wiper position. \n",
                fontContent, col);

    text.append("Sound impact: \n", fontContentBold, col);

    text.append(
        "Strongly interacts with preceding stage impedance and any bright cap present. "
        "Can significantly affect perceived brightness, dynamics, and high-frequency retention at lower settings. "
        "Also influences loading of the previous gain stage, subtly shaping tone even at full volume. \n",
        fontContent, col);

    text.append("Behavior: \n", fontContentBold, col);

    text.append(
        "Acts as a voltage divider between signal source and ground. "
        "Different taper types (audio/log vs linear) drastically affect perceived smoothness of volume change. "
        "High resistance values reduce loading but may increase noise sensitivity. \n",
        fontContent, col);

    text.append("Value effects: \n", fontContentBold, col);

    text.append("Higher values (500k–1M): less loading on previous stage, brighter response, more interaction with parasitic capacitances \n",
                fontContent, col);

    text.append("Lower values (100k–250k): heavier loading, smoother top end, slightly darker tone, more stable interaction with preceding stage \n",
                fontContent, col);

    return text;
}
inline const juce::AttributedString DESCR_COUPLING_CAPACITOR()
{
    juce::AttributedString text;

    auto fontTitle = juce::Font(juce::FontOptions(FONT_TITLE)).boldened();
    auto fontContent = juce::Font(juce::FontOptions(FONT_SUB1)).italicised();
    auto fontContentBold = juce::Font(juce::FontOptions(FONT_SUB1)).boldened();
    auto col = getColourGrey();

    text.append("Coupling capacitor \n", fontTitle, col);

    text.append("Capacitor placed between amplifier stages to block DC while allowing AC audio signal to pass. It isolates bias points of adjacent stages. \n",
                fontContent, col);

    text.append("Sound impact: \n", fontContentBold, col);

    text.append(
        "One of the most important low-frequency shaping components in a tube signal path. "
        "Directly determines how much bass content is preserved or attenuated between stages. "
        "Can make an amp sound tight and focused or loose and vintage depending on value and interacting impedances. \n",
        fontContent, col);

    text.append("Behavior: \n", fontContentBold, col);

    text.append(
        "Forms a high-pass filter with the input resistance of the next stage. "
        "Its cutoff frequency defines low-end roll-off between gain stages. "
        "Also influences feel of attack and low-end 'punch'. \n",
        fontContent, col);

    text.append("Value effects: \n", fontContentBold, col);

    text.append("Larger values: deeper bass extension, fuller tone, looser low-end response, earlier saturation in bass frequencies \n",
                fontContent, col);

    text.append("Smaller values: tighter bass, reduced low-end rumble, more mid-focused response, cleaner low-frequency headroom \n",
                fontContent, col);

    text.append("Too small values: thin tone, weak bass response, overly bright or harsh perception due to missing low-end balance \n",
                fontContent, col);

    return text;
}