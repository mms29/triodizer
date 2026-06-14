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
// BASSMAN TONE STACK
// ===================================================================================================================
void SchematicBuilder::buildBassmanToneStack(SchematicPanel& schematic)
{
    float d=170.0f;
    auto toneStackPosition = Terminal {500, 100};

    schematic.addElement (std::make_unique<ResistorElement> (
        "R4",
        toneStackPosition + Terminal {0.0f, d} ,
        toneStackPosition,
        (int) BassmanToneStackCircuitT<float>::Param::R4
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C1",
        toneStackPosition,
        toneStackPosition+Terminal {d, 0.0f} ,
        (int) BassmanToneStackCircuitT<float>::Param::C1
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C2",
        toneStackPosition + Terminal {0.0f, d},
        toneStackPosition + Terminal {d, d} ,
        (int) BassmanToneStackCircuitT<float>::Param::C2
    ));

    schematic.addElement (std::make_unique<PotElement> (
        "Bass",
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1] + Terminal {0.0f, d},
        schematic.getElement("C2")->getTerminals()[1] + Terminal {0.5f*d, 0.5f*d},
        (int) BassmanToneStackCircuitT<float>::Control::Bass,
        (int) BassmanToneStackCircuitT<float>::Param::RBass
    ));

    schematic.addElement (std::make_unique<PotElement> (
        "Treble",
        schematic.getElement("C1")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C1")->getTerminals()[1] + Terminal {0.25f*d, .5f*d} ,
        (int) BassmanToneStackCircuitT<float>::Control::Treble,
        (int) BassmanToneStackCircuitT<float>::Param::RTreble
    ));
    schematic.addElement (std::make_unique<PotElement> (
        "Mid",     
        schematic.getElement("Bass")->getTerminals()[1]  + Terminal {0.0f, d},
        schematic.getElement("Bass")->getTerminals()[1],
        schematic.getElement("Bass")->getTerminals()[1]  + Terminal {-d*0.25f, .5f*d},
        (int) BassmanToneStackCircuitT<float>::Control::Mid,
        (int) BassmanToneStackCircuitT<float>::Param::RMid
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C3",
        schematic.getElement("Mid")->getTerminals()[2]  + Terminal {-d*0.5f, 0.0f},
        schematic.getElement("Mid")->getTerminals()[2]  ,
        (int) BassmanToneStackCircuitT<float>::Param::C3
    ));
    // Grounds
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Mid")->getTerminals()[0]) );

    schematic.addWire (
        schematic.getElement("C3")->getTerminals()[0] - Terminal{.25f*d, 0.0f},
        schematic.getElement("C3")->getTerminals()[0]
    );

    schematic.addWire (
        schematic.getElement("C3")->getTerminals()[0] - Terminal{.25f*d, 0.0f},
        schematic.getElement("C2")->getTerminals()[0]
    );
    schematic.addWire (
        schematic.getElement("Bass")->getTerminals()[2],
        schematic.getElement("C2")->getTerminals()[1] + Terminal {d*0.5f, 0.0f}
    );
    schematic.addWire (
        schematic.getElement("C2")->getTerminals()[1] ,
        schematic.getElement("C2")->getTerminals()[1] + Terminal {d*0.5f, 0.0f}
    );
    schematic.addWire (
        schematic.getElement("C1")->getTerminals()[0] ,
        schematic.getElement("C1")->getTerminals()[0] + Terminal {-d*0.5f, 0.0f}
    );
    schematic.addWire (
        schematic.getElement("Treble")->getTerminals()[2] ,
        schematic.getElement("Treble")->getTerminals()[2] + Terminal {d, 0.0f}
    );
    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("C1")->getTerminals()[0] + Terminal {-d*0.5f, 0.0f}));
    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Treble")->getTerminals()[2] + Terminal {d, 0.0f}));

}

// ===================================================================================================================
// BASSMAN PREAMP
// ===================================================================================================================
void SchematicBuilder::buildBassmanPreampSmall(SchematicPanel& schematic)
{
    using Param         =  BassmanPreampCircuitT<float>::Param;
    using Control       =  BassmanPreampCircuitT<float>::Control;
    using Monitoring    =  BassmanPreampCircuitT<float>::Monitoring;

    //TRIODE
    schematic.addElement (std::make_unique<TriodeElement> (
        "Triode", 
        Terminal {450, 350},
        (int) Param::Triode,
        0, triodeChoices,
        (int) Monitoring::Ip
    ));
    auto triodeTerms = schematic.getElement("Triode")->getTerminals();

    auto toneStackPosition = triodeTerms[1] + rightXL;

    // Grid circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg",
        triodeTerms[0]  + leftM,
        triodeTerms[0] ,
        (int) Param::Rg 
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ci",
        schematic.getElement("Rg")->getTerminals()[0] + leftL,
        schematic.getElement("Rg")->getTerminals()[0],
        (int) Param::Ci
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Ri",
        schematic.getElement("Rg")->getTerminals()[0],
        schematic.getElement("Rg")->getTerminals()[0] + bottomL,
        (int) Param::Ri
    ));


    // Cathode Circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk",
        triodeTerms[2]+ bottomL,
        triodeTerms[2],
        (int) Param::Rk
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck",
        schematic.getElement("Rk")->getTerminals()[1] +  rightXS,
        schematic.getElement("Rk")->getTerminals()[0] +  rightXS,
        (int) Param::Ck
    ));

    // Plate Circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp",
        triodeTerms[1],
        triodeTerms[1] + topL,
        (int) Param::Rp
    ));
    schematic.addElement (std::make_unique<VoltageElement>(
        "B+",
        schematic.getElement("Rp")->getTerminals()[1], 
        (int) Param::E
    ));

    // Tone Stack
    schematic.addElement (std::make_unique<ResistorElement> (
        "R4",
        toneStackPosition + bottomL,
        toneStackPosition,
        (int) Param::R4
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C1",
        toneStackPosition,
        toneStackPosition + rightL,
        (int) Param::C1
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C2",
        toneStackPosition + bottomL,
        toneStackPosition + bottomL + rightL,
        (int) Param::C2
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C3",
        toneStackPosition + bottomL*2.0f + rightL*0.25f,
        toneStackPosition + bottomL*2.0f + rightL*0.75f,
        (int) Param::C3
    ));

    schematic.addElement (std::make_unique<PotElement> (
        "Treble",
        schematic.getElement("C1")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C1")->getTerminals()[1] + rightXS + bottomL*0.5f ,
        (int) Control::Treble,
        (int) Param::RTreble
    ));
    schematic.addElement (std::make_unique<PotElement> (
        "Bass",
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1] + bottomM,
        schematic.getElement("C2")->getTerminals()[1] + bottomM*0.5f + rightXS,
        (int) Control::Bass,
        (int) Param::RBass
    ));
    schematic.addElement (std::make_unique<PotElement> (
        "Mid",
        schematic.getElement("Bass")->getTerminals()[1]  + bottomM,
        schematic.getElement("Bass")->getTerminals()[1] ,
        schematic.getElement("Bass")->getTerminals()[1]  + bottomM*0.5f + leftXS,
        (int) Control::Mid,
        (int) Param::RMid
    ));
    // Grounds
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Mid")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ri")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck")->getTerminals()[1]) );

    // Monitors
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vp", 
        triodeTerms[1] + leftXS,
        (int) Monitoring::Vp
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vk", 
        triodeTerms[2] + leftXS,
        (int) Monitoring::Vk
    ));


    // Wires
    schematic.addWire (
        schematic.getElement("Ck")->getTerminals()[0],
        schematic.getElement("Rk")->getTerminals()[1]
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
        schematic.getElement("Bass")->getTerminals()[2],
        schematic.getElement("C2")->getTerminals()[1] + rightXS
    );
    schematic.addWire (
        schematic.getElement("C2")->getTerminals()[1] ,
        schematic.getElement("C2")->getTerminals()[1] + rightXS
    );
    schematic.addWire (
        schematic.getElement("Treble")->getTerminals()[2] ,
        schematic.getElement("Treble")->getTerminals()[2] + rightL
    );
    schematic.addWire (
        triodeTerms[1] ,
        schematic.getElement("C1")->getTerminals()[0] 
    );

    // I/O
    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Treble")->getTerminals()[2] + rightL));
    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Ci")->getTerminals()[0]) );

}













// ===================================================================================================================
// DUAL RECTIFIER
// ===================================================================================================================
void SchematicBuilder::buildDualRectifierPreamp(SchematicPanel& schematic)
{
    using Param         =  DualRectifierPreampCircuit::Param;
    using Control       =  DualRectifierPreampCircuit::Control;
    using Monitoring    =  DualRectifierPreampCircuit::Monitoring;

    //V1
    schematic.addElement (std::make_unique<TriodeElement> (
        "V1", 
        Terminal {0, 0},
        (int) Param::V1,
        0, triodeChoices,
        (int) Monitoring::Ik1
    ));
    auto V1pos = schematic.getElement("V1")->getTerminals();


    // Grid circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg1",
        V1pos[0]  + leftL,
        V1pos[0] ,
        (int) Param::Rg1 
    ));

    // Cathode Circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk1",
        V1pos[2]+ bottomL,
        V1pos[2],
        (int) Param::Rk1
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck1",
        schematic.getElement("Rk1")->getTerminals()[1] +  rightXS,
        schematic.getElement("Rk1")->getTerminals()[0] +  rightXS,
        (int) Param::Ck1
    ));

    // Plate Circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp1",
        V1pos[1],
        V1pos[1] + topL,
        (int) Param::Rp1
    ));
    schematic.addElement (std::make_unique<VoltageElement>(
        "E1",
        schematic.getElement("Rp1")->getTerminals()[1], 
        (int) Param::E1
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cp1",
        V1pos[1],
        V1pos[1] + rightXL,
        (int) Param::Cp1
    ));

    // Tone stack 1
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ca1",
        schematic.getElement("Cp1")->getTerminals()[1],
        schematic.getElement("Cp1")->getTerminals()[1] + rightM,
        (int) Param::Ca1
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Ra1",
        schematic.getElement("Ca1")->getTerminals()[1] + bottomXS,
        schematic.getElement("Ca1")->getTerminals()[0] + bottomXS,
        (int) Param::Ra1
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cb1",
        schematic.getElement("Ra1")->getTerminals()[1] + bottomL,
        schematic.getElement("Ra1")->getTerminals()[0] + bottomL,
        (int) Param::Cb1
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rb1",
        schematic.getElement("Cb1")->getTerminals()[1] + bottomXS,
        schematic.getElement("Cb1")->getTerminals()[0] + bottomXS,
        (int) Param::Rb1
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rc1",
        schematic.getElement("Rb1")->getTerminals()[1] + bottomM,
        schematic.getElement("Rb1")->getTerminals()[1],
        (int) Param::Rb1
    ));

    schematic.addElement (std::make_unique<PotElement> (
        "Volume",
        schematic.getElement("Ca1")->getTerminals()[1] + rightM,
        schematic.getElement("Ca1")->getTerminals()[1] + rightM + bottomXL,
        schematic.getElement("Ca1")->getTerminals()[1] + rightM + rightS + bottomXL*0.5f ,
        (int) Control::Volume,
        (int) Param::RVol
    ));

    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cbright",
        schematic.getElement("Volume")->getTerminals()[0] +bottomXS,
        schematic.getElement("Volume")->getTerminals()[0] + rightS+bottomXS,
        (int) Param::Cbright
    ));

    // V2
    schematic.addElement (std::make_unique<TriodeElement> (
        "V2", 
        schematic.getElement("Volume")->getTerminals()[2]+ rightL,
        (int) Param::V2,
        0, triodeChoices,
        (int) Monitoring::Ik2
    ));
    auto V2pos = schematic.getElement("V2")->getTerminals();
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg2",
        schematic.getElement("Volume")->getTerminals()[2],
        V2pos[0],
        (int) Param::Rg2
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk2",
        V2pos[2]+ bottomL,
        V2pos[2],
        (int) Param::Rk2
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp2",
        V2pos[1],
        V2pos[1] + topL,
        (int) Param::Rp2
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck2",
        schematic.getElement("Rk2")->getTerminals()[1] +  rightXS,
        schematic.getElement("Rk2")->getTerminals()[0] +  rightXS,
        (int) Param::Ck2
    ));
    schematic.addElement (std::make_unique<VoltageElement>(
        "E2",
        schematic.getElement("Rp2")->getTerminals()[1], 
        (int) Param::E2
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cp2",
        V2pos[1],
        V2pos[1] + rightXL,
        (int) Param::Cp2
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Ra2",
        schematic.getElement("Cp2")->getTerminals()[1],
        schematic.getElement("Cp2")->getTerminals()[1] + bottomM,
        (int) Param::Ra2
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rb2",
        schematic.getElement("Ra2")->getTerminals()[1],
        schematic.getElement("Ra2")->getTerminals()[1] + bottomXL,
        (int) Param::Rb2
    ));

    // V3
    schematic.addElement (std::make_unique<TriodeElement> (
        "V3", 
        schematic.getElement("Ra2")->getTerminals()[1] +rightL,
        (int) Param::V3,
        0, triodeChoices,
        (int) Monitoring::Ik3
    ));
    auto V3pos = schematic.getElement("V3")->getTerminals();

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk3",
        V3pos[2]+ bottomL,
        V3pos[2],
        (int) Param::Rk3
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp3",
        V3pos[1],
        V3pos[1] + topL,
        (int) Param::Rp3
    ));
    schematic.addElement (std::make_unique<VoltageElement>(
        "E3",
        schematic.getElement("Rp3")->getTerminals()[1], 
        (int) Param::E3
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cp3",
        V3pos[1],
        V3pos[1] + rightL,
        (int) Param::Cp3
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Ra3",
        schematic.getElement("Cp3")->getTerminals()[1]+ bottomM,
        schematic.getElement("Cp3")->getTerminals()[1]+ bottomM + bottomXL,
        (int) Param::Ra3
    ));


    // V4
    schematic.addElement (std::make_unique<TriodeElement> (
        "V4", 
        schematic.getElement("Ra3")->getTerminals()[0] +rightXL,
        (int) Param::V4,
        0, triodeChoices,
        (int) Monitoring::Ik4
    ));
    auto V4pos = schematic.getElement("V4")->getTerminals();
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg4",
        V4pos[0]  + leftM,
        V4pos[0] ,
        (int) Param::Rg4
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk4",
        V4pos[2]+ bottomL,
        V4pos[2],
        (int) Param::Rk4
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp4",
        V4pos[1],
        V4pos[1] + topL,
        (int) Param::Rp4
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck4",
        schematic.getElement("Rk4")->getTerminals()[1] +  rightXS,
        schematic.getElement("Rk4")->getTerminals()[0] +  rightXS,
        (int) Param::Ck4
    ));
    schematic.addElement (std::make_unique<VoltageElement>(
        "E4",
        schematic.getElement("Rp4")->getTerminals()[1], 
        (int) Param::E4
    ));

    // V5
    schematic.addElement (std::make_unique<TriodeElement> (
        "V5", 
        V4pos[1] +rightXL,
        (int) Param::V5,
        0, triodeChoices,
        (int) Monitoring::Ik5
    ));
    auto V5pos = schematic.getElement("V5")->getTerminals();
    schematic.addElement (std::make_unique<VoltageElement>(
        "E5",
        V5pos[1] + topXS, 
        (int) Param::E5
    ));

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk5",
        V5pos[2]+ bottomXL,
        V5pos[2],
        (int) Param::Rk5
    ));

    auto toneStackPosition = V5pos[2] + rightM;

    // Tone Stack
    schematic.addElement (std::make_unique<ResistorElement> (
        "R4",
        toneStackPosition + bottomL,
        toneStackPosition,
        (int) Param::R4
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C1",
        toneStackPosition,
        toneStackPosition + rightL,
        (int) Param::C1
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C2",
        toneStackPosition + bottomL,
        toneStackPosition + bottomL + rightL,
        (int) Param::C2
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C3",
        toneStackPosition + bottomL*2.0f + rightL*0.25f,
        toneStackPosition + bottomL*2.0f + rightL*0.75f,
        (int) Param::C3
    ));

    schematic.addElement (std::make_unique<PotElement> (
        "Treble",
        schematic.getElement("C1")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C1")->getTerminals()[1] + rightXS + bottomL*0.5f ,
        (int) Control::Treble,
        (int) Param::RTreble
    ));
    schematic.addElement (std::make_unique<PotElement> (
        "Bass",
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1] + bottomM,
        schematic.getElement("C2")->getTerminals()[1] + bottomM*0.5f + rightXS,
        (int) Control::Bass,
        (int) Param::RBass
    ));
    schematic.addElement (std::make_unique<PotElement> (
        "Mid",
        schematic.getElement("Bass")->getTerminals()[1]  + bottomM,
        schematic.getElement("Bass")->getTerminals()[1] ,
        schematic.getElement("Bass")->getTerminals()[1]  + bottomM*0.5f + leftXS,
        (int) Control::Mid,
        (int) Param::RMid
    ));

    // Presence
    schematic.addElement (std::make_unique<ResistorElement> (
        "R5",
        schematic.getElement("Treble")->getTerminals()[2] + rightM + bottomS,
        schematic.getElement("Treble")->getTerminals()[2] + rightM,
        (int) Param::R5
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C5",
        schematic.getElement("R5")->getTerminals()[0] + bottomS,
        schematic.getElement("R5")->getTerminals()[0],
        (int) Param::C5
    ));

    schematic.addElement (std::make_unique<PotElement> (
        "Presence",
        schematic.getElement("C5")->getTerminals()[0],
        schematic.getElement("C5")->getTerminals()[0] + bottomM,
        schematic.getElement("C5")->getTerminals()[0] + rightXS + bottomM*0.5f ,
        (int) Control::Presence,
        (int) Param::RPres
    ));

    schematic.addElement (std::make_unique<PotElement> (
        "Master",
        schematic.getElement("R5")->getTerminals()[1] + rightM,
        schematic.getElement("R5")->getTerminals()[1] + rightM + bottomXL,
        schematic.getElement("R5")->getTerminals()[1] + rightM + bottomXL*0.5f + rightXS,
        (int) Control::Master,
        (int) Param::RMas
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

    // Monitors
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vp1", 
        V1pos[1] + leftXS,
        (int) Monitoring::VDCp1
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vk1", 
        V1pos[2] + leftXS,
        (int) Monitoring::VDCk1
    ));

    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vp2", 
        V2pos[1] + leftXS,
        (int) Monitoring::VDCp2
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vk2", 
        V2pos[2] + leftXS,
        (int) Monitoring::VDCk2
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vp3", 
        V3pos[1] + leftXS,
        (int) Monitoring::VDCp3
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vk3", 
        V3pos[2] + leftXS,
        (int) Monitoring::VDCk3
    ));

    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vp4", 
        V4pos[1] + leftXS,
        (int) Monitoring::VDCp4
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vk4", 
        V4pos[2] + leftXS,
        (int) Monitoring::VDCk4
    ));

    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vp5", 
        V5pos[1] + leftXS,
        (int) Monitoring::VDCp5
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vk5", 
        V5pos[2] + leftXS,
        (int) Monitoring::VDCk5
    ));

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
        schematic.getElement("Bass")->getTerminals()[2],
        schematic.getElement("C2")->getTerminals()[1] + rightXS
    );
    schematic.addWire (
        schematic.getElement("C2")->getTerminals()[1] ,
        schematic.getElement("C2")->getTerminals()[1] + rightXS
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

    schematic.addWire (
        schematic.getElement("Presence")->getTerminals()[2],
        schematic.getElement("C5")->getTerminals()[0] + rightXS
    );
    schematic.addWire (
        schematic.getElement("C5")->getTerminals()[0] ,
        schematic.getElement("C5")->getTerminals()[0] + rightXS
    );

    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Master")->getTerminals()[2] + rightS));
    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Rg1")->getTerminals()[0]) );
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
    schematic.addElement (std::make_unique<TriodeElement> (
        "V1", 
        Terminal {250, 350},
        (int) Param::V1,
        0, triodeChoices,
        (int) Monitoring::Ik1,
        (int) Monitoring::Vg1,
        (int) Monitoring::Vk1,
        (int) Monitoring::Vp1
    ));
    auto V1pos = schematic.getElement("V1")->getTerminals();


    // Grid circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg1",
        V1pos[0]  + leftM,
        V1pos[0] ,
        (int) Param::Rg1 ,
        (int) Monitoring::PRg1
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ci1",
        schematic.getElement("Rg1")->getTerminals()[0] + leftL,
        schematic.getElement("Rg1")->getTerminals()[0],
        (int) Param::Ci1,
        (int) Monitoring::PCi1
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Ri1",
        schematic.getElement("Rg1")->getTerminals()[0],
        schematic.getElement("Rg1")->getTerminals()[0] + bottomL,
        (int) Param::Ri1
    ));

    // Cathode Circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk1",
        V1pos[2]+ bottomL,
        V1pos[2],
        (int) Param::Rk1,
        (int) Monitoring::PCk1
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck1",
        schematic.getElement("Rk1")->getTerminals()[1] +  rightXS,
        schematic.getElement("Rk1")->getTerminals()[0] +  rightXS,
        (int) Param::Ck1,
        (int) Monitoring::PCk1
    ));

    // Plate Circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp1",
        V1pos[1],
        V1pos[1] + topL,
        (int) Param::Rp1,
        (int) Monitoring::PCp1
    ));
    schematic.addElement (std::make_unique<VoltageElement>(
        "E1",
        schematic.getElement("Rp1")->getTerminals()[1], 
        (int) Param::E1,
        (int) Monitoring::PCp1
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cp1",
        V1pos[1],
        V1pos[1] + rightXL,
        (int) Param::Cp1,
        (int) Monitoring::PCp1
    ));


    schematic.addElement (std::make_unique<PotElement> (
        "Volume",
        schematic.getElement("Cp1")->getTerminals()[1],
        schematic.getElement("Cp1")->getTerminals()[1] + bottomXL,
        schematic.getElement("Cp1")->getTerminals()[1] + rightXS + bottomXL*0.5f ,
        (int) Control::Volume,
        (int) Param::RVol,
        (int) Monitoring::PRVol
    ));

    // V2
    schematic.addElement (std::make_unique<TriodeElement> (
        "V2", 
        schematic.getElement("Volume")->getTerminals()[2]+ rightL,
        (int) Param::V2,
        0, triodeChoices,
        (int) Monitoring::Ik2,
        (int) Monitoring::Vg2,
        (int) Monitoring::Vk2,
        (int) Monitoring::Vp2
    ));
    auto V2pos = schematic.getElement("V2")->getTerminals();
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg2",
        schematic.getElement("Volume")->getTerminals()[2],
        V2pos[0],
        (int) Param::Rg2
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk2",
        V2pos[2]+ bottomL,
        V2pos[2],
        (int) Param::Rk2,
        (int) Monitoring::PRk2
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp2",
        V2pos[1],
        V2pos[1] + topL,
        (int) Param::Rp2,
        (int) Monitoring::PRp2
    ));
    schematic.addElement (std::make_unique<VoltageElement>(
        "E2",
        schematic.getElement("Rp2")->getTerminals()[1], 
        (int) Param::E2,
        (int) Monitoring::PRp2
    ));


    // V3
    schematic.addElement (std::make_unique<TriodeElement> (
        "V3", 
        V2pos[1]+ rightL*2.0f,
        (int) Param::V3,
        0, triodeChoices,
        (int) Monitoring::Ik3,
        (int) Monitoring::Vg3,
        (int) Monitoring::Vk3,
        (int) Monitoring::Vp3
    ));
    auto V3pos = schematic.getElement("V3")->getTerminals();
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk3",
        V3pos[2]+ bottomM*2.0f,
        V3pos[2],
        (int) Param::Rk3,
        (int) Monitoring::PRk3
    ));
    schematic.addElement (std::make_unique<VoltageElement>(
        "E3",
        V3pos[1], 
        (int) Param::E3,
        (int) Monitoring::PE3
    ));


    auto toneStackPosition = V3pos[2] + rightXL;

    // Tone Stack
    schematic.addElement (std::make_unique<ResistorElement> (
        "R4",
        toneStackPosition + bottomL,
        toneStackPosition,
        (int) Param::R4,
        (int) Monitoring::PR4
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C1",
        toneStackPosition,
        toneStackPosition + rightL,
        (int) Param::C1,
        (int) Monitoring::PC1
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C2",
        toneStackPosition + bottomL,
        toneStackPosition + bottomL + rightL,
        (int) Param::C2,
        (int) Monitoring::PC2
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C3",
        toneStackPosition + bottomL*2.0f + rightL*0.25f,
        toneStackPosition + bottomL*2.0f + rightL*0.75f,
        (int) Param::C3,
        (int) Monitoring::PC3
    ));

    schematic.addElement (std::make_unique<PotElement> (
        "Treble",
        schematic.getElement("C1")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C1")->getTerminals()[1] + rightXS + bottomL*0.5f ,
        (int) Control::Treble,
        (int) Param::RTreble,
        (int) Monitoring::PRTreble
    ));
    schematic.addElement (std::make_unique<PotElement> (
        "Bass",
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1] + bottomM,
        schematic.getElement("C2")->getTerminals()[1] + bottomM*0.5f + rightXS,
        (int) Control::Bass,
        (int) Param::RBass,
        (int) Monitoring::PRBass
    ));
    schematic.addElement (std::make_unique<PotElement> (
        "Mid",
        schematic.getElement("Bass")->getTerminals()[1]  + bottomM,
        schematic.getElement("Bass")->getTerminals()[1] ,
        schematic.getElement("Bass")->getTerminals()[1]  + bottomM*0.5f + leftXS,
        (int) Control::Mid,
        (int) Param::RMid,
        (int) Monitoring::PRMid
    ));
    // Grounds
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Mid")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ri1")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk1")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck1")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk2")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk3")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Volume")->getTerminals()[1]) );


    // Wires
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
        schematic.getElement("Bass")->getTerminals()[2],
        schematic.getElement("C2")->getTerminals()[1] + rightXS
    );
    schematic.addWire (
        schematic.getElement("C2")->getTerminals()[1] ,
        schematic.getElement("C2")->getTerminals()[1] + rightXS
    );
    schematic.addWire (
        schematic.getElement("Treble")->getTerminals()[2] ,
        schematic.getElement("Treble")->getTerminals()[2] + rightL
    );
    schematic.addWireElem (std::make_unique<WireElement>(
        std::vector<Terminal>{
        V3pos[2] ,
        schematic.getElement("C1")->getTerminals()[0] }, true)
    );
    schematic.addWireElem (
        std::make_unique<WireElement>(
            std::vector<Terminal>{V2pos[1], V3pos[0]},
            true
        )
    );

    // I/O
    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Treble")->getTerminals()[2] + rightL));
    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Ci1")->getTerminals()[0]) );

    schematic.getElement("V1")->createSignalPath(0);
    schematic.getElement("Ci1")->createSignalPath(0);
    schematic.getElement("Rg1")->createSignalPath(0);
    schematic.getElement("Cp1")->createSignalPath(0);
    schematic.getElement("Volume")->createSignalPath(0);
    schematic.getElement("Rg2")->createSignalPath(0);
    schematic.getElement("V2")->createSignalPath(0);
    schematic.getElement("C1")->createSignalPath(0);
    schematic.getElement("C2")->createSignalPath(0);
    schematic.getElement("C3")->createSignalPath(0);
    schematic.getElement("R4")->createSignalPath(0);
    schematic.getElement("V3")->createSignalPath(1);

}





// ===================================================================================================================
// Twin Reverb
// ===================================================================================================================
void SchematicBuilder::buildTwinReverb(SchematicPanel& schematic)
{
    using Param         =  TwinReverbCircuit::Param;
    using Control       =  TwinReverbCircuit::Control;
    using Monitoring    =  TwinReverbCircuit::Monitoring;

    //V1
    schematic.addElement (std::make_unique<TriodeElement> (
        "V1", 
        Terminal {0, 0},
        (int) Param::V1,
        0, triodeChoices,
        (int) Monitoring::Ik1
    ));
    auto V1pos = schematic.getElement("V1")->getTerminals();


    // Grid circuit

    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg1",
        V1pos[0] +bottomL +leftXS,
        V1pos[0] +leftXS,
        (int) Param::Rg1 
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Ri1",
        V1pos[0]  + leftL+leftXS,
        V1pos[0] +leftXS,
        (int) Param::Ri1 
    ));
    // Cathode Circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk1",
        V1pos[2]+ bottomL,
        V1pos[2],
        (int) Param::Rk1
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck1",
        schematic.getElement("Rk1")->getTerminals()[1] +  rightXS,
        schematic.getElement("Rk1")->getTerminals()[0] +  rightXS,
        (int) Param::Ck1
    ));

    // Plate Circuit
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp1",
        V1pos[1],
        V1pos[1] + topL,
        (int) Param::Rp1
    ));
    schematic.addElement (std::make_unique<VoltageElement>(
        "E1",
        schematic.getElement("Rp1")->getTerminals()[1], 
        (int) Param::E1
    ));

    // Tone Stack
    auto toneStackPosition = V1pos[1] + rightXL;

    schematic.addElement (std::make_unique<ResistorElement> (
        "R4",
        toneStackPosition + bottomL,
        toneStackPosition,
        (int) Param::R4
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C1",
        toneStackPosition,
        toneStackPosition + rightL,
        (int) Param::C1
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C2",
        toneStackPosition + bottomL,
        toneStackPosition + bottomL + rightL,
        (int) Param::C2
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "C3",
        toneStackPosition + bottomL*2.0f ,
        toneStackPosition + bottomL*2.0f + rightL,
        (int) Param::C3
    ));

    schematic.addElement (std::make_unique<PotElement> (
        "Treble",
        schematic.getElement("C1")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C1")->getTerminals()[1] + rightXS + bottomL*0.5f ,
        (int) Control::Treble,
        (int) Param::RTreble
    ));
    schematic.addElement (std::make_unique<PotElement> (
        "Bass",
        schematic.getElement("C2")->getTerminals()[1],
        schematic.getElement("C2")->getTerminals()[1] + bottomL,
        schematic.getElement("C2")->getTerminals()[1] + bottomL*0.5f + rightXS,
        (int) Control::Bass,
        (int) Param::RBass
    ));
    schematic.addElement (std::make_unique<PotElement> (
        "Mid",
        schematic.getElement("Bass")->getTerminals()[1],
        schematic.getElement("Bass")->getTerminals()[1] + bottomM,
        schematic.getElement("Bass")->getTerminals()[1] + bottomM*0.5f + rightXS,
        (int) Control::Mid,
        (int) Param::RMid
    ));


    schematic.addElement (std::make_unique<PotElement> (
        "Volume",
        schematic.getElement("Treble")->getTerminals()[2] + rightM,
        schematic.getElement("Treble")->getTerminals()[2] + rightM + bottomXL,
        schematic.getElement("Treble")->getTerminals()[2] + rightM + bottomXL*0.5f + rightS,
        (int) Control::Volume,
        (int) Param::RVol
    ));

    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cbright",
        schematic.getElement("Volume")->getTerminals()[0] +bottomXS,
        schematic.getElement("Volume")->getTerminals()[0] + rightS+bottomXS,
        (int) Param::Cbright
    ));

    // V2
    schematic.addElement (std::make_unique<TriodeElement> (
        "V2", 
        schematic.getElement("Volume")->getTerminals()[2]+ rightL,
        (int) Param::V2,
        0, triodeChoices,
        (int) Monitoring::Ik2
    ));
    auto V2pos = schematic.getElement("V2")->getTerminals();
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk2",
        V2pos[2]+ bottomL,
        V2pos[2],
        (int) Param::Rk2
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp2",
        V2pos[1],
        V2pos[1] + topL,
        (int) Param::Rp2
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck2",
        schematic.getElement("Rk2")->getTerminals()[1] +  rightXS,
        schematic.getElement("Rk2")->getTerminals()[0] +  rightXS,
        (int) Param::Ck2
    ));
    schematic.addElement (std::make_unique<VoltageElement>(
        "E2",
        schematic.getElement("Rp2")->getTerminals()[1], 
        (int) Param::E2
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cp2",
        V2pos[1],
        V2pos[1] + rightXL,
        (int) Param::Cp2
    ));

    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cfilt",
        schematic.getElement("Cp2")->getTerminals()[1]+ bottomXL,
        schematic.getElement("Cp2")->getTerminals()[1]+ bottomXL +rightM,
        (int) Param::Cfilt
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Ra2",
        schematic.getElement("Cfilt")->getTerminals()[1] +bottomM,
        schematic.getElement("Cfilt")->getTerminals()[1],
        (int) Param::Ra2
    ));
    // V3
    schematic.addElement (std::make_unique<TriodeElement> (
        "V3", 
        schematic.getElement("Cfilt")->getTerminals()[1]+rightM, 
        (int) Param::V3,
        0, triodeChoices,
        (int) Monitoring::Ik3
    ));
    auto V3pos = schematic.getElement("V3")->getTerminals();
    
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk3",
        V3pos[2]+ bottomM,
        V3pos[2],
        (int) Param::Rk3
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck3",
        schematic.getElement("Rk3")->getTerminals()[1] +  rightXS,
        schematic.getElement("Rk3")->getTerminals()[0] +  rightXS,
        (int) Param::Ck2
    ));
    schematic.addElement (std::make_unique<TransformerElement> (
        "Treverb",
        V3pos[1], V3pos[1]+topM, V3pos[1] + rightXS, V3pos[1]+topM + rightXS, 
        (int) Param::TR3
    ));
    schematic.addElement (std::make_unique<VoltageElement>(
        "E3",
        schematic.getElement("Treverb")->getTerminals()[1], 
        (int) Param::E3
    ));


    // V4
    schematic.addElement (std::make_unique<TriodeElement> (
        "V4", 
        V3pos[0] +rightXL*3,
        (int) Param::V4,
        0, triodeChoices,
        (int) Monitoring::Ik4
    ));
    auto V4pos = schematic.getElement("V4")->getTerminals();
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg4",
        V4pos[0]  + bottomL,
        V4pos[0] ,
        (int) Param::Rg4
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk4",
        V4pos[2]+ bottomM,
        V4pos[2],
        (int) Param::Rk4
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp4",
        V4pos[1],
        V4pos[1] + topM,
        (int) Param::Rp4
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck4",
        schematic.getElement("Rk4")->getTerminals()[1] +  rightXS,
        schematic.getElement("Rk4")->getTerminals()[0] +  rightXS,
        (int) Param::Ck4
    ));
    schematic.addElement (std::make_unique<VoltageElement>(
        "E4",
        schematic.getElement("Rp4")->getTerminals()[1], 
        (int) Param::E4
    ));

    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cp4",
        V4pos[1],
        V4pos[1] + rightXL,
        (int) Param::Cp4
    ));


    schematic.addElement (std::make_unique<PotElement> (
        "Reverb",
        schematic.getElement("Cp4")->getTerminals()[1],
        schematic.getElement("Cp4")->getTerminals()[1] + bottomXL,
        schematic.getElement("Cp4")->getTerminals()[1] + bottomXL*0.5f + rightS,
        (int) Control::Reverb,
        (int) Param::RVerb
    ));


    schematic.addElement (std::make_unique<ResistorElement> (
        "Rwet",
        schematic.getElement("Reverb")->getTerminals()[2],
        schematic.getElement("Reverb")->getTerminals()[2] + topL ,
        (int) Param::Rwet
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rdry",
        schematic.getElement("Cp2")->getTerminals()[1] + topL,
        schematic.getElement("Rwet")->getTerminals()[1]+ topXL,
        (int) Param::Rdry
    ));
    // V5
    schematic.addElement (std::make_unique<TriodeElement> (
        "V5", 
        schematic.getElement("Rwet")->getTerminals()[1] +rightXL,
        (int) Param::V5,
        0, triodeChoices,
        (int) Monitoring::Ik5
    ));
    auto V5pos = schematic.getElement("V5")->getTerminals();
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rg5",
        V5pos[0]  + bottomL,
        V5pos[0] ,
        (int) Param::Rg5
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rk5",
        V5pos[2]+ bottomL,
        V5pos[2],
        (int) Param::Rk5
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rp5",
        V5pos[1],
        V5pos[1] + topL,
        (int) Param::Rp5
    ));
    schematic.addElement (std::make_unique<CapacitorElement> (
        "Ck5",
        schematic.getElement("Rk5")->getTerminals()[1] +  rightXS,
        schematic.getElement("Rk5")->getTerminals()[0] +  rightXS,
        (int) Param::Ck5
    ));
    schematic.addElement (std::make_unique<VoltageElement>(
        "E5",
        schematic.getElement("Rp5")->getTerminals()[1], 
        (int) Param::E5
    ));

    schematic.addElement (std::make_unique<CapacitorElement> (
        "Cp5",
        V5pos[1],
        V5pos[1] + rightXL,
        (int) Param::Cp5
    ));
    schematic.addElement (std::make_unique<ResistorElement> (
        "Rout",
        schematic.getElement("Cp5")->getTerminals()[1], 
        schematic.getElement("Cp5")->getTerminals()[1] + bottomXL, 
        (int) Param::Rout
    ));

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
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck3")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk3")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck4")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk4")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Reverb")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Ck5")->getTerminals()[1]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rk5")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rg5")->getTerminals()[0]) );
    schematic.addElement (std::make_unique<GroundElement>(schematic.getElement("Rout")->getTerminals()[1]) );

    // Monitors
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vp1", 
        V1pos[1] + leftXS,
        (int) Monitoring::VACp1
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vk1", 
        V1pos[2] + leftXS,
        (int) Monitoring::VDCk1
    ));

    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vp2", 
        V2pos[1] + leftXS,
        (int) Monitoring::VACp2
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vk2", 
        V2pos[2] + leftXS,
        (int) Monitoring::VDCk2
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vp3", 
        V3pos[1] + leftXS,
        (int) Monitoring::VACp3
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vk3", 
        V3pos[2] + leftXS,
        (int) Monitoring::VDCk3
    ));

    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vp4", 
        V4pos[1] + leftXS,
        (int) Monitoring::VACp4
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vk4", 
        V4pos[2] + leftXS,
        (int) Monitoring::VDCk4
    ));

    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vp5", 
        V5pos[1] + leftXS,
        (int) Monitoring::VDCp5
    ));
    schematic.addElement (std::make_unique<VoltmeterElement> (
        "Vk5", 
        V5pos[2] + leftXS,
        (int) Monitoring::VDCk5
    ));

    // Wires
    schematic.addWire (
        schematic.getElement("Rg1")->getTerminals()[1] ,
        V1pos[0]
    );
    schematic.addWire (
        schematic.getElement("Ck1")->getTerminals()[0],
        schematic.getElement("Rk1")->getTerminals()[1]
    );
    schematic.addWire (
        schematic.getElement("Ck2")->getTerminals()[0],
        schematic.getElement("Rk2")->getTerminals()[1]
    );
    schematic.addWire (
        schematic.getElement("Ck3")->getTerminals()[0],
        schematic.getElement("Rk3")->getTerminals()[1]
    );
    schematic.addWire (
        schematic.getElement("Ck4")->getTerminals()[0],
        schematic.getElement("Rk4")->getTerminals()[1]
    );
    schematic.addWire (
        schematic.getElement("Cbright")->getTerminals()[1],
        schematic.getElement("Volume")->getTerminals()[2]
    );
    schematic.addWire (
        schematic.getElement("C3")->getTerminals()[0],
        schematic.getElement("C2")->getTerminals()[0]
    );
    schematic.addWire (
        schematic.getElement("Bass")->getTerminals()[2],
        schematic.getElement("C2")->getTerminals()[1] + rightXS
    );
    schematic.addWire (
        schematic.getElement("C2")->getTerminals()[1] ,
        schematic.getElement("C2")->getTerminals()[1] + rightXS
    );
    schematic.addWire (
        schematic.getElement("Mid")->getTerminals()[2],
        schematic.getElement("C3")->getTerminals()[1] + rightXS
    );
    schematic.addWire (
        schematic.getElement("C3")->getTerminals()[1] ,
        schematic.getElement("C3")->getTerminals()[1] + rightXS
    );
    schematic.addWire (
        schematic.getElement("C1")->getTerminals()[0] ,
        V1pos[1]
    );
    schematic.addWire (
        schematic.getElement("Volume")->getTerminals()[0] ,
        schematic.getElement("Treble")->getTerminals()[2]
    );
    schematic.addWire (
        schematic.getElement("Volume")->getTerminals()[2] ,
        V2pos[0]
    );

    schematic.addWire (
        schematic.getElement("Cfilt")->getTerminals()[0] ,
        schematic.getElement("Cp2")->getTerminals()[1] 
    );
    schematic.addWire (
        schematic.getElement("Treverb")->getTerminals()[2] ,
        schematic.getElement("Treverb")->getTerminals()[2] +rightS
    );
    schematic.addWire (
        schematic.getElement("Rdry")->getTerminals()[0] ,
        schematic.getElement("Cp2")->getTerminals()[1]
    );
    schematic.addWire (
        schematic.getElement("Rdry")->getTerminals()[1] ,
        schematic.getElement("Rwet")->getTerminals()[1]
    );

    schematic.addWire (
        schematic.getElement("Rg5")->getTerminals()[1] ,
        schematic.getElement("Rwet")->getTerminals()[1]
    );

    schematic.addWire (
        schematic.getElement("Cfilt")->getTerminals()[1] ,
        V3pos[0]
    );
    schematic.addWire (
        schematic.getElement("Ck5")->getTerminals()[0],
        schematic.getElement("Rk5")->getTerminals()[1]
    );
    schematic.addWire (
        schematic.getElement("Rout")->getTerminals()[0] ,
        schematic.getElement("Rout")->getTerminals()[0] +rightS
    );

    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Rout")->getTerminals()[0] + rightS));
    schematic.addElement (std::make_unique<JunctionElement>(schematic.getElement("Ri1")->getTerminals()[0]) );
}

