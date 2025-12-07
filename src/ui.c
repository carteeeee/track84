#include "ui.h"
#include "data/strings.h"
#include "fonts/fonts.h"

#include "soundasm.h"
#include "usbaudio.h"

#include <ti/getcsc.h>
#include <ti/getkey.h>
#include <graphx.h>
#include <tice.h>
#include <usbdrvce.h>
#include <fileioc.h>
#include <string.h>

#define PATTERN_STEP {{.editable=true,.value=0,.strings=&note_names,},}
#define SONG_OSC     {.editable=true,.value=0,.strings=&oscillator_types,}
#define SONG_STEP    {.editable=true,.value=0,.strings=&pattern_numbers,}
#define SONG_ROW     {SONG_STEP, SONG_STEP, SONG_STEP, SONG_STEP, SONG_STEP, SONG_STEP}

const static gui_element_t menu_gui[MENU_HEIGHT][MENU_WIDTH] = {
    {
        {
            .editable = false,
            .value = 0,
            .strings = &menu_buttons,
        },
    },
    {
        {
            .editable = false,
            .value = 1,
            .strings = &menu_buttons,
        },
    },
    {
        {
            .editable = false,
            .value = 2,
            .strings = &menu_buttons,
        },
    },
    {
        {
            .editable = false,
            .value = 3,
            .strings = &menu_buttons,
        },
    },
};

static gui_element_t pattern_gui[PATTERN_HEIGHT][PATTERN_WIDTH] = {
    {
        {
            .editable = true,
            .value = 0,
            .strings = &pattern_numbers,
        },
    },
    PATTERN_STEP, PATTERN_STEP, PATTERN_STEP, PATTERN_STEP,
    PATTERN_STEP, PATTERN_STEP, PATTERN_STEP, PATTERN_STEP,
    PATTERN_STEP, PATTERN_STEP, PATTERN_STEP, PATTERN_STEP,
    PATTERN_STEP, PATTERN_STEP, PATTERN_STEP, PATTERN_STEP,

    PATTERN_STEP, PATTERN_STEP, PATTERN_STEP, PATTERN_STEP,
    PATTERN_STEP, PATTERN_STEP, PATTERN_STEP, PATTERN_STEP,
    PATTERN_STEP, PATTERN_STEP, PATTERN_STEP, PATTERN_STEP,
    PATTERN_STEP, PATTERN_STEP, PATTERN_STEP, PATTERN_STEP,
    
    PATTERN_STEP, PATTERN_STEP, PATTERN_STEP, PATTERN_STEP,
    PATTERN_STEP, PATTERN_STEP, PATTERN_STEP, PATTERN_STEP,
    PATTERN_STEP, PATTERN_STEP, PATTERN_STEP, PATTERN_STEP,
    PATTERN_STEP, PATTERN_STEP, PATTERN_STEP, PATTERN_STEP,

    PATTERN_STEP, PATTERN_STEP, PATTERN_STEP, PATTERN_STEP,
    PATTERN_STEP, PATTERN_STEP, PATTERN_STEP, PATTERN_STEP,
    PATTERN_STEP, PATTERN_STEP, PATTERN_STEP, PATTERN_STEP,
    PATTERN_STEP, PATTERN_STEP, PATTERN_STEP, PATTERN_STEP,
};

static gui_element_t song_gui[SONG_HEIGHT][SONG_WIDTH] = {
    {
        {
            .editable = true,
            .value = 0,
            .strings = &tempo_numbers,
        },
        {
            .editable = true,
            .value = 0,
            .strings = &pattern_length_numbers,
        },
    },
    {SONG_OSC, SONG_OSC, SONG_OSC, SONG_OSC, SONG_OSC, SONG_OSC},

    SONG_ROW, SONG_ROW, SONG_ROW, SONG_ROW,
    SONG_ROW, SONG_ROW, SONG_ROW, SONG_ROW,
    SONG_ROW, SONG_ROW, SONG_ROW, SONG_ROW,
    SONG_ROW, SONG_ROW, SONG_ROW, SONG_ROW,

    SONG_ROW, SONG_ROW, SONG_ROW, SONG_ROW,
    SONG_ROW, SONG_ROW, SONG_ROW, SONG_ROW,
    SONG_ROW, SONG_ROW, SONG_ROW, SONG_ROW,
    SONG_ROW, SONG_ROW, SONG_ROW, SONG_ROW,

    SONG_ROW, SONG_ROW, SONG_ROW, SONG_ROW,
    SONG_ROW, SONG_ROW, SONG_ROW, SONG_ROW,
    SONG_ROW, SONG_ROW, SONG_ROW, SONG_ROW,
    SONG_ROW, SONG_ROW, SONG_ROW, SONG_ROW,

    SONG_ROW, SONG_ROW, SONG_ROW, SONG_ROW,
    SONG_ROW, SONG_ROW, SONG_ROW, SONG_ROW,
    SONG_ROW, SONG_ROW, SONG_ROW, SONG_ROW,
    SONG_ROW, SONG_ROW, SONG_ROW, SONG_ROW,
};

static gui_element_t *gui_elements = &menu_gui;

int width = MENU_WIDTH;
int height = MENU_HEIGHT;
int cursorx = 0;
int cursory = 0;
int scrolly = 0;
bool editing = false;

ui_menu_t menu = MAIN_MENU;

static gui_element_t *ui_GetElement(int x, int y) {
    return &gui_elements[y * width + x]; // trust :pray:
}

#define ui_GetCursor() ui_GetElement(cursorx, cursory)

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
    int realHeight = MIN(height - scrolly, MAX_HEIGHT);
    for (int y = scrolly; y < realHeight + scrolly; y++) {
        for (int x = 0; x < width; x++) {
            gui_element_t *element = ui_GetElement(x, y);
            bool selected = (cursorx == x) && (cursory == y);
            char *string = element->strings->strings[element->value];

            int sx = (x + 0.5) * BOX_HSPACING;
            int sy = (y - scrolly) * BOX_VSPACING + DMARGIN;

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

static void ui_LoadProject(void) {
    static const uint8_t numtable[] = {
        [sk_7] = 7, [sk_8] = 8, [sk_9] = 9,
        [sk_4] = 4, [sk_5] = 5, [sk_6] = 6,
        [sk_1] = 1, [sk_2] = 2, [sk_3] = 3,
        [sk_0] = 0,
    };

    char magic[] = {APPVAR_MAGIC, 0};
    char *name;
    void *ptr = NULL;
    char names[LOAD_MAX][MAX_NAME_LENGTH + 1];
    uint8_t index = 0;
    uint8_t key = 0;
    bool done = false;
    
    while (!done) {
        gfx_ZeroScreen();
        while (index < LOAD_MAX && (name = ti_Detect(&ptr, magic))) {
            fontlib_SetCursorPosition(LOAD_VSPACING, (index + 1) * LOAD_VSPACING);
            fontlib_DrawUInt(index, 1);
            fontlib_DrawString(": ");
            fontlib_DrawString(name);
            strcpy(names[index], name);
            index++;
        }
        gfx_SwapDraw();
        while (!(key = os_GetCSC())) {}
        switch (key) {
            case sk_0:
            case sk_1:
            case sk_2:
            case sk_3:
            case sk_4:
            case sk_5:
            case sk_6:
            case sk_7:
            case sk_8:
            case sk_9:
                name = names[numtable[key]];
                if (name == NULL) break;
                done = true;
                break;
            case sk_Down:
                index = 0;
                if (name == NULL) ptr = NULL;
                memset(names, 0, sizeof(names));
                break;
            default:
                return;
        }
    }
    
    uint8_t handle = ti_Open(name, "r");
    ti_Read(&soundasm_state, sizeof(soundasm_state), 1, handle);
    ti_Close(handle);
}

static void ui_SaveProject(void) {
    soundasm_state.magic[0] = APPVAR_MAGIC0;
    soundasm_state.magic[1] = APPVAR_MAGIC1;
    soundasm_state.magic[2] = APPVAR_MAGIC2;
    soundasm_state.magic[3] = APPVAR_MAGIC3;
    char name[MAX_NAME_LENGTH + 1] = {0}; // 1 more for the \0
    uint8_t index = 0;
    uint16_t key = 0;

    do {
        if (key == k_Clear) return;    

        if (index < MAX_NAME_LENGTH) {
            if (key >= k_CapA && key <= k_CapZ) {
                name[index++] = key - k_CapA + 'a';
            }
            if (index != 0 && key >= k_0 && key <= k_9) {
                name[index++] = key - k_0 + '0';
            }
        }

        if (key == k_Del) {
            name[--index] = 0;
        }

        gfx_ZeroScreen();
        fontlib_SetCursorPosition(20, 20);
        fontlib_DrawString("names can't start with numbers");
        fontlib_SetCursorPosition(20, 28);
        fontlib_DrawString("use enter to save!");
        fontlib_SetCursorPosition(20, 50);
        fontlib_DrawString(name);
        if (index != MAX_NAME_LENGTH) fontlib_DrawString("|");
        gfx_SwapDraw();
    } while ((key = os_GetKey()) != k_Enter);
    
    if (index == 0) return;
    
    uint8_t handle = ti_Open(name, "w");
    ti_Write(&soundasm_state, sizeof(soundasm_state), 1, handle);
    ti_Close(handle);
}

static void ui_LoadPattern() {
    if (menu != PATTERN_EDIT) return;
    int index = ui_GetElement(0, 0)->value;
    for (int i = 0; i < MAX_PATTERN_LENGTH; i++) {
        ui_GetElement(0, i + 1)->value = soundasm_state.patterns[index].steps[i];
    }
}

static void ui_LoadSong() {
    if (menu != SONG_EDIT) return;
    ui_GetElement(0, 0)->value = soundasm_state.tempo;
    ui_GetElement(1, 0)->value = soundasm_state.pattern_length;
    
    for (int i = 0; i < NUM_OSCS; i++) {
        ui_GetElement(i, 1)->value = soundasm_state.oscs[i].index;
    }

    for (int i = 0; i < MAX_SONG_LENGTH; i++) {
        for (int x = 0; x < NUM_OSCS; x++) {
            ui_GetElement(x, i + SONG_HEADER)->value = soundasm_state.oscs[x].arrangement[i];
        }
    }
}

static void ui_ChangeMenu(ui_menu_t new_menu) {
    cursorx = 0;
    cursory = 0;
    scrolly = 0;
    editing = false;
    menu = new_menu;
    switch (menu) {
        case MAIN_MENU:
            gui_elements = &menu_gui;
            width = MENU_WIDTH;
            height = MENU_HEIGHT;
            break;
        case PATTERN_EDIT:
            gui_elements = &pattern_gui;
            width = PATTERN_WIDTH;
            height = soundasm_state.pattern_length + 1;
            ui_LoadPattern();
            break;
        case SONG_EDIT:
            gui_elements = &song_gui;
            width = SONG_WIDTH;
            height = SONG_HEIGHT;
            ui_LoadSong();
            break;
    }
}

static void ui_UpdateScroll(void) {
    if (cursory < scrolly) {
        scrolly = cursory;
    } else if (cursory >= scrolly + MAX_HEIGHT) {
        scrolly = cursory - (MAX_HEIGHT - 1);
    }
}

static void ui_EditCell(ui_edit_type_t type) {
    gui_element_t *element = ui_GetCursor();
    int max = element->strings->length;
    int dist;
    switch (type) {
        case ONE_DOWN:
            dist = max - 1;
            break;
        case ONE_UP:
            dist = 1;
            break;
        case JUMP_DOWN:
            dist = max - element->strings->jump;
            break;
        case JUMP_UP:
            dist = element->strings->jump;
            break;
    }
    element->value = (element->value + dist) % max;
}

static void ui_MoveCursor(ui_move_type_t type) {
    switch (type) {
        case MOVE_RIGHT:
            cursorx++;
            cursorx %= width;
            break;
        case MOVE_LEFT:
            cursorx += width - 1;
            cursorx %= width;
            break;
        case MOVE_DOWN:
            cursory++;
            cursory %= height;
            break;
        case MOVE_UP:
            cursory += height - 1;
            cursory %= height;
            break;
    }
    ui_UpdateScroll();
}

static void ui_SpecialUpdate(void) {
    switch (menu) {
        case PATTERN_EDIT:
            if (cursory == 0) {
                ui_LoadPattern();
            } else {
                int index = ui_GetElement(0, 0)->value;
                soundasm_state.patterns[index].steps[cursory - 1] = ui_GetCursor()->value;
            }
            break;
        case SONG_EDIT: {
            int value = ui_GetCursor()->value;
            if (cursory == 0) {
                switch (cursorx) {
                    case 0:
                        soundasm_state.tempo = value;
                        break;
                    case 1:
                        soundasm_state.pattern_length = value;
                        break;
                }
            } else if (cursory == 1) {
                soundasm_state.oscs[cursorx].index = value;
            } else {
                soundasm_state.oscs[cursorx].arrangement[cursory - SONG_HEADER] = value;
            }
            break;
        }
        default:
            break;
    }
}

static bool ui_Navigation(void) {
    switch (os_GetCSC()) {
        case 0:
            break;
        case sk_Right:
            if (editing) {
                ui_EditCell(JUMP_UP);
            } else {
                ui_MoveCursor(MOVE_RIGHT);
            }
            break;
        case sk_Left:
            if (editing) {
                ui_EditCell(JUMP_DOWN);
            } else {
                ui_MoveCursor(MOVE_LEFT);
            }
            break;
        case sk_Down:
            if (editing) {
                ui_EditCell(ONE_DOWN);
            } else {
                ui_MoveCursor(MOVE_DOWN);
            }
            break;
        case sk_Up:
            if (editing) {
                ui_EditCell(ONE_UP);
            } else {
                ui_MoveCursor(MOVE_UP);
            }
            break;
        case sk_Math:
            // copy prev cell value & go down
            if (menu != PATTERN_EDIT && menu != SONG_EDIT) break; 
            if (editing) break;
            if (cursory == 0 || cursory == 1) break;
            ui_GetCursor()->value = ui_GetElement(cursorx, cursory - 1)->value;
            ui_SpecialUpdate();
            ui_MoveCursor(MOVE_DOWN);
            break;
        case sk_Recip:
            // clear cell
            if (menu != PATTERN_EDIT && menu != SONG_EDIT) break; 
            if (editing) break;
            ui_GetCursor()->value = 0;
            ui_SpecialUpdate();
            break;
        case sk_2nd:
            if (menu == MAIN_MENU) {
                switch (ui_GetCursor()->value) {
                    case 0:
                        ui_SaveProject();
                        break;
                    case 1:
                        ui_LoadProject();
                        break;
                    case 2:
                        ui_ChangeMenu(PATTERN_EDIT);
                        break;
                    case 3:
                        ui_ChangeMenu(SONG_EDIT);
                        break;
                }
            } else {
                if (editing) {
                    editing = false;
                    ui_SpecialUpdate();
                } else if (ui_GetCursor()->editable) {
                    editing = true;
                }
            }
            break;
        case sk_Enter:
            // playing is blocking lol
            if (editing) break;
            ui_DrawPlaying();
            usbaudio_Play();
            break;
        case sk_Clear:
            if (menu == MAIN_MENU) {
                return false;
            } else {
                ui_ChangeMenu(MAIN_MENU);
            }
            break;
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
