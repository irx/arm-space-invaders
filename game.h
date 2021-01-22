#define TICK_RATE 25
#define SPRITES_TOTAL 11


enum entity_type{ PLAYER, INVADER, SAUCER, MISSILE_GOOD, MISSILE_BAD };
enum direction{ LEFT, RIGHT};



void game_loop(void);
void init_level(void);
void move_invaders(void);
void move_player(enum direction dir);
void game_over(void);
void move_projectiles(void);
void player_hit(void);
void delay_ms(int);
