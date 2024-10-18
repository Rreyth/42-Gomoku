#ifndef ANIMATION_HPP
# define ANIMATION_HPP

# include <define.hpp>

# include <SFML/Graphics.hpp>
# include <string>

# define NB_ANIM 36

typedef enum e_anim_sprite
{
	AMOGUS_BLUE_CAPT_1,
	AMOGUS_BLUE_CAPT_2,
	AMOGUS_BLUE_CAPT_3,
	AMOGUS_BLUE_CAPT_4,
	AMOGUS_BLUE_CAPT_5,
	AMOGUS_BLUE_CAPT_6,
	AMOGUS_RED_CAPT_1,
	AMOGUS_RED_CAPT_2,
	AMOGUS_RED_CAPT_3,
	AMOGUS_RED_CAPT_4,
	AMOGUS_RED_CAPT_5,
	AMOGUS_RED_CAPT_6,
	COIN_BLUE_CAPT_1,
	COIN_BLUE_CAPT_2,
	COIN_BLUE_CAPT_3,
	COIN_BLUE_CAPT_4,
	COIN_BLUE_CAPT_5,
	COIN_BLUE_CAPT_6,
	COIN_RED_CAPT_1,
	COIN_RED_CAPT_2,
	COIN_RED_CAPT_3,
	COIN_RED_CAPT_4,
	COIN_RED_CAPT_5,
	COIN_RED_CAPT_6,
	ROCK_BLUE_CAPT_1,
	ROCK_BLUE_CAPT_2,
	ROCK_BLUE_CAPT_3,
	ROCK_BLUE_CAPT_4,
	ROCK_BLUE_CAPT_5,
	ROCK_BLUE_CAPT_6,
	ROCK_RED_CAPT_1,
	ROCK_RED_CAPT_2,
	ROCK_RED_CAPT_3,
	ROCK_RED_CAPT_4,
	ROCK_RED_CAPT_5,
	ROCK_RED_CAPT_6
}	anim_sprite;

class Animation
{
public:
	Animation(anim_sprite start, int x, int y, int gridX, int gridY);
	~Animation();

	bool	getRunning(void);

	void	tick(float delta);
	void	loadTextures(void);
	void	draw(sf::RenderWindow *window);
	
private:
	sf::Texture	textures[NB_ANIM];
	sf::Sprite	sprites[NB_ANIM];
	float		timer;
	int			frame;
	bool		running;
	anim_sprite	start_sprite;
	int			x, y;

	void loadTexture(anim_sprite name, std::string filename);
	void tqt(anim_sprite name, std::string filename);
};

#endif
