////////////////

MARCDUINO_ACTION(PeriscopeDown, *PS01, ({
    // Periscope Down
    CommandEvent::process("PSDOWN");
}))

////////////////

MARCDUINO_ACTION(PeriscopeUp, *PS02, ({
    // Periscope Up
    // play whist- sounds
    CommandEvent::process("PSUP");
}))

////////////////

MARCDUINO_ACTION(PeriscopeSearchCCW, *PS03, ({
    // Periscope Search Light CCW
    CommandEvent::process("PSSCCW");
}))

////////////////

MARCDUINO_ACTION(PeriscopeRandomFast, *PS04, ({
    // Periscope Random Fast
    CommandEvent::process("PSFAST");
}))

////////////////

MARCDUINO_ACTION(PeriscopeRandomSlow, *PS05, ({
    // Periscope Random Slow
    CommandEvent::process("PSSLOW");
}))

////////////////

MARCDUINO_ACTION(PeriscopeForwards, *PS06, ({
    // Periscope Face Forward
    CommandEvent::process("PSFWD");
}))

////////////////

MARCDUINO_ACTION(PeriscopeSearchCW, *PS07, ({
    // Periscope Search Light CW
    CommandEvent::process("PSCW");
}))

////////////////
