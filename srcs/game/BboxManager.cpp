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

//---- Setters -----------------------------------------------------------------

//---- Operators ---------------------------------------------------------------

BboxManager	&BboxManager::operator=(const BboxManager &obj)
{
	if (this == &obj)
		return (*this);

	for (int i = 0; i < obj.nbBbox; i++)
		this->bboxes.push_back(obj.bboxes[i]);
	this->nbBbox = obj.nbBbox;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	BboxManager::saveBboxes(std::vector<Bbox> *bboxes)
{
	bboxes->clear();
	for (int i = 0; i < this->nbBbox; i++)
		bboxes->push_back(this->bboxes[i]);
}


void	BboxManager::loadBboxes(std::vector<Bbox> *bboxes)
{
	this->bboxes.clear();
	for (int i = 0; i < this->nbBbox; i++)
		this->bboxes.push_back(bboxes->at(i));
	this->nbBbox = this->bboxes.size();
}


void	BboxManager::update(int x, int y)
{
	bool				skipNewBbox;
	int					idNewBbox, idBbox;
	Bbox				tmp, *newBbox, *bbox;
	std::vector<Bbox>	newBboxes;

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
				// If newBBox collide with bbox on the x axis side
				if (bbox->Ly <= newBbox->Ly && bbox->Ry >= newBbox->Ry)
				{
					// If newBbox is on the left of bbox
					if (newBbox->Lx < bbox->Lx)
						newBbox->Rx = bbox->Lx - 1;
					// If newBbox is on the right of bbox
					else
						newBbox->Lx = bbox->Rx + 1;
				}

				// If newBBox collide with bbox on the y axis side
				else if (bbox->Lx <= newBbox->Lx && bbox->Rx >= newBbox->Rx)
				{
					// If newBbox is on the top of bbox
					if (newBbox->Ly < bbox->Ly)
						newBbox->Ry = bbox->Ly - 1;
					// If newBbox is on the bot of bbox
					else
						newBbox->Ly = bbox->Ry + 1;
				}

				// Else the bbox is in a corner
				else
				{
					// If newBbox is in top left corner of bbox
					if (bbox->Lx > newBbox->Lx && bbox->Ly > newBbox->Ly)
					{
						// Create a new bbox for fill the empty space left by cutting newBbox
						tmp = Bbox();
						tmp.Lx = newBbox->Lx;
						tmp.Rx = bbox->Lx - 1;
						tmp.Ly = bbox->Ly;
						tmp.Ry = newBbox->Ry;
						newBboxes.push_back(tmp);
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
			// Check if newBbox can merge with bbox on x or y axis
			if ((newBbox->Ly == bbox->Ly and newBbox->Ry == bbox->Ry
					&& newBbox->Lx <= bbox->Ry + 1 && newBbox->Rx >= bbox->Lx - 1)
				|| (newBbox->Lx == bbox->Lx && newBbox->Rx == bbox->Rx
					&& newBbox->Ly <= bbox->Ry && newBbox->Ry >= bbox->Ly))
			{
				newBbox->merge(bbox);
				this->bboxes.erase(this->bboxes.begin() + idBbox);
				this->nbBbox -= 1;
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
		for (int y = this->bboxes[i].Ly ; y <= this->bboxes[i].Ry; y++)
			for (int x = this->bboxes[i].Lx ; x <= this->bboxes[i].Rx; x++)
				positions.push_back(sf::Vector2i(x, y));

	return (positions);
}


void	BboxManager::clear(void)
{
	this->bboxes.clear();
	this->nbBbox = 0;
}

//**** PRIVATE METHODS *********************************************************
