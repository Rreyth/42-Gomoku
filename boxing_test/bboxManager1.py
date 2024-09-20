from define import GRID_SIZE, COLORS_PANELS, COLORS_PANELS_NB, TILE_COLORS

class Bbox:
	def __init__(self, x, y, colors):
		self.x = x
		self.y = y
		self.Rx = x
		self.Ry = y
		self.colors = colors


	def __str__(self) -> str:
		return f"({self.x}-{self.Rx}, {self.y}-{self.Ry})"


	def __repr__(self) -> str:
		return self.__str__()


	def update(self, x, y):
		if (self.x == -1):
			self.x = max(x - 1, 0)
			self.y = max(y - 1, 0)
			self.Rx = min(x + 1, GRID_SIZE - 1)
			self.Ry = min(y + 1, GRID_SIZE - 1)

		if x <= self.x:
			self.x = max(x - 1, 0)

		if x >= self.Rx:
			self.Rx = min(x + 1, GRID_SIZE - 1)

		if y <= self.y:
			self.y = max(y - 1, 0)

		if y >= self.Ry:
			self.Ry = min(y + 1, GRID_SIZE - 1)


	def isInside(self, x, y):
		return x >= self.x and x <= self.Rx and y >= self.y and y <= self.Ry


	def isInPerimeter(self, x, y):
		return x >= self.x - 1 and x <= self.Rx + 1\
				and y >= self.y - 1 and y <= self.Ry + 1


	def collideBbox(self, bbox: 'Bbox'):
		return bbox.Rx >= self.x and bbox.x <= self.Rx\
				and bbox.Ry >= self.y and bbox.y <= self.Ry


	def merge(self, bbox: 'Bbox'):
		if bbox.x <= self.x:
			self.x = bbox.x

		if bbox.Rx >= self.Rx:
			self.Rx = bbox.Rx

		if bbox.y <= self.y:
			self.y = bbox.y

		if bbox.Ry >= self.Ry:
			self.Ry = bbox.Ry


	def copy(self):
		bbox = Bbox(self.x, self.y, self.colors)
		bbox.Rx = self.Rx
		bbox.Ry = self.Ry
		return bbox



class BboxManager:
	def __init__(self):
		self.bboxes : list[Bbox] = []
		self.nbBoxes = 0


	def update(self, x, y):
		for i in range(self.nbBoxes):
			bbox = self.bboxes[i]
			if bbox.isInPerimeter(x, y):
				bbox.update(x, y)

				j = 0
				while j < self.nbBoxes:
					if i != j and bbox.collideBbox(self.bboxes[j]):
						bbox.merge(self.bboxes[j])
						self.bboxes.pop(j)
						self.nbBoxes -= 1
						if i > j:
							i -= 1
						j = 0
					else:
						j += 1
				return

		bbox = Bbox(-1, -1, COLORS_PANELS[self.nbBoxes % COLORS_PANELS_NB])
		bbox.update(x, y)
		self.bboxes.append(bbox)
		self.nbBoxes += 1


	def countBboxCover(self, x, y):
		count = 0
		for bbox in self.bboxes:
			if bbox.isInside(x, y):
				count += 1
		return count


	def getColors(self, x, y):
		for bbox in self.bboxes:
			if bbox.isInside(x, y):
				return bbox.colors

		return TILE_COLORS


	def clear(self):
		self.bboxes.clear()
		self.nbBoxes = 0


	def copy(self):
		bboxManager = BboxManager()
		for bbox in self.bboxes:
			bboxManager.bboxes.append(bbox.copy())
		return bboxManager
