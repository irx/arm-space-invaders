#define TICK_RATE 25
#define SPRITES_TOTAL 11
#define SHOOT_COOLDOWN 15


enum entity_type{ PLAYER, INVADER, SAUCER, MISSILE_GOOD, MISSILE_BAD };
enum state{ MENU, PAUSE, LEVEL };
enum direction{ LEFT, RIGHT};

extern enum state game_state;
	
void game_loop(void);
void init_level(uint8_t, uint8_t); //player position
void move_invaders(void);
void move_player(enum direction dir);
void game_over(void);
void move_projectiles(void);
void player_hit(void);
void delay_ms(int);
void player_shoot(void);
void invader_shoot(Entity *e);
