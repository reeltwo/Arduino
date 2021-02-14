#include "ReelTwoSMQ.h"    /* include first to enable SMQ support */
#include "ReelTwo.h"
#include "core/HeartBeat.h"
#include "dome/ButtonController.h"

void setup()
{
    REELTWO_READY();

    SetupEvent::ready();
}

void loop()
{
    AnimatedEvent::process();
    sendHeartBeat("DB", 1000);
}
