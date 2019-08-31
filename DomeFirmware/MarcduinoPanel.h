////////////////

MARCDUINO_ACTION(CloseAllPanels, :CL00, ({
    Marcduino::processCommand(player, "@4S3");
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllClose, ALL_DOME_PANELS_MASK);
}))

////////////////

MARCDUINO_ACTION(OpenAllPanels, :OP00, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllOpen, ALL_DOME_PANELS_MASK);
}))

////////////////

MARCDUINO_ACTION(FlutterAllPanels, :OF00, ({
    SEQUENCE_PLAY_ONCE_VARSPEED(servoSequencer, SeqPanelAllFlutter, ALL_DOME_PANELS_MASK, 10, 50);
}))

////////////////

MARCDUINO_ACTION(OpenPanelGroup1, :OP01, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllOpen, PANEL_GROUP_1);
}))

////////////////

MARCDUINO_ACTION(OpenPanelGroup2, :OP02, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllOpen, PANEL_GROUP_2);
}))

////////////////

MARCDUINO_ACTION(OpenPanelGroup3, :OP03, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllOpen, PANEL_GROUP_3);
}))

////////////////

MARCDUINO_ACTION(OpenPanelGroup4, :OP04, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllOpen, PANEL_GROUP_4);
}))

////////////////

MARCDUINO_ACTION(OpenPanelGroup5, :OP05, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllOpen, PANEL_GROUP_5);
}))

////////////////

MARCDUINO_ACTION(OpenPanelGroup6, :OP06, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllOpen, PANEL_GROUP_6);
}))

////////////////

MARCDUINO_ACTION(OpenPanelGroup7, :OP07, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllOpen, PANEL_GROUP_7);
}))

////////////////

MARCDUINO_ACTION(OpenPanelGroup8, :OP08, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllOpen, PANEL_GROUP_8);
}))

////////////////

MARCDUINO_ACTION(OpenPanelGroup9, :OP09, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllOpen, PANEL_GROUP_9);
}))

////////////////

MARCDUINO_ACTION(OpenPanelGroup10, :OP10, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllOpen, PANEL_GROUP_10);
}))

////////////////

MARCDUINO_ACTION(OpenTopPanels, :OP11, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllOpen, PIE_PANEL);
}))

////////////////

MARCDUINO_ACTION(OpenBottomPanels, :OP12, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllOpen, DOME_PANELS_MASK);
}))


MARCDUINO_ACTION(ClosePanelGroup1, :CLP01, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllClose, PANEL_GROUP_1);
}))

////////////////

MARCDUINO_ACTION(ClosePanelGroup2, :CL02, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllClose, PANEL_GROUP_2);
}))

////////////////

MARCDUINO_ACTION(ClosePanelGroup3, :CL03, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllClose, PANEL_GROUP_3);
}))

////////////////

MARCDUINO_ACTION(ClosePanelGroup4, :CL04, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllClose, PANEL_GROUP_4);
}))

////////////////

MARCDUINO_ACTION(ClosePanelGroup5, :CL05, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllClose, PANEL_GROUP_5);
}))

////////////////

MARCDUINO_ACTION(ClosePanelGroup6, :CL06, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllClose, PANEL_GROUP_6);
}))

////////////////

MARCDUINO_ACTION(ClosePanelGroup7, :CL07, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllClose, PANEL_GROUP_7);
}))

////////////////

MARCDUINO_ACTION(ClosePanelGroup8, :CL08, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllClose, PANEL_GROUP_8);
}))

////////////////

MARCDUINO_ACTION(ClosePanelGroup9, :CL09, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllClose, PANEL_GROUP_9);
}))

////////////////

MARCDUINO_ACTION(ClosePanelGroup10, :CL10, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllClose, PANEL_GROUP_10);
}))

////////////////

MARCDUINO_ACTION(CloseTopPanels, :CL11, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllClose, PIE_PANEL);
}))

////////////////

MARCDUINO_ACTION(CloseBottomPanels, :CL12, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllClose, DOME_PANELS_MASK);
}))

////////////////

MARCDUINO_ACTION(FlutterPanelGroup1, :OF01, ({
    SEQUENCE_PLAY_ONCE_VARSPEED(servoSequencer, SeqPanelAllFlutter, PANEL_GROUP_1, 10, 50);
}))

////////////////

MARCDUINO_ACTION(FlutterPanelGroup2, :OF02, ({
    SEQUENCE_PLAY_ONCE_VARSPEED(servoSequencer, SeqPanelAllFlutter, PANEL_GROUP_2, 10, 50);
}))

////////////////

MARCDUINO_ACTION(FlutterPanelGroup3, :OF03, ({
    SEQUENCE_PLAY_ONCE_VARSPEED(servoSequencer, SeqPanelAllFlutter, PANEL_GROUP_3, 10, 50);
}))

////////////////

MARCDUINO_ACTION(FlutterPanelGroup4, :OF04, ({
    SEQUENCE_PLAY_ONCE_VARSPEED(servoSequencer, SeqPanelAllFlutter, PANEL_GROUP_4, 10, 50);
}))

////////////////

MARCDUINO_ACTION(FlutterPanelGroup5, :OF05, ({
    SEQUENCE_PLAY_ONCE_VARSPEED(servoSequencer, SeqPanelAllFlutter, PANEL_GROUP_5, 10, 50);
}))

////////////////

MARCDUINO_ACTION(FlutterPanelGroup6, :OF06, ({
    SEQUENCE_PLAY_ONCE_VARSPEED(servoSequencer, SeqPanelAllFlutter, PANEL_GROUP_6, 10, 50);
}))

////////////////

MARCDUINO_ACTION(FlutterPanelGroup7, :OF07, ({
    SEQUENCE_PLAY_ONCE_VARSPEED(servoSequencer, SeqPanelAllFlutter, PANEL_GROUP_7, 10, 50);
}))

////////////////

MARCDUINO_ACTION(FlutterPanelGroup8, :OF08, ({
    SEQUENCE_PLAY_ONCE_VARSPEED(servoSequencer, SeqPanelAllFlutter, PANEL_GROUP_8, 10, 50);
}))

////////////////

MARCDUINO_ACTION(FlutterPanelGroup9, :OF09, ({
    SEQUENCE_PLAY_ONCE_VARSPEED(servoSequencer, SeqPanelAllFlutter, PANEL_GROUP_9, 10, 50);
}))

////////////////

MARCDUINO_ACTION(FlutterPanelGroup10, :OF10, ({
    SEQUENCE_PLAY_ONCE_VARSPEED(servoSequencer, SeqPanelAllFlutter, PANEL_GROUP_10, 10, 50);
}))
