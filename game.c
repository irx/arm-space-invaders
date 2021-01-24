#include "MKL05Z4.h"
#include "sprite.h"
#include "entity.h"
#include "game.h"
#include "SSD1331.h"


static Entity *player;
static Entity *saucer;
static enum direction invaders_dir = RIGHT;
static uint8_t level_speed = 10; //default 1, max 90, temp 10 for tests
static uint16_t score = 0;
static uint16_t high_score = 0;
static uint8_t lives = 3;
static uint8_t cooldown=0; //player shooting cooldown
static uint8_t kill_count=0;
static uint8_t projectile_count=0;
static uint8_t input_queue = 0;
static uint8_t pending_kill = 0;

static uint8_t speedup_timer = TICKS_PER_SPEEDUP;
static uint8_t pending_render=0;
static uint8_t render_projectiles=0;
static uint8_t animation_cooldown=5;

static uint8_t boss=0;
static uint8_t boss_lives = 6;
static uint8_t boss_cooldown = 150;
static uint8_t boss_iframes = 0;
//static uint8_t shield;


enum state game_state = MENU;

void game_loop()
{
	uint8_t ticks_till_move = (uint8_t)(1 + TICK_RATE);
	Entity *k = player;
	game_menu();
	
	#if SKIP_TO_BOSS //SKIPPING TO BOSS FOR TESTING
		//static uint8_t k_count = 0;
		//kill_count = 19;
		/*while (k->next != NULL)
		{
			k = k->next;
			if (k->type == INVADER)
			{
				kill_entity(k);
				k->val = 1;
				kill_entity(k);
				if ((++k_count)==19) break;
			}
		}*/
	#endif


	while (1)
	{
		if (game_state == LEVEL)
		{
			if (pending_kill) //death animation cleanup
			{
				k = player;
				while (k->next != NULL)
				{
					k = k->next;
					if (k->val > 1)
					{
						if (--(k->val) == 1)
						{
							kill_entity(k);
							if (!(--pending_kill)) break;
						}
					}
				}
			}

			if (input_queue) //input check
			{
				if (input_queue & INPUT_SHOOT) player_shoot();
				if (input_queue & INPUT_LEFT) move_player(LEFT);
					else if (input_queue & INPUT_RIGHT) move_player(RIGHT);
				if (input_queue & INPUT_PAUSE) game_pause();
				input_queue = 0;
			}

			if (!boss) //normal level stuff
			{
				if (!(--ticks_till_move))
				{
					move_invaders();
					ticks_till_move = (uint8_t)(1 + TICK_RATE - level_speed*TICK_RATE/100);
				}
				if (level_speed < 90)
					{
						if (speedup_timer) --speedup_timer;
							else {  ++level_speed; speedup_timer = TICKS_PER_SPEEDUP; }
					}
			}
			else //boss fight stuff
			{
				move_saucer();
				if (!(--boss_cooldown)) //saucer shooting
				{
					boss_cooldown = 165-(level_speed/2);
					saucer_shoot();
				}
			}

			if (animation_cooldown) --animation_cooldown;
			if (cooldown) --cooldown;
			move_projectiles();
		}

		if (pending_render) { render_entities(); pending_render=0; render_projectiles = 0;}
		else if (render_projectiles)
		{
			k = player;
			while (k->next != NULL)
			{
				k = k->next;
				if ((k->type == MISSILE_GOOD) || (k->type == MISSILE_BAD))
				{
					render_entity(k);
				}
			}
			render_projectiles = 0;
		}
		delay_ms((int)(1000/TICK_RATE));
	}

}


void init_level(uint8_t x, uint8_t y)
{
	ssd1331_clear_screen(BLACK);
	int i, j;
	lives = 3;
	boss = 0;
	invaders_dir = RIGHT;
	init_entities();
	player = create_entity(&sprite_player_g, &sprite_player_y, &sprite_player_r, x, y, PLAYER);
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 5; j++)
		{
			switch (i)
			{
				case 0:
					create_entity(&sprite_invader2, &sprite_invader2_alt, &sprite_invader2_death, 2+13*j, 9*i, INVADER);
					break;
				case 3:
					create_entity(&sprite_invader3, &sprite_invader3_alt, &sprite_invader3_death, 2+13*j, 9*i, INVADER);
					break;
				default:
					create_entity(&sprite_invader, &sprite_invader_alt, &sprite_invader_death, 2+13*j, 9*i, INVADER);
			}
		}
	}
	game_state = LEVEL;
	render_entities();
}


void game_over()
{
	char buf[8];


	game_state = PAUSE;
	delete_entity(player);
	render_entities();
	delay_ms(1000);
	input_queue = 0;
	ssd1331_clear_screen(BLACK);
	if (score>high_score)
	{
		ssd1331_display_string(0, 8, "NEW HI-SCORE", FONT_1608, RED);
		high_score = score;
	}
	else ssd1331_display_string(16, 8, "SCORE:", FONT_1608, GREEN);
	ssd1331_display_string(24, 28, u16toa(buf, 8, score), FONT_1608, WHITE);
	ssd1331_display_string(10, 52, "Press any key", FONT_1206, WHITE);

	while (!input_queue) delay_ms(500/TICK_RATE);
	input_queue = 0;

	score = 0;
	level_speed = 10;
	game_menu();

}

void move_invaders()
{
	Entity *i = player;
	static uint8_t dir_swap = 0;
	static uint8_t move_down = 0;

	i = player;
	while (i->next != NULL) //moving left|right
	{
		i = i->next;
		if (i->type == INVADER)
		{
			if (move_down)
			{
				++(i->y);
				if ((i->y)>43) game_over();
			}
			else if (((i->x == 1) && !invaders_dir) || ((i->x == 81) && invaders_dir)) dir_swap = 1; //checking if direction swap takes place

			if (!animation_cooldown && i->frame != 2) // animate 'em!
				{
					i->frame = ((i->frame)+1)%2;
				}
			if (invaders_dir) ++(i->x);
			else --(i->x);
		}
	}
	if (dir_swap){ invaders_dir = (invaders_dir+1)%2; move_down = 1; dir_swap = 0;}
		else move_down = 0;
	if (!animation_cooldown) animation_cooldown = 10;
	pending_render=1;
}
void move_projectiles()
{
	Entity *i = player; //bullet pointer
	Entity *j = player; //scanned entities pointer
	if (projectile_count)
	{
	while ((i->next != NULL) && projectile_count)
		{
			i = i->next;
			
			if (i->type == MISSILE_GOOD)
			{
				render_projectiles=1;
				--(i->y);
				i->frame = ((i->frame)+1)%2;
				if((i->y)==7)
				{
					if (((i->x)+(i->sprite[0]->w)-2 > saucer->x) && ((i->x)+1)<(saucer->x)+(saucer->sprite[0]->w)-1)
					{
						saucer_hit();
					}
				}
				else
				{
					while (j->next != NULL) //scanning entities for hit
					{
						j = j->next;
						if((i->y)==11 && (j->type == SHIELD) && ((i->x)+(i->sprite[0]->w)-2 > (j->x)) && ((i->x)+1 < (j->x)+(j->sprite[0]->w)-1))
						{
							--projectile_count;
							kill_entity(j);
						}
						if ((j->type == INVADER) && ((i->x)+(i->sprite[0]->w)-2 > (j->x)) && ((i->x)+1 < (j->x)+(j->sprite[0]->w)-1) && (((i->y) == (j->y)+(j->sprite[0]->h)-1) || ((i->y) == (j->y)+(j->sprite[0]->h)-2)) )
						{

							--projectile_count;
							delete_entity(i);
							if (!(j->val)) kill_entity(j);
							break;
						}
						else if ((i->y) == 0) delete_entity(i); //projectile out of bonds
					}
				}
			}
			else if(i->type == MISSILE_BAD)
			{
				render_projectiles=1;
				++(i->y);
				i->frame = ((i->frame)+1)%2;
				
				if ((i->y) == 58)
				{
					if ( ((i->x)+(i->sprite[0]->w)-2 > (player->x)) && ((i->x)+1 < (player->x)+(player->sprite[0]->w)-1) ) //player hit
						player_hit();
					
					--projectile_count; delete_entity(i);
					
				}
				else if ((i->y) == 60) { --projectile_count; delete_entity(i); }
			}
		}
	}
}
void move_player(enum direction dir)
{
	if ((player->x >= 1-dir) && (player->x <= 84-dir)) //out-of-bonds checking
	{
		if (dir) ++(player->x);
		else --(player->x);
		render_entity(player);
		//draw_sprite(player->sprite[player->frame], player->x, player->y);
	}

}

void player_shoot()
{
	if (!cooldown)
	{
		++projectile_count;
		create_entity(&sprite_laser2, &sprite_laser2_alt, &sprite_laser2_alt, (player->x)+(player->sprite[0]->w)/2, 55-(sprite_laser2.h), MISSILE_GOOD);
		cooldown = SHOOT_COOLDOWN;

	}
}
void saucer_shoot()
{
	++projectile_count;
	static enum direction alternate = RIGHT; //fire from alternating sides
	create_entity(&sprite_laser1, &sprite_laser1_alt, &sprite_laser1_alt, (saucer->x)+2+alternate*12, (saucer->y)+6, MISSILE_BAD);
	alternate=(alternate+1)%2;
}
void saucer_hit()
{

}



void player_hit()
{
	if (!(--lives)) game_over();
	else { ++(player->frame); render_entity(player); }


}

void delay_ms( int n)
{
	volatile int i, j;
	for( i = 0 ; i < n; i++)
	for(j = 0; j < 3500; j++) {}
}

void queue_input(enum input_type input)
{
	input_queue = input_queue | input;
}


void game_pause()
{
	char buf[8];

	input_queue = 0;
	game_state = PAUSE;
	ssd1331_clear_screen(BLUE);

	ssd1331_display_string(16, 8, "SCORE:", FONT_1608, WHITE);
	ssd1331_display_string(24, 28, u16toa(buf, 8, score), FONT_1608, WHITE);
	ssd1331_display_string(10, 52, "Press any key", FONT_1206, WHITE);

	while(!input_queue) delay_ms(500/TICK_RATE);
	input_queue = 0;
	ssd1331_clear_screen(BLACK);
	game_state = LEVEL;
	render_entities();
	delay_ms(250);
}


void boss_fight()
{
	delay_ms(100);
	ssd1331_clear_screen(RED);
	delay_ms(150);
	ssd1331_clear_screen(BLACK);
	render_entities();
	delay_ms(300);
	ssd1331_clear_screen(RED);
	delay_ms(150);
	ssd1331_clear_screen(BLACK);
	saucer = create_entity(&sprite_saucer, &sprite_saucer_alt, &sprite_saucer_death, 30, 3, SAUCER);
	render_entities();
	boss=1;
	//healthbar
}

void move_saucer()
{
	static uint8_t saucer_timer = 3;
	static enum direction saucer_dir = RIGHT;
	if (!(--saucer_timer))
	{
		saucer_timer = (uint8_t)(2+(TICK_RATE/3) - level_speed*TICK_RATE/260);
	if(saucer_dir) ++(saucer->x);
		else --(saucer->x);
	if (((saucer->x == 0) && !saucer_dir) || ((saucer->x == 79) && saucer_dir)) saucer_dir =(saucer_dir+1)%2;
	render_entity(saucer);
	}
}

void game_menu()
{
	game_state = MENU;
	ssd1331_clear_screen(BLACK);
	ssd1331_display_string(9, 0, "Space", FONT_1608, GREEN);
	ssd1331_display_string(32, 16, "Invaders", FONT_1608, GREEN);
	ssd1331_display_string(10, 52, "Press any key", FONT_1206, WHITE);
	while (!input_queue) delay_ms(500/TICK_RATE);
	input_queue = 0;
	init_level(42,54);
}

void kill_entity(Entity *e)
{
	if ((e->val)==1)
	{
		if (e->type == INVADER)
		{
			score+=10;
			delete_entity(e);
			if (++kill_count == 20)
			{
				kill_count = 0;
				boss_fight();
			}
		}
		else if (e->type == SHIELD)
		{
			delete_entity(e);
		}
	}
	else { (e->frame)=2; (e->val)=12; ++pending_kill;}
	pending_render = 1;
}
