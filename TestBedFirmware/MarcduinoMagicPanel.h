////////////////

MARCDUINO_ACTION(MagicOff, *MF00, ({
    // Magic Panel Off
    CommandEvent::process("MP00000");
}))

////////////////

MARCDUINO_ACTION(MagicSolid, *MF01, ({
    // Magic Panel Solid (5 seconds)
    CommandEvent::process("MP10005");
}))

////////////////

MARCDUINO_ACTION(MagicToggle, *MF02, ({
    // Magic Panel Toggle (5 seconds)
    CommandEvent::process("MP20005");
}))

////////////////

MARCDUINO_ACTION(MagicFlash, *MF03, ({
    // Magic Panel Flash (5 seconds)
    CommandEvent::process("MP30005");
}))

////////////////

MARCDUINO_ACTION(MagicAlert, *MF04, ({
    // Magic Panel Alert (5 seconds)
    CommandEvent::process("MP40005");
}))

////////////////

MARCDUINO_ACTION(MagicHScan, *MF05, ({
    // Magic Panel Horizontal Scan (5 seconds)
    CommandEvent::process("MP50005");
}))

////////////////

MARCDUINO_ACTION(MagicVScan, *MF06, ({
    // Magic Panel Vertical Scan (5 seconds)
    CommandEvent::process("MP60005");
}))

////////////////

MARCDUINO_ACTION(MagicGameOfLife, *MF07, ({
    // Magic Panel Game of Life
    CommandEvent::process("MP70000");
}))

////////////////

MARCDUINO_ACTION(MagicExpandSolid, *MF08, ({
    // Magic Panel Expand Solid (5 seconds)
    CommandEvent::process("MP80005");
}))

////////////////

MARCDUINO_ACTION(MagicCollapseSolid, *MF09, ({
    // Magic Panel Collapse Solid (5 seconds)
    CommandEvent::process("MP90005");
}))

////////////////

MARCDUINO_ACTION(MagicExpandHollow, *MF10, ({
    // Magic Panel Expand Hollow (5 seconds)
    CommandEvent::process("MP100005");
}))

////////////////

MARCDUINO_ACTION(MagicCollapseHollow, *MF11, ({
    // Magic Panel Collapse Hollow (5 seconds)
    CommandEvent::process("MP110005");
}))

////////////////

MARCDUINO_ACTION(MagicForwardQ, *MF12, ({
    // Magic Panel Forward Q (5 seconds)
    CommandEvent::process("MP120005");
}))

////////////////

MARCDUINO_ACTION(MagicReverseQ, *MF13, ({
    // Magic Panel Reverse Q (5 seconds)
    CommandEvent::process("MP130005");
}))

////////////////
