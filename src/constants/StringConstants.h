#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <constants/SchematicConstants.h>

inline juce::AttributedString markdownToAttributedString (
    const juce::String& markdown)
{
    juce::AttributedString text;

    auto fontTitle   = juce::Font (juce::FontOptions (FONT_TITLE)).boldened();
    auto fontsubTitle   = juce::Font (juce::FontOptions (FONT_SUB1)).boldened();
    auto fontContent = juce::Font (juce::FontOptions (FONT_SUB1)).italicised();
    auto fontNewline = juce::Font (juce::FontOptions (10)).italicised();

    auto col = getColourGrey();

    auto lines = juce::StringArray::fromLines (markdown);

    for (auto line : lines)
    {
        line = line.trim();

        if (line.isEmpty())
        {
            text.append ("\n");
            continue;
        }

        if (line.startsWith ("# "))
        {
            auto title = line.substring (2).trim();

            text.append ("\n", fontNewline, col);
            text.append (
                title,
                fontTitle,
                col);
            text.append ("\n", fontNewline, col);
        }
        else if (line.startsWith ("##"))
        {
            auto title = line.substring (2).trim();

            text.append ("\n", fontNewline, col);
            text.append (
                title,
                fontsubTitle,
                col);
            text.append ("\n", fontNewline, col);
        }
        else if (line.startsWith ("\n"))
        {
            text.append ("\n", fontNewline, col);
        }
        else
        {
            text.append (
                "\t\t" +line,
                fontContent,
                col);
            text.append ("\n", fontNewline, col);

        }
    }

    return text;
}

inline const juce::AttributedString DESCR_PLATE_RESISTOR()
{
    return markdownToAttributedString (
        R"(# Plate resistor

        Determines gain and operating point.

        Higher values increase gain but reduce headroom and can sound more compressed.

        Lower values reduce gain and increase headroom, can sound cleaner, tighter, and more linear.
        )");
}
inline const juce::AttributedString DESCR_CATHODE_RESISTOR()
{
    return markdownToAttributedString (
        R"(# Cathode resistor

        Sets the bias of the tube via self-biasing (voltage developed across the cathode).

        Controls operating point and linearity. Strongly affects warmth, compression, and headroom. Affects how early or late the stage enters distortion.

        Higher values: cooler bias, less current, softer attack, earlier breakup, looser feel

        Lower values: hotter bias, more current, tighter response, more headroom, firmer low end
        )");
}
inline const juce::AttributedString DESCR_CATHODE_BYPASS_CAP()
{
    return markdownToAttributedString (
        R"(# Cathode bypass capacitor

        Placed in parallel with the cathode resistor to increase AC gain by bypassing part of the cathode degeneration.

        One of the most important tone-shaping components in a gain stage. Controls how much low-frequency content is amplified. Directly affects perceived gain, fullness, and tightness of the stage.

        Large values: full-frequency bypass, more gain, thicker low end, more saturation

        Small values: partial bypass, reduced bass gain, tighter response, more mid focus

        No capacitor: no bypassing, lower gain, very linear response, lean and clean tone
        )");
}
inline const juce::AttributedString DESCR_GRID_STOPPER()
{
    return markdownToAttributedString (
        R"(# Grid stopper resistor

        Small resistor placed directly at the tube grid input, typically right at the socket pin.

        Primarily used for stability and high-frequency control. Prevents parasitic oscillations and taming of ultrasonic oscillations. Can subtly shape top-end harshness when values are increased.

        Small values (1k-10k): mostly transparent, stability only, minimal tone change

        Medium values (10k-33k): smoother high end, reduced edge, slightly softer attack

        Large values (33k-68k+): noticeably smoother highs, earlier treble roll-off, more controlled distortion
        )");
}

inline const juce::AttributedString DESCR_GRID_LEAK_RESISTOR()
{
    return markdownToAttributedString (
        R"(# Grid leak resistor

        Provides a DC reference path for the control grid, ensuring it has a defined bias point. Usually transparent in normal operation.

        Higher values (1M-5M): higher input impedance, more sensitivity, slightly more gain perception, increased noise susceptibility

        Lower values (220k-1M): more loading on previous stage, reduced sensitivity, slightly darker and more stable behavior
        )");
}


inline const juce::AttributedString DESCR_TRIODE_COMMON_CATHODE()
{
    return markdownToAttributedString (
        R"(# Triode - Common cathode amplifier

        The standard voltage amplification configuration for a triode tube stage. Input is applied to the grid, output is taken from the plate, with the cathode referenced to ground via a resistor (and optionally bypass capacitor).

        ## Sound impact:

        Primary gain stage topology in most tube amps. Produces voltage gain with strong harmonic character, typically rich in even-order harmonics when driven. Adds warmth, musical distortion, and natural compression when pushed into non-linearity.

        ## Behavior:

        Inverts phase, provides moderate-to-high voltage gain depending on biasing and load. Highly sensitive to plate resistor, cathode resistor, and bypass capacitor values.
        ## Value effects:

        Higher transconductance: increases voltage gain and can produce stronger drive into the following stage. Often gives a more immediate and responsive character.

        Lower transconductance: reduces gain and generally produces a softer, less aggressive response. The stage becomes more dependent on the load and bias point.

        12AX7: high amplification factor and relatively low transconductance. The standard choice for high-gain common-cathode stages, providing substantial voltage gain and early distortion when driven hard.

        12AT7: higher transconductance and lower plate resistance than the 12AX7. Provides lower voltage gain but stronger current drive and a firmer response. Often useful where more headroom and drive capability are desired.

        12AU7: low amplification factor with relatively high current capability. Produces substantially less voltage gain than a 12AX7 and is useful when a cleaner, lower-gain stage is desired.

        12AY7: medium amplification factor with lower gain than a 12AX7. Often used for a smoother, lower-gain response while retaining the basic character of a small-signal triode stage.
        )");
}

inline const juce::AttributedString DESCR_CATHODE_FOLLOWER()
{
    return markdownToAttributedString (
        R"(# Triode - Cathode follower

        A buffer configuration where the output is taken from the cathode instead of the plate. The stage provides approximately unity voltage gain but significantly lowers output impedance.

        ## Sound impact:

        Extremely transparent in most operating conditions. Does not significantly amplify voltage, but greatly improves current drive for the following stage. 

        ## Behavior:

        Non-inverting stage with near unity gain. Strongly reduces loading effects on tone stacks or following gain stages. Highly dependent on correct biasing and sufficient current through the cathode resistor.

        ## Value effects:

        Higher transconductance: lowers output impedance and improves current drive, giving a firmer and more controlled response.

        Lower transconductance: higher output impedance and weaker current drive, making the stage more sensitive to the following load.

        12AX7: low transconductance and high gain. Common and usable, but not the strongest choice when low output impedance is the priority.

        12AT7: high transconductance and strong current drive. A common choice for cathode followers requiring a lower output impedance.

        12AU7: high current capability with lower voltage gain. Can provide a strong cathode follower when substantial current drive is desired.

        )");
    
}

inline const juce::AttributedString DESCR_VOLUME_POT()
{
    return markdownToAttributedString (
        R"(# Volume potentiometer

        Variable resistor used to control signal level by attenuating the input or interstage signal. It reduces signal amplitude depending on wiper position.

        ## Sound impact:

        Strongly interacts with preceding stage impedance and any bright cap present. Can significantly affect perceived brightness, dynamics, and high-frequency retention at lower settings. Also influences loading of the previous gain stage, subtly shaping tone even at full volume.

        ## Behavior:

        Acts as a voltage divider between signal source and ground. Different taper types (audio/log vs linear) drastically affect perceived smoothness of volume change. High resistance values reduce loading but may increase noise sensitivity.

        ## Value effects:

        Higher values (500k-1M): less loading on previous stage, brighter response, more interaction with parasitic capacitances

        Lower values (100k-250k): heavier loading, smoother top end, slightly darker tone, more stable interaction with preceding stage
        )");
}
inline const juce::AttributedString DESCR_COUPLING_CAPACITOR()
{
    return markdownToAttributedString (
        R"(# Coupling capacitor

        Capacitor placed between amplifier stages to block DC while allowing AC audio signal to pass. It isolates bias points of adjacent stages.

        ## Sound impact:

        One of the most important low-frequency shaping components in a tube signal path. Directly determines how much bass content is preserved or attenuated between stages. Can make an amp sound tight and focused or loose and vintage depending on value and interacting impedances.

        ## Behavior:

        Forms a high-pass filter with the input resistance of the next stage. Its cutoff frequency defines low-end roll-off between gain stages. Also influences feel of attack and low-end 'punch'.

        ## Value effects:

        Larger values: deeper bass extension, fuller tone, looser low-end response, earlier saturation in bass frequencies

        Smaller values: tighter bass, reduced low-end rumble, more mid-focused response, cleaner low-frequency headroom

        Too small values: thin tone, weak bass response, overly bright or harsh perception due to missing low-end balance
        )");
}
inline const juce::AttributedString DESCR_DIODE_PAIR()
{
    return markdownToAttributedString (
        R"(# Diode pair
        
        A pair of diodes used to create symmetrical nonlinear clipping. The diodes begin conducting when the signal exceeds their forward-voltage threshold, limiting the waveform and generating harmonic distortion.

        ## Sound impact:

        Produces fast, symmetrical clipping with a relatively hard transition into distortion. Lower diode thresholds create earlier, denser distortion, while higher thresholds allow more headroom and produce a more open response.

        ## Behavior:

        Clips both positive and negative halves of the waveform. The diode type, forward voltage, and surrounding circuit determine the clipping threshold and character.

        ## Value effects:

        Silicon: lower threshold, earlier clipping, tighter and more aggressive distortion.

        Germanium: lower threshold, softer clipping, warmer and more compressed character.

        LED: higher threshold, more headroom, louder and more open clipping.
        )");
}
