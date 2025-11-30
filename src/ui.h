#ifndef _UI_H
#define _UI_H

#include <stdbool.h>
#include <graphx.h>

#define SONG_HEADER 2

#define MENU_WIDTH     1
#define MENU_HEIGHT    4
#define PATTERN_WIDTH  1
#define PATTERN_HEIGHT (MAX_PATTERN_LENGTH + 1)
#define SONG_WIDTH     NUM_OSCS
#define SONG_HEIGHT    (MAX_SONG_LENGTH + SONG_HEADER)
#define MAX_HEIGHT     15

#define BACKGROUND_COLOR 0x00 /* black */
#define DEFAULT_COLOR    0xFF /* white */
#define SELECTED_COLOR   0xBC /* lilac */
#define EDITING_COLOR    0xE9 /*  red  */

#define MARGIN 2
#define DMARGIN (MARGIN * 2)
#define BOX_WIDTH (GFX_LCD_WIDTH / 6 - DMARGIN)
#define BOX_HEIGHT 8 + DMARGIN
#define BOX_HSPACING (GFX_LCD_WIDTH / 6)
#define BOX_VSPACING (DMARGIN + BOX_HEIGHT)

// useful
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef enum ui_menu {
    MAIN_MENU,
    PATTERN_EDIT,
    SONG_EDIT,
} ui_menu_t;

typedef enum ui_edit_type {
    ONE_DOWN,
    ONE_UP,
    JUMP_DOWN,
    JUMP_UP,
} ui_edit_type_t;

typedef enum ui_move_type {
    MOVE_RIGHT,
    MOVE_LEFT,
    MOVE_DOWN,
    MOVE_UP,
} ui_move_type_t;

typedef struct proj_state {
    // name(?), tempo, patterns, instruments, song pattern
    int pattern_length;
} proj_state_t;

typedef struct string_table {
    unsigned int length;
    unsigned int jump;
    char *strings[];
} string_table_t;

typedef struct gui_element {
    bool editable;
    int value;
    const string_table_t *strings;
} gui_element_t;

void ui_Init(void);
void ui_Loop(void);

#endif
