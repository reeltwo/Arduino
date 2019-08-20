////////////////////////////////////////////////////////////////////////////////
//
// https://www.curiousmarc.com/r2-d2/marcduino-system/marcduino-software-reference/marcduino-command-reference
//
////////////////////////////////////////////////////////////////////////////////

static void ProcessMarcMessage(const char* buffer)
{
    int arg;
    unsigned long cmd;
    if (buffer[0] != '$')
    {
        cmd = MARCMSG(buffer[0], buffer[1], buffer[2]);
        arg = atoi(&buffer[3]);
    }
    else
    {
        cmd = '$';
        arg = atoi(&buffer[1]);
    }
    switch (cmd)
    {
        ///////////////////////////////////////////////////////
        // MarcDuino Sound Commands
        ///////////////////////////////////////////////////////
        case '$':
        {
            if (arg >= 1 && arg <= 4)
            {
                /* Play next sound from bank 'arg' */
            }
            else if (arg >= 5 && arg <= 9)
            {
                /* Play first sound from bank 'arg' */
            }
            else if (arg >= 100)
            {
                /* $xyy: play sound yy from bank x */
                byte bank = arg / 100;
                byte sound = arg % 100;
            }
            else if (arg >= 10)
            {
                /* $xy: play sound yy from bank x */
                byte bank = arg / 10;
                byte sound = arg % 10;
            }
            else
            {
                switch (buffer[1])
                {
                    case 'O':
                        /* Sound off */
                        break;
                    case 's':
                        /* Stop sound */
                        break;
                    case 'R':
                        /* Random sound */
                        break;
                    case '-':
                        /* Volume down */
                        break;
                    case '+':
                        /* Volume up */
                        break;
                    case 'm':
                        /* Volume mid */
                        break;
                    case 'f':
                        /* Volume max */
                        break;
                    case 'S':
                        /* Scream (bank 6 sound 1) */
                        break;
                    case 'F':
                        /* Faint/short circuit (bank 6 sound 3) */
                        break;
                    case 'L':
                        /* Leia message (bank 7 sound 1) */
                        break;
                    case 'c':
                        /* Short Cantina music (bank 8 sound 1) */
                        break;
                    case 'W':
                        /* Star Wars (bank 8 sound 2) */
                        break;
                    case 'M':
                        /* Imperial March (bank 8 sound 3) */
                        break;
                    case 'C':
                        /* Long Cantina (bank 8 sound 5) */
                        break;
                    case 'D':
                        /* Disco Wars (bank 8 sound 6) */
                        break;
                }
            }
            break;
        }
        ///////////////////////////////////////////////////////
        // MarcDuino Sequence Commands
        ///////////////////////////////////////////////////////
        // :SExx
        case MARCMSG(':','S','E'):
        {
            switch (arg)
            {
                case 0:
                    // Close all panels (full speed), servo off - use as init only. Use CL00 for all soft close.
                    // servoDispatch.moveServosTo(ALL_DOME_PANELS_MASK, 2400);
                    break;
                case 1:
                    // Scream, with all panels open
                    // servoDispatch.moveServosTo(ALL_DOME_PANELS_MASK, 700);
                    break;
                case 2:
                    // Wave, one panel at a time
                    break;
                case 3:
                    // Fast (Smirk) back and forth wave
                    break;
                case 4:
                    // Wave 2 (open progressively all panels, then close one by one)
                    break;
                case 5:
                    // Beep Cantina (with marching ants panel action)
                    break;
                case 6:
                    // Faint/Short Circuit
                    break;
                case 7:
                    // Cantina dance (orchestral, rhythmic panel dance)
                    break;
                case 8:
                    // Leia
                    break;
                case 9:
                    // Disco
                    break;
                case 10:
                    // Quite Mode reset (panel close, stop holos, stop sounds)
                    break;
                case 11:
                    // Full Awake Mode reset (panel close, random sound, holo movement, no holo lights)
                    break;
                case 12:
                    // Top Panels to RC
                    break;
                case 13:
                    // Mid Awake Mode reset (panel close, random sound, stop holos)
                    break;
                case 14:
                    // Awake+ Mode reset ((panel close, random sound, holo movement, lights on)
                    break;
                case 15:
                    // Screams no panels
                    break;

                ///////////////////////////////////////////////////////
                // Panel Moves and light display only, no sound
                ///////////////////////////////////////////////////////
                case 51:
                    // Scream, with all panels open
                    break;
                case 52:
                    // Wave, one panel at a time
                    break;
                case 53:
                    // Fast (Smirk) back and forth wave
                    break;
                case 54:
                    // Wave 2 (open progressively all panels, then close one by one)
                    break;
                case 55:
                    // Marching ants 
                    break;
                case 56:
                    // Faint/Short Circuit
                    break;
                case 57:
                    // Rhythmic cantina panel dance
                    break;
            }
            break;
        }
        ///////////////////////////////////////////////////////
        // MarcDuino Panel Commands
        ///////////////////////////////////////////////////////
        // :OPxx
        case MARCMSG(':','O','P'):
        {
            switch (arg)
            {
                case 0:
                    // Open All
                    break;
                case 1:
                    // Open #1
                    break;
                case 2:
                    // Open #2
                    break;
                case 3:
                    // Open #3
                    break;
                case 4:
                    // Open #4
                    break;
                case 5:
                    // Open #5
                    break;
                case 6:
                    // Open #6
                DEBUG_PRINTLN("YOLO");
                    break;
                case 7:
                    // Open #7
                    break;
                case 8:
                    // Open #8
                    break;
                case 9:
                    // Open #9
                    break;
                case 10:
                    // Open #10
                    break;
                case 11:
                    // Open top panels
                    break;
                case 12:
                    // Open bottom panels
                    break;
            }
            break;
        }
        ///////////////////////////////////////////////////////
        // :CLxx
        case MARCMSG(':','C','L'):
        {
            switch (arg)
            {
                case 0:
                    // Close All
                    break;
                case 1:
                    // Close #1
                    break;
                case 2:
                    // Close #2
                    break;
                case 3:
                    // Close #3
                    break;
                case 4:
                    // Close #4
                    break;
                case 5:
                    // Close #5
                    break;
                case 6:
                    // Close #6
                    break;
                case 7:
                    // Close #7
                    break;
                case 8:
                    // Close #8
                    break;
                case 9:
                    // Close #9
                    break;
                case 10:
                    // Close #10
                    break;
                case 11:
                    // Close top panels
                    break;
                case 12:
                    // Close bottom panels
                    break;
            }
            break;
        }
        ///////////////////////////////////////////////////////
        // MarcDuino Holo Commands
        ///////////////////////////////////////////////////////
        // *RDxx
        case MARCMSG('*','R','D'):
        {
            switch (arg)
            {
                case 0:
                    // Random Holo Movement
                    break;
                case 1:
                    // Random Front Holo Movement
                    break;
                case 2:
                    // Random Rear Holo Movement
                    break;
                case 3:
                    // Random Top Holo Movement
                    break;
            }
            break;
        }

        ///////////////////////////////////////////////////////
        // MarcDuino Magic Commands
        ///////////////////////////////////////////////////////
        // *MDxx
        case MARCMSG('*','M','D'):
        {
            switch (arg)
            {
                case 0:
                    /* Magic panel off */
                    break;
                case 99:
                    /* Magic panel on */
                    break;
                default:
                    /* Magic panel on for 'arg' seconds */
                    break;
            }
            break;
        }

        ///////////////////////////////////////////////////////
        // :MFxx
        case MARCMSG('*','M','F'):
        {
            switch (arg)
            {
                case 0:
                    /* Magic panel flicker off */
                    break;
                case 99:
                    /* Magic panel flicker on */
                    break;
                default:
                    /* Magic panel flicker on for 'arg' seconds */
                    break;
            }
            break;
        }

        ///////////////////////////////////////////////////////
        // *ONxx
        case MARCMSG('*','O','N'):
        {
            switch (arg)
            {
                case 0:
                    // Turns all Holo Lights on
                    break;
                case 1:
                    // Turns front Holo Lights on
                    break;
                case 2:
                    // Turns rear Holo Lights on
                    break;
                case 3:
                    // Turns top Holo Lights on
                    break;
            }
            break;
        }

        ///////////////////////////////////////////////////////
        // *OFxx
        case MARCMSG('*','O','F'):
        {
            switch (arg)
            {
                case 0:
                    // Turns all Holo Lights off
                    break;
                case 1:
                    // Turns front Holo Lights off
                    break;
                case 2:
                    // Turns rear Holo Lights off
                    break;
                case 3:
                    // Turns top Holo Lights off
                    break;
            }
            break;
        }

        ///////////////////////////////////////////////////////
        // *ONxx
        case MARCMSG('*','H','D'):
        {
            switch (arg)
            {
                case 0:
                    // Stop all Holo movement
                    break;
                case 1:
                    // Stop front Holo movement
                    break;
                case 2:
                    // Stop rear Holo movement
                    break;
                case 3:
                    // Stop top Holo movement
                    break;
            }
            break;
        }

        ///////////////////////////////////////////////////////
        // *STxx
        case MARCMSG('*','S','T'):
        {
            switch (arg)
            {
                case 0:
                    // Reset all Holos. Stops random movement, servos off, turns lights off, and RC off
                    break;
                case 1:
                    // Reset front Holo. Stops random movement, servos off, turns lights off, and RC off
                    break;
                case 2:
                    // Reset rear Holo. Stops random movement, servos off, turns lights off, and RC off
                    break;
                case 3:
                    // Reset top Holo. Stops random movement, servos off, turns lights off, and RC off
                    break;
            }
            break;
        }

        ///////////////////////////////////////////////////////
        // *H1xx
        case MARCMSG('*','H','1'):
        {
            switch (arg)
            {
                case 0:
                    /* Turn front holo off */
                    break;
                case 99:
                    /* Turn front holo on */
                    break;
                default:
                    /* Turn front holo on for 'arg' seconds */
                    break;
            }
            break;
        }

        ///////////////////////////////////////////////////////
        // *H2xx
        case MARCMSG('*','H','2'):
        {
            switch (arg)
            {
                case 0:
                    /* Turn rear holo off */
                    break;
                case 99:
                    /* Turn rear holo on */
                    break;
                default:
                    /* Turn rear holo on for 'arg' seconds */
                    break;
            }
            break;
        }

        ///////////////////////////////////////////////////////
        // *H3xx
        case MARCMSG('*','H','3'):
        {
            switch (arg)
            {
                case 0:
                    /* Turn top holo off */
                    break;
                case 99:
                    /* Turn top holo on */
                    break;
                default:
                    /* Turn top holo on for 'arg' seconds */
                    break;
            }
            break;
        }

        ///////////////////////////////////////////////////////
        // *H0xx
        case MARCMSG('*','H','0'):
        {
            switch (arg)
            {
                case 0:
                    /* Turn all holos off */
                    break;
                case 99:
                    /* Turn all holos on */
                    break;
                default:
                    /* Turn all holos on for 'arg' seconds */
                    break;
            }
            break;
        }

        ///////////////////////////////////////////////////////
        // *F1xx
        case MARCMSG('*','F','1'):
        {
            switch (arg)
            {
                case 0:
                    /* Flicker front holo off */
                    break;
                case 99:
                    /* Flicker front holo on */
                    break;
                default:
                    /* Flicker front holo on for 'arg' seconds */
                    break;
            }
            break;
        }

        ///////////////////////////////////////////////////////
        // *F2xx
        case MARCMSG('*','F','2'):
        {
            switch (arg)
            {
                case 0:
                    /* Flicker rear holo off */
                    break;
                case 99:
                    /* Flicker rear holo on */
                    break;
                default:
                    /* Flicker rear holo on for 'arg' seconds */
                    break;
            }
            break;
        }

        ///////////////////////////////////////////////////////
        // *F3xx
        case MARCMSG('*','F','3'):
        {
            switch (arg)
            {
                case 0:
                    /* Flicker top holo off */
                    break;
                case 99:
                    /* Flicker top holo on */
                    break;
                default:
                    /* Flicker top holo on for 'arg' seconds */
                    break;
            }
            break;
        }

        ///////////////////////////////////////////////////////
        // *F0xx
        case MARCMSG('*','F','0'):
        {
            switch (arg)
            {
                case 0:
                    /* Flicker all holos off */
                    break;
                case 99:
                    /* Flicker all holos on */
                    break;
                default:
                    /* Flicker all holos on for 'arg' seconds */
                    break;
            }
            break;
        }
    }
}
