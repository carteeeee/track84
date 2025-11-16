#include "ui.h"
#include "data/strings.h"
#include "fonts/fonts.h"

#include "sound.h"
#include "usbaudio.h"

#include <ti/getcsc.h>
#include <graphx.h>
#include <tice.h>
#include <usbdrvce.h>

static gui_element_t gui_elements[GUI_HEIGHT][GUI_WIDTH] = {
    {
        {
            .editable = true,
            .value = 0,
            .strings = &oscilator_types,
        },
        {
            .editable = true,
            .value = 0,
            .strings = &oscilator_types,
        },
        {
            .editable = true,
            .value = 0,
            .strings = &oscilator_types,
        },
        {
            .editable = true,
            .value = 0,
            .strings = &oscilator_types,
        },
        {
            .editable = true,
            .value = 0,
            .strings = &oscilator_types,
        },
        {
            .editable = true,
            .value = 0,
            .strings = &oscilator_types,
        },
    },
    {
        {
            .editable = true,
            .value = 48,
            .strings = &note_names,
        },
        {
            .editable = true,
            .value = 48,
            .strings = &note_names,
        },
        {
            .editable = true,
            .value = 48,
            .strings = &note_names,
        },
        {
            .editable = true,
            .value = 48,
            .strings = &note_names,
        },
        {
            .editable = true,
            .value = 48,
            .strings = &note_names,
        },
        {
            .editable = true,
            .value = 48,
            .strings = &note_names,
        },
    },
};

int cursorx = 0;
int cursory = 0;
bool editing = false;

void ui_Init(void) {
    gfx_Begin();
    gfx_SetDrawBuffer();
    
    fontlib_SetFont(hw_font, 0);
    fontlib_SetTransparency(true);

    fontlib_HomeUp();
    fontlib_SetColors(DEFAULT_COLOR, BACKGROUND_COLOR);
}

static void ui_Draw(void) {
    gfx_ZeroScreen();
    
    // please kill me
    for (int y = 0; y < GUI_HEIGHT; y++) {
        for (int x = 0; x < GUI_WIDTH; x++) {
            bool selected = (cursorx == x) && (cursory == y);
            gui_element_t element = gui_elements[y][x];
            char *string = element.strings->strings[element.value]; 
            int sx = (x + 0.5) * BOX_HSPACING;
            int sy = y * BOX_VSPACING + DMARGIN;
            fontlib_SetCursorPosition(sx - fontlib_GetStringWidth(string) / 2, sy);
            fontlib_DrawString(string);
            gfx_SetColor(selected ? (editing ? EDITING_COLOR : SELECTED_COLOR) : DEFAULT_COLOR);
            gfx_Rectangle(sx - (BOX_WIDTH) / 2, sy - MARGIN, BOX_WIDTH, BOX_HEIGHT);
        }
    }
}

static void ui_DrawPlaying(void) {
    gfx_ZeroScreen();
    fontlib_SetCursorPosition(100, 100);
    fontlib_DrawString("hi the audio is playing :3");
    fontlib_SetCursorPosition(100, 108);
    fontlib_DrawString("press any key to stop");
    gfx_SwapDraw();
}

static bool ui_Navigation(void) {
    switch (os_GetCSC()) {
        case 0:
            // idk wat im cooking here
            //usb_HandleEvents();
            break;
        case sk_Right:
            if (!editing) {
                cursorx++;
                cursorx %= GUI_WIDTH;
            }
            break;
        case sk_Left:
            if (!editing) {
                cursorx += GUI_WIDTH - 1;
                cursorx %= GUI_WIDTH;
            }
            break;
        case sk_Down:
            if (editing) {
                gui_element_t *element = &gui_elements[cursory][cursorx];
                int max = element->strings->length;
                element->value = (element->value + (max - 1)) % max;
            } else {
                cursory++;
                cursory %= GUI_HEIGHT;
            }
            break;
        case sk_Up:
            if (editing) {
                gui_element_t *element = &gui_elements[cursory][cursorx];
                int max = element->strings->length;
                element->value = (element->value + 1) % max;
            } else {
                cursory += GUI_HEIGHT - 1;
                cursory %= GUI_HEIGHT;
            }
            break;
        case sk_Mode:
            if (editing) {
                editing = false;
            } else if (gui_elements[cursory][cursorx].editable) {
                editing = true;
            }
            break;
        case sk_Enter:
            // playing is blocking lol
            editing = false;
            sound_SetOscs(gui_elements[0][0].value,
                          gui_elements[0][1].value,
                          gui_elements[0][2].value,
                          gui_elements[0][3].value,
                          gui_elements[0][4].value,
                          gui_elements[0][5].value);
            sound_SetPitches(gui_elements[1][0].value,
                             gui_elements[1][1].value,
                             gui_elements[1][2].value,
                             gui_elements[1][3].value,
                             gui_elements[1][4].value,
                             gui_elements[1][5].value);
            ui_DrawPlaying();
            usbaudio_Init();
            usbaudio_Play();
            usbaudio_Cleanup();
            break;
        case sk_Clear:
            return false;
    }

    return true;
}

void ui_Loop(void) {
    do {
        ui_Draw();
        gfx_SwapDraw();
    } while (ui_Navigation());
}

__attribute__((destructor)) static void ui_Cleanup(void) {
    gfx_End();
}
