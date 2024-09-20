from define import GRID_SIZE, COLORS_PANELS, COLORS_PANELS_NB, TILE_COLORS

class Bbox:
	def __init__(self, x, y, colors):
		self.x = x
		self.y = y
		self.Rx = x
		self.Ry = y
		self.colors = colors
		self.positions = []


	def __str__(self) -> str:
		return f"({self.x}-{self.Rx}, {self.y}-{self.Ry})"


	def __repr__(self) -> str:
		return self.__str__()


	def update(self, x, y):
		# If empty, create bbox
		if (self.x == -1):
			self.x = max(x - 1, 0)
			self.y = max(y - 1, 0)
			self.Rx = min(x + 1, GRID_SIZE - 1)
			self.Ry = min(y + 1, GRID_SIZE - 1)

			self.positions.append((x - 1, y - 1))
			self.positions.append((x, y - 1))
			self.positions.append((x + 1, y - 1))
			self.positions.append((x - 1, y ))
			self.positions.append((x + 1, y ))
			self.positions.append((x - 1, y + 1))
			self.positions.append((x, y + 1))
			self.positions.append((x + 1, y + 1))
			return

		nx = x
		ny = y

		# Remove new full pos
		try:
			self.positions.remove((nx, ny))
		except: pass

		# Update border
		if x <= self.x:
			x = max(x - 1, 0)
			for i in range(self.y, self.Ry + 1):
				for j in range(x, self.x):
					self.positions.append((j, i))
			self.x = x

		if x >= self.Rx:
			rx = min(x + 1, GRID_SIZE - 1)
			for i in range(self.y, self.Ry + 1):
				for j in range(self.Rx + 1, rx + 1):
					self.positions.append((j, i))
			self.Rx = rx

		if y <= self.y:
			y = max(y - 1, 0)
			for i in range(self.x, self.Rx + 1):
				for j in range(y, self.y):
					self.positions.append((i, j))
			self.y = y

		if y >= self.Ry:
			ry = min(y + 1, GRID_SIZE - 1)
			for i in range(self.x, self.Rx + 1):
				for j in range(self.Ry + 1, ry + 1):
					self.positions.append((i, j))
			self.Ry = ry


	def isInside(self, x, y):
		return (x, y) in self.positions


	def isInPerimeter(self, x, y):
		# return x >= self.x - 1 and x <= self.Rx + 1\
		# 		and y >= self.y - 1 and y <= self.Ry + 1
		return False


	def collideBbox(self, bbox: 'Bbox'):
		return bbox.Rx >= self.x and bbox.x <= self.Rx\
				and bbox.Ry >= self.y and bbox.y <= self.Ry


	def merge(self, bbox: 'Bbox'):
		# if bbox.x <= self.x:
		# 	self.x = bbox.x

		# if bbox.Rx >= self.Rx:
		# 	self.Rx = bbox.Rx

		# if bbox.y <= self.y:
		# 	self.y = bbox.y

		# if bbox.Ry >= self.Ry:
		# 	self.Ry = bbox.Ry
		pass


	def copy(self):
		bbox = Bbox(self.x, self.y, self.colors)
		bbox.Rx = self.Rx
		bbox.Ry = self.Ry
		bbox.positions = self.positions
		return bbox



class BboxManager:
	def __init__(self):
		self.bboxes : list[Bbox] = []
		self.nbBoxes = 0


	def update(self, x, y):
		newBbox = Bbox(-1, -1, COLORS_PANELS[self.nbBoxes % COLORS_PANELS_NB])
		newBbox.update(x, y)

		for i in range(self.nbBoxes):
			bbox = self.bboxes[i]
			if bbox.isInside(x, y):
				bbox.update(x, y)
				return

			# if bbox.isInPerimeter(x, y):
			# 	# Update bbox if interesting
			# 	if (newBbox.x == bbox.x and newBbox.Rx == bbox.Rx)\
			# 			or (newBbox.y == bbox.y and newBbox.Ry == bbox.Ry):
			# 		bbox.update(x, y)

			# 		j = 0
			# 		while j < self.nbBoxes:
			# 			if i != j and bbox.collideBbox(self.bboxes[j]):

			# 				# Merge bbox if interesting
			# 				if (bbox.x == self.bboxes[j].x and bbox.Rx == self.bboxes[j].Rx)\
			# 					or (bbox.y == self.bboxes[j].y and bbox.Ry == self.bboxes[j].Ry):
			# 					bbox.merge(self.bboxes[j])
			# 					self.bboxes.pop(j)
			# 					self.nbBoxes -= 1
			# 					if i > j:
			# 						i -= 1
			# 					j = -1

			# 				# Reduce bbox to avoid overlap
			# 				else:

			# 					pass

			# 			j += 1
			# 		return

			# 	# print(f"NewBbox {newBbox} | bbox {bbox}")

			# 	# Reduce bbox size to avoid overlap with other bbox
			# 	if bbox.Rx >= newBbox.x and newBbox.x >= bbox.x and\
			# 			bbox.y <= newBbox.y and bbox.Ry >= newBbox.Ry:
			# 		newBbox.x = bbox.Rx + 1
			# 	if bbox.x <= newBbox.Rx and newBbox.Rx <= bbox.Rx and\
			# 			bbox.y <= newBbox.y and bbox.Ry >= newBbox.Ry:
			# 		newBbox.Rx = bbox.x - 1

			# 	if bbox.Ry >= newBbox.y and newBbox.y >= bbox.y and\
			# 			bbox.x <= newBbox.x and bbox.Rx >= newBbox.Rx:
			# 		newBbox.y = bbox.Ry + 1
			# 	if bbox.y <= newBbox.Ry and newBbox.Ry <= bbox.Ry and\
			# 			bbox.x <= newBbox.x and bbox.Rx >= newBbox.Rx:
			# 		newBbox.Ry = bbox.y - 1

		self.bboxes.append(newBbox)
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
