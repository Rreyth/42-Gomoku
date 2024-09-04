#ifndef TEXTUREMANAGER_HPP
# define TEXTUREMANAGER_HPP

# include <define.hpp>

# include <SFML/Graphics.hpp>
# include <string>

# define NB_SPRITES 19

typedef enum e_sprite_name
{
	SPRITE_GRID,
	SPRITE_SQUARE_ROUNDED_BUTTON_OFF,
	SPRITE_SQUARE_ROUNDED_BUTTON_ON,
	SPRITE_COMPACT_SQUARE_ROUNDED_BUTTON_OFF,
	SPRITE_COMPACT_SQUARE_ROUNDED_BUTTON_ON,
	SPRITE_SQUARE_BUTTON_OFF,
	SPRITE_SQUARE_BUTTON_ON,
	SPRITE_GEAR_BUTTON_OFF,
	SPRITE_GEAR_BUTTON_ON,
	SPRITE_SMALL_ROUND_BUTTON_OFF,
	SPRITE_SMALL_ROUND_BUTTON_ON,
	SPRITE_STONE_RED,
	SPRITE_STONE_BLUE,
	SPRITE_COIN_RED,
	SPRITE_COIN_BLUE,
	SPRITE_AMOGUS_RED,
	SPRITE_AMOGUS_BLUE,
	SPRITE_STONE_PREVIEW,
	SPRITE_ARROW,
}	sprite_name;

class TextureManager
{
public:
	TextureManager(void);
	~TextureManager();

	void	loadTextures(void);
	void	scaleSprite(sprite_name name, float x, float y);
	void	drawTexture(sf::RenderWindow *window, sprite_name name, int x, int y, draw_pos pos);

private:
	sf::Texture	textures[NB_SPRITES];
	sf::Sprite	sprites[NB_SPRITES];

	void loadTexture(sprite_name name, std::string filename);
};

#endif
