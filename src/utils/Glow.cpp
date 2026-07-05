#include "utils/Glow.h"


void drawGlowAndCorePath(juce::Graphics& g,
                  const juce::Path& path,
                  float intensity, 
                  juce::Colour coreColour, 
                  juce::Colour glowColour, 
                  bool highlight
)
{
    drawGlowPath(g, path, glowColour, intensity);
    drawCorePath(g, path, coreColour, highlight);
}



void drawSolidCorePath(juce::Graphics& g,
                  const juce::Path& path,
                bool highlight,
                const juce::Colour& col
)
{
    g.setColour(highlight ? juce::Colours::white :  col);
    g.strokePath(path, juce::PathStrokeType( highlight ? STROKE_HIGHLIGHT : STROKE_NORMAL, STROKE_JOINT_STYLE, STROKE_END_STYLE));
}


void drawCorePath(juce::Graphics& g,
                  const juce::Path& path,
                  juce::Colour coreColour, 
                  bool highlight
)
{

    float coreIntensity = 1.0f;

    auto white = juce::Colours::white;


    // // contrast
    g.setColour(getColourBackground().withAlpha(0.5f * coreIntensity));
    g.strokePath(path, juce::PathStrokeType(5.5f, STROKE_JOINT_STYLE, STROKE_END_STYLE));

    // Bright plasma
    g.setColour(coreColour.withAlpha(0.6f * coreIntensity));
    g.strokePath(path, juce::PathStrokeType(4.0f, STROKE_JOINT_STYLE, STROKE_END_STYLE));
    g.setColour(coreColour.withAlpha(0.7f * coreIntensity));
    g.strokePath(path, juce::PathStrokeType(3.0f, STROKE_JOINT_STYLE, STROKE_END_STYLE));

    // White hot core
    g.setColour(white.withAlpha(coreIntensity));
    g.strokePath(path, juce::PathStrokeType(1.5f, STROKE_JOINT_STYLE, STROKE_END_STYLE));

    if (highlight){

        g.setColour(white.withAlpha(coreIntensity));
        g.strokePath(path, juce::PathStrokeType(STROKE_HIGHLIGHT, STROKE_JOINT_STYLE, STROKE_END_STYLE));
    }
}


void drawGlowPath(juce::Graphics& g,
                  const juce::Path& path,
                  juce::Colour glowColour,
                  float intensity
)
{
    intensity = juce::jlimit (0.0f, 1.0f, intensity);

    if(intensity>0.0f){
        int init = 5-intensity*5;
        for (int i = init; i<9; i++){
            g.setColour(glowColour.withAlpha(0.006f*(i+1)*(i+1) * intensity));
            g.strokePath(path, juce::PathStrokeType(6* (10-i), STROKE_JOINT_STYLE, STROKE_END_STYLE));
        }
    }

}


void drawPowerGlowPath(juce::Graphics& g,
                  const juce::Path& path,
                  float intensity
)
{
    auto colorE2 = getPowerColourGradient(0.0); 
    auto colorE1 = getPowerColourGradient(0.5); 
    auto colorE0 = getPowerColourGradient(1.0); 
    intensity = juce::jlimit (0.0f, 1.0f, intensity);


    auto intensityE2 = juce::jlimit (0.0f, 1.0f, intensity*2e2f);
    auto intensityE1 = juce::jlimit (0.0f, 1.0f, intensity*2e1f);
    auto intensityE0 = juce::jlimit (0.0f, 1.0f, intensity*2e0f);
    drawGlowPath(g, path, colorE2, intensityE2*0.5f);
    if (intensity>0.01f ){
        drawGlowPath(g, path, colorE1,intensityE1*0.5f);
        if (intensity>0.1f){
            drawGlowPath(g, path, colorE0, intensityE0*0.5f);
        }
    }
}
