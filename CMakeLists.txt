cmake_minimum_required(VERSION 3.22)
project(TriodePlugin VERSION 1.0.0)

# Add JUCE as a subdirectory
add_subdirectory(juce)

# Create the plugin target
juce_add_plugin(TriodePlugin
    COMPANY_NAME "YourName"
    PLUGIN_MANUFACTURER_CODE "YrNm"
    PLUGIN_CODE "Trd1"
    FORMATS VST3 AU Standalone
    ProductName "Triode"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
)

# Add source files
target_sources(TriodePlugin PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

# Link JUCE modules
target_link_libraries(TriodePlugin PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)

# chowdsp_wdf include path (header-only library)
target_include_directories(TriodePlugin PRIVATE
    ${CMAKE_SOURCE_DIR}/chowdsp_wdf/include
)

# C++ standard
target_compile_features(TriodePlugin PRIVATE cxx_std_17)

# Enable useful warnings
target_compile_definitions(TriodePlugin PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST2_ENABLED=0
    JUCE_IGNORE_VST3_MISMATCHED_PARAMETER_ID_WARNING=1
)

# Disable VST2 build globally
set(JUCE_BUILD_VST2_CLIENT FALSE CACHE BOOL "" FORCE)
