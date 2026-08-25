#pragma once

#include <juce_gui_basics/juce_gui_basics.h>


enum PresetId
{
    PRESET_DEFAULT = 0,

    // Preamps
    PRESET_BASSMAN_PREAMP,
    PRESET_DUAL_RECTIFIER_PREAMP,
    PRESET_TWIN_REVERB,

    // Distortion / Coloring
    PRESET_DIODE_CLIPPER,
    PRESET_TRIODE_GAIN_STAGE,

    PRESET_COUNT
};
struct PresetInfo
{
    int id;
    const char* name;
    const char* category;
};
static constexpr PresetInfo presets[]
{
    { PRESET_DEFAULT,             "Default",                    "General" },
    { PRESET_BASSMAN_PREAMP,      "Fender Bassman Preamp",      "Preamps" },
    { PRESET_DUAL_RECTIFIER_PREAMP, "Mesa/Boogie Dual Rectifier", "Preamps" },
    { PRESET_TWIN_REVERB,          "Twin Reverb",               "Preamps" },
    { PRESET_DIODE_CLIPPER,        "Diode Clipper",             "Distortion & Coloring" },
    { PRESET_TRIODE_GAIN_STAGE,    "Triode Gain Stage",         "Distortion & Coloring" }
};

inline juce::StringArray getPresetNames()
{
    juce::StringArray names;

    for (const auto& preset : presets)
        names.add (preset.name);

    return names;
}
constexpr int menuIdForPreset (int presetIndex)
{
    return presetIndex + 1;
}

constexpr int presetIndexFromMenuId (int menuId)
{
    return menuId - 1;
}