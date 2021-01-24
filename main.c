/**
 * 2021, Maksymilian Mruszczak <u at one u x dot o r g>
 *
 * Space Invaders implementation for FRDM-KL05Z
 * used with Waveshare SSD1331 OLED display
 * and UART at 9600 for user input
 */

#include "MKL05Z4.h"
#include "spi.h"
#include "uart.h"
#include "SSD1331.h"
#include "Fonts.h"

#include "sprite.h"
#include "entity.h"

#include "game.h"

int
main(void)
{
	Entity *logo, *invader;

	spi_init();
	ssd1331_init();
	uart_init();
	init_sprites();
	init_entities();

	draw_sprite(&sprite_logo, 0, 0);
	delay_ms(2000);

	game_loop();
}

void
handle_input(char c)
{
	switch (c) {
	case ',':
		queue_input(INPUT_LEFT);
		break;
	case '.':
		queue_input(INPUT_RIGHT);
		break;
	case ' ':
		queue_input(INPUT_SHOOT);
		break;
	case 'p':
		queue_input(INPUT_PAUSE);
		break;
	}
}
