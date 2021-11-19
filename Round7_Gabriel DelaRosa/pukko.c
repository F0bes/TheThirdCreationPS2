/*********************************************************************
 * Pad demo app
 * Quick and dirty, little or no error checks etc..
 * Distributed as is
 *                      -pukko
 */
#include <stdio.h>
#include <tamtypes.h>
#include <kernel.h>
#include <sifcmd.h>
#include <sifrpc.h>

#include "hw.h"
#include "pad.h"
#include "loadmodule.h"


#if defined(ROM_PADMAN) && defined(NEW_PADMAN)
#error Only one of ROM_PADMAN & NEW_PADMAN should be defined!
#endif

#if !defined(ROM_PADMAN) && !defined(NEW_PADMAN)
#error ROM_PADMAN or NEW_PADMAN must be defined!
#endif

/*
 * Macros
 */

#define WAIT_PAD_READY(p, s) {while(padGetState((p),(s)) != PAD_STATE_STABLE) WaitForNextVRstart(1); }


/*
 * Global var's
 */
// pad_dma_buf is provided by the user, one buf for each pad
// contains the pad's current state
char padBuf[256] __attribute__((aligned(64)));

static char actAlign[6];
static int actuators;


/*
 * Local functions
 */

/*
 * loadModules()
 */
void
loadModules(void)
{
    int ret;

    if((ret =_lf_bind(0)) != 0)
        scr_printf("_lf_bind: %d\n", ret);

#ifdef ROM_PADMAN
    ret = _sifLoadModule("rom0:SIO2MAN", 0, NULL, 0);
#else
    ret = _sifLoadModule("rom0:XSIO2MAN", 0, NULL, 0);
#endif
    if (ret < 0) {
        scr_printf("sifLoadModule sio failed: %d\n", ret);
        k_SleepThread();
    }

#ifdef ROM_PADMAN
    ret = _sifLoadModule("rom0:PADMAN", 0, NULL, 0);
#else
    ret = _sifLoadModule("rom0:XPADMAN", 0, NULL, 0);
#endif
    if (ret < 0) {
        scr_printf("sifLoadModule pad failed: %d\n", ret);
        k_SleepThread();
    }
}

/*
 * initializePad()
 */
int
initializePad(int port, int slot)
{

    int ret;


    while((ret=padGetState(port, slot)) != PAD_STATE_STABLE) {
        if(ret==0) {
            // No pad connected!
            scr_printf("Pad(%d, %d) is disconnected\n", port, slot);
            return 0;
        }
        WaitForNextVRstart(1);
    }

    /* InfoMode does not work with rom0:padman */
#ifndef ROM_PADMAN
    scr_printf("padInfoMode: %d\n", padInfoMode(port, slot, PAD_MODECURID, 0));

    // If ExId == 0x07 => This is a dual shock controller
    if (padInfoMode(port, slot, PAD_MODECUREXID, 0) == 0) {
        scr_printf("This is NOT a dual shock controller\n");
        scr_printf("Did you forget to define RAM_PADMAN perhaps?\n");
        return 1;
    }
#endif
    scr_printf("Enabling dual shock functions\n");

    scr_printf("setMainMode dualshock (locked): %d\n",
               padSetMainMode(port, slot,
                              PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK));

    WAIT_PAD_READY(port, slot);
    scr_printf("infoPressMode: %d\n", padInfoPressMode(port, slot));

    WAIT_PAD_READY(port, slot);
    scr_printf("enterPressMode: %d\n", padEnterPressMode(port, slot));

    WAIT_PAD_READY(port, slot);
    actuators = padInfoAct(port, slot, -1, 0);
    scr_printf("# of actuators: %d\n",actuators);

    if (actuators != 0) {
        actAlign[0] = 0;   // Enable small engine
        actAlign[1] = 1;   // Enable big engine
        actAlign[2] = 0xff;
        actAlign[3] = 0xff;
        actAlign[4] = 0xff;
        actAlign[5] = 0xff;

        WAIT_PAD_READY(port, slot);
        scr_printf("padSetActAlign: %d\n",
                   padSetActAlign(port, slot, actAlign));
    }
    else {
        scr_printf("Did not find any actuators.\n");
    }

    WAIT_PAD_READY(port, slot);

    return 1;
}




/*===============================================================================
int
main()
{
    int ret;
    int port, slot;
    int i;
    struct padButtonStatus buttons;
    u32 paddata;
    u32 old_pad = 0;
    u32 new_pad;


    sif_rpc_init(0);

    init_scr();
    install_VRstart_handler();

    scr_printf("Hi!\n"
               "Very welcome to this small and stupid pad test application\n"
               "Hope you will find the source useful though =)\n"
               "Please, use & abuse the code, but I would not mind \n"
               "a small greeting when you do :)\n"
               "I myself would like to leave a special thanks to Gustavo "
               "S:\nWithout your psx2lib this would've been impossible\n\n"
               "                                                 - pukko\n\n");


    loadModules();

    padInit(0);

    port = 0;
    slot = 0;

    scr_printf("PortMax: %d\n", padGetPortMax());
    scr_printf("SlotMax: %d\n", padGetSlotMax(port));


    if((ret = padPortOpen(0, 0, padBuf)) == 0) {
        scr_printf("padOpenPort failed: %d\n", ret);
        k_SleepThread();
    }

    if(!initializePad(0, 0)) {
        scr_printf("pad initalization failed!\n");
        k_SleepThread();
    }

    for (;;) {      // We are phorever people

        WaitForNextVRstart(1);

        i=0;
        while(padGetState(port, slot) != PAD_STATE_STABLE) {
            if (i==0) {
                scr_printf("Please wait, pad state != OK\n");
                i = 1;
            }
            WaitForNextVRstart(1); // Perhaps a bit to long ;)
        }
        if(i==1) {
            scr_printf("Pad: OK!\n");
        }

        ret = padRead(port, slot, &buttons); // port, slot, buttons

        if (ret != 0) {
            paddata = 0xffff ^ ((buttons.btns[0] << 8) |
                                buttons.btns[1]);

            new_pad = paddata & ~old_pad;
            old_pad = paddata;

            // Directions
            if(new_pad & PAD_LEFT) {
                scr_printf("LEFT\n");
            }
            if(new_pad & PAD_DOWN) {
                scr_printf("DOWN\n");
            }
            if(new_pad & PAD_RIGHT) {
                scr_printf("RIGHT\n");
//                  padSetMainMode(port, slot,
//                       PAD_MMODE_DIGITAL, PAD_MMODE_LOCK));
            }
            if(new_pad & PAD_UP) {
                scr_printf("UP\n");
            }
            if(new_pad & PAD_START) {
                scr_printf("START\n");
            }
            if(new_pad & PAD_R3) {
                scr_printf("R3\n");
            }
            if(new_pad & PAD_L3) {
                scr_printf("L3\n");
            }
            if(new_pad & PAD_SELECT) {
                scr_printf("SELECT\n");
            }
            if(new_pad & PAD_SQUARE) {
                scr_printf("SQUARE\n");
            }
            if(new_pad & PAD_CROSS) {
                padEnterPressMode(port, slot);
                scr_printf("CROSS - Enter press mode\n");
            }
            if(new_pad & PAD_CIRCLE) {
                padExitPressMode(port, slot);
                scr_printf("CIRCLE - Exit press mode\n");
            }
            if(new_pad & PAD_TRIANGLE) {
                scr_printf("TRIANGLE (press hard!)\n");
            }
            if(new_pad & PAD_R1) {
                actAlign[0] = 1; // Start small engine
                padSetActDirect(port, slot, actAlign);
                scr_printf("R1 - Start little engine\n");
            }
            if(new_pad & PAD_L1) {
                actAlign[0] = 0; // Stop
                padSetActDirect(port, slot, actAlign);
                scr_printf("L1 - Stop little engine\n");
            }
            if(new_pad & PAD_R2) {
                scr_printf("R2\n");
            }
            if(new_pad & PAD_L2) {
                scr_printf("L2\n");
                //                break;
            }

            // Test the press mode
            if(buttons.triangle_p) {
                actAlign[1] = buttons.triangle_p;  // Big engine
                padSetActDirect(port, slot, actAlign);
            }
            if(buttons.rjoy_h > 0xf0)
            {
                actAlign[0] = 1;
                padSetActDirect(port, slot, actAlign);
            }
        }
    } // for

    scr_printf("Goto sleep!\n");
    k_SleepThread();

    return 0;
}


===============================================================================*/
