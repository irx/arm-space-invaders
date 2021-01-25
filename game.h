#define TICK_RATE 50
#define SHOOT_COOLDOWN 35

enum state {
	MENU,
	PAUSE,
	LEVEL
};

enum input_type {
	INPUT_LEFT = 1,
	INPUT_RIGHT = 2,
	INPUT_SHOOT = 4,
	INPUT_PAUSE = 8
};



void game_loop(void);
void delay_ms(int);
void queue_input(enum input_type);
