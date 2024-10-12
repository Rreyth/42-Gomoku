#include <game/BboxManager.hpp>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

BboxManager::BboxManager( void )
{
	this->nbBbox = 0;
}


BboxManager::BboxManager(const BboxManager &obj)
{
	for (int i = 0; i < obj.nbBbox; i++)
		this->bboxes.push_back(obj.bboxes[i]);
	this->nbBbox = obj.nbBbox;
}

//---- Destructor --------------------------------------------------------------

BboxManager::~BboxManager( void )
{

}


//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

std::vector<Bbox>	*BboxManager::getBboxes(void)
{
	return (&this->bboxes);
}

//---- Setters -----------------------------------------------------------------

//---- Operators ---------------------------------------------------------------

BboxManager	&BboxManager::operator=(const BboxManager &obj)
{
	if (this == &obj)
		return (*this);

	this->bboxes.clear();
	for (int i = 0; i < obj.nbBbox; i++)
		this->bboxes.push_back(obj.bboxes[i]);
	this->nbBbox = obj.nbBbox;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	BboxManager::update(int x, int y)
{
	bool				skipNewBbox;
	int					idNewBbox, idBbox;
	Bbox				tmp, *newBbox, *bbox;
	std::vector<Bbox>	newBboxes;

	tmp = Bbox();
	tmp.update(x, y);
	newBboxes.push_back(tmp);

	// For each new bbox
	idNewBbox = 0;
	while (idNewBbox < newBboxes.size())
	{
		newBbox = &newBboxes[idNewBbox];
		skipNewBbox = false;

		// Cut bbox to avoid overlaping
		idBbox = 0;
		while (idBbox < this->nbBbox)
		{
			bbox = &this->bboxes[idBbox];

			// If newBbox is in another bbox, stop it and skip adding this new bbox
			if (bbox->containsBbox(newBbox))
			{
				skipNewBbox = true;
				break;
			}

			// If bbox is contains in newBbox, delete bbox
			if (newBbox->containsBbox(bbox))
			{
				this->bboxes.erase(this->bboxes.begin() + idBbox);
				this->nbBbox--;
				continue;
			}

			// Check if newBbox collide another bbox
			if (newBbox->collideBbox(bbox))
			{
				// If newBbox collide with bbox on the x axis side
				if (bbox->Ly <= newBbox->Ly && bbox->Ry >= newBbox->Ry)
				{
					// If newBbox is on the left of bbox
					if (newBbox->Lx < bbox->Lx && newBbox->Rx <= bbox->Rx)
						newBbox->Rx = bbox->Lx - 1;
					// If newBbox is on the right of bbox
					else if (newBbox->Rx > bbox->Rx && newBbox->Lx >= bbox->Lx)
						newBbox->Lx = bbox->Rx + 1;
					// Else newBbox is cut by bbox
					else
					{
						// Create a new bbox on the right of bbox
						tmp = Bbox();
						tmp.Lx = bbox->Rx + 1;
						tmp.Rx = newBbox->Rx;
						tmp.Ly = newBbox->Ly;
						tmp.Ry = newBbox->Ry;
						newBboxes.push_back(tmp);
						newBbox = &newBboxes[idNewBbox];

						// Update newBbox to be on the left of bbox
						newBbox->Rx = bbox->Lx - 1;
					}
				}

				// If newBbox collide with bbox on the y axis side
				else if (bbox->Lx <= newBbox->Lx && bbox->Rx >= newBbox->Rx)
				{
					// If newBbox is on the top of bbox
					if (newBbox->Ly < bbox->Ly && newBbox->Ry <= bbox->Ry)
						newBbox->Ry = bbox->Ly - 1;
					// If newBbox is on the bot of bbox
					else if (newBbox->Ry > bbox->Ry && newBbox->Ly >= bbox->Ly)
						newBbox->Ly = bbox->Ry + 1;
					// Else newBbox is cut by bbox
					else
					{
						// Create a new bbox on the top of bbox
						tmp = Bbox();
						tmp.Lx = newBbox->Lx;
						tmp.Rx = newBbox->Rx;
						tmp.Ly = bbox->Ry + 1;
						tmp.Ry = newBbox->Ry;
						newBboxes.push_back(tmp);
						newBbox = &newBboxes[idNewBbox];

						// Update newBbox to be on the bot of bbox
						newBbox->Ry = bbox->Ly - 1;
					}
				}

				// Else the bbox is in a corner
				else
				{
					// If bbox is 1 tile high
					if (bbox->Ry - bbox->Ly == 0)
					{
						// If new bbox is on the left of bbox
						if (bbox->Lx > newBbox->Lx)
						{
							// Create new bbox for top left
							tmp = Bbox();
							tmp.Lx = bbox->Lx;
							tmp.Rx = newBbox->Rx;
							tmp.Ly = newBbox->Ly;
							tmp.Ry = bbox->Ly - 1;
							newBboxes.push_back(tmp);
							newBbox = &newBboxes[idNewBbox];

							// Create new bbox for bot left
							tmp = Bbox();
							tmp.Lx = bbox->Lx;
							tmp.Rx = newBbox->Rx;
							tmp.Ly = bbox->Ry + 1;
							tmp.Ry = newBbox->Ry;
							newBboxes.push_back(tmp);
							newBbox = &newBboxes[idNewBbox];

							// Update newBbox
							newBbox->Rx = bbox->Lx - 1;
						}

						// If new bbox is on the right of bbox
						else
						{
							// Create new bbox for top right
							tmp = Bbox();
							tmp.Lx = newBbox->Lx;
							tmp.Rx = bbox->Rx;
							tmp.Ly = newBbox->Ly;
							tmp.Ry = bbox->Ly - 1;
							newBboxes.push_back(tmp);
							newBbox = &newBboxes[idNewBbox];

							// Create new bbox for bot right
							tmp = Bbox();
							tmp.Lx = newBbox->Lx;
							tmp.Rx = bbox->Rx;
							tmp.Ly = bbox->Ry + 1;
							tmp.Ry = newBbox->Ry;
							newBboxes.push_back(tmp);
							newBbox = &newBboxes[idNewBbox];

							// Update newBbox
							newBbox->Lx = bbox->Rx + 1;
						}
					}

					// If bbox is 1 tile width
					else if (bbox->Rx - bbox->Lx == 0)
					{
						// If new bbox is on the top of bbox
						if (bbox->Ly > newBbox->Ly)
						{
							// Create new bbox for bot left
							tmp = Bbox();
							tmp.Lx = newBbox->Lx;
							tmp.Rx = bbox->Lx - 1;
							tmp.Ly = bbox->Ly;
							tmp.Ry = newBbox->Ry;
							newBboxes.push_back(tmp);
							newBbox = &newBboxes[idNewBbox];

							// Create new bbox for bot right
							tmp = Bbox();
							tmp.Lx = bbox->Rx + 1;
							tmp.Rx = newBbox->Rx;
							tmp.Ly = bbox->Ly;
							tmp.Ry = newBbox->Ry;
							newBboxes.push_back(tmp);
							newBbox = &newBboxes[idNewBbox];

							// Update newBbox
							newBbox->Ry = bbox->Ly - 1;
						}

						// If new bbox is on the bot of bbox
						else
						{
							// Create new bbox for top left
							tmp = Bbox();
							tmp.Lx = newBbox->Lx;
							tmp.Rx = bbox->Lx - 1;
							tmp.Ly = newBbox->Ly;
							tmp.Ry = bbox->Ry;
							newBboxes.push_back(tmp);
							newBbox = &newBboxes[idNewBbox];

							// Create new bbox for top right
							tmp = Bbox();
							tmp.Lx = bbox->Rx + 1;
							tmp.Rx = newBbox->Rx;
							tmp.Ly = newBbox->Ly;
							tmp.Ry = bbox->Ry;
							newBboxes.push_back(tmp);
							newBbox = &newBboxes[idNewBbox];

							// Update newBbox
							newBbox->Ly = bbox->Ry + 1;
						}
					}

					// If newBbox is in top left corner of bbox
					else if (bbox->Lx > newBbox->Lx && bbox->Ly > newBbox->Ly)
					{
						// Create a new bbox for fill the empty space left by cutting newBbox
						tmp = Bbox();
						tmp.Lx = newBbox->Lx;
						tmp.Rx = bbox->Lx - 1;
						tmp.Ly = bbox->Ly;
						tmp.Ry = newBbox->Ry;
						newBboxes.push_back(tmp);
						newBbox = &newBboxes[idNewBbox];
						// Cut newBbox to avoid overlap with bbox
						newBbox->Ry = bbox->Ly - 1;
					}

					// If newBbox is in bot left corner of bbox
					else if (bbox->Lx > newBbox->Lx && bbox->Ry < newBbox->Ry)
					{
						// Create a new bbox for fill the empty space left by cutting newBbox
						tmp = Bbox();
						tmp.Lx = newBbox->Lx;
						tmp.Rx = bbox->Lx - 1;
						tmp.Ly = newBbox->Ly;
						tmp.Ry = bbox->Ry;
						newBboxes.push_back(tmp);
						newBbox = &newBboxes[idNewBbox];
						// Cut newBbox to avoid overlap with bbox
						newBbox->Ly = bbox->Ry + 1;
					}

					// If newBbox is in top right corner of bbox
					else if (bbox->Rx < newBbox->Rx && bbox->Ly > newBbox->Ly)
					{
						// Create a new bbox for fill the empty space left by cutting newBbox
						tmp = Bbox();
						tmp.Lx = bbox->Rx + 1;
						tmp.Rx = newBbox->Rx;
						tmp.Ly = bbox->Ly;
						tmp.Ry = newBbox->Ry;
						newBboxes.push_back(tmp);
						newBbox = &newBboxes[idNewBbox];
						// Cut newBbox to avoid overlap with bbox
						newBbox->Ry = bbox->Ly - 1;
					}

					// If newBbox is in bot right corner of bbox
					else
					{
						// Create a new bbox for fill the empty space left by cutting newBbox
						tmp = Bbox();
						tmp.Lx = bbox->Rx + 1;
						tmp.Rx = newBbox->Rx;
						tmp.Ly = newBbox->Ly;
						tmp.Ry = bbox->Ry;
						newBboxes.push_back(tmp);
						newBbox = &newBboxes[idNewBbox];
						// Cut newBbox to avoid overlap with bbox
						newBbox->Ly = bbox->Ry + 1;
					}
				}
			}

			idBbox++;
		}


		if (skipNewBbox)
		{
			idNewBbox++;
			continue;
		}

		// Merging bbox to lower their number
		idBbox = 0;
		while (idBbox < this->nbBbox)
		{
			bbox = &this->bboxes[idBbox];

			// Check if newBbox can merge with bbox on x or y axis
			if ((newBbox->Ly == bbox->Ly && newBbox->Ry == bbox->Ry
					&& newBbox->Lx <= bbox->Rx + 1 && newBbox->Rx >= bbox->Lx - 1)
				|| (newBbox->Lx == bbox->Lx && newBbox->Rx == bbox->Rx
					&& newBbox->Ly <= bbox->Ry + 1 && newBbox->Ry >= bbox->Ly - 1))
			{
				newBbox->merge(bbox);
				this->bboxes.erase(this->bboxes.begin() + idBbox);
				this->nbBbox--;
				idBbox = 0;
				continue;
			}

			idBbox++;
		}

		idNewBbox++;
		this->nbBbox++;
		this->bboxes.push_back(*newBbox);
	}
}


std::vector<sf::Vector2i>	BboxManager::getListPosition(void) const
{
	std::vector<sf::Vector2i>	positions;

	for (int i = 0; i < this->nbBbox; i++)
		for (int y = this->bboxes[i].Ly; y <= this->bboxes[i].Ry; y++)
			for (int x = this->bboxes[i].Lx; x <= this->bboxes[i].Rx; x++)
				positions.push_back(sf::Vector2i(x, y));

	return (positions);
}


void	BboxManager::clear(void)
{
	this->bboxes.clear();
	this->nbBbox = 0;
}

//**** PRIVATE METHODS *********************************************************
