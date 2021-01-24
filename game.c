#include "MKL05Z4.h"
#include "sprite.h"
#include "entity.h"
#include "game.h"
#include "SSD1331.h"


static Entity *player;
static Entity *saucer;
static enum direction invaders_dir = RIGHT;
static uint8_t level_speed = 6; //default 1, max 45, temp 6 for tests
static uint16_t score = 0;
static uint16_t high_score = 0;
static uint8_t lives = 3;
static uint8_t cooldown=0; //player shooting cooldown
static uint8_t kill_count=0;
static uint8_t projectile_count=0;
static uint8_t input_queue = 0;
static uint8_t saucer_cooldown = 100;
static uint8_t speedup_timer = TICKS_PER_SPEEDUP;
static uint8_t pending_render=0;
static uint8_t animation_cooldown=5;
static uint8_t boss=0;
enum state game_state = MENU;

void game_loop()
{
	uint8_t ticks_till_move = (uint8_t)(1 + TICK_RATE);
	
	game_menu();

	while (1)
	{
		if (game_state == LEVEL)
		{
			if (input_queue)
			{
				if (input_queue & INPUT_SHOOT) player_shoot();
				if (input_queue & INPUT_LEFT) move_player(LEFT);
					else if (input_queue & INPUT_RIGHT) move_player(RIGHT);
				if (input_queue & INPUT_PAUSE) game_pause();
				input_queue = 0;
			}
			
			if (!boss) //normal level stuff
			{
				if (animation_cooldown) --animation_cooldown;
				if (!(--ticks_till_move))
				{
					move_invaders();
					ticks_till_move = (uint8_t)(1 + TICK_RATE - level_speed*TICK_RATE/100);
				}
				if (level_speed < 80)
					{
						if (speedup_timer) --speedup_timer;
							else {  ++level_speed; speedup_timer = TICKS_PER_SPEEDUP; }
					}
				if (cooldown) --cooldown;
				move_projectiles();
			}
			else //boss fight stuff
			{
				move_saucer();
				if (!(--saucer_cooldown))
				{

					saucer_cooldown = 100-(level_speed/2);
					saucer_shoot;
				}
			}
			
		}
		
		if (pending_render) { render_entities(); pending_render=0;}
		delay_ms((int)(1000/TICK_RATE));
	}

}


void init_level(uint8_t x, uint8_t y)
{
	ssd1331_clear_screen(BLACK);
	int i, j;
	lives = 3;
	invaders_dir = RIGHT;
	init_entities();
	player = create_entity(&sprite_player_g, &sprite_player_y, x, y, INVADER);
	player->sprite[2] = &sprite_player_r;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 5; j++)
		{
			switch (i)
			{
				case 0:
					create_entity(&sprite_invader2, &sprite_invader2_alt, 2+13*j, 9*i, INVADER);
					break;
				case 3:
					create_entity(&sprite_invader3, &sprite_invader3_alt, 2+13*j, 9*i, INVADER);
					break;
				default:
					create_entity(&sprite_invader, &sprite_invader_alt, 2+13*j, 9*i, INVADER);
			}
		}
	}
	game_state = LEVEL;
	render_entities();
}


void game_over()
{
	game_state = PAUSE;
	delete_entity(player);
	delay_ms(1000);
	
	if (score>high_score)
	{
		ssd1331_display_string(8, 8, "NEW HIGHSCORE:", FONT_1608, RED);
		high_score = score;
	}
	else ssd1331_display_string(16, 8, "SCORE:", FONT_1608, GREEN);
	//ssd1331_display_num(24, 32, score, 4, FONT_1608, WHITE);
	ssd1331_display_string(10, 52, "Press any key", FONT_1206, WHITE);
	
	while (!input_queue);
	input_queue = 0;
	
	score = 0;
	level_speed = 1;
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
			
			if (!animation_cooldown) // animate 'em!
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
				--(i->y);
				i->frame = ((i->frame)+1)%2;
				while (j->next != NULL) //scanning entities for hit
				{
					j = j->next;
					if ((j->type == INVADER) && ((i->x)+(i->sprite[0]->w)-2 > (j->x)) && ((i->x)+1 < (j->x)+(j->sprite[0]->w)-1) && (((i->y) == (j->y)+(j->sprite[0]->h)-1) || ((i->y) == (j->y)+(j->sprite[0]->h)-2)) )
					{
						score+=10;
						--projectile_count;
						delete_entity(i);
						delete_entity(j);
						if (++kill_count == 20)
						{
							kill_count = 0;
							boss_fight();
							//init_level(player->x,player->y);
						}
						break;
					}
					else if ((i->y) == 0) delete_entity(i); //projectile out of bonds
				}
			}
			else if(i->type == MISSILE_BAD)
			{
				++(i->y);
				i->frame = ((i->frame)+1)%2;
				if ( ((i->y) == 60) && ((i->x)+(i->sprite[0]->w)-2 > (player->x)) && ((i->x)+1 < (player->x)+(player->sprite[0]->w)-1) ) //player hit
					player_hit();
			}
		}
		pending_render=1;
	}
}
void move_player(enum direction dir)
{
	if ((player->x >= 1-dir) && (player->x <= 86+dir)) //out-of-bonds checking
	{
		if (dir) ++(player->x);
		else --(player->x);
		draw_sprite(player->sprite[player->frame], player->x, player->y);
	}
	
}

void player_shoot()
{
	if (!cooldown)
	{
		++projectile_count;
		create_entity(&sprite_laser2, &sprite_laser2_alt, (player->x)+(player->sprite[0]->w)/2, 55-(sprite_laser2.h), MISSILE_GOOD);
		cooldown = SHOOT_COOLDOWN;
	}
}
void saucer_shoot(Entity *e)
{
	static enum direction alternate = RIGHT; //fire from alternating sides
	create_entity(&sprite_laser1, &sprite_laser1_alt, (e->x)+2+alternate*12, (e->y)+6, MISSILE_BAD);
}	




void player_hit()
{
	if (!(--lives)) game_over();
	if ( player->frame != 3) ++(player->frame);
	//else do some player hit stuff like changing sprites

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
	input_queue = 0;
	game_state = PAUSE;
	ssd1331_clear_screen(BLUE);
	
	ssd1331_display_string(16, 8, "SCORE:", FONT_1608, WHITE);
	//ssd1331_display_num(24, 32, score, 4, FONT_1608, WHITE);
	ssd1331_display_string(10, 52, "Press any key", FONT_1206, WHITE);
	
	while(!input_queue);
	input_queue = 0;
	ssd1331_clear_screen(BLACK);
	game_state = LEVEL;
	render_entities();
	delay_ms(250);
}


void boss_fight()
{
	delay_ms(60);
	ssd1331_clear_screen(RED);
	delay_ms(90);
	render_entities();
	delay_ms(130);
	ssd1331_clear_screen(RED);
	delay_ms(90);
	saucer = create_entity(&sprite_invader4,&sprite_invader4,0,1,SAUCER);
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
		saucer_timer=2+(TICK_RATE/3) - level_speed*TICK_RATE/240;
	if(saucer_dir) ++(saucer->x);
		else --(saucer->x);
	if (((saucer->x == 0) && !saucer_dir) || ((saucer->x == 82) && saucer_dir)) saucer_dir =(saucer_dir+1)%2;
	}
}

void game_menu()
{
	game_state = MENU;
	ssd1331_clear_screen(BLACK);
	ssd1331_display_string(9, 0, "Space", FONT_1608, GREEN);
	ssd1331_display_string(32, 16, "Invaders", FONT_1608, GREEN);
	ssd1331_display_string(10, 52, "Press any key", FONT_1206, WHITE);
	while (!input_queue);
	input_queue = 0;
	init_level(42,54);
}


