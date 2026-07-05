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


// // ===================================================================================================================
// // BASSMAN TONE STACK
// // ===================================================================================================================
// void SchematicBuilder::buildBassmanToneStack(SchematicPanel& schematic)
// {
//     float d=170.0f;
//     auto toneStackPosition = Terminal {500, 100};

//     schematic.addElement (std::make_unique<ResistorElement> (
//         "R4",
//         toneStackPosition + Terminal {0.0f, d} ,
//         toneStackPosition,
//         (int) BassmanToneStackCircuitT<float>::Param::R4
//     ));
//     schematic.addElement (std::make_unique<CapacitorElement> (
//         "C1",
//         toneStackPosition,
//         toneStackPosition+Terminal {d, 0.0f} ,
//         (int) BassmanToneStackCircuitT<float>::Param::C1
//     ));
//     schematic.addElement (std::make_unique<CapacitorElement> (
//         "C2",
//         toneStackPosition + Terminal {0.0f, d},
//         toneStackPosition + Terminal {d, d} ,
//         (int) BassmanToneStackCircuitT<float>::Param::C2
//     ));

//     schematic.addElement (std::make_unique<PotElement> (
//         "Bass",
//         schematic.getElement("C2")->getTerminals()[1],
//         schematic.getElement("C2")->getTerminals()[1] + Terminal {0.0f, d},
//         schematic.getElement("C2")->getTerminals()[1] + Terminal {0.5f*d, 0.5f*d},
//         (int) BassmanToneStackCircuitT<float>::Control::Bass,
//         (int) BassmanToneStackCircuitT<float>::Param::RBass
//     ));

//     schematic.addElement (std::make_unique<PotElement> (
//         "Treble",
//         schematic.getElement("C1")->getTerminals()[1],
//         schematic.getElement("C2")->getTerminals()[1],
//         schematic.getElement("C1")->getTerminals()[1] + Terminal {0.25f*d, .5f*d} ,
//         (int) BassmanToneStackCircuitT<float>::Control::Treble,
//         (int) BassmanToneStackCircuitT<float>::Param::RTreble
//     ));
//     schematic.addElement (std::make_unique<PotElement> (
//         "Mid",     
//         schematic.getElement("Bass")->getTerminals()[1]  + Terminal {0.0f, d},
//         schematic.getElement("Bass")->getTerminals()[1],
//         schematic.getElement("Bass")->getTerminals()[1]  + Terminal {-d*0.25f, .5f*d},
//         (int) BassmanToneStackCircuitT<float>::Control::Mid,
//         (int) BassmanToneStackCircuitT<float>::Param::RMid
//     ));
//     schematic.addElement (std::make_unique<CapacitorElement> (
//         "C3",
//         schematic.getElement("Mid")->getTerminals()[2]  + Terminal {-d*0.5f, 0.0f},
//         schematic.getElement("Mid")->getTerminals()[2]  ,
//         (int) BassmanToneStackCircuitT<float>::Param::C3
//     ));
//     // Grounds
//     schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Mid")->getTerminals()[0]) );

//     schematic.addWire (
//         schematic.getElement("C3")->getTerminals()[0] - Terminal{.25f*d, 0.0f},
//         schematic.getElement("C3")->getTerminals()[0]
//     );

//     schematic.addWire (
//         schematic.getElement("C3")->getTerminals()[0] - Terminal{.25f*d, 0.0f},
//         schematic.getElement("C2")->getTerminals()[0]
//     );
//     schematic.addWire (
//         schematic.getElement("Bass")->getTerminals()[2],
//         schematic.getElement("C2")->getTerminals()[1] + Terminal {d*0.5f, 0.0f}
//     );
//     schematic.addWire (
//         schematic.getElement("C2")->getTerminals()[1] ,
//         schematic.getElement("C2")->getTerminals()[1] + Terminal {d*0.5f, 0.0f}
//     );
//     schematic.addWire (
//         schematic.getElement("C1")->getTerminals()[0] ,
//         schematic.getElement("C1")->getTerminals()[0] + Terminal {-d*0.5f, 0.0f}
//     );
//     schematic.addWire (
//         schematic.getElement("Treble")->getTerminals()[2] ,
//         schematic.getElement("Treble")->getTerminals()[2] + Terminal {d, 0.0f}
//     );
//     schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("C1")->getTerminals()[0] + Terminal {-d*0.5f, 0.0f}));
//     schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Treble")->getTerminals()[2] + Terminal {d, 0.0f}));

// }

// // ===================================================================================================================
// // BASSMAN PREAMP
// // ===================================================================================================================
// void SchematicBuilder::buildBassmanPreampSmall(SchematicPanel& schematic)
// {
//     using Param         =  BassmanPreampCircuitT<float>::Param;
//     using Control       =  BassmanPreampCircuitT<float>::Control;
//     using Monitoring    =  BassmanPreampCircuitT<float>::Monitoring;

//     //TRIODE
//     schematic.addElement (std::make_unique<TriodeElement> (
//         "Triode", 
//         Terminal {450, 350},
//         (int) Param::Triode,
//         0, triodeChoices,
//         (int) Monitoring::Ip
//     ));
//     auto triodeTerms = schematic.getElement("Triode")->getTerminals();

//     auto toneStackPosition = triodeTerms[1] + rightXL;

//     // Grid circuit
//     schematic.addElement (std::make_unique<ResistorElement> (
//         "Rg",
//         triodeTerms[0]  + leftM,
//         triodeTerms[0] ,
//         (int) Param::Rg 
//     ));
//     schematic.addElement (std::make_unique<CapacitorElement> (
//         "Ci",
//         schematic.getElement("Rg")->getTerminals()[0] + leftL,
//         schematic.getElement("Rg")->getTerminals()[0],
//         (int) Param::Ci
//     ));
//     schematic.addElement (std::make_unique<ResistorElement> (
//         "Ri",
//         schematic.getElement("Rg")->getTerminals()[0],
//         schematic.getElement("Rg")->getTerminals()[0] + bottomL,
//         (int) Param::Ri
//     ));


//     // Cathode Circuit
//     schematic.addElement (std::make_unique<ResistorElement> (
//         "Rk",
//         triodeTerms[2]+ bottomL,
//         triodeTerms[2],
//         (int) Param::Rk
//     ));
//     schematic.addElement (std::make_unique<CapacitorElement> (
//         "Ck",
//         schematic.getElement("Rk")->getTerminals()[1] +  rightXS,
//         schematic.getElement("Rk")->getTerminals()[0] +  rightXS,
//         (int) Param::Ck
//     ));

//     // Plate Circuit
//     schematic.addElement (std::make_unique<ResistorElement> (
//         "Rp",
//         triodeTerms[1],
//         triodeTerms[1] + topL,
//         (int) Param::Rp
//     ));
//     schematic.addElement (std::make_unique<VoltageElement>(
//         "B+",
//         schematic.getElement("Rp")->getTerminals()[1], 
//         (int) Param::E
//     ));

//     // Tone Stack
//     schematic.addElement (std::make_unique<ResistorElement> (
//         "R4",
//         toneStackPosition + bottomL,
//         toneStackPosition,
//         (int) Param::R4
//     ));
//     schematic.addElement (std::make_unique<CapacitorElement> (
//         "C1",
//         toneStackPosition,
//         toneStackPosition + rightL,
//         (int) Param::C1
//     ));
//     schematic.addElement (std::make_unique<CapacitorElement> (
//         "C2",
//         toneStackPosition + bottomL,
//         toneStackPosition + bottomL + rightL,
//         (int) Param::C2
//     ));
//     schematic.addElement (std::make_unique<CapacitorElement> (
//         "C3",
//         toneStackPosition + bottomL*2.0f + rightL*0.25f,
//         toneStackPosition + bottomL*2.0f + rightL*0.75f,
//         (int) Param::C3
//     ));

//     schematic.addElement (std::make_unique<PotElement> (
//         "Treble",
//         schematic.getElement("C1")->getTerminals()[1],
//         schematic.getElement("C2")->getTerminals()[1],
//         schematic.getElement("C1")->getTerminals()[1] + rightXS + bottomL*0.5f ,
//         (int) Control::Treble,
//         (int) Param::RTreble
//     ));
//     schematic.addElement (std::make_unique<PotElement> (
//         "Bass",
//         schematic.getElement("C2")->getTerminals()[1],
//         schematic.getElement("C2")->getTerminals()[1] + bottomM,
//         schematic.getElement("C2")->getTerminals()[1] + bottomM*0.5f + rightXS,
//         (int) Control::Bass,
//         (int) Param::RBass
//     ));
//     schematic.addElement (std::make_unique<PotElement> (
//         "Mid",
//         schematic.getElement("Bass")->getTerminals()[1]  + bottomM,
//         schematic.getElement("Bass")->getTerminals()[1] ,
//         schematic.getElement("Bass")->getTerminals()[1]  + bottomM*0.5f + leftXS,
//         (int) Control::Mid,
//         (int) Param::RMid
//     ));
//     // Grounds
//     schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Mid")->getTerminals()[0]) );
//     schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ri")->getTerminals()[1]) );
//     schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk")->getTerminals()[0]) );
//     schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck")->getTerminals()[1]) );

//     // Monitors
//     schematic.addElement (std::make_unique<VoltmeterElement> (
//         "Vp", 
//         triodeTerms[1] + leftXS,
//         (int) Monitoring::Vp
//     ));
//     schematic.addElement (std::make_unique<VoltmeterElement> (
//         "Vk", 
//         triodeTerms[2] + leftXS,
//         (int) Monitoring::Vk
//     ));


//     // Wires
//     schematic.addWire (
//         schematic.getElement("Ck")->getTerminals()[0],
//         schematic.getElement("Rk")->getTerminals()[1]
//     );
//     schematic.addWire (
//         schematic.getElement("C3")->getTerminals()[0] ,
//         schematic.getElement("C3")->getTerminals()[0] + leftL*0.25f
//     );
//     schematic.addWire (
//         schematic.getElement("C3")->getTerminals()[0]+ leftL*0.25f,
//         schematic.getElement("C2")->getTerminals()[0]
//     );
//     schematic.addWire (
//         schematic.getElement("Bass")->getTerminals()[2],
//         schematic.getElement("C2")->getTerminals()[1] + rightXS
//     );
//     schematic.addWire (
//         schematic.getElement("C2")->getTerminals()[1] ,
//         schematic.getElement("C2")->getTerminals()[1] + rightXS
//     );
//     schematic.addWire (
//         schematic.getElement("Treble")->getTerminals()[2] ,
//         schematic.getElement("Treble")->getTerminals()[2] + rightL
//     );
//     schematic.addWire (
//         triodeTerms[1] ,
//         schematic.getElement("C1")->getTerminals()[0] 
//     );

//     // I/O
//     schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Treble")->getTerminals()[2] + rightL));
//     schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Ci")->getTerminals()[0]) );

// }













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

    schematic.addElement (std::make_unique<TriodeElement> (
        "V1", 
        Terminal {0, 0},
        (int) Param::V1,
        0, triodeChoices,
        (int) Monitoring::V1,
        SIGNALPATH_MODE_REVERSE_FORWARD,
        schematic.getElement("Rp1")->getSignalPath(),
        DESCR_TRIODE_COMMON_CATHODE()
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
        schematic.getElement("Rk1")->getTerminals()[1] +  rightXS,
        schematic.getElement("Rk1")->getTerminals()[0] +  rightXS,
        (int) Param::Ck1,
        (int) Monitoring::Ck1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V1")->getSignalPath(),
        DESCR_CATHODE_BYPASS_CAP()
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

    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cbright",
        schematic.getElement("Volume")->getTerminals()[0] +bottomXS,
        schematic.getElement("Volume")->getTerminals()[0] + rightS+bottomXS,
        (int) Param::Cbright,
        (int) Monitoring::Cbright
    ));

    //============================================================================================================
    // V2

    auto V2center = schematic.getElement("Volume")->getTerminals()[2]+ rightL;
    auto V2pos = getTriodeTerminals(V2center);

    schematic.addElement (std::make_unique<VoltageElement>(
        "E2",
        V2pos[1] + topL, 
        (int) Param::E2,
        (int) Monitoring::E2
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp2",
        V2pos[1],
        V2pos[1] + topL,
        (int) Param::Rp2,
        (int) Monitoring::Rp2,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("E2")->getSignalPath(),
        DESCR_PLATE_RESISTOR()
    ));
    schematic.addElement (std::make_unique<TriodeElement> (
        "V2", 
        V2center,
        (int) Param::V2,
        0, triodeChoices,
        (int) Monitoring::V2,
        SIGNALPATH_MODE_REVERSE_FORWARD,
        schematic.getElement("Rp2")->getSignalPath(),
        DESCR_TRIODE_COMMON_CATHODE()
    ));
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
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk2",
        V2pos[2]+ bottomL,
        V2pos[2],
        (int) Param::Rk2,
        (int) Monitoring::Rk2,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V2")->getSignalPath(),
        DESCR_CATHODE_RESISTOR()
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck2",
        schematic.getElement("Rk2")->getTerminals()[1] +  rightXS,
        schematic.getElement("Rk2")->getTerminals()[0] +  rightXS,
        (int) Param::Ck2,
        (int) Monitoring::Ck2,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V2")->getSignalPath(),
        DESCR_CATHODE_BYPASS_CAP()
    ));
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
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Ra2")->getSignalPath(), 
        DESCR_GRID_LEAK_RESISTOR()
    ));

    //============================================================================================================
    // V3

    auto V3center = schematic.getElement("Ra2")->getTerminals()[1] +rightL;
    auto V3pos = getTriodeTerminals(V3center);

    schematic.addElement (std::make_unique<VoltageElement>(
        "E3",
        V3pos[1] + topL, 
        (int) Param::E3,
        (int) Monitoring::E3
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp3",
        V3pos[1],
        V3pos[1] + topL,
        (int) Param::Rp3,
        (int) Monitoring::Rp3,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("E3")->getSignalPath(),
        DESCR_PLATE_RESISTOR()
    ));
    schematic.addElement (std::make_unique<TriodeElement> (
        "V3", 
        schematic.getElement("Ra2")->getTerminals()[1] +rightL,
        (int) Param::V3,
        0, triodeChoices,
        (int) Monitoring::V3,
        SIGNALPATH_MODE_REVERSE_FORWARD,
        schematic.getElement("Rp3")->getSignalPath(),
        DESCR_TRIODE_COMMON_CATHODE()
    ));

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk3",
        V3pos[2]+ bottomL,
        V3pos[2],
        (int) Param::Rk3,
        (int) Monitoring::Rk3,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V3")->getSignalPath(),
        DESCR_CATHODE_RESISTOR()
    ));
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
    schematic.addElement (std::make_unique<ResistorElement> (
        "Ra3",
        schematic.getElement("Cp3")->getTerminals()[1]+ bottomM,
        schematic.getElement("Cp3")->getTerminals()[1]+ bottomM + bottomXL,
        (int) Param::Ra3,
        (int) Monitoring::Ra3,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Cp3")->getSignalPath(),
        DESCR_GRID_LEAK_RESISTOR()
    ));


    //============================================================================================================
    // V4

    auto V4center = schematic.getElement("Ra3")->getTerminals()[0] +rightXL;
    auto V4pos = getTriodeTerminals(V4center);

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg4",
        V4pos[0]  + leftM,
        V4pos[0] ,
        (int) Param::Rg4,
        (int) Monitoring::Rg4,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Ra3")->getSignalPath(),
        DESCR_GRID_STOPPER()
    ));
    schematic.addElement (std::make_unique<VoltageElement>(
        "E4",
        V4pos[1] + topL, 
        (int) Param::E4,
        (int) Monitoring::E4
    ));

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp4",
        V4pos[1],
        V4pos[1] + topL,
        (int) Param::Rp4,
        (int) Monitoring::Rp4,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("E4")->getSignalPath(),
        DESCR_PLATE_RESISTOR()
    ));
    schematic.addElement (std::make_unique<TriodeElement> (
        "V4", 
        V4center,
        (int) Param::V4,
        0, triodeChoices,
        (int) Monitoring::V4,
        SIGNALPATH_MODE_REVERSE_FORWARD,
        schematic.getElement("Rp4")->getSignalPath(),
        DESCR_GRID_STOPPER()
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk4",
        V4pos[2]+ bottomL,
        V4pos[2],
        (int) Param::Rk4,
        (int) Monitoring::Rk4,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V4")->getSignalPath(),
        DESCR_CATHODE_RESISTOR()
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck4",
        schematic.getElement("Rk4")->getTerminals()[1] +  rightXS,
        schematic.getElement("Rk4")->getTerminals()[0] +  rightXS,
        (int) Param::Ck4,
        (int) Monitoring::Ck4,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V4")->getSignalPath(),
        DESCR_CATHODE_BYPASS_CAP()
    ));
    //============================================================================================================
    // V5

    auto V5center = V4pos[1] +rightXL;
    auto V5pos = getTriodeTerminals(V5center);

    schematic.addElement (std::make_unique<VoltageElement>(
        "E5",
        V5pos[1] + topXS, 
        (int) Param::E5,
        (int) Monitoring::E5
    ));
    schematic.addElement (std::make_unique<TriodeElement> (
        "V5", 
        V5center,
        (int) Param::V5,
        0, triodeChoices,
        (int) Monitoring::V5,
        SIGNALPATH_MODE_REVERSE_FORWARD,
        schematic.getElement("E5")->getSignalPath(),
        DESCR_CATHODE_FOLLOWER()
    ));

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk5",
        V5pos[2]+ bottomXL,
        V5pos[2],
        (int) Param::Rk5,
        (int) Monitoring::Rk5,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V5")->getSignalPath(),
        DESCR_CATHODE_RESISTOR()
    ));

    auto toneStackPosition = V5pos[2] + rightM;

    //============================================================================================================
    // Tone Stack
    schematic.addElement (std::make_unique<ResistorElement> (
        "R4",
        toneStackPosition + bottomL,
        toneStackPosition,
        (int) Param::R4,
        (int) Monitoring::R4,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V5")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C1",
        toneStackPosition,
        toneStackPosition + rightL,
        (int) Param::C1,
        (int) Monitoring::C1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V5")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C2",
        toneStackPosition + bottomL,
        toneStackPosition + bottomL + rightL,
        (int) Param::C2,
        (int) Monitoring::C2,
        SIGNALPATH_MODE_NORMAL_BACKWARD,
        schematic.getElement("R4")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C3",
        toneStackPosition + bottomL*2.0f + rightL*0.25f,
        toneStackPosition + bottomL*2.0f + rightL*0.75f,
        (int) Param::C3,
        (int) Monitoring::C3,
        SIGNALPATH_MODE_NORMAL_BACKWARD,
        schematic.getElement("R4")->getSignalPath()
    ));

    schematic.addElement (std::make_unique<PotElement> (
        "Treble",
        schematic.getElement("C1")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C1")->getTerminals()[1] + rightXS + bottomL*0.5f ,
        (int) Control::Treble,
        (int) Param::RTreble,
        (int) Monitoring::RTreble_plus,
        (int) Monitoring::RTreble_minus,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("C1")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<VarResElement> (
        "Bass",
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1] + bottomM,
        (int) Control::Bass,
        (int) Param::RBass,
        (int) Monitoring::RBass,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("C2")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<PotElement> (
        "Mid",
        schematic.getElement("Bass")->getTerminals()[1]  + bottomM,
        schematic.getElement("Bass")->getTerminals()[1] ,
        schematic.getElement("Bass")->getTerminals()[1]  + bottomM*0.5f + leftXS,
        (int) Control::Mid,
        (int) Param::RMid,
        (int) Monitoring::RMid_plus,
        (int) Monitoring::RMid_minus,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("C3")->getSignalPath()
    ));

    //============================================================================================================
    // Presence
    schematic.addElement (std::make_unique<ResistorElement> (
        "R5",
        schematic.getElement("Treble")->getTerminals()[2] + rightM + bottomS,
        schematic.getElement("Treble")->getTerminals()[2] + rightM,
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

    // Grounds
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk1")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck1")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rc1")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk2")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck2")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Volume")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rb2")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk3")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ra3")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk4")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck4")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk5")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Mid")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Presence")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Master")->getTerminals()[1]) );

    // Wires
    schematic.addWire (
        schematic.getElement("Ck1")->getTerminals()[0],
        schematic.getElement("Rk1")->getTerminals()[1]
    );
    schematic.addWire (
        schematic.getElement("Ck2")->getTerminals()[0],
        schematic.getElement("Rk2")->getTerminals()[1]
    );
    schematic.addWire (
        schematic.getElement("Rb1")->getTerminals()[0],
        schematic.getElement("Ca1")->getTerminals()[1]
    );
    schematic.addWire (
        schematic.getElement("Rb1")->getTerminals()[1],
        schematic.getElement("Ca1")->getTerminals()[0]
    );
    schematic.addWire (
        schematic.getElement("Ca1")->getTerminals()[1],
        schematic.getElement("Volume")->getTerminals()[0]
    );
    schematic.addWire (
        schematic.getElement("Cbright")->getTerminals()[1],
        schematic.getElement("Volume")->getTerminals()[2]
    );
    schematic.addWire (
        schematic.getElement("Cp3")->getTerminals()[1],
        schematic.getElement("Ra3")->getTerminals()[0]
    );
    schematic.addWire (
        schematic.getElement("Ck4")->getTerminals()[0],
        schematic.getElement("Rk4")->getTerminals()[1]
    );
    schematic.addWire (
        V3pos[0],
        schematic.getElement("Ra2")->getTerminals()[1]
    );
    schematic.addWire (
        V4pos[1],
        V5pos[0]
    );
    schematic.addWire (
        schematic.getElement("E5")->getTerminals()[0],
        V5pos[1]
    );
    schematic.addWire (
        schematic.getElement("Ck1")->getTerminals()[0],
        schematic.getElement("Rk1")->getTerminals()[1]
    );
    schematic.addWire (
        schematic.getElement("C3")->getTerminals()[0] ,
        schematic.getElement("C3")->getTerminals()[0] + leftL*0.25f
    );
    schematic.addWire (
        schematic.getElement("C3")->getTerminals()[0]+ leftL*0.25f,
        schematic.getElement("C2")->getTerminals()[0]
    );
    schematic.addWire (
        schematic.getElement("Treble")->getTerminals()[2] ,
        schematic.getElement("R5")->getTerminals()[1]
    );
    schematic.addWire (
        V5pos[2] ,
        schematic.getElement("C1")->getTerminals()[0] 
    );
    schematic.addWire (
        schematic.getElement("Rg4")->getTerminals()[0] ,
        schematic.getElement("Ra3")->getTerminals()[0] 
    );
    schematic.addWire (
        schematic.getElement("Master")->getTerminals()[2] ,
        schematic.getElement("Master")->getTerminals()[2] +rightS
    );
    schematic.addWire (
        schematic.getElement("R5")->getTerminals()[1] ,
        schematic.getElement("Master")->getTerminals()[0]
    );

    schematic.addElement (std::make_unique<JackElement>("OUTPUT",schematic.getElement("Master")->getTerminals()[2] + rightS));
    schematic.addElement (std::make_unique<JackElement>("INPUT",schematic.getElement("Rg1")->getTerminals()[0], JUSTIFY_RIGHT));
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
    auto V1center = Terminal {250, 350};
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
    schematic.addElement (std::make_unique<VoltageElement>(
        "E2",
        V2pos[1] + topL, 
        (int) Param::E2,
        (int) Monitoring::E2
    ));

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp2",
        V2pos[1],
        V2pos[1] + topL,
        (int) Param::Rp2,
        (int) Monitoring::Rp2,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("E2")->getSignalPath(),
        DESCR_PLATE_RESISTOR()
    ));

    schematic.addElement (std::make_unique<TriodeElement> (
        "V2", 
        V2center,
        (int) Param::V2,
        0, 
        triodeChoices,
        (int) Monitoring::V2,
        SIGNALPATH_MODE_REVERSE_FORWARD,
        schematic.getElement("Rp2")->getSignalPath(),
        DESCR_TRIODE_COMMON_CATHODE()
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk2",
        V2pos[2]+ bottomL,
        V2pos[2],
        (int) Param::Rk2,
        (int) Monitoring::Rk2,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V2")->getSignalPath(),
        DESCR_CATHODE_RESISTOR()
    ));

    // V3
    auto V3center = V2pos[1]+ rightL*2.0f;
    auto V3pos = getTriodeTerminals(V3center);

    schematic.addElement (std::make_unique<VoltageElement>(
        "E3",
        V3pos[1], 
        (int) Param::E3,
        (int) Monitoring::E3
    ));
    schematic.addElement (std::make_unique<TriodeElement> (
        "V3", 
        V3center,
        (int) Param::V3,
        0, 
        triodeChoices,
        (int) Monitoring::V3,
        SIGNALPATH_MODE_REVERSE_FORWARD,
        schematic.getElement("E3")->getSignalPath(),
        DESCR_CATHODE_FOLLOWER()
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk3",
        V3pos[2]+ bottomM*2.0f,
        V3pos[2],
        (int) Param::Rk3,
        (int) Monitoring::Rk3,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V3")->getSignalPath(),
        DESCR_CATHODE_RESISTOR()
    ));


    auto toneStackPosition = V3pos[2] + rightXL;

    // Tone Stack
    schematic.addElement (std::make_unique<ResistorElement> (
        "R4",
        toneStackPosition + bottomL,
        toneStackPosition,
        (int) Param::R4,
        (int) Monitoring::R4,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V3")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C1",
        toneStackPosition,
        toneStackPosition + rightL,
        (int) Param::C1,
        (int) Monitoring::C1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V3")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C2",
        toneStackPosition + bottomL,
        toneStackPosition + bottomL + rightL,
        (int) Param::C2,
        (int) Monitoring::C2,
        SIGNALPATH_MODE_NORMAL_BACKWARD,
        schematic.getElement("R4")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C3",
        toneStackPosition + bottomL*2.0f + rightL*0.25f,
        toneStackPosition + bottomL*2.0f + rightL*0.75f,
        (int) Param::C3,
        (int) Monitoring::C3,
        SIGNALPATH_MODE_NORMAL_BACKWARD,
        schematic.getElement("R4")->getSignalPath()
    ));

    schematic.addElement (std::make_unique<PotElement> (
        "Treble",
        schematic.getElement("C1")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C1")->getTerminals()[1] + rightXS + bottomL*0.5f ,
        (int) Control::Treble,
        (int) Param::RTreble,
        (int) Monitoring::RTreble_plus,
        (int) Monitoring::RTreble_minus,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("C1")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<VarResElement> (
        "Bass",
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1] + bottomM,
        (int) Control::Bass,
        (int) Param::RBass,
        (int) Monitoring::RBass,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("C2")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<PotElement> (
        "Mid",
        schematic.getElement("Bass")->getTerminals()[1] ,
        schematic.getElement("Bass")->getTerminals()[1]  + bottomM,
        schematic.getElement("Bass")->getTerminals()[1]  + bottomM*0.5f + leftXS,
        (int) Control::Mid,
        (int) Param::RMid,
        (int) Monitoring::RMid_plus,
        (int) Monitoring::RMid_minus,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("C3")->getSignalPath()
    ));

    // Wire
    schematic.addElement (std::make_unique<WireElement>(
        std::vector<Terminal>{
        schematic.getElement("C3")->getTerminals()[0] ,
        schematic.getElement("C3")->getTerminals()[0] + leftL*0.25f},
        (int) Monitoring::C3,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("R4")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<WireElement>(
        std::vector<Terminal>{
        schematic.getElement("C3")->getTerminals()[0]+ leftL*0.25f,
        schematic.getElement("C2")->getTerminals()[0]},
        (int) Monitoring::C3,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("R4")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<WireElement>(
        std::vector<Terminal>{
        schematic.getElement("Treble")->getTerminals()[2] ,
        schematic.getElement("Treble")->getTerminals()[2] + rightL},
        (int) Monitoring::Rout,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Volume")->getSignalPath()
    ));
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
    schematic.addElement (std::make_unique<WireElement>(
        std::vector<Terminal>{
        schematic.getElement("Ck1")->getTerminals()[0], 
        schematic.getElement("Rk1")->getTerminals()[1]},
        (int) Monitoring::Ck1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V1")->getSignalPath()
        )
    );

    // Grounds
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Mid")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ri1")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk1")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck1")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk2")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk3")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Volume")->getTerminals()[1]) );

    // I/O
    schematic.addElement (std::make_unique<JackElement>("OUTPUT",schematic.getElement("Treble")->getTerminals()[2] + rightL));
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
        (int) Monitoring::C3,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Rg1")->getSignalPath()
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
        0, triodeChoices,
        (int) Monitoring::V1,
        SIGNALPATH_MODE_REVERSE_FORWARD,
        schematic.getElement("Rp1")->getSignalPath(),
        schematic.getElement("Rg1")->getSignalPath(),
        DESCR_TRIODE_COMMON_CATHODE()
    ));
    schematic.getElement("V1")->addPointToTerminal(V1pos[0] +leftXS, 0);
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
        schematic.getElement("Rk1")->getTerminals()[1] +  rightXS,
        schematic.getElement("Rk1")->getTerminals()[0] +  rightXS,
        (int) Param::Ck1,
        (int) Monitoring::Ck1,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V1")->getSignalPath(),
        DESCR_CATHODE_BYPASS_CAP()
    ));

    schematic.getElement("Ck1")->addPointToTerminal(V1pos[2], 0);

    //============================================================================================================
    // Tone Stack
    auto toneStackPosition = V1pos[1] + rightXL;

    schematic.addElement (std::make_unique<ResistorElement> (
        "R4",
        toneStackPosition + bottomL,
        toneStackPosition,
        (int) Param::R4,
        (int) Monitoring::R4,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Rp1")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C1",
        toneStackPosition,
        toneStackPosition + rightL,
        (int) Param::C1,
        (int) Monitoring::C1,
        SIGNALPATH_MODE_NORMAL_BACKWARD,
        schematic.getElement("Rp1")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C2",
        toneStackPosition + bottomL,
        toneStackPosition + bottomL + rightL,
        (int) Param::C2,
        (int) Monitoring::C2,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("R4")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C3",
        toneStackPosition + bottomL*2.0f ,
        toneStackPosition + bottomL*2.0f + rightL,
        (int) Param::C3,
        (int) Monitoring::C3,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("R4")->getSignalPath()
    ));
    schematic.getElement("C3")->addPointToTerminal(schematic.getElement("C2")->getTerminals()[0], 0);

    schematic.addElement (std::make_unique<PotElement> (
        "Treble",
        schematic.getElement("C1")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C1")->getTerminals()[1] + rightXS + bottomL*0.5f ,
        (int) Control::Treble,
        (int) Param::RTreble,
        (int) Monitoring::RTreble_plus,
        (int) Monitoring::RTreble_minus,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("C1")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<VarResElement> (
        "Bass",
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1] + bottomL,
        (int) Control::Bass,
        (int) Param::RBass,
        (int) Monitoring::RBass,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("C2")->getSignalPath()
    ));
    schematic.addElement (std::make_unique<VarResElement> (
        "Mid",
        schematic.getElement("Bass")->getTerminals()[1],
        schematic.getElement("Bass")->getTerminals()[1] + bottomM,
        (int) Control::Mid,
        (int) Param::RMid,
        (int) Monitoring::RMid_minus,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("C3")->getSignalPath()
    ));


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


    schematic.addElement (std::make_unique<VoltageElement>(
        "E2",
        V2pos[1] + topL, 
        (int) Param::E2,
        (int) Monitoring::E2
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp2",
        V2pos[1],
        V2pos[1] + topL,
        (int) Param::Rp2,
        (int) Monitoring::Rp2,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("E2")->getSignalPath(),
        DESCR_PLATE_RESISTOR()
    ));
    schematic.addElement (std::make_unique<TriodeElement> (
        "V2", 
        V2center,
        (int) Param::V2,
        0, triodeChoices,
        (int) Monitoring::V2,
        SIGNALPATH_MODE_REVERSE_FORWARD,
        schematic.getElement("Rp2")->getSignalPath(),
        schematic.getElement("Volume")->getSignalPath(1),
        DESCR_TRIODE_COMMON_CATHODE()
    ));
    schematic.getElement("V2")->addPointToTerminal(schematic.getElement("Volume")->getTerminals()[2], 0);

     
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk2",
        V2pos[2]+ bottomL,
        V2pos[2],
        (int) Param::Rk2,
        (int) Monitoring::Rk2,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V2")->getSignalPath(),
        DESCR_CATHODE_RESISTOR()
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck2",
        schematic.getElement("Rk2")->getTerminals()[1] +  rightXS,
        schematic.getElement("Rk2")->getTerminals()[0] +  rightXS,
        (int) Param::Ck2,
        (int) Monitoring::Ck2,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V2")->getSignalPath(),
        DESCR_CATHODE_BYPASS_CAP()
    ));
    schematic.getElement("Ck2")->addPointToTerminal(V2pos[2], 0);

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

    schematic.addElement (std::make_unique<ResistorElement> ( // TODO rename Rg3
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
        V3pos[2]+ bottomM + rightXS,
        V3pos[2]+ rightXS,
        (int) Param::Ck3,
        (int) Monitoring::Ck3,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V3")->getSignalPath(),
        DESCR_CATHODE_BYPASS_CAP()
    ));
    schematic.getElement("Ck3")->addPointToTerminal(V3pos[2], 1);

    //============================================================================================================
    // V4
    auto V4center = V3pos[0] +rightXL*3 + rightXS;
    auto V4pos = getTriodeTerminals(V4center);

    schematic.addElement (std::make_unique<ReverbTankElement> (
        "Rev. Tank",
        V4pos[0]+  leftXL +leftXS ,
        V4pos[0] ,
        (int) Monitoring::TVerbSec,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Treverb")->getSignalPath(1)
    ));
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

    schematic.addElement (std::make_unique<VoltageElement>(
        "E4",
        V4pos[1] + topM, 
        (int) Param::E4,
        (int) Monitoring::E4
    ));

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp4",
        V4pos[1],
        V4pos[1] + topM,
        (int) Param::Rp4,
        (int) Monitoring::Rp4,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("E4")->getSignalPath(),
        DESCR_PLATE_RESISTOR()
    ));
    schematic.addElement (std::make_unique<TriodeElement> (
        "V4", 
        V4center,
        (int) Param::V4,
        0, triodeChoices,
        (int) Monitoring::V4,
        SIGNALPATH_MODE_REVERSE_FORWARD,
        schematic.getElement("Rp4")->getSignalPath(),
        DESCR_TRIODE_COMMON_CATHODE()
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
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk4",
        V4pos[2]+ bottomM,
        V4pos[2],
        (int) Param::Rk4,
        (int) Monitoring::Rk4,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V4")->getSignalPath(),
        DESCR_CATHODE_RESISTOR()
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck4",
        schematic.getElement("Rk4")->getTerminals()[1] +  rightXS,
        schematic.getElement("Rk4")->getTerminals()[0] +  rightXS,
        (int) Param::Ck4,
        (int) Monitoring::Ck4,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V4")->getSignalPath(),
        DESCR_CATHODE_BYPASS_CAP()
    ));
    schematic.getElement("Ck4")->addPointToTerminal(V4pos[2], 0);

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

    schematic.addElement (std::make_unique<VoltageElement>(
        "E5",
        V5pos[1] + topL, 
        (int) Param::E5,
        (int) Monitoring::E5
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp5",
        V5pos[1],
        V5pos[1] + topL,
        (int) Param::Rp5,
        (int) Monitoring::Rp5,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("E5")->getSignalPath(),
        DESCR_PLATE_RESISTOR()
    ));
    schematic.addElement (std::make_unique<TriodeElement> (
        "V5", 
        V5center,
        (int) Param::V5,
        0, triodeChoices,
        (int) Monitoring::V4,
        SIGNALPATH_MODE_REVERSE_FORWARD,
        schematic.getElement("Rp5")->getSignalPath(),
        DESCR_TRIODE_COMMON_CATHODE()
    ));
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


    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk5",
        V5pos[2]+ bottomL,
        V5pos[2],
        (int) Param::Rk5,
        (int) Monitoring::Rk5,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V5")->getSignalPath(),
        DESCR_CATHODE_RESISTOR()
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck5",
        schematic.getElement("Rk5")->getTerminals()[1] +  rightXS,
        schematic.getElement("Rk5")->getTerminals()[0] +  rightXS,
        (int) Param::Ck5,
        (int) Monitoring::Ck5,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("V5")->getSignalPath(),
        DESCR_CATHODE_BYPASS_CAP()
    ));
    schematic.getElement("Ck5")->addPointToTerminal(V5pos[2], 0);

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
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk1")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck1")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Mid")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Volume")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk2")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck2")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ra2")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Treverb")->getTerminals()[2] + rightS));
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rg4")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck3")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk3")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck4")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk4")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Reverb")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck5")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk5")->getTerminals()[0]) );
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
    schematic.addWire (
        schematic.getElement("Volume")->getTerminals()[0] ,
        schematic.getElement("Treble")->getTerminals()[2]
    );
    schematic.addElement (std::make_unique<WireElement>(
        std::vector<Terminal>{
        schematic.getElement("Rout")->getTerminals()[0] ,
        schematic.getElement("Rout")->getTerminals()[0] +rightS
        },
        (int) Monitoring::Rout,
        SIGNALPATH_MODE_NORMAL_FORWARD,
        schematic.getElement("Cp5")->getSignalPath()
    ));

    schematic.addElement (std::make_unique<JackElement>("OUTPUT", schematic.getElement("Rout")->getTerminals()[0] + rightS));
    schematic.addElement (std::make_unique<JackElement>("INPUT", schematic.getElement("Ri1")->getTerminals()[0], JUSTIFY_RIGHT));
}

