#ifndef _UI_H
#define _UI_H
#include <stdbool.h>
#include <graphx.h>

#define GUI_WIDTH 6
#define GUI_HEIGHT 2

#define BACKGROUND_COLOR 0x00 /* black */
#define DEFAULT_COLOR 0xFF    /* white */
#define SELECTED_COLOR 0xBC   /* lilac */
#define EDITING_COLOR 0xE9    /*  red  */

#define MARGIN 2
#define DMARGIN (MARGIN * 2)
#define BOX_WIDTH (GFX_LCD_WIDTH / 6 - DMARGIN)
#define BOX_HEIGHT 8 + DMARGIN
#define BOX_HSPACING (GFX_LCD_WIDTH / 6)
#define BOX_VSPACING (DMARGIN + BOX_HEIGHT)

typedef struct string_table {
    unsigned int length;
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
