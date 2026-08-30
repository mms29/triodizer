#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "schematic/SchematicPanel.h"
#include "dsp/TriodeGainStage.h"
#include "dsp/BassmanPreamp.h"
#include "dsp/FullBassmanPreamp.h"
#include "dsp/DualRectifierPreamp.h"
#include "dsp/DiodeClipper.h"
#include "dsp/TwinReverb.h"
#include "dsp/BassmanToneStack.h"
#include "constants/StringConstants.h"


class SchematicBuilder
{
public:
    SchematicBuilder () = default;

    void buildDefault(SchematicPanel& schematic);
    // void buildCommonCathodeStage(SchematicPanel& schematic);
    void buildBassmanToneStack(SchematicPanel& schematic);
    void buildTriodeGainStage(SchematicPanel& schematic);
    void buildBassmanPreamp(SchematicPanel& schematic);
    void buildDualRectifierPreamp(SchematicPanel& schematic);
    void buildTwinReverb(SchematicPanel& schematic);
    void buildDiodeClipper(SchematicPanel& schematic);
    void buildFenderToneStack(SchematicPanel& schematic);

private:

        inline static const Terminal left   {-1.0f,  0.0f};
        inline static const Terminal right  { 1.0f,  0.0f};
        inline static const Terminal top    { 0.0f, -1.0f};
        inline static const Terminal bottom { 0.0f,  1.0f};

        inline static const Terminal leftXS   = left   * SPACING_XS;
        inline static const Terminal rightXS  = right  * SPACING_XS;
        inline static const Terminal topXS    = top    * SPACING_XS;
        inline static const Terminal bottomXS = bottom * SPACING_XS;

        inline static const Terminal leftS    = left   * SPACING_S;
        inline static const Terminal rightS   = right  * SPACING_S;
        inline static const Terminal topS     = top    * SPACING_S;
        inline static const Terminal bottomS  = bottom * SPACING_S;

        inline static const Terminal leftM    = left   * SPACING_M;
        inline static const Terminal rightM   = right  * SPACING_M;
        inline static const Terminal topM     = top    * SPACING_M;
        inline static const Terminal bottomM  = bottom * SPACING_M;

        inline static const Terminal leftL    = left   * SPACING_L;
        inline static const Terminal rightL   = right  * SPACING_L;
        inline static const Terminal topL     = top    * SPACING_L;
        inline static const Terminal bottomL  = bottom * SPACING_L;

        inline static const Terminal leftXL   = left   * SPACING_XL;
        inline static const Terminal rightXL  = right  * SPACING_XL;
        inline static const Terminal topXL    = top    * SPACING_XL;
        inline static const Terminal bottomXL = bottom * SPACING_XL;
};

static const std::vector<ValueChoice> triodeChoices =
{
    { "12AX7",    0.0f},
    { "12AX7A",   1.0f},
    { "12AX7ASYL",2.0f},
    { "12AT7",    3.0f},
    { "12AU7",    4.0f},
    { "12AY7",    5.0f},
    { "12AZ7",    6.0f},
    { "12BH7A",   7.0f},
    { "6AN8T",    8.0f},
    { "6DJ8",     9.0f},
    { "7025",     10.0f},
    { "SV6N1P",   11.0f},
    { "ECC83",    12.0f},
    { "ECC81",    13.0f}
};
static const std::vector<ValueChoice> diodeChoices =
{
    { "1N4148",  0.0f},
    { "1N914",   1.0f},
    { "1N4007",  2.0f},
    { "1N34A",   3.0f},
    { "1N5819",  4.0f},
    { "GZ34",  5.0f}
};