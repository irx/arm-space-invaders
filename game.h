#define TICK_RATE 50
#define SHOOT_COOLDOWN 0

#define SKIP_TO_BOSS 0

enum entity_type{ PLAYER, INVADER, SAUCER, MISSILE_GOOD, MISSILE_BAD, SHIELD, HEALTHBAR};
enum state{ MENU, PAUSE, LEVEL };
enum direction{ LEFT, RIGHT};
enum input_type{ INPUT_LEFT=1, INPUT_RIGHT=2, INPUT_SHOOT=4, INPUT_PAUSE=8};


extern enum state game_state;

void game_loop(void);
void init_level(uint8_t, uint8_t); //player position
void move_invaders(void);
void move_player(enum direction dir);
void game_over(void);
void game_menu(void);
void move_projectiles(void);
void player_hit(void);
void delay_ms(int);
void player_shoot(void);
void saucer_shoot(void);
void queue_input(enum input_type input);
void game_pause(void);
void boss_fight(void);
void move_saucer(void);
void saucer_hit(void);
void kill_entity(Entity *e);
void saucer_shield(uint8_t, uint8_t);
