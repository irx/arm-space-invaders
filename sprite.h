/*
 * 2021, Maksymilian Mruszczak <u at one u x dot o r g>
 *
 * A utility for drawing a sprite on SSD1331-based OLED display
 */


typedef struct {
	uint16_t *data;
	uint8_t w, h;
} Sprite;


void		draw_sprite(const Sprite *, uint8_t, uint8_t);
