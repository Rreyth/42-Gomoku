#include <Select.hpp>
#include <Functions.hpp>

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

Select::Select()
{
	this->x = 0;
	this->y = 0;
	this->w = 0;
	this->h = 0;

	this->fontSize = 0;
	this->textPos = TOP_LEFT;
	this->textColor = sf::Color::Black;

	this->selected = 0;
	this->mouseOverSelected = 0;

	this->mouseOver = false;
	this->toggle = false;

	this->onSprite = SPRITE_SQUARE_BUTTON_ON;
	this->offSprite = SPRITE_SQUARE_BUTTON_OFF;
}


Select::Select(std::vector<std::string> &texts, int fontSize, draw_pos textPos, sf::Color textColor,
				int x, int y, float w, float h,
				sprite_name onSprite, sprite_name offSprite, sprite_name toggleSprite)
{
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;

	this->texts = texts;
	this->fontSize = fontSize;
	this->textPos = textPos;
	this->textColor = textColor;

	this->selected = 0;
	this->mouseOverSelected = 0;

	this->mouseOver = false;
	this->toggle = false;

	this->onSprite = onSprite;
	this->offSprite = offSprite;
	this->toggleSprite = toggleSprite;
}


Select::~Select()
{

}

////////////////////////////////////////////////////////////////////////////////
// Getters and setters
////////////////////////////////////////////////////////////////////////////////

int	Select::getSelected(void)
{
	return (this->selected);
}


void	Select::setSelected(int selected)
{
	if (selected < 0 || selected >= this->texts.size())
		return ;
	this->selected = selected;
}

////////////////////////////////////////////////////////////////////////////////
// Public methods
////////////////////////////////////////////////////////////////////////////////

void Select::tick(Mouse *mouse)
{
	this->mouseOver = mouse->inRectangle(this->x, this->y, this->w, this->h);

	if (this->mouseOver && mouse->isPressed(MBUT_LEFT))
		this->toggle = !this->toggle;

	if (!this->toggle)
		return ;

	int tmpY = this->y + this->h;
	this->mouseOverSelected = -1;
	for (std::size_t i = 0; i < this->texts.size(); i++)
	{
		if (mouse->inRectangle(this->x, tmpY, this->w, this->h))
		{
			this->mouseOverSelected = i;
			if (mouse->isPressed(MBUT_LEFT))
			{
				this->toggle = false;
				this->selected = i;
			}
			break;
		}
		tmpY += this->h;
	}

	if (!this->mouseOver && this->mouseOverSelected == -1 && mouse->isPressed(MBUT_LEFT))
		this->toggle = false;
}


void Select::draw(sf::RenderWindow *window, sf::Text *text, TextureManager *textureManager)
{
	if (!this->toggle)
	{
		if (!this->mouseOver)
			textureManager->drawTexture(window, this->offSprite, this->x, this->y, TOP_LEFT);
		else
			textureManager->drawTexture(window, this->onSprite, this->x, this->y, TOP_LEFT);

		if (this->texts.size() == 0)
			return ;
		drawText(window, text, this->texts[this->selected],
			this->x + (this->w / 2), this->y + (this->h / 2),
			this->fontSize, this->textColor, this->textPos);
		return ;
	}

	if (!this->mouseOver)
		textureManager->drawTexture(window, this->toggleSprite, this->x, this->y, TOP_LEFT);
	else
		textureManager->drawTexture(window, this->onSprite, this->x, this->y, TOP_LEFT);

	if (this->texts.size() == 0)
		return ;
	drawText(window, text, this->texts[this->selected],
		this->x + (this->w / 2), this->y + (this->h / 2),
		this->fontSize, this->textColor, this->textPos);

	int tmpY = this->y + this->h;
	for (std::size_t i = 0; i < this->texts.size(); i++)
	{
		if (i == this->mouseOverSelected)
			textureManager->drawTexture(window, this->onSprite, this->x, tmpY, TOP_LEFT);
		else
			textureManager->drawTexture(window, this->offSprite, this->x, tmpY, TOP_LEFT);

		drawText(window, text, this->texts[i],
			this->x + (this->w / 2), tmpY + (this->h / 2),
			this->fontSize, this->textColor, this->textPos);

		tmpY += this->h;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Operator
////////////////////////////////////////////////////////////////////////////////

Select	&Select::operator=(const Select &btn)
{
	this->x = btn.x;
	this->y = btn.y;
	this->w = btn.w;
	this->h = btn.h;

	this->texts = btn.texts;
	this->fontSize = btn.fontSize;
	this->textPos = btn.textPos;
	this->textColor = btn.textColor;

	this->selected = btn.selected;
	this->mouseOverSelected = btn.mouseOverSelected;

	this->mouseOver = btn.mouseOver;
	this->toggle = btn.toggle;

	this->onSprite = btn.onSprite;
	this->offSprite = btn.offSprite;
	this->toggleSprite = btn.toggleSprite;

	return (*this);
}
