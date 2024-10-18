#include <utils/Animation.hpp>

#include <iostream>
#include <stdexcept>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

Animation::Animation(anim_sprite start, int x, int y, int gridX, int gridY)
{
	this->timer = 0;
	this->frame = 0;
	this->running = true;
	this->start_sprite = start;
	this->x = gridX + x * GRID_SQUARE_SIZE + 10;
	this->y = gridY + y * GRID_SQUARE_SIZE + 10;
	this->loadTextures();
}


Animation::~Animation(void)
{

}

////////////////////////////////////////////////////////////////////////////////
// Getters and setters
////////////////////////////////////////////////////////////////////////////////

bool	Animation::getRunning(void)
{
	return (this->running);
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////

void	Animation::loadTextures(void)
{
	// Load sprites
	this->loadTexture(AMOGUS_BLUE_CAPT_1, "amogus_blue_capture1.png");
	this->loadTexture(AMOGUS_BLUE_CAPT_2, "amogus_blue_capture2.png");
	this->loadTexture(AMOGUS_BLUE_CAPT_3, "amogus_blue_capture3.png");
	this->loadTexture(AMOGUS_BLUE_CAPT_4, "amogus_blue_capture4.png");
	this->loadTexture(AMOGUS_BLUE_CAPT_5, "amogus_blue_capture5.png");
	this->loadTexture(AMOGUS_BLUE_CAPT_6, "amogus_blue_capture6.png");
	this->loadTexture(AMOGUS_RED_CAPT_1, "amogus_red_capture1.png");
	this->loadTexture(AMOGUS_RED_CAPT_2, "amogus_red_capture2.png");
	this->loadTexture(AMOGUS_RED_CAPT_3, "amogus_red_capture3.png");
	this->loadTexture(AMOGUS_RED_CAPT_4, "amogus_red_capture4.png");
	this->loadTexture(AMOGUS_RED_CAPT_5, "amogus_red_capture5.png");
	this->loadTexture(AMOGUS_RED_CAPT_6, "amogus_red_capture6.png");
	this->loadTexture(COIN_BLUE_CAPT_1, "coin_blue_capture1.png");
	this->loadTexture(COIN_BLUE_CAPT_2, "coin_blue_capture2.png");
	this->loadTexture(COIN_BLUE_CAPT_3, "coin_blue_capture3.png");
	this->loadTexture(COIN_BLUE_CAPT_4, "coin_blue_capture4.png");
	this->loadTexture(COIN_BLUE_CAPT_5, "coin_blue_capture5.png");
	this->loadTexture(COIN_BLUE_CAPT_6, "coin_blue_capture6.png");
	this->loadTexture(COIN_RED_CAPT_1, "coin_red_capture1.png");
	this->loadTexture(COIN_RED_CAPT_2, "coin_red_capture2.png");
	this->loadTexture(COIN_RED_CAPT_3, "coin_red_capture3.png");
	this->loadTexture(COIN_RED_CAPT_4, "coin_red_capture4.png");
	this->loadTexture(COIN_RED_CAPT_5, "coin_red_capture5.png");
	this->loadTexture(COIN_RED_CAPT_6, "coin_red_capture6.png");
	this->loadTexture(ROCK_BLUE_CAPT_1, "rock_blue_capture1.png");
	this->loadTexture(ROCK_BLUE_CAPT_2, "rock_blue_capture2.png");
	this->loadTexture(ROCK_BLUE_CAPT_3, "rock_blue_capture3.png");
	this->loadTexture(ROCK_BLUE_CAPT_4, "rock_blue_capture4.png");
	this->loadTexture(ROCK_BLUE_CAPT_5, "rock_blue_capture5.png");
	this->loadTexture(ROCK_BLUE_CAPT_6, "rock_blue_capture6.png");
	this->loadTexture(ROCK_RED_CAPT_1, "rock_red_capture1.png");
	this->loadTexture(ROCK_RED_CAPT_2, "rock_red_capture2.png");
	this->loadTexture(ROCK_RED_CAPT_3, "rock_red_capture3.png");
	this->loadTexture(ROCK_RED_CAPT_4, "rock_red_capture4.png");
	this->loadTexture(ROCK_RED_CAPT_5, "rock_red_capture5.png");
	this->loadTexture(ROCK_RED_CAPT_6, "rock_red_capture6.png");
}


void	Animation::tick(float delta)
{
	if (!this->running)
		return;

	this->timer += delta;
	if (this->timer >= 0.1)
	{
		this->frame++;
		this->timer = 0;
	}
	if (this->frame >= 6)
		this->running = false;
}


void	Animation::draw(sf::RenderWindow *window)
{
	if (!this->running)
		return;

	int w, h;

	sf::Vector2u txt_size = this->textures[this->start_sprite + this->frame].getSize();
	sf::Vector2f scale = this->sprites[this->start_sprite + this->frame].getScale();

	w = txt_size.x * scale.x;
	h = txt_size.y * scale.y;

	this->sprites[this->start_sprite + this->frame].setPosition(this->x - w / 2, this->y - h / 2);
	window->draw(this->sprites[this->start_sprite + this->frame]);
}

////////////////////////////////////////////////////////////////////////////////
// Private methods
////////////////////////////////////////////////////////////////////////////////

void	Animation::loadTexture(anim_sprite name, std::string filename)
{
	if (!this->textures[name].loadFromFile("data/anim/" + filename))
		throw std::invalid_argument("");
	this->sprites[name].setTexture(this->textures[name]);
}
