#include "MKL05Z4.h"
#include "sprite.h"
#include "entity.h"
#include "game.h"
#include "SSD1331.h"


static Sprite *bullet1 = &sprite_player;
static Sprite *bullet2 = &sprite_player;
	

static Entity *player;
static Entity *saucer;
static enum direction invaders_dir = RIGHT;
static uint8_t level_speed = 6; //default 1, max 30, temp 6 for tests
static uint16_t score = 0;
static uint16_t high_score = 0;
static uint8_t lives = 3;
static uint8_t cooldown=0;
static uint8_t kill_count=0;
static uint8_t input_queue = 0;
static uint8_t saucer_timer = 255;
enum state game_state = MENU;

void game_loop()
{

	score = 0;
	uint8_t ticks_till_move = (uint8_t)(2 + TICK_RATE);// formula: (uint8_t)(2 + TICK_RATE - level_speed*TICK_RATE/32)
	init_level(42,54);
	
	if (!input_queue)
	{
		if (input_queue & INPUT_SHOOT) player_shoot();
		if (input_queue & INPUT_LEFT) move_player(LEFT);
		else if (input_queue & INPUT_RIGHT) move_player(RIGHT);
	}
	
	while (1)
	{
		if (game_state == LEVEL)
		{
			if (!(--ticks_till_move))
			{
				move_invaders();
				ticks_till_move = (uint8_t)(2 + TICK_RATE - level_speed*TICK_RATE/32);
			}
			if (cooldown) --cooldown;
			move_projectiles();
		}
		
		render_entities();
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
	player = create_entity(&sprite_player, &sprite_player, x, y, INVADER);
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
}


void game_over()
{
	game_state = PAUSE;
	delete_entity(player);
	//gonna display score and wait for any input to restart the game
	//show score, compare score and current highscore
	delay_ms(2000);
	level_speed = 6;
	init_level(42,54);
	
}

void move_invaders()
{
	Entity *i = player;
	while (i->next != NULL)
	{
		i = i->next;
		if (i->type == INVADER)
		{
			if (((i->x == 0) && !invaders_dir) || ((i->x == 82) && invaders_dir)) //checking if direction swap takes place
			{
				i = player;
				if (level_speed<31) ++level_speed;
				while (i->next != NULL) //moving invaders down
				{
					i = i->next;
					if (i->type == INVADER)
					{
						++(i->y);
						if ((i->y)>43) game_over();
					}
				}
				invaders_dir = (invaders_dir+1)%2;
				break;
			}
		}
	}
	i = player;
	while (i->next != NULL) //moving left|right
	{
		i = i->next;
		if (i->type == INVADER)
		{
			i->frame = ((i->frame)+1)%2; // animate 'em!
			if (invaders_dir) ++(i->x);
			else --(i->x);
		}
	}
}
void move_projectiles()
{
	Entity *i = player; //bullet pointer
	Entity *j = player; //scanned entities pointer
	while (i->next != NULL)
	{
		i = i->next;
		if (i->type == MISSILE_GOOD)
		{
			--(i->y);
			while (j->next != NULL) //scanning entities for hit
			{
				j = j->next;
				if ((j->type == INVADER) && ((i->x)+(i->sprite[0]->w)-2 > (j->x)) && ((i->x)+1 < (j->x)+(j->sprite[0]->w)-1) && ((i->y) == (j->y)+(j->sprite[0]->h)-1) )
				{
					score+=10;
					delete_entity(i);
					delete_entity(j);
					if (++kill_count == 15)
					{
						kill_count = 0;
						init_level(player->x,player->y);
					}
					break;
				}
				else if ((i->y) == 0) delete_entity(i); //projectile out of bonds
			}
		}
		else if(i->type == MISSILE_BAD)
		{
			++(i->y);
			if ( ((i->y) == 59) && ((i->x)+(i->sprite[0]->w)-2 > (player->x)) && ((i->x)+1 < (player->x)+(player->sprite[0]->w)-1) ) //player hit
				player_hit();
		}
	}
}
void move_player(enum direction dir)
{
	if ((player->x >= 1-dir) && (player->x <= 86+dir)) //out-of-bonds checking
	{
		if (dir) ++(player->x);
		else --(player->x);
	}
}

void player_shoot()
{
	if (!cooldown)
	{
	create_entity(bullet1, bullet1, (player->x)+(player->sprite[0]->w)/2, 55-(bullet1->h), MISSILE_GOOD);
	cooldown = SHOOT_COOLDOWN;
	}
}
void invader_shoot(Entity *e)
{
	create_entity(bullet2, bullet2, (e->x)+(e->sprite[0]->w)/2, (e->y)+1, MISSILE_BAD);
}




void player_hit()
{
	if (!(--lives)) game_over();
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
