#include "fonts.h"

static const unsigned char hw_font_data[] = {
    #include "hwfont.inc"
};

const fontlib_font_t *hw_font = (fontlib_font_t *)hw_font_data;
