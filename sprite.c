/*
 * 2021, Maksymilian Mruszczak <u at one u x dot o r g>
 *
 * A utility for drawing a sprite on SSD1331-based OLED display
 */

#include "LIB_Config.h"
#include "SSD1331.h"
#include "sprite.h"


void
draw_sprite(const Sprite *s, uint8_t x, uint8_t y)
{
	/* prepare for CMD input */
	__SSD1331_DC_CLR();
	__SSD1331_CS_CLR();

	/* set x */
	__SSD1331_WRITE_BYTE(&SET_COLUMN_ADDRESS);
	__SSD1331_WRITE_BYTE(&x);
	__SSD1331_WRITE_BYTE(&(OLED_WIDTH - 1));
	/* set y */
	__SSD1331_WRITE_BYTE(&SET_ROW_ADDRESS);
	__SSD1331_WRITE_BYTE(&y);
	__SSD1331_WRITE_BYTE(&(OLED_HEIGHT - 1));

	/* change to DATA input */
	__SSD1331_DC_SET();
	for (i = 0; i < s->h; ++i) {
		/* write a row of pixels and change pos to the next line */
		for (j = 0; j < s->w; ++j) {
			__SSD1331_WRITE_BYTE(&(s->data[i][j] >> 8));
			__SSD1331_WRITE_BYTE(&(s->data[i][j]));
		}
		/* prepare for CMD input */
		__SSD1331_DC_CLR();
		/* set x */
		__SSD1331_WRITE_BYTE(&SET_COLUMN_ADDRESS);
		__SSD1331_WRITE_BYTE(&x);
		__SSD1331_WRITE_BYTE(&(OLED_WIDTH - 1));
		/* set y */
		__SSD1331_WRITE_BYTE(&SET_ROW_ADDRESS);
		__SSD1331_WRITE_BYTE(&(++y));
		__SSD1331_WRITE_BYTE(&(OLED_HEIGHT - 1));
		/* go back to DATA mode */
		__SSD1331_DC_SET();
	}
	__SSD1331_CS_SET();
}
