/*
 * 2021, Maksymilian Mruszczak <u at one u x dot o r g>
 *
 * Waveshare SSD1331 OLED display test for FRDM-KL05Z
 * based on example for STM32 XNUCLEO-F103RB from Waveshare Team
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

	ssd1331_clear_screen(RED);
	delay_ms(1000);
	delay_ms(1000);
	ssd1331_clear_screen(GREEN);
	delay_ms(1000);
	delay_ms(1000);
	ssd1331_clear_screen(BLUE);
	delay_ms(1000);
	delay_ms(1000);
	ssd1331_clear_screen(BLACK);
	
	ssd1331_display_string(10, 0, "0.95inch OLED", FONT_1206, WHITE);
	ssd1331_display_string(0, 16, "This is a demo for SSD1331 OLED board!", FONT_1206, YELLOW);
	ssd1331_display_string(22, 52, "WaveShare", FONT_1206, GREEN);
	delay_ms(1000);
	delay_ms(1000);
	ssd1331_clear_screen(BLACK);

	ssd1331_draw_bitmap(0, 2, &c_chSingal816[0], 16, 8, YELLOW);
	ssd1331_draw_bitmap(19, 2, &c_chMsg816[0], 16, 8, YELLOW);
	ssd1331_draw_bitmap(38, 2, &c_chBluetooth88[0], 8, 8, YELLOW);
	ssd1331_draw_bitmap(52, 2, &c_chGPRS88[0], 8, 8, YELLOW);
	ssd1331_draw_bitmap(66, 2, &c_chAlarm88[0], 8, 8, YELLOW);
	ssd1331_draw_bitmap(80, 2, &c_chBat816[0], 16, 8, YELLOW);

	ssd1331_draw_3216char(8,16, '2', BLUE);
	ssd1331_draw_3216char(24,16, '3', BLUE);
	ssd1331_draw_3216char(40,16, ':', RED);
	ssd1331_draw_3216char(56,16, '5', BLUE);
	ssd1331_draw_3216char(72,16, '6', BLUE);

	ssd1331_display_string(9, 48, "2015-01-27", FONT_1608, GREEN);

	logo = create_entity(&sprite_logo, &sprite_logo, 0, 0, 0);
	invader = create_entity(&sprite_invader, &sprite_invader_alt, 10, 10, 0);

	for (;;) {
		render_entities();
		delay_ms(2000);
		++(invader->y);
		invader->frame = invader->y % 2;
	}
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
	}
}
