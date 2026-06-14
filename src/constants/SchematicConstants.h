#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

// ==============================================================================
// Schematic rendering constants - colors, strokes, fonts

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

// Fonts
inline constexpr int FONT_TITLE = 22;
inline constexpr int FONT_SUB1 = 18;
inline constexpr int FONT_SUB2 = 14;

// Power scaling for display
inline constexpr float POWER_SCALING = 1e3f;

// ==============================================================================
// Schematic element size constants

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


// ==============================================================================
// Plugin parameter and UI constants
// Shared between PluginProcessor and PluginEditor

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

// Preset selection indices
inline constexpr int PRESET_DEFAULT = 1;
inline constexpr int PRESET_COMMONCATHODE = 2;
inline constexpr int PRESET_BASSMAN_TS = 3;
inline constexpr int PRESET_BASSMAN_PREAMP_SMALL = 4;
inline constexpr int PRESET_BASSMAN_PREAMP = 5;
inline constexpr int PRESET_DUAL_RECTIFIER_PREAMP = 6;
inline constexpr int PRESET_LCLADDER = 7;
inline constexpr int PRESET_TWIN_REVERB = 8;