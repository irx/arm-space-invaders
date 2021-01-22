/*
 * 2021, Maksymilian Mruszczak <u at one u x dot o r g>
 *
 * A utility for drawing a sprite on SSD1331-based OLED display
 */

#include "MKL05Z4.h"
#include "LIB_Config.h"
#include "SSD1331.h"
#include "sprite.h"

/* sprite data */
#include "assets/invader.h"
#include "assets/logo.h"


Sprite				sprite_invader;
Sprite				sprite_logo;

static const uint8_t		oled_width = 96;
static const uint8_t		oled_height = 64;
static const uint8_t		set_column_address = 0x15;
static const uint8_t		set_row_address = 0x75;


void
init_sprites(void)
{
	sprite_invader.data = (uint16_t *)sprite_invader_data;
	sprite_invader.w = 14;
	sprite_invader.h = 10;

	sprite_logo.data = (uint16_t *)sprite_logo_data;
	sprite_logo.w = 96;
	sprite_logo.h = 64;
}

void
draw_sprite(const Sprite *s, uint8_t x, uint8_t y)
{
	int i, j, idx;
	uint8_t h, l;

	/* prepare for CMD input */
	__SSD1331_DC_CLR();
	__SSD1331_CS_CLR();

	/* set x */
	__SSD1331_WRITE_BYTE(&set_column_address);
	__SSD1331_WRITE_BYTE(&x);
	__SSD1331_WRITE_BYTE(&oled_width);
	/* set y */
	__SSD1331_WRITE_BYTE(&set_row_address);
	__SSD1331_WRITE_BYTE(&y);
	__SSD1331_WRITE_BYTE(&oled_height);

	/* change to DATA input */
	__SSD1331_DC_SET();
	for (i = 0; i < s->h; ++i) {
		/* write a row of pixels and change pos to the next line */
		for (j = 0; j < s->w; ++j) {
			idx = i * s->w + j;
			h = s->data[idx] >> 8;
			l = s->data[idx];
			__SSD1331_WRITE_BYTE(&h);
			__SSD1331_WRITE_BYTE(&l);
		}
		/* prepare for CMD input */
		__SSD1331_DC_CLR();
		/* set x */
		__SSD1331_WRITE_BYTE(&set_column_address);
		__SSD1331_WRITE_BYTE(&x);
		__SSD1331_WRITE_BYTE(&oled_width);
		/* set y */
		++y;
		__SSD1331_WRITE_BYTE(&set_row_address);
		__SSD1331_WRITE_BYTE(&y);
		__SSD1331_WRITE_BYTE(&oled_height);
		/* go back to DATA mode */
		__SSD1331_DC_SET();
	}
	__SSD1331_CS_SET();
}
