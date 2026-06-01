cmake_minimum_required(VERSION 3.22)
set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "" FORCE)

project(TriodePlugin VERSION 1.0.0)

include(FetchContent)

FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.13   
)

FetchContent_MakeAvailable(juce)

FetchContent_Declare(
    chowdsp_wdf
    GIT_REPOSITORY https://github.com/Chowdhury-DSP/chowdsp_wdf.git
    GIT_TAG main 
)

FetchContent_MakeAvailable(chowdsp_wdf)

# Create the plugin target
juce_add_plugin(TriodizerPlugin
    COMPANY_NAME "VuilzAudio"
    PLUGIN_MANUFACTURER_CODE "VzAu"
    PLUGIN_CODE "TRDZ"
    FORMATS VST3 AU Standalone
    ProductName "Triodizer"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
)

# Add source files
target_sources(TriodizerPlugin PRIVATE
    src/plugin/PluginProcessor.cpp
    src/plugin/PluginEditor.cpp
    src/gui/WaveformDisplay.cpp
    src/gui/Knob.cpp
    src/schematic/SchematicElement.cpp
    src/schematic/TwoTermElement.cpp
    src/schematic/OneTermElement.cpp
    src/schematic/TriodeElement.cpp
    src/schematic/SchematicPanel.cpp
)

# Link JUCE modules
target_link_libraries(TriodizerPlugin PRIVATE
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_gui_basics
)

# chowdsp_wdf include path (header-only library)
# target_include_directories(TriodePlugin PRIVATE
#     ${CMAKE_SOURCE_DIR}/chowdsp_wdf/include
#     src
#     )
target_include_directories(TriodizerPlugin PRIVATE
    ${chowdsp_wdf_SOURCE_DIR}/include
    src
)
# C++ standard
target_compile_features(TriodizerPlugin PRIVATE cxx_std_17)

# Enable useful warnings
target_compile_definitions(TriodizerPlugin PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST2_ENABLED=0
    JUCE_IGNORE_VST3_MISMATCHED_PARAMETER_ID_WARNING=1
)

# Disable VST2 build globally
set(JUCE_BUILD_VST2_CLIENT FALSE CACHE BOOL "" FORCE)

