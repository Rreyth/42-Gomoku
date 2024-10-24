#ifndef GAME_HPP
# define GAME_HPP

# include <define.hpp>
# include <ui/Button.hpp>
# include <game/Grid.hpp>
# include <game/Player.hpp>
# include <AI/Evaluation.hpp>

class Game
{
public:
	Game(void);
	~Game();

	void				tick(display_state *displayState, float delta, Mouse *mouse);
	void				draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
	void				setGame(player_type playerLeft, player_type playerRight,
								game_mode mode, game_rules rule,
								AI_difficulty aiLeft, AI_difficulty aiRight,
								starter startingPlayer, stone_sprite *sprite,
								bool suggestion);
	void				replay(display_state *displayState);

	Player				*getLeftPlayer(void);
	Player				*getRightPlayer(void);
	Grid				*getGrid(void);

private:
	sf::RectangleShape	rect;
	Button				leave;
	Grid				grid;
	Player 				playerLeft, playerRight;
	game_mode			mode;
	starter				startingPlayer;
	Evaluation			evaluator;
	bool				suggestion;

	void				swapTurn(void);
	void				drawLeftSide(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
	void				drawRightSide(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
	void				drawBottom(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager);
};

#endif
