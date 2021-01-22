#include "MKL05Z4.h"
#include "game.h"
#include "SSD1331.h"
#include "sprite.h"

#include "entity.h"

//-------------------temps
static Sprite *sprite_player = &sprite_invader; //TEMP
static Sprite *sprite_invader_1a = &sprite_invader; //TEMP
static Sprite *sprite_invader_1b = &sprite_invader; //TEMP
static Sprite *sprite_invader_2a = &sprite_invader; //TEMP
static Sprite *sprite_invader_2b = &sprite_invader; //TEMP
static Sprite *sprite_invader_3a = &sprite_invader; //TEMP
static Sprite *sprite_invader_3b = &sprite_invader; //TEMP
//-----------------

static Entity *player;
static Entity *saucer;
static enum direction invaders_dir = RIGHT;
static uint8_t level_speed = 5; //default 1, max 30
static uint16_t score = 0;
static uint16_t high_score = 0;
static uint8_t lives = 3;

void game_loop()
{

	score = 0;
	uint8_t ticks_till_move = (uint8_t)(2 + TICK_RATE);// formula: (uint8_t)(2 + TICK_RATE - level_speed*TICK_RATE/32)
	init_level();
	while (1)
	{
		if (!(--ticks_till_move))
		{
			move_invaders();
			ticks_till_move = (uint8_t)(2 + TICK_RATE - level_speed*TICK_RATE/32);
		}

		render_entities();
		delay_ms((int)(1000/TICK_RATE));
	}

}


void init_level()
{
	ssd1331_clear_screen(BLACK);
	int i, j;
	lives = 3;
	init_entities();
	player = create_entity(sprite_player, sprite_player, 26, 56, INVADER);

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 5; j++)
		{
			switch (i)
			{
				case 0:
					create_entity(sprite_invader_1a, sprite_invader_1b, 13+13*i, 9+9*j, INVADER);
					break;
				case 3:
					create_entity(sprite_invader_2a, sprite_invader_2b, 13+13*i, 9+9*j, INVADER);
					break;
				default:
					create_entity(sprite_invader_3a, sprite_invader_3b, 13+13*i, 9+9*j, INVADER);
			}
		}
	}
}


void game_over()
{
	delete_entity(player);
	//gonna display score and wait for any input to restart the game
	//show score, compare score and current highscore
}

void move_invaders()
{
	Entity *i = player;
	while (i->next != NULL)
	{
		i = i->next;
		if (i->type == INVADER)
		{
			if (((i->x == 0) && !invaders_dir) || ((i->x == 52) && invaders_dir)) //checking if direction swap takes place
			{
				i = player;
				while (i->next != NULL) //moving invaders down
				{
					i = i->next;
					++(i->y);
					if ((i->y)>41) game_over();
					if (level_speed<31) ++level_speed;
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
			if (invaders_dir) ++(i->x);
			else --(i->x);
		}
	}
}
void move_projectiles()
{
	Entity *i = player;
	Entity *j = player;
	while (i->next != NULL)
	{
		i = i->next;
		if (i->type == MISSILE_GOOD)
		{
			++(i->y);
			while (j->next != NULL) //scanning entities for hit
			{
				j = j->next;
				if ( ((i->x) < (j->x)) && ((i->x)+(i->sprite[0]->w)-1 > (j->x)) && ((i->y) == (j->y)+2) )
				{
					score+=10;
					delete_entity(i);
					delete_entity(j);
					break;
				}
				else if ((i->y) == 62) delete_entity(i); //projectile out of bonds
			}
		}
		else if(i->type == MISSILE_BAD)
		{
			--(i->y);
			if ( ((i->y) == 4) && ((i->x) > (player->x)) && ((i->x) < (player->x)+11) ) //player hit
				player_hit();
		}
	}
}
void move_player(enum direction dir)
{
	if ((player->x > 0+dir) && (player->x < 54-dir)) //out-of-bonds checking
	{
		if (dir) ++(player->x);
		else --(player->x);
	}
}

void player_hit()
{
	if (!(--lives)) game_over();
	//else do some player hit stuff like changing sprits

}

void delay_ms( int n)
{
	volatile int i, j;
	for( i = 0 ; i < n; i++)
	for(j = 0; j < 3500; j++) {}
}
