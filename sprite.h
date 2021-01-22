/*
 * 2021, Maksymilian Mruszczak <u at one u x dot o r g>
 *
 * A utility for drawing a sprite on SSD1331-based OLED display
 */


typedef struct {
	uint16_t *data;
	uint8_t w, h;
} Sprite;


extern Sprite	sprite_invader;
extern Sprite	sprite_invader_alt;
extern Sprite	sprite_invader2;
extern Sprite	sprite_invader2_alt;
extern Sprite	sprite_invader3;
extern Sprite	sprite_invader3_alt;
extern Sprite	sprite_invader4;
extern Sprite	sprite_logo;

void		init_sprites(void);
void		draw_sprite(const Sprite *, uint8_t, uint8_t);
