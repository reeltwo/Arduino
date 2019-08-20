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

MARCDUINO_ACTION(TFLDScrollTextLeft, @1M, ({
    FLD.selectScrollTextLeft(Marcduino::getCommand());
}))

////////////////

MARCDUINO_ACTION(BFLDScrollTextLeft, @2M, ({
    FLD.selectScrollTextLeft(Marcduino::getCommand());
}))

////////////////

MARCDUINO_ACTION(RLDScrollTextLeft, @3M, ({
    RLD.selectScrollTextLeft(Marcduino::getCommand());
}))

////////////////
