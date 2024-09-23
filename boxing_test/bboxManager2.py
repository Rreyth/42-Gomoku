from define import GRID_SIZE, COLORS_PANELS, COLORS_PANELS_NB, TILE_COLORS

class Bbox:
	def __init__(self, x, y, colors):
		self.x = x
		self.y = y
		self.Rx = x
		self.Ry = y
		self.colors = colors
		colorId = COLORS_PANELS.index(colors)
		colorsLetters = "RGBYCP"
		self.colorsLetter = colorsLetters[colorId]


	def __str__(self) -> str:
		return f"({self.x}-{self.Rx}, {self.y}-{self.Ry}, {self.colorsLetter})"


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


	def cutOverlap(self, bbox: Bbox, newBbox: Bbox) -> bool:
		expandBbox = False

		# Get the bbox dir. s = square, x = x axis, y = y axis
		bboxDir = 's'
		if bbox.Rx - bbox.x > bbox.Ry - bbox.y:
			bboxDir = 'x'
		elif bbox.Rx - bbox.x < bbox.Ry - bbox.y:
			bboxDir = 'y'

		# If new bbox is on the top left corner
		if bbox.x > newBbox.x and bbox.y > newBbox.y:
			# If bbox dir isn't y axis, we can expand it to x axis
			if bboxDir != 'y':
				bbox.x -= bbox.x - newBbox.x
			else:
				bbox.y -= bbox.y - newBbox.y
			expandBbox = True

		elif bbox.x > newBbox.x and bbox.Ry < newBbox.Ry:
			# If bbox dir isn't y axis, we can expand it to x axis
			if bboxDir != 'y':
				bbox.x -= bbox.x - newBbox.x
			else:
				bbox.Ry += newBbox.Ry - bbox.Ry
			expandBbox = True

		elif bbox.Rx < newBbox.Rx and bbox.y > newBbox.y:
			# If bbox dir isn't y axis, we can expand it to x axis
			if bboxDir != 'y':
				bbox.Rx += newBbox.Rx - bbox.Rx
			else:
				bbox.y -= bbox.y - newBbox.y
			expandBbox = True

		elif bbox.Rx < newBbox.Rx and bbox.Ry < newBbox.Ry:
			# If bbox dir isn't y axis, we can expand it to x axis
			if bboxDir != 'y':
				bbox.Rx += newBbox.Rx - bbox.Rx
			else:
				bbox.Ry += newBbox.Ry - bbox.Ry
			expandBbox = True

		# If new bbox is on the x side of bbox
		if bbox.y <= newBbox.y and bbox.Ry >= newBbox.Ry:
			# new bbox on the left of bbox
			if newBbox.x < bbox.x:
				newBbox.Rx = bbox.x - 1
			# new bbox on the right of bbox
			else:
				newBbox.x = bbox.Rx + 1

		# If new bbox is on the y side of bbox
		elif bbox.x <= newBbox.x and bbox.Rx >= newBbox.Rx:
			# new bbox on the top of bbox
			if newBbox.y < bbox.y:
				newBbox.Ry = bbox.y - 1
			# new bbox on the bottom of bbox
			else:
				newBbox.y = bbox.Ry + 1

		return expandBbox


	def updateBbox(self, bbox: Bbox, idBbox, newBbox: Bbox, x, y):
		# Check if we can merge in x or y axis
		if (bbox.y == newBbox.y and bbox.Ry == newBbox.Ry) or\
				(bbox.x == newBbox.x and bbox.Rx == newBbox.Rx):
			bbox.update(x, y)
			return idBbox
		else:
			self.cutOverlap(bbox, newBbox)

		return -1


	def update(self, x, y):
		# Create bbox
		newBbox = Bbox(-1, -1, COLORS_PANELS[self.nbBoxes % COLORS_PANELS_NB])
		newBbox.update(x, y)
		print(f"\nNew stone at {x} {y}")

		inBbox = -1
		borderBbox = []

		# For each bbox, check if the stone border or is in other bbox
		for i in range(self.nbBoxes):
			bbox = self.bboxes[i]

			if bbox.isInside(x, y):
				inBbox = i
				break

			if bbox.collideBbox(newBbox):
				borderBbox.append(i)

		newBboxId = -1

		# New stone in bbox, update it
		if inBbox != -1:
			print(" update bbox")
			bbox = self.bboxes[inBbox]

			newBboxId = self.updateBbox(bbox, inBbox, newBbox, x, y)
			if newBboxId == -1:
				self.bboxes.append(newBbox)
				newBboxId = self.nbBoxes
				self.nbBoxes += 1

		# New stone border some other bbox
		elif len(borderBbox) != 0:
			print(" border bbox")
			bboxUpdate = False

			for i in borderBbox:
				bbox : Bbox = self.bboxes[i]

				newBboxId = self.updateBbox(bbox, i, newBbox, x, y)
				if newBboxId != -1:
					bboxUpdate = True
					break

			if not bboxUpdate:
				self.bboxes.append(newBbox)
				newBboxId = self.nbBoxes
				self.nbBoxes += 1

		# New stone is alone
		else:
			print(f" new bbox append {newBbox}")
			self.bboxes.append(newBbox)
			newBboxId = self.nbBoxes
			self.nbBoxes += 1

		print(f" nbBbox {self.nbBoxes}")
		newBbox = self.bboxes[newBboxId]
		# Check for merge bbox if possible
		i = 0
		while i < self.nbBoxes:
			# Skip the check of same bbox
			if i == newBboxId:
				i += 1
				continue

			bbox = self.bboxes[i]

			print(f"  bbox {bbox}, newBbox {newBbox}")
			# Check if bbox can merge on x axis
			if bbox.y == newBbox.y and bbox.Ry == newBbox.Ry and\
					bbox.x <= newBbox.Rx + 1 and bbox.Rx >= newBbox.x - 1:
				print("   merge x")
				# Merge bbox
				bbox.x = min(bbox.x, newBbox.x)
				bbox.Rx = max(bbox.Rx, newBbox.Rx)
				print(f"   bbox merged {bbox}")

				# Remove newBbox de boxes
				self.bboxes.pop(newBboxId)
				self.nbBoxes -= 1

				# Get new bbox
				if newBboxId < i:
					i -= 1
				newBboxId = i
				newBbox = self.bboxes[newBboxId]
				i = -1

			# Check if bbox can merge on y axis
			elif bbox.x == newBbox.x and bbox.Rx == newBbox.Rx and\
					bbox.y <= newBbox.Ry + 1 and bbox.Ry >= newBbox.y - 1:
				print("   merge y")
				# Merge bbox
				bbox.y = min(bbox.y, newBbox.y)
				bbox.Ry = max(bbox.Ry, newBbox.Ry)
				print(f"   bbox merged {bbox}")

				# Remove newBbox de boxes
				self.bboxes.pop(newBboxId)
				self.nbBoxes -= 1

				# Get new bbox
				if newBboxId < i:
					i -= 1
				newBboxId = i
				newBbox = self.bboxes[newBboxId]
				i = -1

			# Delete overlap
			elif bbox.collideBbox(newBbox):
				print("overlap")
				self.cutOverlap(bbox, newBbox)
				print(f"   bbox overlaped {newBbox}")

			print()

			i += 1



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
