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

    // FILTERs
    PRESET_FENDER_TONE_STACK,

    PRESET_COUNT
};
struct PresetInfo
{
    int id;
    const char* name;
    const char* category;
};
static constexpr PresetInfo PRESET_LIST_DETAILS[]
{
    { PRESET_DEFAULT,             "Default",                    "General" },
    { PRESET_BASSMAN_PREAMP,      "Fender Bassman",              "Preamps" },
    { PRESET_DUAL_RECTIFIER_PREAMP, "Mesa/Boogie Dual Rectifier", "Preamps" },
    { PRESET_TWIN_REVERB,          "Twin Reverb",               "Preamps" },
    { PRESET_DIODE_CLIPPER,        "Diode Clipper",             "Distortion & Coloring" },
    { PRESET_TRIODE_GAIN_STAGE,    "Triode Gain Stage",         "Distortion & Coloring" },
    { PRESET_FENDER_TONE_STACK,    "Fender Tone Stack",         "Filters" }
};

inline juce::StringArray getPresetNames()
{
    juce::StringArray names;

    for (const auto& preset : PRESET_LIST_DETAILS)
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

inline juce::PopupMenu buildPresetMenu(){

        juce::PopupMenu menu, menu_preamps, menu_coloring, menu_filters;

        for (const auto& preset : PRESET_LIST_DETAILS)
        {
            auto itemId = menuIdForPreset (preset.id);

            if (juce::String (preset.category) == "Distortion & Coloring")
                menu_coloring.addItem (itemId, preset.name);

            else if (juce::String (preset.category) == "Filters")
                menu_filters.addItem (itemId, preset.name);

            else if (juce::String (preset.category) == "Preamps")
                menu_preamps.addItem (itemId, preset.name);

            else if (juce::String (preset.category) == "General")
                menu.addItem (itemId, preset.name);
        }
        menu.addSubMenu ("Distortion & Coloring", menu_coloring);
        menu.addSubMenu ("Filters", menu_filters);
        menu.addSubMenu ("Preamps", menu_preamps);
        return menu;
}