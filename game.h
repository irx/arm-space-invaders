#define TICK_RATE 50
#define SPRITES_TOTAL 11
#define SHOOT_COOLDOWN 15
#define TICKS_PER_SPEEDUP 25

enum entity_type{ PLAYER, INVADER, SAUCER, MISSILE_GOOD, MISSILE_BAD};
enum state{ MENU, PAUSE, LEVEL };
enum direction{ LEFT, RIGHT};
enum input_type{ INPUT_LEFT=1, INPUT_RIGHT=2, INPUT_SHOOT=4, INPUT_PAUSE=8};

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
void saucer_shoot(Entity *e);
void queue_input(enum input_type input);
void game_pause(void);
void boss_fight(void);
void move_saucer(void);

