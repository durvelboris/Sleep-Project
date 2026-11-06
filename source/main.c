// SleepButton - outil de veille logicielle pour 2DS / 3DS (Luma3DS)
// Deux modes :
//  1. Veille reelle : demande la veille au systeme (ptm:sysm).
//  2. Ecran eteint  : coupe le retroeclairage, n'importe quel bouton rallume.

#include <3ds.h>
#include <stdio.h>
#include <string.h>

#define MENU_FIRST_ROW 8
#define MENU_ROW_STEP  3

enum { ITEM_SLEEP = 0, ITEM_SCREEN_OFF, ITEM_QUIT, ITEM_COUNT };

static const char *const labels[ITEM_COUNT] = {
    "Veille reelle (test)",
    "Ecran eteint (economie)",
    "Quitter",
};

static PrintConsole topScreen, bottomScreen;
static bool ptmReady = false;
static bool lcdReady = false;
static char status[96] = "Choisis une action.";

static void present(void)
{
    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();
}

static void drawTop(void)
{
    consoleSelect(&topScreen);
    consoleClear();
    printf("\x1b[1;1H=== Veille 2DS ===\n\n");
    printf("Haut/Bas + A, ou touche l'ecran du bas.\n\n");
    printf("Veille reelle : demande la veille au systeme.\n");
    printf("Reveil : a tester (POWER, etc.).\n\n");
    printf("Ecran eteint : coupe le retroeclairage.\n");
    printf("Reveil : n'importe quel bouton ou toucher\n");
    printf("(sauf HOME et POWER).\n\n");
    printf("START : quitter\n\n");
    printf("ptm:sysm : %s\n", ptmReady ? "OK" : "indisponible");
    printf("gsplcd   : %s\n\n", lcdReady ? "OK" : "indisponible");
    printf("%s\n", status);
}

static void drawBottom(int sel)
{
    consoleSelect(&bottomScreen);
    consoleClear();
    for (int i = 0; i < ITEM_COUNT; i++) {
        printf("\x1b[%d;3H%s %s", MENU_FIRST_ROW + i * MENU_ROW_STEP,
               i == sel ? ">" : " ", labels[i]);
    }
}

static void doSleep(void)
{
    if (!ptmReady) {
        snprintf(status, sizeof status, "ptm:sysm indisponible.");
        return;
    }
    for (int s = 3; s > 0; s--) {
        snprintf(status, sizeof status, "Veille dans %d s...", s);
        drawTop();
        present();
        svcSleepThread(1000000000LL);
    }
    Result rc = PTMSYSM_RequestSleep();
    snprintf(status, sizeof status, "RequestSleep : 0x%08lX", (unsigned long)rc);
}

static void doScreenOff(void)
{
    if (!lcdReady) {
        snprintf(status, sizeof status, "gsplcd indisponible.");
        return;
    }
    aptSetHomeAllowed(false);
    GSPLCD_PowerOffBacklight(GSPLCD_SCREEN_BOTH);

    // Attendre que tous les boutons soient relaches
    do {
        hidScanInput();
        gspWaitForVBlank();
    } while (aptMainLoop() && hidKeysHeld());

    // Attendre un appui (bouton ou toucher)
    while (aptMainLoop()) {
        hidScanInput();
        if (hidKeysDown())
            break;
        svcSleepThread(20000000LL);
    }

    GSPLCD_PowerOnBacklight(GSPLCD_SCREEN_BOTH);
    aptSetHomeAllowed(true);
    snprintf(status, sizeof status, "Ecran rallume.");
}

int main(void)
{
    gfxInitDefault();
    consoleInit(GFX_TOP, &topScreen);
    consoleInit(GFX_BOTTOM, &bottomScreen);

    ptmReady = R_SUCCEEDED(ptmSysmInit());
    lcdReady = R_SUCCEEDED(gspLcdInit());

    int sel = 0;
    bool redraw = true;

    while (aptMainLoop()) {
        hidScanInput();
        u32 down = hidKeysDown();

        if (down & KEY_START)
            break;

        int act = -1;
        if (down & KEY_UP) {
            sel = (sel + ITEM_COUNT - 1) % ITEM_COUNT;
            redraw = true;
        }
        if (down & KEY_DOWN) {
            sel = (sel + 1) % ITEM_COUNT;
            redraw = true;
        }
        if (down & KEY_A)
            act = sel;

        if (down & KEY_TOUCH) {
            touchPosition t;
            hidTouchRead(&t);
            int row = t.py / 8;
            for (int i = 0; i < ITEM_COUNT; i++) {
                int r = MENU_FIRST_ROW + i * MENU_ROW_STEP;
                if (row >= r - 1 && row <= r + 1) {
                    sel = i;
                    act = i;
                    redraw = true;
                }
            }
        }

        if (act == ITEM_QUIT)
            break;
        if (act == ITEM_SLEEP) {
            doSleep();
            redraw = true;
        } else if (act == ITEM_SCREEN_OFF) {
            doScreenOff();
            redraw = true;
        }

        if (redraw) {
            drawTop();
            drawBottom(sel);
            redraw = false;
        }
        present();
    }

    if (lcdReady) {
        GSPLCD_PowerOnBacklight(GSPLCD_SCREEN_BOTH);
        gspLcdExit();
    }
    if (ptmReady)
        ptmSysmExit();
    gfxExit();
    return 0;
}
