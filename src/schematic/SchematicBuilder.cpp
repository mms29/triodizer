#include "schematic/SchematicBuilder.h"

// ===================================================================================================================
// DEFAULT
// ===================================================================================================================
void SchematicBuilder::buildDefault(SchematicPanel& schematic)
{
    auto t1 = Terminal {600, 300};
    auto t2 = Terminal {700, 300};
    schematic.addElement (std::make_unique<JunctionElement>(t1));
    schematic.addElement (std::make_unique<JunctionElement>(t2));
    schematic.addWire (t1, t2);
}
// ===================================================================================================================
// HELPERs
// ===================================================================================================================


void SchematicBuilder::_buildFenderToneStack(
    SchematicPanel& schematic,
    Terminal& toneStackPosition, 
    juce::String nameR4, int paramR4,  int monitorR4,
    juce::String nameC1, int paramC1,  int monitorC1,
    juce::String nameC2, int paramC2,  int monitorC2,
    juce::String nameC3, int paramC3,  int monitorC3,
    juce::String nameTreble, int paramTreble,  int controlTreble, int monitorRplusTreble, int monitorRminusTreble,
    juce::String nameMid, int paramMid,  int controlMid, int monitorRplusMid, int monitorRminusMid,
    juce::String nameBass, int paramBass,  int controlBass, int monitorBass, 
    juce::String nameInputSig
){

    bool midVarRes = monitorRplusMid==-1;

    schematic.addElement (std::make_unique<ResistorElement> (
        nameR4,
        toneStackPosition + bottomL,
        toneStackPosition,
        paramR4, 
        monitorR4,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        (nameInputSig == "" ) ? nullptr : schematic.getElement(nameInputSig)->getSignalPath()
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        nameC1,
        toneStackPosition,
        toneStackPosition + rightL,
        paramC1,
        monitorC1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        (nameInputSig == "" ) ? nullptr : schematic.getElement(nameInputSig)->getSignalPath()
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        nameC2,
        toneStackPosition + bottomL,
        toneStackPosition + bottomL + rightL,
        paramC2,
        monitorC2,
        SIGNALPATH_MODE_NORMAL_BACKWARD,
        schematic.getElement(nameR4)->getSignalPath()
    ));
    schematic.addElement (std::make_unique<PotElement> (
        nameTreble,
        schematic.getElement(nameC1)->getTerminals()[1],
        schematic.getElement(nameC2)->getTerminals()[1],
        schematic.getElement(nameC1)->getTerminals()[1] + rightXS + bottomL*0.5f ,
        controlTreble,
        paramTreble,
        monitorRplusTreble,
        monitorRminusTreble,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement(nameC1)->getSignalPath()
    ));
    schematic.addElement (std::make_unique<VarResElement> (
        nameBass,
        schematic.getElement(nameC2)->getTerminals()[1],
        schematic.getElement(nameC2)->getTerminals()[1] + (midVarRes ? bottomL : bottomM),
        controlBass,
        paramBass,
        monitorBass,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement(nameC2)->getSignalPath()
    ));
    if (monitorRplusMid!=-1)
    {
        schematic.addElement (std::make_unique<CapacitorElement> (
            nameC3,
            toneStackPosition + bottomL*2.0f + rightL*0.25f,
            toneStackPosition + bottomL*2.0f + rightL*0.75f,
            paramC3,
            monitorC3,
            SIGNALPATH_MODE_NORMAL_BACKWARD,
            schematic.getElement(nameR4)->getSignalPath()
        ));
        schematic.getElement(nameC3)->addPointToTerminal(toneStackPosition + bottomL*2.0f,0);
        schematic.getElement(nameC3)->addPointToTerminal(toneStackPosition + bottomL,0);
        schematic.addElement (std::make_unique<PotElement> (
            nameMid,
            schematic.getElement(nameBass)->getTerminals()[1]  + bottomM,
            schematic.getElement(nameBass)->getTerminals()[1] ,
            schematic.getElement(nameBass)->getTerminals()[1]  + bottomM*0.5f + leftXS,
            controlMid,
            paramMid,
            monitorRplusMid,
            monitorRminusMid,
            SIGNALPATH_MODE_NORMAL_FORWARD,
            schematic.getElement(nameC3)->getSignalPath()
        ));
        schematic.addElement (std::make_unique<GroundElement>(schematic.getElement(nameMid)->getTerminals()[0]) );
    }
    else{
        schematic.addElement (std::make_unique<CapacitorElement> (
            nameC3,
            toneStackPosition + bottomL*2.0f,
            toneStackPosition + bottomL*2.0f + rightL,
            paramC3,
            monitorC3,
            SIGNALPATH_MODE_NORMAL_BACKWARD,
            schematic.getElement(nameR4)->getSignalPath()
        ));
        schematic.getElement(nameC3)->addPointToTerminal(toneStackPosition + bottomL,0);

        schematic.addElement (std::make_unique<VarResElement> (
            nameMid,
            schematic.getElement(nameBass)->getTerminals()[1],
            schematic.getElement(nameBass)->getTerminals()[1] + bottomM,
            controlMid,
            paramMid,
            monitorRminusMid,
            SIGNALPATH_MODE_NORMAL_FORWARD,
            schematic.getElement(nameC3)->getSignalPath()
        ));
        schematic.addElement (std::make_unique<GroundElement>(schematic.getElement(nameMid)->getTerminals()[1]) );

    }

}

void SchematicBuilder::_buildCathodeFollower(
    SchematicPanel& schematic,
    Terminal& pos, 
    juce::String nameE, int paramE,  int monitorE,
    juce::String nameV, int paramV, int monitorV,
    juce::String nameRk, int paramRk, int monitorRk,
    juce::String nameRg 
)
{
    auto Vpos = getTriodeTerminals(pos);

    schematic.addElement (std::make_unique<VoltageElement>(
        nameE,
        Vpos[1] + topXS, 
        paramE,
        monitorE
    ));
    schematic.addElement (std::make_unique<TriodeElement> (
        nameV, 
        pos,
        paramV,
        0, triodeChoices,
        monitorV,
        SIGNALPATH_MODE_REVERSE_FORWARD,
        schematic.getElement(nameE)->getSignalPath(),
        (nameRg == "" ) ? nullptr : schematic.getElement(nameRg)->getSignalPath(),
        DESCR_CATHODE_FOLLOWER()
    ));

    schematic.addElement (std::make_unique<WireElement>(
        std::vector<Terminal>{
            Vpos[1] + topXS,
            Vpos[1],
        },
        monitorE,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        nullptr
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        nameRk,
        Vpos[2]+ bottomXL,
        Vpos[2],
        paramRk,
        monitorRk,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement(nameV)->getSignalPath(),
        DESCR_CATHODE_RESISTOR()
    ));

    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement(nameRk)->getTerminals()[0]) );

}
void SchematicBuilder::_buildCommonCathodeUnbypassed(
    SchematicPanel& schematic,
    Terminal& pos, 
    juce::String nameE, int paramE,  int monitorE,
    juce::String nameRp, int paramRp, int monitorRp,
    juce::String nameV, int paramV, int monitorV,
    juce::String nameRk, int paramRk, int monitorRk,
    juce::String nameRg 
)
{
    auto V1pos = getTriodeTerminals(pos);

    // Plate Circuit
    schematic.addElement (std::make_unique<VoltageElement>(
        nameE,
        V1pos[1] + topL, 
        paramE,
        monitorE
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        nameRp,
        V1pos[1],
        V1pos[1] + topL,
        paramRp,
        monitorRp,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement(nameE)->getSignalPath(),
        DESCR_PLATE_RESISTOR()
    ));

    schematic.addElement (std::make_unique<TriodeElement> (
        nameV, 
        pos,
        paramV,
        0, triodeChoices,
        monitorV,
        SIGNALPATH_MODE_REVERSE_FORWARD,
        schematic.getElement(nameRp)->getSignalPath(),
        (nameRg == "" ) ? nullptr : schematic.getElement(nameRg)->getSignalPath(),
        DESCR_TRIODE_COMMON_CATHODE()
    ));


    // Cathode Circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        nameRk,
        V1pos[2]+ bottomL,
        V1pos[2],
        paramRk,
        monitorRk,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement(nameV)->getSignalPath(),
        DESCR_CATHODE_RESISTOR()
    ));
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement(nameRk)->getTerminals()[0]) );

}

void SchematicBuilder::_buildCommonCathode(
    SchematicPanel& schematic,
    Terminal& pos, 
    juce::String nameE, int paramE,  int monitorE,
    juce::String nameRp, int paramRp, int monitorRp,
    juce::String nameV, int paramV, int monitorV,
    juce::String nameRk, int paramRk, int monitorRk,
    juce::String nameCk, int paramCk, int monitorCk,
    juce::String nameRg 
)
{
    _buildCommonCathodeUnbypassed(
        schematic,
        pos,
        nameE, paramE, monitorE,
        nameRp, paramRp, monitorRp,
        nameV, paramV, monitorV,
        nameRk, paramRk, monitorRk,
        nameRg
    );
    schematic.addElement (std::make_unique<CapacitorElement> (
        nameCk,
        schematic.getElement(nameRk)->getTerminals()[1] +  rightXS,
        schematic.getElement(nameRk)->getTerminals()[0] +  rightXS,
        paramCk,
        monitorCk,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement(nameV)->getSignalPath(),
        DESCR_CATHODE_BYPASS_CAP()
    ));
    schematic.getElement(nameCk)->addPointToTerminal(schematic.getElement(nameRk)->getTerminals()[1], 0);
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement(nameCk)->getTerminals()[1]) );

}

// ===================================================================================================================
// DIODE CLIPPER
// ===================================================================================================================
void SchematicBuilder::buildDiodeClipper(SchematicPanel& schematic)
{
    using Param         =  DiodeClipperCircuit::Param;
    using Control       =  DiodeClipperCircuit::Control;
    using Monitoring    =  DiodeClipperCircuit::Monitoring;

    auto t0 = Terminal {0, 0};

    schematic.addElement (std::make_unique<CapacitorElement> (
        "C1",
        t0 + rightL + rightM + bottomL,
        t0 + rightL + rightM,
        (int) Param::C1,
        (int) Monitoring::C1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        nullptr
    ));

    schematic.addElement (std::make_unique<GainElement>("Gain",
        t0, 
        t0 + rightL,
        (int) Param::Gain,
        (int) Control::Gain,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("C1")->getSignalPath()
    ));
    
    schematic.addElement (std::make_unique<ResistorElement> (
        "R1",
        schematic.getElement("Gain")->getTerminals()[1],
        schematic.getElement("Gain")->getTerminals()[1] + rightM,
        (int) Param::R1,
        (int) Monitoring::R1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("C1")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<DiodeElement>(
        "D1",
        schematic.getElement("R1")->getTerminals()[1] + bottomL+rightM,
        schematic.getElement("R1")->getTerminals()[1] +rightM,
        (int) Param::D1,
        0,
        diodeChoices,
        (int) Monitoring::D1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("C1")->getSignalPath()
    ));
    
    schematic.addElement (std::make_unique<DiodeElement>(
        "D2",
        schematic.getElement("D1")->getTerminals()[1] + rightXS,
        schematic.getElement("D1")->getTerminals()[0] + rightXS,
        (int) Param::D1,
        0,
        diodeChoices,
        (int) Monitoring::D1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("C1")->getSignalPath()
    ));


    
    schematic.addElement (std::make_unique<JackElement>("OUTPUT",schematic.getElement("D2")->getTerminals()[0] + rightL));
    schematic.addElement (std::make_unique<JackElement>("INPUT",t0, JUSTIFY_RIGHT));
    
    schematic.addElement (std::make_unique<WireElement>(
        std::vector<Terminal> {
            schematic.getElement("R1")->getTerminals()[1], 
            schematic.getElement("D2")->getTerminals()[0] + rightL,
        },
        (int) Monitoring::D1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("C1")->getSignalPath()
    ));

    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("D2")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("D1")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("C1")->getTerminals()[0]) );

}

// ===================================================================================================================
// DUAL RECTIFIER
// ===================================================================================================================
void SchematicBuilder::buildDualRectifierPreamp(SchematicPanel& schematic)
{
    using Param         =  DualRectifierPreampCircuit::Param;
    using Control       =  DualRectifierPreampCircuit::Control;
    using Monitoring    =  DualRectifierPreampCircuit::Monitoring;

    //============================================================================================================
    //V1

    auto V1center = Terminal {0, 0};
    auto V1pos = getTriodeTerminals(V1center);

    // Grid circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg1",
        V1pos[0]  + leftL,
        V1pos[0] ,
        (int) Param::Rg1 ,
        (int) Monitoring::Rg1 ,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        nullptr,
        DESCR_GRID_LEAK_RESISTOR()
    ));

    _buildCommonCathode(
        schematic,
        V1center,
        "E1", (int) Param::E1, (int) Monitoring::E1,
        "Rp1", (int) Param::Rp1, (int) Monitoring::Rp1,
        "V1", (int) Param::V1, (int) Monitoring::V1,
        "Rk1", (int) Param::Rk1, (int) Monitoring::Rk1,
        "Ck1", (int) Param::Ck1, (int) Monitoring::Ck1,
        "Rg1"
    );

    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cp1",
        V1pos[1],
        V1pos[1] + rightXL,
        (int) Param::Cp1,
        (int) Monitoring::Cp1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Rp1")->getSignalPath(),
        DESCR_COUPLING_CAPACITOR()
    ));

    //============================================================================================================
    // Tone stack 1
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ca1",
        schematic.getElement("Cp1")->getTerminals()[1],
        schematic.getElement("Cp1")->getTerminals()[1] + rightM,
        (int) Param::Ca1,
        (int) Monitoring::Ca1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Cp1")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Ra1",
        schematic.getElement("Ca1")->getTerminals()[1] + bottomXS,
        schematic.getElement("Ca1")->getTerminals()[0] + bottomXS,
        (int) Param::Ra1,
        (int) Monitoring::Ra1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Cp1")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cb1",
        schematic.getElement("Ra1")->getTerminals()[1] + bottomL,
        schematic.getElement("Ra1")->getTerminals()[0] + bottomL,
        (int) Param::Cb1,
        (int) Monitoring::Cb1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Cp1")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rb1",
        schematic.getElement("Cb1")->getTerminals()[1] + bottomXS,
        schematic.getElement("Cb1")->getTerminals()[0] + bottomXS,
        (int) Param::Rb1,
        (int) Monitoring::Rb1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Cp1")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rc1",
        schematic.getElement("Rb1")->getTerminals()[1] + bottomM,
        schematic.getElement("Rb1")->getTerminals()[1],
        (int) Param::Rb1,
        (int) Monitoring::Rc1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Cp1")->getSignalPath()
    ));

    schematic.addElement (std::make_unique<WireElement>(
        std::vector<Terminal>{
            schematic.getElement("Ca1")->getTerminals()[0],
            schematic.getElement("Rb1")->getTerminals()[1],
        },
        (int) Monitoring::Rb1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Cp1")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<WireElement>(
        std::vector<Terminal>{
            schematic.getElement("Ca1")->getTerminals()[1],
            schematic.getElement("Rb1")->getTerminals()[0],
        },
        (int) Monitoring::Rb1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Ca1")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<PotElement> (
        "Volume",
        schematic.getElement("Ca1")->getTerminals()[1] + rightM,
        schematic.getElement("Ca1")->getTerminals()[1] + rightM + bottomXL,
        schematic.getElement("Ca1")->getTerminals()[1] + rightM + rightS + bottomXL*0.5f ,
        (int) Control::Volume,
        (int) Param::RVol,
        (int) Monitoring::RVol_plus,
        (int) Monitoring::RVol_minus
    ));
    schematic.addElement (std::make_unique<WireElement>(
        std::vector<Terminal>{
            schematic.getElement("Ca1")->getTerminals()[1],
            schematic.getElement("Volume")->getTerminals()[0],
        },
        (int) Monitoring::Rb1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Ca1")->getSignalPath()
    ));

    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cbright",
        schematic.getElement("Volume")->getTerminals()[0] +bottomXS,
        schematic.getElement("Volume")->getTerminals()[0] + rightS+bottomXS,
        (int) Param::Cbright,
        (int) Monitoring::Cbright
    ));
    schematic.getElement("Cbright")->addPointToTerminal(schematic.getElement("Volume")->getTerminals()[2], 1);

    //============================================================================================================
    // V2

    auto V2center = schematic.getElement("Volume")->getTerminals()[2]+ rightXL;
    auto V2pos = getTriodeTerminals(V2center);

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg2",
        schematic.getElement("Volume")->getTerminals()[2],
        V2pos[0],
        (int) Param::Rg2,
        (int) Monitoring::Rg2,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Volume")->getSignalPath(),
        DESCR_GRID_STOPPER()
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cg2",
        V2pos[0] + bottomM,
        V2pos[0],
        (int) Param::Cg2,
        (int) Monitoring::Cg2,
        SIGNALPATH_MODE_REVERSE_FORWARD,
        nullptr
    ));


    _buildCommonCathode(
        schematic,
        V2center,
        "E2", (int) Param::E2, (int) Monitoring::E2,
        "Rp2", (int) Param::Rp2, (int) Monitoring::Rp2,
        "V2", (int) Param::V2, (int) Monitoring::V2,
        "Rk2", (int) Param::Rk2, (int) Monitoring::Rk2,
        "Ck2", (int) Param::Ck2, (int) Monitoring::Ck2,
        "Rg2"
    );

    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cp2",
        V2pos[1],
        V2pos[1] + rightXL,
        (int) Param::Cp2,
        (int) Monitoring::Cp2,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Rp2")->getSignalPath(),
        DESCR_COUPLING_CAPACITOR()
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Ra2",
        schematic.getElement("Cp2")->getTerminals()[1],
        schematic.getElement("Cp2")->getTerminals()[1] + bottomM,
        (int) Param::Ra2,
        (int) Monitoring::Ra2,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Cp2")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rb2",
        schematic.getElement("Ra2")->getTerminals()[1],
        schematic.getElement("Ra2")->getTerminals()[1] + bottomXL,
        (int) Param::Rb2,
        (int) Monitoring::Rb2,
        SIGNALPATH_MODE_REVERSE_FORWARD,
        schematic.getElement("Ra2")->getSignalPath(), 
        DESCR_GRID_LEAK_RESISTOR()
    ));

    //============================================================================================================
    // V3

    auto V3center = schematic.getElement("Ra2")->getTerminals()[1] +rightL;
    auto V3pos = getTriodeTerminals(V3center);

    schematic.addElement (std::make_unique<WireElement>(
        std::vector<Terminal>{
            schematic.getElement("Rb2")->getTerminals()[0],
            V3pos[0],
        },
        -1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Ra2")->getSignalPath()
    ));


    _buildCommonCathodeUnbypassed(
        schematic,
        V3center,
        "E3", (int) Param::E3, (int) Monitoring::E3,
        "Rp3", (int) Param::Rp3, (int) Monitoring::Rp3,
        "V3", (int) Param::V3, (int) Monitoring::V3,
        "Rk3", (int) Param::Rk3, (int) Monitoring::Rk3,
        "Ra2"
    );

    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cp3",
        V3pos[1],
        V3pos[1] + rightL,
        (int) Param::Cp3,
        (int) Monitoring::Cp3,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Rp3")->getSignalPath(),
        DESCR_COUPLING_CAPACITOR()
    ));
    schematic.getElement("Cp3")->addPointToTerminal(schematic.getElement("Cp3")->getTerminals()[1]+ bottomM, 1);

    schematic.addElement (std::make_unique<ResistorElement> (
        "Ra3",
        schematic.getElement("Cp3")->getTerminals()[1],
        schematic.getElement("Cp3")->getTerminals()[1]+ bottomL ,
        (int) Param::Ra3,
        (int) Monitoring::Ra3,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Cp3")->getSignalPath(),
        DESCR_GRID_LEAK_RESISTOR()
    ));


    //============================================================================================================
    // V4

    auto V4center = schematic.getElement("Ra3")->getTerminals()[0] +rightL;
    auto V4pos = getTriodeTerminals(V4center);

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg4",
        V4pos[0]  + leftS,
        V4pos[0] ,
        (int) Param::Rg4,
        (int) Monitoring::Rg4,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Ra3")->getSignalPath(),
        DESCR_GRID_STOPPER()
    ));
    _buildCommonCathode(
        schematic,
        V4center,
        "E4", (int) Param::E4, (int) Monitoring::E4,
        "Rp4", (int) Param::Rp4, (int) Monitoring::Rp4,
        "V4", (int) Param::V4, (int) Monitoring::V4,
        "Rk4", (int) Param::Rk4, (int) Monitoring::Rk4,
        "Ck4", (int) Param::Ck4, (int) Monitoring::Ck4,
        "Rg4"
    );


    //============================================================================================================
    // V5

    auto V5center = V4pos[1] +rightXL;
    auto V5pos = getTriodeTerminals(V5center);


    _buildCathodeFollower(
        schematic,
        V5center,
        "E5", (int) Param::E5, (int) Monitoring::E5,
        "V5", (int) Param::V5, (int) Monitoring::V5,
        "Rk5", (int) Param::Rk5, (int) Monitoring::Rk5,
        "Rp4"
    );

    schematic.addElement (std::make_unique<WireElement>(
        std::vector<Terminal>{
            V4pos[1],
            V5pos[0]
        },
        -1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Rp4")->getSignalPath()
    ));
    auto toneStackPosition = V5pos[2] + rightM;

    //============================================================================================================
    // Tone Stack
    _buildFenderToneStack(
        schematic,
        toneStackPosition, 
        "R4", (int) Param::R4, (int) Monitoring::R4,
        "C1", (int) Param::C1, (int) Monitoring::C1,
        "C2", (int) Param::C2, (int) Monitoring::C2,
        "C3", (int) Param::C3, (int) Monitoring::C3,
        "Treble", (int) Param::RTreble, (int) Control::Treble, (int) Monitoring::RTreble_plus, (int) Monitoring::RTreble_minus,
        "Mid", (int) Param::RMid, (int) Control::Mid, (int) Monitoring::RMid_plus, (int) Monitoring::RMid_minus,
        "Bass", (int) Param::RBass, (int) Control::Bass, (int) Monitoring::RBass,
        "V5"
    );

    schematic.addElement (std::make_unique<WireElement>(
        std::vector<Terminal>{
            V5pos[2],
            toneStackPosition,
        },
        (int) Monitoring::R4,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V5")->getSignalPath()
    ));
    //============================================================================================================
    // Presence
    schematic.getElement("Treble")->addPointToTerminal(schematic.getElement("Treble")->getTerminals()[2] + rightM ,2);

    schematic.addElement (std::make_unique<ResistorElement> (
        "R5",
        schematic.getElement("Treble")->getTerminals()[2] + bottomS,
        schematic.getElement("Treble")->getTerminals()[2] ,
        (int) Param::R5,
        (int) Monitoring::R5
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C5",
        schematic.getElement("R5")->getTerminals()[0] + bottomS,
        schematic.getElement("R5")->getTerminals()[0],
        (int) Param::C5,
        (int) Monitoring::C5
    ));

    schematic.addElement (std::make_unique<VarResElement> (
        "Presence",
        schematic.getElement("C5")->getTerminals()[0],
        schematic.getElement("C5")->getTerminals()[0] + bottomM,
        (int) Control::Presence,
        (int) Param::RPres,
        (int) Monitoring::RPres
    ));

    schematic.addElement (std::make_unique<PotElement> (
        "Master",
        schematic.getElement("R5")->getTerminals()[1] + rightM,
        schematic.getElement("R5")->getTerminals()[1] + rightM + bottomXL,
        schematic.getElement("R5")->getTerminals()[1] + rightM + bottomXL*0.5f + rightXS,
        (int) Control::Master,
        (int) Param::RMas,
        (int) Monitoring::RMas_plus,
        (int) Monitoring::RMas_minus
    ));
    schematic.addElement (std::make_unique<WireElement>(
        std::vector<Terminal>{
            schematic.getElement("R5")->getTerminals()[1] ,
            schematic.getElement("R5")->getTerminals()[1] + rightM,
        },
        (int) Monitoring::RMas_plus,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Master")->getSignalPath()
    ));

    // Grounds
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rc1")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Cg2")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Volume")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rb2")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ra3")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Presence")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Master")->getTerminals()[1]) );


    schematic.addElement (std::make_unique<JackElement>("OUTPUT",schematic.getElement("Master")->getTerminals()[2] + rightS));
    schematic.addElement (std::make_unique<JackElement>("INPUT",schematic.getElement("Rg1")->getTerminals()[0], JUSTIFY_RIGHT));
}


//============================================================================================================
    // Tone Stack
void SchematicBuilder::buildFenderToneStack(SchematicPanel& schematic)
{
    using Param         =  BassmanToneStackCircuitT<float>::Param;
    using Control       =  BassmanToneStackCircuitT<float>::Control;
    using Monitoring    =  BassmanToneStackCircuitT<float>::Monitoring;

    auto toneStackPosition = Terminal {0,0};

    _buildFenderToneStack(
        schematic,
        toneStackPosition, 
        "R4", (int) Param::R4, (int) Monitoring::R4,
        "C1", (int) Param::C1, (int) Monitoring::C1,
        "C2", (int) Param::C2, (int) Monitoring::C2,
        "C3", (int) Param::C3, (int) Monitoring::C3,
        "Treble", (int) Param::RTreble, (int) Control::Treble, (int) Monitoring::RTreble_plus, (int) Monitoring::RTreble_minus,
        "Mid", (int) Param::RMid, (int) Control::Mid, (int) Monitoring::RMid_plus, (int) Monitoring::RMid_minus,
        "Bass", (int) Param::RBass, (int) Control::Bass, (int) Monitoring::RBass
    );
    schematic.getElement("Treble")->addPointToTerminal(schematic.getElement("Treble")->getTerminals()[2] + rightL, 2, false);

    schematic.addElement (std::make_unique<WireElement>(
        std::vector<Terminal>{
            toneStackPosition +leftL ,
            toneStackPosition
        },
        (int) Monitoring::R4,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        nullptr
    ));

    // I/O
    schematic.addElement (std::make_unique<JackElement>("INPUT", toneStackPosition +leftL, JUSTIFY_RIGHT) );
    schematic.addElement (std::make_unique<JackElement>("OUTPUT",schematic.getElement("Treble")->getTerminals()[2], JUSTIFY_LEFT) );
}
// ===================================================================================================================
// TRIODE GAIN STAGE
// ===================================================================================================================
void SchematicBuilder::buildTriodeGainStage(SchematicPanel& schematic)
{
    using Param         =  TriodeGainStageCircuit::Param;
    using Control       =  TriodeGainStageCircuit::Control;
    using Monitoring    =  TriodeGainStageCircuit::Monitoring;


    //V1
    auto V1center = Terminal {0, 0};
    auto V1pos = getTriodeTerminals(V1center);

    // Grid circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Ri1",
        V1pos[0]  + leftM,
        V1pos[0]  + leftM + bottomL,
        (int) Param::Ri1,
        (int) Monitoring::Ri1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        nullptr,
        DESCR_GRID_LEAK_RESISTOR()
    ));

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg1",
        V1pos[0]  + leftM,
        V1pos[0] ,
        (int) Param::Rg1 ,
        (int) Monitoring::Rg1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Ri1")->getSignalPath(),
        DESCR_GRID_LEAK_RESISTOR()
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ci1",
        V1pos[0]  + leftM,
        V1pos[0]  + leftM + leftL,
        (int) Param::Ci1,
        (int) Monitoring::Ci1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Ri1")->getSignalPath(),
        DESCR_COUPLING_CAPACITOR()
    ));

    schematic.addElement (std::make_unique<GainElement>("Gain",
        schematic.getElement("Ci1")->getTerminals()[1]+ leftL, 
        schematic.getElement("Ci1")->getTerminals()[1],
        (int) Param::Gain,
        (int) Control::Gain,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Ci1")->getSignalPath()
    ));

    // Plate Circuit
    schematic.addElement (std::make_unique<VoltageElement>(
        "E1",
        V1pos[1] + topL, 
        (int) Param::E1,
        (int) Monitoring::E1
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp1",
        V1pos[1],
        V1pos[1] + topL,
        (int) Param::Rp1,
        (int) Monitoring::Rp1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("E1")->getSignalPath(),
        DESCR_PLATE_RESISTOR()
    ));
    schematic.addElement (std::make_unique<TriodeElement> (
        "V1", 
        V1center,
        (int) Param::V1,
        0, 
        triodeChoices,
        (int) Monitoring::V1,
        SIGNALPATH_MODE_REVERSE_FORWARD,
        schematic.getElement("Rp1")->getSignalPath(),
        schematic.getElement("Rg1")->getSignalPath(),
        DESCR_TRIODE_COMMON_CATHODE()
    ));

    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cp1",
        V1pos[1],
        V1pos[1] + rightXL,
        (int) Param::Cp1,
        (int) Monitoring::Cp1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Rp1")->getSignalPath(),
        DESCR_COUPLING_CAPACITOR()
    ));

    // Cathode Circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk1",
        V1pos[2]+ bottomL,
        V1pos[2],
        (int) Param::Rk1,
        (int) Monitoring::Rk1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V1")->getSignalPath(),
        DESCR_CATHODE_RESISTOR()
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck1",
        V1pos[2] +  rightXS,
        V1pos[2]+ bottomL +  rightXS,
        (int) Param::Ck1,
        (int) Monitoring::Ck1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V1")->getSignalPath(),
        DESCR_CATHODE_BYPASS_CAP()
    ));
    schematic.getElement("Ck1")->addPointToTerminal(V1pos[2], 0, false);

    schematic.addElement (std::make_unique<PotElement> (
        "Volume",
        schematic.getElement("Cp1")->getTerminals()[1],
        schematic.getElement("Cp1")->getTerminals()[1] + bottomXL,
        schematic.getElement("Cp1")->getTerminals()[1] + rightXS + bottomXL*0.5f ,
        (int) Control::Volume,
        (int) Param::RVol,
        (int) Monitoring::RVol_plus,
        (int) Monitoring::RVol_minus,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Cp1")->getSignalPath()
    ));

    // Grounds
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ri1")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk1")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck1")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Volume")->getTerminals()[1]) );

    // I/O
    schematic.addElement (std::make_unique<JackElement>("INPUT",schematic.getElement("Gain")->getTerminals()[0], JUSTIFY_RIGHT) );
    schematic.addElement (std::make_unique<JackElement>("OUTPUT",schematic.getElement("Volume")->getTerminals()[2], JUSTIFY_LEFT) );

}



// ===================================================================================================================
// BASSMAN PREAMP
// ===================================================================================================================
void SchematicBuilder::buildBassmanPreamp(SchematicPanel& schematic)
{
    using Param         =  FullBassmanPreampCircuitT<float>::Param;
    using Control       =  FullBassmanPreampCircuitT<float>::Control;
    using Monitoring    =  FullBassmanPreampCircuitT<float>::Monitoring;


    //V1
    auto V1center = Terminal {0, 0};
    auto V1pos = getTriodeTerminals(V1center);

    // Grid circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Ri1",
        V1pos[0]  + leftM,
        V1pos[0]  + leftM + bottomL,
        (int) Param::Ri1,
        (int) Monitoring::Ri1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        nullptr,
        DESCR_GRID_LEAK_RESISTOR()
    ));

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg1",
        V1pos[0]  + leftM,
        V1pos[0] ,
        (int) Param::Rg1 ,
        (int) Monitoring::Rg1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Ri1")->getSignalPath(),
        DESCR_GRID_LEAK_RESISTOR()
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ci1",
        V1pos[0]  + leftM + leftL,
        V1pos[0]  + leftM,
        (int) Param::Ci1,
        (int) Monitoring::Ci1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Ri1")->getSignalPath(),
        DESCR_COUPLING_CAPACITOR()
    ));

    _buildCommonCathode(
        schematic,
        V1center,
        "E1", (int) Param::E1, (int) Monitoring::E1,
        "Rp1", (int) Param::Rp1, (int) Monitoring::Rp1,
        "V1", (int) Param::V1, (int) Monitoring::V1,
        "Rk1", (int) Param::Rk1, (int) Monitoring::Rk1,
        "Ck1", (int) Param::Ck1, (int) Monitoring::Ck1,
        "Rg1"
    );

    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cp1",
        V1pos[1],
        V1pos[1] + rightXL,
        (int) Param::Cp1,
        (int) Monitoring::Cp1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Rp1")->getSignalPath(),
        DESCR_COUPLING_CAPACITOR()
    ));

    schematic.addElement (std::make_unique<PotElement> (
        "Volume",
        schematic.getElement("Cp1")->getTerminals()[1],
        schematic.getElement("Cp1")->getTerminals()[1] + bottomXL,
        schematic.getElement("Cp1")->getTerminals()[1] + rightXS + bottomXL*0.5f ,
        (int) Control::Volume,
        (int) Param::RVol,
        (int) Monitoring::RVol_plus,
        (int) Monitoring::RVol_minus,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Cp1")->getSignalPath()
    ));

    // V2
    auto V2center = schematic.getElement("Volume")->getTerminals()[2]+ rightL;
    auto V2pos = getTriodeTerminals(V2center);

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg2",
        schematic.getElement("Volume")->getTerminals()[2],
        V2pos[0],
        (int) Param::Rg2,
        (int) Monitoring::Rg2,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Volume")->getSignalPath(),
        DESCR_GRID_STOPPER()
    ));
    _buildCommonCathodeUnbypassed(
        schematic,
        V2center,
        "E2", (int) Param::E2, (int) Monitoring::E2,
        "Rp2", (int) Param::Rp2, (int) Monitoring::Rp2,
        "V2", (int) Param::V2, (int) Monitoring::V2,
        "Rk2", (int) Param::Rk2, (int) Monitoring::Rk2,
        "Rg2"
    );

    // V3
    auto V3center = V2pos[1]+ rightXL;
    auto V3pos = getTriodeTerminals(V3center);
    _buildCathodeFollower(
        schematic,
        V3center,
        "E3", (int) Param::E3, (int) Monitoring::E3,
        "V3", (int) Param::V3, (int) Monitoring::V3,
        "Rk3", (int) Param::Rk3, (int) Monitoring::Rk3,
        "Rp2"
    );


    // Tone Stack
    auto toneStackPosition = V3pos[2] + rightXL;
    _buildFenderToneStack(
        schematic,
        toneStackPosition, 
        "R4", (int) Param::R4, (int) Monitoring::R4,
        "C1", (int) Param::C1, (int) Monitoring::C1,
        "C2", (int) Param::C2, (int) Monitoring::C2,
        "C3", (int) Param::C3, (int) Monitoring::C3,
        "Treble", (int) Param::RTreble, (int) Control::Treble, (int) Monitoring::RTreble_plus, (int) Monitoring::RTreble_minus,
        "Mid", (int) Param::RMid, (int) Control::Mid, (int) Monitoring::RMid_plus, (int) Monitoring::RMid_minus,
        "Bass", (int) Param::RBass, (int) Control::Bass, (int) Monitoring::RBass,
        "V3"
    );

    schematic.getElement("Treble")->addPointToTerminal(schematic.getElement("Treble")->getTerminals()[2] + rightL, 2);

    schematic.addElement (std::make_unique<WireElement>(
        std::vector<Terminal>{
        V3pos[2] ,
        schematic.getElement("C1")->getTerminals()[0] },
        (int) Monitoring::R4,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V3")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<WireElement>(
        std::vector<Terminal>{V2pos[1], V3pos[0]},
        -1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Rp2")->getSignalPath()
    ));
    // Grounds
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ri1")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Volume")->getTerminals()[1]) );

    // I/O
    schematic.addElement (std::make_unique<JackElement>("OUTPUT",schematic.getElement("Treble")->getTerminals()[2] ));
    schematic.addElement (std::make_unique<JackElement>("INPUT",schematic.getElement("Ci1")->getTerminals()[0], JUSTIFY_RIGHT) );

}





// ===================================================================================================================
// Twin Reverb
// ===================================================================================================================
void SchematicBuilder::buildTwinReverb(SchematicPanel& schematic)
{
    using Param         =  TwinReverbCircuit::Param;
    using Control       =  TwinReverbCircuit::Control;
    using Monitoring    =  TwinReverbCircuit::Monitoring;

    //============================================================================================================
    //V1
    auto V1center = Terminal {0, 0};
    auto V1pos = getTriodeTerminals(V1center);
    
    // Grid circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg1",
        V1pos[0] +bottomL +leftXS,
        V1pos[0] +leftXS,
        (int) Param::Rg1 ,
        (int) Monitoring::Rg1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        nullptr,
        DESCR_GRID_LEAK_RESISTOR()
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Ri1",
        V1pos[0]  + leftL+leftXS,
        V1pos[0] +leftXS,
        (int) Param::Ri1 ,
        (int) Monitoring::Ri1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Rg1")->getSignalPath()
    ));

    _buildCommonCathode(
        schematic,
        V1center,
        "E1", (int) Param::E1, (int) Monitoring::E1,
        "Rp1", (int) Param::Rp1, (int) Monitoring::Rp1,
        "V1", (int) Param::V1, (int) Monitoring::V1,
        "Rk1", (int) Param::Rk1, (int) Monitoring::Rk1,
        "Ck1", (int) Param::Ck1, (int) Monitoring::Ck1,
        "Rg1"
    );

    //============================================================================================================
    // Tone Stack
    auto toneStackPosition = V1pos[1] + rightXL;

    _buildFenderToneStack(
        schematic,
        toneStackPosition, 
        "R4", (int) Param::R4, (int) Monitoring::R4,
        "C1", (int) Param::C1, (int) Monitoring::C1,
        "C2", (int) Param::C2, (int) Monitoring::C2,
        "C3", (int) Param::C3, (int) Monitoring::C3,
        "Treble", (int) Param::RTreble, (int) Control::Treble, (int) Monitoring::RTreble_plus, (int) Monitoring::RTreble_minus,
        "Mid", (int) Param::RMid, (int) Control::Mid, -1, (int) Monitoring::RMid_minus,
        "Bass", (int) Param::RBass, (int) Control::Bass, (int) Monitoring::RBass,
        "Rp1"
    );


    schematic.addElement (std::make_unique<PotElement> (
        "Volume",
        schematic.getElement("Treble")->getTerminals()[2] + rightM,
        schematic.getElement("Treble")->getTerminals()[2] + rightM + bottomXL,
        schematic.getElement("Treble")->getTerminals()[2] + rightM + bottomXL*0.5f + rightS,
        (int) Control::Volume,
        (int) Param::RVol,
        (int) Monitoring::RVol_plus,
        (int) Monitoring::RVol_minus,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Treble")->getSignalPath()
    ));

    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cbright",
        schematic.getElement("Volume")->getTerminals()[0] +bottomXS,
        schematic.getElement("Volume")->getTerminals()[0] + rightS+bottomXS,
        (int) Param::Cbright,
        (int) Monitoring::Cbright,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Treble")->getSignalPath()
    ));
    schematic.getElement("Cbright")->addPointToTerminal(schematic.getElement("Volume")->getTerminals()[2], 1);

    //============================================================================================================
    // V2
    auto V2center = schematic.getElement("Volume")->getTerminals()[2]+ rightL;
    auto V2pos = getTriodeTerminals(V2center);

    _buildCommonCathode(
        schematic,
        V2center,
        "E2", (int) Param::E2, (int) Monitoring::E2,
        "Rp2", (int) Param::Rp2, (int) Monitoring::Rp2,
        "V2", (int) Param::V2, (int) Monitoring::V2,
        "Rk2", (int) Param::Rk2, (int) Monitoring::Rk2,
        "Ck2", (int) Param::Ck2, (int) Monitoring::Ck2,
        "Volume"
    );

    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cp2",
        V2pos[1],
        V2pos[1] + rightXL,
        (int) Param::Cp2,
        (int) Monitoring::Cp2,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Rp2")->getSignalPath(),
        DESCR_COUPLING_CAPACITOR()
    ));

    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cfilt",
        schematic.getElement("Cp2")->getTerminals()[1]+ bottomXL,
        schematic.getElement("Cp2")->getTerminals()[1]+ bottomXL +rightM,
        (int) Param::Cfilt,
        (int) Monitoring::Cfilt,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Cp2")->getSignalPath()
    ));
    schematic.getElement("Cfilt")->addPointToTerminal(schematic.getElement("Cp2")->getTerminals()[1], 0);

    schematic.addElement (std::make_unique<ResistorElement> (
        "Ra2",
        schematic.getElement("Cfilt")->getTerminals()[1] +bottomM,
        schematic.getElement("Cfilt")->getTerminals()[1],
        (int) Param::Ra2,
        (int) Monitoring::Ra2,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Cfilt")->getSignalPath(),
        DESCR_GRID_LEAK_RESISTOR()
    ));
    //============================================================================================================
    // V3
    auto V3center = schematic.getElement("Cfilt")->getTerminals()[1]+rightL;
    auto V3pos = getParallelTriodeTerminals(V3center);
    schematic.getElement("Cfilt")->addPointToTerminal(V3pos[0], 1);

    schematic.addElement (std::make_unique<VoltageElement>(
        "E3",
        V3pos[1]+topL, 
        (int) Param::E3, 
        (int) Monitoring::E3
    ));

    schematic.addElement (std::make_unique<TransformerElement> (
        "Treverb",
        V3pos[1],                   V3pos[1]+topL, 
        V3pos[1] + rightXS +topXS*0.5F     ,   V3pos[1]+topM +topXS*0.5F+ rightXS, 
        (int) Param::TR3,
        (int) Monitoring::TVerbPrim,
        (int) Monitoring::TVerbSec,
        SIGNALPATH_MODE_NORMAL_BACKWARD,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("E3")->getSignalPath(),
        nullptr
    ));

    schematic.addElement (std::make_unique<ParallelTriodeElement> (
        "V3", 
        V3center, 
        (int) Param::V3,
        0, triodeChoices,
        (int) Monitoring::V3,
        SIGNALPATH_MODE_REVERSE_FORWARD,
        schematic.getElement("Treverb")->getSignalPath(0),
        schematic.getElement("Ra2")->getSignalPath(),
        DESCR_TRIODE_COMMON_CATHODE()
    ));
    
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk3",
        V3pos[2]+ bottomM + leftXS,
        V3pos[2]+ leftXS,
        (int) Param::Rk3,
        (int) Monitoring::Rk3,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V3")->getSignalPath(),
        DESCR_CATHODE_RESISTOR()
    ));
    schematic.getElement("Rk3")->addPointToTerminal(V3pos[2], 1, false);

    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck3",
        V3pos[2]+ rightXS,
        V3pos[2]+ bottomM + rightXS,
        (int) Param::Ck3,
        (int) Monitoring::Ck3,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V3")->getSignalPath(),
        DESCR_CATHODE_BYPASS_CAP()
    ));
    schematic.getElement("Ck3")->addPointToTerminal(V3pos[2], 0);

    //============================================================================================================
    // V4
    auto V4center = V3pos[0] +rightXL*3 + rightXS;
    auto V4pos = getTriodeTerminals(V4center);

    auto tank = std::make_unique<ReverbTankElement> (
        "Rev. Tank",
        V4pos[0]+  leftXL +leftXS ,
        V4pos[0] ,
        (int) Monitoring::TVerbSec,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Treverb")->getSignalPath(1)
    );
    tank->addFloatParam ((int) Param::SpringFeedback, "Feedback",
        [] (float v) { return juce::String (v * 100.0f, 1) + " %"; },
        [] (const juce::String& s) { return s.getFloatValue() / 100.0f; });
    tank->addFloatParam ((int) Param::SpringDelay, "Delay",
        [] (float v) { return juce::String (v, 0) + " ms"; },
        [] (const juce::String& s) { return s.getFloatValue(); });
    tank->addFloatParam ((int) Param::SpringDecay, "Decay",
        [] (float v) { return juce::String (v, 2); },
        [] (const juce::String& s) { return s.getFloatValue(); });
    tank->addFloatParam ((int) Param::SpringHfCut, "Damping",
        [] (float v) { return juce::String (v / 1000.0f, 1) + " kHz"; },
        [] (const juce::String& s) { return s.getFloatValue() * 1000.0f; });
    schematic.addElement (std::move (tank));
    schematic.addElement (std::make_unique<WireElement> (
        std::vector<Terminal>{
            schematic.getElement("Treverb")->getTerminals()[3],
            schematic.getElement("Treverb")->getTerminals()[3] + rightS +rightXS,
            V4pos[0]+  leftXL + leftXS
        },
        (int) Monitoring::TVerbSec,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Treverb")->getSignalPath(1)
    ));

    schematic.addElement (std::make_unique<WireElement>(
        std::vector<Terminal>{
        schematic.getElement("Treverb")->getTerminals()[2] +rightS ,
        schematic.getElement("Treverb")->getTerminals()[2]
        },
        (int) Monitoring::TVerbSec,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Treverb")->getSignalPath(1)
    ));

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg4",
        V4pos[0]  + bottomM,
        V4pos[0] ,
        (int) Param::Rg4,
        (int) Monitoring::Rg4,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        nullptr,
        DESCR_GRID_LEAK_RESISTOR()
    ));

    _buildCommonCathode(
        schematic,
        V4center,
        "E4", (int) Param::E4, (int) Monitoring::E4,
        "Rp4", (int) Param::Rp4, (int) Monitoring::Rp4,
        "V4", (int) Param::V4, (int) Monitoring::V4,
        "Rk4", (int) Param::Rk4, (int) Monitoring::Rk4,
        "Ck4", (int) Param::Ck4, (int) Monitoring::Ck4,
        "Rg4"
    );
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cp4",
        V4pos[1],
        V4pos[1] + rightXL,
        (int) Param::Cp4,
        (int) Monitoring::Cp4,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Rp4")->getSignalPath(),
        DESCR_COUPLING_CAPACITOR()
    ));
    schematic.addElement (std::make_unique<PotElement> (
        "Reverb",
        schematic.getElement("Cp4")->getTerminals()[1],
        schematic.getElement("Cp4")->getTerminals()[1] + bottomXL,
        schematic.getElement("Cp4")->getTerminals()[1] + bottomXL*0.5f + rightS,
        (int) Control::Reverb,
        (int) Param::RVerb,
        (int) Monitoring::RVerb_plus,
        (int) Monitoring::RVerb_minus,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Cp4")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rwet",
        schematic.getElement("Reverb")->getTerminals()[2],
        schematic.getElement("Reverb")->getTerminals()[2] + topL ,
        (int) Param::Rwet,
        (int) Monitoring::Rwet,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Reverb")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rdry",
        schematic.getElement("Cp2")->getTerminals()[1] + topL,
        schematic.getElement("Rwet")->getTerminals()[1]+ topXL,
        (int) Param::Rdry,
        (int) Monitoring::Rdry,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Cp2")->getSignalPath()
    ));

    schematic.getElement("Rdry")->addPointToTerminal(schematic.getElement("Cp2")->getTerminals()[1], 0, true);
    schematic.getElement("Rdry")->addPointToTerminal(schematic.getElement("Rwet")->getTerminals()[1], 1, false);

    //============================================================================================================
    // V5
    auto V5center = schematic.getElement("Rwet")->getTerminals()[1] +rightXL;
    auto V5pos = getTriodeTerminals(V5center);

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg5",
        V5pos[0]  + bottomM,
        V5pos[0] ,
        (int) Param::Rg5,
        (int) Monitoring::Rg5,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        nullptr,
        DESCR_GRID_LEAK_RESISTOR()
    ));
    schematic.getElement("Rg5")->addPointToTerminal(schematic.getElement("Rwet")->getTerminals()[1], 1);

    _buildCommonCathode(
        schematic,
        V5center,
        "E5", (int) Param::E5, (int) Monitoring::E5,
        "Rp5", (int) Param::Rp5, (int) Monitoring::Rp5,
        "V5", (int) Param::V5, (int) Monitoring::V5,
        "Rk5", (int) Param::Rk5, (int) Monitoring::Rk5,
        "Ck5", (int) Param::Ck5, (int) Monitoring::Ck5,
        "Rg5"
    );
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cp5",
        V5pos[1],
        V5pos[1] + rightXL,
        (int) Param::Cp5,
        (int) Monitoring::Cp5,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Rp5")->getSignalPath(),
        DESCR_COUPLING_CAPACITOR()
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rout",
        schematic.getElement("Cp5")->getTerminals()[1], 
        schematic.getElement("Cp5")->getTerminals()[1] + bottomXL, 
        (int) Param::Rout,
        (int) Monitoring::Rout,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Cp5")->getSignalPath()
    ));

    //============================================================================================================
    // Grounds
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rg1")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Volume")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ra2")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Treverb")->getTerminals()[2] + rightS));
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rg4")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck3")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk3")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Reverb")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rg5")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rout")->getTerminals()[1]) );

    // Wires
    schematic.addElement (std::make_unique<WireElement>(
        std::vector<Terminal>{
            schematic.getElement("C1")->getTerminals()[0] ,
            V1pos[1]
        },
        (int) Monitoring::R4,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Rp1")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<WireElement>(
        std::vector<Terminal>{
        schematic.getElement("Treble")->getTerminals()[2],
        schematic.getElement("Volume")->getTerminals()[0]
        },
        (int) Monitoring::RVol_plus,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Treble")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<WireElement>(
        std::vector<Terminal>{
        schematic.getElement("Rout")->getTerminals()[0] ,
        schematic.getElement("Rout")->getTerminals()[0] +rightS
        },
        (int) Monitoring::Rout,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Cp5")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<WireElement>(
        std::vector<Terminal>{
        schematic.getElement("Rg1")->getTerminals()[1] ,
        V1pos[0]
        },
        -1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Rg1")->getSignalPath()
    ));
    schematic.getElement("Volume")->addPointToTerminal(V2pos[0], 2);

    schematic.addElement (std::make_unique<JackElement>("OUTPUT", schematic.getElement("Rout")->getTerminals()[0] + rightS));
    schematic.addElement (std::make_unique<JackElement>("INPUT", schematic.getElement("Ri1")->getTerminals()[0], JUSTIFY_RIGHT));
}

