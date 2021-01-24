/*
 * 2021, Maksymilian Mruszczak <u at one u x dot o r g>
 *
 * Entities procedures
 */


#include "MKL05Z4.h"
#include "sprite.h"
#include "entity.h"


#define NULL			(void *)0
#define ENTITY_CNT		32


static Entity			entity_buf[ENTITY_CNT];
static Entity			*entity_first;
static Entity			*entity_last;


void
init_entities(void)
{
	int i;
	for (i = 0; i < ENTITY_CNT; ++i)
		entity_buf[i].exists = 0;
	entity_first = NULL;
	entity_last = NULL;
}

Entity *
create_entity(const Sprite *spr, const Sprite *alt, const Sprite *alt2, uint8_t x, uint8_t y, uint8_t t)
{
	int i = -1; /* search for free space */
	while (entity_buf[++i].exists)
		if (i+1 >= ENTITY_CNT)
			return NULL;

	entity_buf[i].type = t;
	entity_buf[i].exists = 1;
	entity_buf[i].frame = 0;
	entity_buf[i].x = x;
	entity_buf[i].y = y;
	entity_buf[i].sprite[0] = (Sprite *)spr;
	entity_buf[i].sprite[1] = (Sprite *)alt;
	entity_buf[i].sprite[2] = (Sprite *)alt2;
	if (entity_last == NULL) {
		entity_buf[i].prev = NULL;
		entity_first = &entity_buf[i];
	} else {
		entity_buf[i].prev = entity_last;
		entity_last->next = &entity_buf[i];
	}
	entity_buf[i].next = NULL;
	entity_last = &entity_buf[i];
	return entity_last;
}

void
delete_entity(Entity *e)
{
	clear_sprite(e->sprite[e->frame], e->x, e->y);
	/* relink list and freeup the slot */
	if (e->prev != NULL)
		e->prev->next = e->next;
	if (e->next != NULL)
		e->next->prev = e->prev;
	if (e == entity_first)
		entity_first = e->next;
	if (e == entity_last)
		entity_last = e->prev;
	e->exists = 0;
}

void
render_entities(void)
{
	Entity *e = entity_first;
	while (e != NULL) {
		draw_sprite(e->sprite[e->frame], e->x, e->y);
		e = e->next;
	}
}
