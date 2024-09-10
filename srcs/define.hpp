#ifndef DEFINE_HPP
# define DEFINE_HPP

// WINDOW DEFINE
# define WIN_W 1600
# define WIN_H 900
# define WIN_POINT 5
# define WIN_CAPTURE 10
# define MAX_FPS 1000

# define FONT_PATH_SQUADA "data/fonts/SquadaOne.ttf"
# define FONT_PATH_ROBOTO "data/fonts/Rrroboto.ttf"
# define FONT_PATH_SANKOFA "data/fonts/Sankofa.ttf"

// GRID DEFINE
# define GRID_SQUARE_HALF_SIZE 17
# define GRID_SQUARE_SIZE 34
# define GRID_SIZE 633
# define GRID_W_INTER 19
# define GRID_NB_INTER 361 // 361 = 19 * 19
# define OPTI_BOARD_SIZE 91 // 361 / 4 = 90.25 -> 91

# define AI_MAX_DEPTH 10

typedef enum e_font {
	FONT_SQUADA,
	FONT_ROBOTO,
	FONT_SANKOFA
}	t_font;

typedef enum e_draw_pos {
	TOP_LEFT,
	TOP_CENTER,
	TOP_RIGHT,
	MID_LEFT,
	MID_CENTER,
	MID_RIGHT,
	BOT_LEFT,
	BOT_CENTER,
	BOT_RIGHT
}	draw_pos;

typedef enum e_display_state {
	DISPLAY_MENU,
	DISPLAY_SETTINGS,
	DISPLAY_MODEMENU,
	DISPLAY_GAME,
	DISPLAY_END,
	DISPLAY_QUIT
}	display_state;

typedef enum e_player_type {
	PLAYER,
	AI_PLAYER
}	player_type;

typedef enum e_stone_sprite {
	DEFAULT,
	AMOGUS,
	COINS
}	stone_sprite;

typedef enum e_game_mode {
	NORMAL,
	BLITZ
}	game_mode;

typedef enum e_game_rules {
	STANDARD,
	PRO
}	game_rules;

typedef enum e_win_state {
	WIN_STATE_NONE,
	WIN_STATE_ALIGN,
	WIN_STATE_CAPTURE,
	WIN_STATE_TIME
}	win_state;

typedef enum e_dir_neighbor
{
	DIR_L = 0,
	DIR_UL,
	DIR_U,
	DIR_UR,
	DIR_R,
	DIR_DR,
	DIR_D,
	DIR_DL,
}	dir_neighbor;


typedef enum e_inter_type
{
	INTER_EMPTY = 0,
	INTER_LEFT,
	INTER_RIGHT,
	INTER_INVALID,
}	inter_type;

typedef enum e_AI_difficulty
{
	RANDOM,
	BETTER_RANDOM,
	EASY,
	MEDIUM,
	HARD
}	AI_difficulty;

typedef struct s_intersection
{
	inter_type	type;
	int			neighbor[8];
}	intersection;

// TODO : REMOVE
typedef struct s_tracker
{
	int	nbEvaluations;
	int	nbEvaluationsMemory;
	int	evaluationTime;

	int	getSortMoveNumber;
	int	getMoveTime;
	int	sortMoveTime;

	int	updateMoveNumber;
	int	updateMoveTime;

	int	checkStoneNumber;
	int	putStoneTime;
	int	removeStoneTime;
	int	checkWinTime;

	int	reverseStoneCaptureNumber;
	int	reverseStoneCaptureTime;
}	Tracker;

#endif
