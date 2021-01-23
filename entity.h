/*
 * 2021, Maksymilian Mruszczak <u at one u x dot o r g>
 *
 * Entities procedures
 */


typedef struct Entity Entity;
struct Entity {
	Sprite *sprite[3];
	uint8_t exists, frame, x, y, type;
	Entity *next, *prev;
};

void			init_entities(void);
Entity			*create_entity(const Sprite *, const Sprite *, uint8_t, uint8_t, uint8_t);
void			delete_entity(Entity *);
void			render_entities(void);
