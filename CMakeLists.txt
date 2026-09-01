cmake_minimum_required(VERSION 3.22)

project(Cathodyne VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/EHsc)
endif()

include(FetchContent)

set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "" FORCE)

FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.13   
)
FetchContent_MakeAvailable(juce)

FetchContent_Declare(
    xsimd
    GIT_REPOSITORY https://github.com/xtensor-stack/xsimd.git
    GIT_TAG 14.2.0 
)
FetchContent_MakeAvailable(xsimd)

FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
)

FetchContent_GetProperties(eigen)

if (NOT eigen_POPULATED)
    FetchContent_Populate(eigen)
endif()

add_library(Eigen3::Eigen INTERFACE IMPORTED GLOBAL)

set_target_properties(Eigen3::Eigen PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${eigen_SOURCE_DIR}"
)

FetchContent_Declare(
    chowdsp_wdf
    GIT_REPOSITORY     https://github.com/mms29/chowdsp_wdf.git #  https://github.com/Chowdhury-DSP/chowdsp_wdf.git
    GIT_TAG patch-1 
)

FetchContent_MakeAvailable(chowdsp_wdf)


# Create the plugin target
juce_add_plugin(Cathodyne
    COMPANY_NAME "RemAudio"
    PLUGIN_MANUFACTURER_CODE "RA"
    PLUGIN_CODE "TRDZ"
    FORMATS VST3 AU Standalone
    ProductName "Cathodyne"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
)

# Add source files
target_sources(Cathodyne PRIVATE
    src/plugin/PluginProcessor.cpp
    src/plugin/PluginEditor.cpp
    src/gui/WaveformDisplay.cpp
    src/gui/Knob.cpp
    src/gui/Button.cpp
    src/gui/Selector.cpp
    src/schematic/SchematicElement.cpp
    src/schematic/TwoTermElement.cpp
    src/schematic/MultiTermElement.cpp
    src/schematic/OneTermElement.cpp
    src/schematic/TriodeElement.cpp
    src/schematic/SchematicPanel.cpp
    src/schematic/SchematicBuilder.cpp
    src/utils/Glow.cpp
    src/utils/SignalPath.cpp
)

# Link JUCE modules
target_link_libraries(Cathodyne PRIVATE
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_gui_basics
    xsimd
    Eigen3::Eigen
)

# target_include_directories(Cathodyne PRIVATE
#     ${CMAKE_SOURCE_DIR}/chowdsp_wdf/include
#     src
#     )
target_include_directories(Cathodyne PRIVATE
    ${chowdsp_wdf_SOURCE_DIR}/include
    src
)

# Enable useful warnings
target_compile_definitions(Cathodyne PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST2_ENABLED=0
    JUCE_IGNORE_VST3_MISMATCHED_PARAMETER_ID_WARNING=1
)


# Disable VST2 build globally
set(JUCE_BUILD_VST2_CLIENT FALSE CACHE BOOL "" FORCE)

