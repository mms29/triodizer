#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

// Stroke widths
inline constexpr float STROKE_NORMAL = 2.5f;
inline constexpr float STROKE_HIGHLIGHT = 5.0f;

// Colors
inline juce::Colour getColourBackground() { return juce::Colour (10, 5, 0); }
inline juce::Colour getColourNormal() { return juce::Colour (195, 176, 123); }
inline juce::Colour getColourHighlight() { return juce::Colour (252, 237, 173); }
inline juce::Colour getColourAmber() { return juce::Colour (255, 166, 38); }
inline juce::Colour getColourElectrical() { return juce::Colour (80, 180, 255); }
inline juce::Colour getColourPurple() { return juce::Colour (180, 110, 255); }
inline juce::Colour getColourHotRed() { return juce::Colour (255, 110, 60); }
inline juce::Colour getColourLaserGreen() { return juce::Colour (51, 255, 102); }
inline juce::Colour getColourGrey() { return juce::Colours::grey; }
inline juce::Colour getVoltageColourGradient(float alpha) { 
    auto k = 10.0f;
    alpha = std::log(1 + k*alpha) / std::log(1 + k);
    if (alpha <0.25f)
        return getColourPurple().interpolatedWith(getColourElectrical(), alpha*4.0f); 
    if (alpha <0.5f)
        return getColourElectrical().interpolatedWith(getColourLaserGreen(), (alpha - 0.25f)*4.0f); 
    if (alpha <0.75f)
        return getColourLaserGreen().interpolatedWith(getColourAmber(), (alpha- 0.5f)*4.0f); 
    
    return getColourAmber().interpolatedWith(getColourHotRed(), (alpha- 0.75f)*4.0f ); 
}
// inline juce::Colour getVoltageColourGradient (float t)
// {
//     t = juce::jlimit (0.0f, 1.0f, t);

//     float hue = 0.75f - 0.75f * t; // purple -> blue -> green -> yellow
//     return juce::Colour::fromHSV (hue, .8f, .5f, 1.0f);
// }
inline juce::Colour getPowerColourGradient(float alpha) { return juce::Colours::black.interpolatedWith(juce::Colours::white, alpha); }

// Fonts
inline constexpr int FONT_MAINTITLE = 24;
inline constexpr int FONT_TITLE = 22;
inline constexpr int FONT_SUB1 = 18;
inline constexpr int FONT_SUB2 = 14;

// Power scaling for display
inline constexpr float POWER_SCALING = .5e3f;


// Tube dimensions
inline constexpr float TUBE_WIDTH = 150.0f;
inline constexpr float TUBE_HEIGHT = 200.0f;

// Resistor zigzag pattern
inline constexpr int RESISTOR_ZIGZAG_COUNT = 6;
inline constexpr float RESISTOR_ZIGZAG_AMPLITUDE = 10.0f;
inline constexpr int RESISTOR_ZIGZAG_LENGTH = 40;

// Capacitor plates
inline constexpr int CAPACITOR_PLATE_WIDTH = 35;
inline constexpr float CAPACITOR_PLATE_GAP = 10.0f;

//One term
inline constexpr float SCHEMATIC_GAIN_SIZE = 50.0f;
inline constexpr float SCHEMATIC_GROUND_SIZE = 20.0f;
inline constexpr float SCHEMATIC_DIODE_SIZE = 20.0f;
inline constexpr float SCHEMATIC_JUNC_SIZE = 5.0f;
inline constexpr float SCHEMATIC_JACK_SIZE = 35.0f;

// Transformer coils
inline constexpr float TRANSFORMER_COIL_LENGTH = 100.0f;
inline constexpr float TRANSFORMER_COIL_WIDTH = 20.0f;

// Voltmeter
inline constexpr float VOLTMETER_RADIUS = 18.0f;

// Layout spacing (for SchematicBuilder)
inline constexpr float SPACING_XS = 50.0f;
inline constexpr float SPACING_S = 75.0f;
inline constexpr float SPACING_M = 100.0f;
inline constexpr float SPACING_L = 150.0f;
inline constexpr float SPACING_XL = 200.0f;

inline constexpr float SCHEMATIC_BUTTON_SIZE = 120.0f;
inline constexpr float SCHEMATIC_INSPECTOR_SIZE = 400.0f;


// Signal Path
inline constexpr float SIGNALPATH_SPEED = 4.0f;
inline constexpr float SIGNALPATH_BEAD_SPACING = 16;
inline constexpr float SIGNALPATH_BEAD_SIZE = 2;
inline constexpr float SIGNALPATH_SAMPLE_SPACING = 4.0f; // pixels
inline constexpr float MONITORING_SMOOTHING_ALPHA=.1f;

// Drive control parameters
inline constexpr float DRIVE_MIN = -60.0f;
inline constexpr float DRIVE_MAX = 60.0f;
inline constexpr float DRIVE_DEFAULT = 0.0f;
inline constexpr float DRIVE_STEP = 0.1f;

// Gain control parameters
inline constexpr float GAIN_MIN = -60.0f;
inline constexpr float GAIN_MAX = 60.0f;
inline constexpr float GAIN_DEFAULT = 0.0f;
inline constexpr float GAIN_STEP = 0.1f;

// Window dimensions
inline constexpr int WINDOW_WIDTH = 1600;
inline constexpr int WINDOW_HEIGHT = 920;
inline constexpr int WINDOW_LEFT_PANEL = 130;
inline constexpr int WINDOW_TOP_PANEL = 60;
inline constexpr int WINDOW_TITLE_SIZE = 250;
inline constexpr int WINDOW_SUBTITLE_SIZE = 300;
inline constexpr int WINDOW_PRESET_SIZE = 500;

inline constexpr int TOGGLE_BUTTON_SIZE = 100;
inline constexpr int INSPECTOR_BUTTON_SIZE = 76;

// Preset selection indices
inline constexpr int PRESET_DEFAULT = 1;
inline constexpr int PRESET_BASSMAN_PREAMP = 2;
inline constexpr int PRESET_DUAL_RECTIFIER_PREAMP = 3;
inline constexpr int PRESET_TWIN_REVERB = 4;
inline constexpr int PRESET_DIODE_CLIPPER = 5;


// Signal path modes
inline constexpr int SIGNALPATH_MODE_NORMAL_FORWARD = 0;
inline constexpr int SIGNALPATH_MODE_REVERSE_FORWARD = 1;   
inline constexpr int SIGNALPATH_MODE_NORMAL_BACKWARD = 2;
inline constexpr int SIGNALPATH_MODE_REVERSE_BACKWARD = 3;   