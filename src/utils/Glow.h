#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "constants/SchematicConstants.h"


void drawGlowPath(juce::Graphics& g,
                  const juce::Path& path,
                  float intensity, 
                  juce::Colour coreColour, 
                  juce::Colour glowColour, 
                  bool highlight);