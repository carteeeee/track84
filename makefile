# ----------------------------
# Makefile Options
# ----------------------------

NAME = TRACK84
ICON = icon.png
DESCRIPTION = "a music tracker for the ti-84+ce"
COMPRESSED = YES
ARCHIVED = YES
VERSION = 0.2.0

CFLAGS = -Wall -Wextra -O3
CXXFLAGS = -Wall -Wextra -O3

FONTDIR = $(SRCDIR)/fonts
FONT = $(FONTDIR)/hwfont.fnt
FONT_INC = $(FONTDIR)/hwfont.inc

DEPS = $(FONT_INC)

# ----------------------------

include $(shell cedev-config --makefile)

# ----------------------------

$(FONT_INC): $(FONT)
	$(Q)$(call MKDIR,$(@D))
	$(Q)convfont -o carray -f $< $@

final:
	make
	convbin -r -e zx0 -k 8xp-compressed -u -n $(NAME) -i bin/$(NAME).bin -o bin/$(NAME).8xp
