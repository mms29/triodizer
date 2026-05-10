cmake_minimum_required(VERSION 3.22)
project(test VERSION 1.0.0)

# Add JUCE as a subdirectory (relative to project root) and specify where to build it
add_subdirectory(${CMAKE_SOURCE_DIR}/../juce ${CMAKE_BINARY_DIR}/juce_build)

# Create the executable target
add_executable(test
    TestWDF.cpp
)

# Link JUCE modules
target_link_libraries(test PRIVATE
    juce::juce_core
    juce::juce_dsp
    juce::juce_audio_basics
    juce::juce_audio_formats
    juce::juce_gui_basics
    juce::juce_graphics
)

# chowdsp_wdf include path (header-only library) and project src
target_include_directories(test PRIVATE
    ${CMAKE_SOURCE_DIR}/../chowdsp_wdf/include
    ${CMAKE_SOURCE_DIR}/../src
)

# C++ standard
target_compile_features(test PRIVATE cxx_std_17)

# Enable useful warnings
target_compile_definitions(test PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST2_ENABLED=0
    JUCE_IGNORE_VST3_MISMATCHED_PARAMETER_ID_WARNING=1
)