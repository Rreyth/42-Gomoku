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
	AI
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

#endif
