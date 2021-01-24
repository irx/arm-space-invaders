/**
 * 2021, Maksymilian Mruszczak <u at one u x dot o r g>
 *
 * A utility for drawing a sprite on SSD1331-based OLED display
 */

#include "MKL05Z4.h"
#include "LIB_Config.h"
#include "SSD1331.h"
#include "sprite.h"

/* sprite data */
#include "assets/laser.h"
#include "assets/invader.h"
#include "assets/logo.h"
#include "assets/player.h"
#include "assets/saucer.h"


Sprite				sprite_laser1;
Sprite				sprite_laser1_alt;
Sprite				sprite_laser2;
Sprite				sprite_laser2_alt;
Sprite				sprite_invader;
Sprite				sprite_invader_alt;
Sprite				sprite_invader_death;
Sprite				sprite_invader2;
Sprite				sprite_invader2_alt;
Sprite				sprite_invader2_death;
Sprite				sprite_invader3;
Sprite				sprite_invader3_alt;
Sprite				sprite_invader3_death;
Sprite				sprite_logo;
Sprite				sprite_player_g;
Sprite				sprite_player_y;
Sprite				sprite_player_r;
Sprite				sprite_saucer;
Sprite				sprite_saucer_alt;
Sprite				sprite_saucer_death;
Sprite				sprite_shield;
Sprite				sprite_shield_alt;
Sprite				sprite_shield_death;

static const uint8_t		oled_width = 96;
static const uint8_t		oled_height = 64;
static const uint8_t		set_column_address = 0x15;
static const uint8_t		set_row_address = 0x75;


void
init_sprites(void)
{
	/* laser1 */
	sprite_laser1.data = (uint16_t *)sprite_laser1_data;
	sprite_laser1.w = 2;
	sprite_laser1.h = 5;

	sprite_laser1_alt.data = (uint16_t *)sprite_laser1_alt_data;
	sprite_laser1_alt.w = 2;
	sprite_laser1_alt.h = 5;

	/* laser2 */
	sprite_laser2.data = (uint16_t *)sprite_laser2_data;
	sprite_laser2.w = 1;
	sprite_laser2.h = 5;

	sprite_laser2_alt.data = (uint16_t *)sprite_laser2_alt_data;
	sprite_laser2_alt.w = 1;
	sprite_laser2_alt.h = 5;

	/* invader */
	sprite_invader.data = (uint16_t *)sprite_invader_data;
	sprite_invader.w = 14;
	sprite_invader.h = 10;

	sprite_invader_alt.data = (uint16_t *)sprite_invader_alt_data;
	sprite_invader_alt.w = 14;
	sprite_invader_alt.h = 10;

	sprite_invader_death.data = (uint16_t *)sprite_invader_death_data;
	sprite_invader_death.w = 14;
	sprite_invader_death.h = 10;

	/* invader2 */
	sprite_invader2.data = (uint16_t *)sprite_invader2_data;
	sprite_invader2.w = 14;
	sprite_invader2.h = 10;

	sprite_invader2_alt.data = (uint16_t *)sprite_invader2_alt_data;
	sprite_invader2_alt.w = 14;
	sprite_invader2_alt.h = 10;

	sprite_invader2_death.data = (uint16_t *)sprite_invader2_death_data;
	sprite_invader2_death.w = 14;
	sprite_invader2_death.h = 10;

	/* invader3 */
	sprite_invader3.data = (uint16_t *)sprite_invader3_data;
	sprite_invader3.w = 14;
	sprite_invader3.h = 10;

	sprite_invader3_alt.data = (uint16_t *)sprite_invader3_alt_data;
	sprite_invader3_alt.w = 14;
	sprite_invader3_alt.h = 10;

	sprite_invader3_death.data = (uint16_t *)sprite_invader3_death_data;
	sprite_invader3_death.w = 14;
	sprite_invader3_death.h = 10;

	/* logo */
	sprite_logo.data = (uint16_t *)sprite_logo_data;
	sprite_logo.w = 96;
	sprite_logo.h = 64;

	/* player */
	sprite_player_g.data = (uint16_t *)sprite_player_g_data;
	sprite_player_g.w = 13;
	sprite_player_g.h = 5;

	sprite_player_y.data = (uint16_t *)sprite_player_y_data;
	sprite_player_y.w = 13;
	sprite_player_y.h = 5;

	sprite_player_r.data = (uint16_t *)sprite_player_r_data;
	sprite_player_r.w = 13;
	sprite_player_r.h = 5;

	/* saucer */
	sprite_saucer.data = (uint16_t *)sprite_saucer_data;
	sprite_saucer.w = 18;
	sprite_saucer.h = 15;

	sprite_saucer_alt.data = (uint16_t *)sprite_saucer_alt_data;
	sprite_saucer_alt.w = 18;
	sprite_saucer_alt.h = 15;

	sprite_saucer_death.data = (uint16_t *)sprite_saucer_death_data;
	sprite_saucer_death.w = 18;
	sprite_saucer_death.h = 15;

	sprite_shield.data = (uint16_t *)sprite_shield_data;
	sprite_shield.w = 16;
	sprite_shield.h = 3;

	sprite_shield_alt.data = (uint16_t *)sprite_shield_alt_data;
	sprite_shield_alt.w = 16;
	sprite_shield_alt.h = 3;

	sprite_shield_death.data = (uint16_t *)sprite_shield_death_data;
	sprite_shield_death.w = 16;
	sprite_shield_death.h = 3;
}

/**
 * like itoa (int to str) but specifically for uint16
 * returns null-terminated string located at addr
 * pointed by first arg
 */
char *
u16toa(char *dst, unsigned int len, uint16_t src)
{
	int i;
	uint16_t dig, div = 1;
	while (div < src)
		div *= 10;
	i = 0;
	while (i++ < len && src > 0) {
		div /= 10;
		dig = src / div;
		dst[i] = (char)(dig + 48);
		src -= dig * div;
	}
	dst[i] = '\0';
	return dst;
}

/**
 * draw or clear sprite
 * writes bytes directly to SSD1331 over SPI
 */
void
draw_clear_sprite(uint8_t draw, const Sprite *s, uint8_t x, uint8_t y)
{
	uint8_t h, l;
	int i, j, idx = 0;

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
			idx = (i * s->w + j) * draw;
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
