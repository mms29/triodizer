#include "utils/Glow.h"


void drawGlowPath(juce::Graphics& g,
                  const juce::Path& path,
                  float intensity, 
                  juce::Colour coreColour, 
                  juce::Colour glowColour, 
                  bool highlight
)
{

    intensity = juce::jlimit (0.0f, 1.0f, intensity);
    float coreIntensity = 0.7f + intensity*0.3f;

    auto white = juce::Colours::white;

    juce::PathStrokeType::JointStyle joinStyle = juce::PathStrokeType::curved;
    juce::PathStrokeType::EndCapStyle endStyle = juce::PathStrokeType::rounded;

    // if(intensity>0.0f){}

    int init = 5-intensity*5;
    for (int i = init; i<9; i++){
        g.setColour(glowColour.withAlpha(0.006f*(i+1)*(i+1) * intensity));
        g.strokePath(path, juce::PathStrokeType(6* (10-i), joinStyle, endStyle));
    }
    // // contrast
    g.setColour(getColourBackground().withAlpha(0.5f * coreIntensity));
    g.strokePath(path, juce::PathStrokeType(5.5f, joinStyle, endStyle));

    // Bright plasma
    g.setColour(coreColour.withAlpha(0.6f * coreIntensity));
    g.strokePath(path, juce::PathStrokeType(4.0f, joinStyle, endStyle));
    g.setColour(coreColour.withAlpha(0.7f * coreIntensity));
    g.strokePath(path, juce::PathStrokeType(3.0f, joinStyle, endStyle));

    // White hot core
    g.setColour(white.withAlpha(coreIntensity));
    g.strokePath(path, juce::PathStrokeType(1.5f, joinStyle, endStyle));

    if (highlight){

        g.setColour(white.withAlpha(coreIntensity));
        g.strokePath(path, juce::PathStrokeType(STROKE_HIGHLIGHT, joinStyle, endStyle));
    }
}
