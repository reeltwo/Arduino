////////////////

MARCDUINO_ACTION(NormalSequence, @0T1, ({
    FLD.selectSequence(LogicEngineRenderer::NORMAL);
    RLD.selectSequence(LogicEngineRenderer::NORMAL);
}))

////////////////

MARCDUINO_ACTION(FlashSequence, @0T2, ({
    FLD.selectSequence(LogicEngineRenderer::FLASHCOLOR);
    RLD.selectSequence(LogicEngineRenderer::FLASHCOLOR);
}))

////////////////

MARCDUINO_ACTION(AlarmSequence, @0T3, ({
    FLD.selectSequence(LogicEngineRenderer::ALARM);
    RLD.selectSequence(LogicEngineRenderer::ALARM);
}))

////////////////

MARCDUINO_ACTION(FailureSequence, @0T4, ({
    FLD.selectSequence(LogicEngineRenderer::FAILURE);
    RLD.selectSequence(LogicEngineRenderer::FAILURE);
}))

////////////////

MARCDUINO_ACTION(MarchSequence, @0T11, ({
    FLD.selectSequence(LogicEngineRenderer::MARCH);
    RLD.selectSequence(LogicEngineRenderer::MARCH);
}))

////////////////

MARCDUINO_ACTION(DiscoLogicsSequence, @0T92, ({
    FLD.selectSequence(LogicEngineRenderer::RAINBOW);
    RLD.selectSequence(LogicEngineRenderer::RAINBOW);
}))

////////////////

static char sTFLDText[32];
static char sBFLDText[32];
static char sFLDText[65];
static char sRLDText[64];

MARCDUINO_ACTION(TFLDScrollTextLeft, @1M, ({
    strcpy(sTFLDText, Marcduino::getCommand());
    strcpy(sFLDText, sTFLDText);
    strcat(sFLDText, "\n");
    strcat(sFLDText, sBFLDText);
    FLD.selectScrollTextLeft(sFLDText, FLD.randomColor());
}))

////////////////

MARCDUINO_ACTION(BFLDScrollTextLeft, @2M, ({
    strcpy(sBFLDText, Marcduino::getCommand());
    strcpy(sFLDText, sTFLDText);
    strcat(sFLDText, "\n");
    strcat(sFLDText, sBFLDText);
    FLD.selectScrollTextLeft(sFLDText, FLD.randomColor());
}))

////////////////

MARCDUINO_ACTION(RLDScrollTextLeft, @3M, ({
    strcpy(sRLDText, Marcduino::getCommand());
    RLD.selectScrollTextLeft(sRLDText, RLD.randomColor());
}))

////////////////

MARCDUINO_ACTION(TFLDTextLatin, @1P60, ({
    FLD.setEffectFontNum(0);
}))

////////////////

MARCDUINO_ACTION(BFLDTextLatin, @2P60, ({
    FLD.setEffectFontNum(0);
}))

////////////////

MARCDUINO_ACTION(RLDTextLatin, @3P60, ({
    RLD.setEffectFontNum(0);
}))

////////////////

MARCDUINO_ACTION(TFLDTextAurabesh, @1P61, ({
    FLD.setEffectFontNum(1);
}))

////////////////

MARCDUINO_ACTION(BFLDTextAurabesh, @2P61, ({
    FLD.setEffectFontNum(1);
}))

////////////////

MARCDUINO_ACTION(RLDTextAurabesh, @3P61, ({
    RLD.setEffectFontNum(1);
}))

////////////////
