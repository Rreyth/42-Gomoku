from define import GRID_SIZE, COLORS_PANELS, COLORS_PANELS_NB, TILE_COLORS, TILE_SIZE, OFFSET

import pygame as pg
import time

class Bbox:
	def __init__(self, x, y, colors):
		self.x = x
		self.y = y
		self.Rx = x
		self.Ry = y
		self.colors = colors
		self.colorsLetter = colors[2]


	def __str__(self) -> str:
		return f"({self.x}-{self.Rx}, {self.y}-{self.Ry}, {self.colorsLetter})"


	def __repr__(self) -> str:
		return self.__str__()


	def __eq__(self, value: object) -> bool:
		if type(value) != Bbox:
			return False

		return self.x == value.x and self.Rx == value.Rx and\
				self.y == value.y and self.Ry == value.Ry


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


	def collideBbox(self, bbox: 'Bbox'):
		return bbox.Rx >= self.x and bbox.x <= self.Rx\
				and bbox.Ry >= self.y and bbox.y <= self.Ry


	def containBbox(self, bbox: 'Bbox'):
		return self.x <= bbox.x and self.Rx >= bbox.Rx and\
				self.y <= bbox.y and self.Ry >= bbox.Ry


	def merge(self, bbox: 'Bbox'):
		if bbox.x <= self.x:
			self.x = bbox.x

		if bbox.Rx >= self.Rx:
			self.Rx = bbox.Rx

		if bbox.y <= self.y:
			self.y = bbox.y

		if bbox.Ry >= self.Ry:
			self.Ry = bbox.Ry

		self.colors = bbox.colors
		self.colorsLetter = bbox.colorsLetter


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
		# Create bbox
		tmp = Bbox(-1, -1, COLORS_PANELS[self.nbBoxes % COLORS_PANELS_NB])
		tmp.update(x, y)

		start = time.time()
		nbSave = self.nbBoxes

		newBboxes = [tmp]
		print(f"\n\n============================================================\nNew stone at {x} {y}")

		idNewBbox = 0
		while idNewBbox < len(newBboxes):
			addNewBbox = True
			newBbox = newBboxes[idNewBbox]
			print(f"\nnewBbox {newBbox}-----------------------------------")

			print("\ncontains and cut part")
			i = 0
			while i < self.nbBoxes:
				bbox = self.bboxes[i]
				print(f"newBbox {newBbox}, bbox {bbox}")

				# If newBbox is contain by another bbox, quit here
				if bbox.containBbox(newBbox):
					print(" newBbox is in bbox, stop here")
					addNewBbox = False
					break

				# If newBbox conain bbox, remove bbox
				if newBbox.containBbox(bbox):
					print(" newBbox is canibal, eating bbox")
					self.bboxes.pop(i)
					self.nbBoxes -= 1
					continue

				# Check collide
				if newBbox.collideBbox(bbox):
					print(" newBbox collid bbox")

					# If new bbox is on the x side of bbox
					if bbox.y <= newBbox.y and bbox.Ry >= newBbox.Ry:
						print("  newBbox collide with bbox on x axis")
						# new bbox on the left of bbox
						if newBbox.x < bbox.x and newBbox.Rx <= bbox.Rx:
							print("   right newBbox full in bbox")
							newBbox.Rx = bbox.x - 1
						# new bbox on the right of bbox
						elif newBbox.Rx > bbox.Rx and newBbox.x >= bbox.x:
							print("   left newBbox full in bbox")
							newBbox.x = bbox.Rx + 1
						else:
							print("   newBbox splited by bbox")
							# Create new bbox
							tmpNewBBox = newBbox.copy()
							tmpNewBBox.x = bbox.Rx + 1
							newBboxes.append(tmpNewBBox)

							# Update newBbox
							newBbox.Rx = bbox.x - 1
							pass

					# If new bbox is on the y side of bbox
					elif bbox.x <= newBbox.x and bbox.Rx >= newBbox.Rx:
						print("  newBbox collide with bbox on y axis")
						# new bbox on the top of bbox
						if newBbox.y < bbox.y and newBbox.Ry <= bbox.Ry:
							print("   bot newBbox full in bbox")
							newBbox.Ry = bbox.y - 1
						# new bbox on the bottom of bbox
						elif newBbox.Ry > bbox.Ry and newBbox.y >= bbox.y:
							print("   up newBbox full in bbox")
							newBbox.y = bbox.Ry + 1
						else:
							print("   newBbox splited by bbox")
							# Create new bbox
							tmpNewBBox = newBbox.copy()
							tmpNewBBox.y = bbox.Ry + 1
							newBboxes.append(tmpNewBBox)

							# Update newBbox
							newBbox.Ry = bbox.y - 1
							pass

					else:
						# If new bbox is on the top left corner
						if bbox.x > newBbox.x and bbox.y > newBbox.y:
							print("  collide with bbox top left corner")
							# Create a new bbox to fill empty space left by cutting newBbox
							tmp = Bbox(0, 0, COLORS_PANELS[(self.nbBoxes + len(newBboxes)) \
																% COLORS_PANELS_NB])
							tmp.x = newBbox.x
							tmp.Rx = bbox.x - 1
							tmp.y = bbox.y
							tmp.Ry = newBbox.Ry
							newBboxes.append(tmp)
							# Cut newBbox
							newBbox.Ry = bbox.y - 1
							print(f"   newBbox corner cut {newBbox}")
							print(f"   new tmp bbox {tmp}")

						# If new bbox is on the bot left corner
						elif bbox.x > newBbox.x and bbox.Ry < newBbox.Ry:
							print("  collide with bbox bot left corner")
							# Create a new bbox to fill empty space left by cutting newBbox
							tmp = Bbox(0, 0, COLORS_PANELS[(self.nbBoxes + len(newBboxes)) \
																% COLORS_PANELS_NB])
							tmp.x = newBbox.x
							tmp.Rx = bbox.x - 1
							tmp.y = newBbox.y
							tmp.Ry = bbox.Ry
							newBboxes.append(tmp)
							# Cut newBbox
							newBbox.y = bbox.Ry + 1
							print(f"   newBbox corner cut {newBbox}")
							print(f"   new tmp bbox {tmp}")

						# If new bbox is on the top right corner
						elif bbox.Rx < newBbox.Rx and bbox.y > newBbox.y:
							print("  collide with bbox top right corner")
							# Create a new bbox to fill empty space left by cutting newBbox
							tmp = Bbox(0, 0, COLORS_PANELS[(self.nbBoxes + len(newBboxes)) \
																% COLORS_PANELS_NB])
							tmp.x = bbox.Rx + 1
							tmp.Rx = newBbox.Rx
							tmp.y = bbox.y
							tmp.Ry = newBbox.Ry
							newBboxes.append(tmp)
							# Cut newBbox
							newBbox.Ry = bbox.y - 1
							print(f"   newBbox corner cut {newBbox}")
							print(f"   new tmp bbox {tmp}")

						# If new bbox is on the bot right corner
						else:
							print("  collide with bbox bot right corner")
							# Create a new bbox to fill empty space left by cutting newBbox
							tmp = Bbox(0, 0, COLORS_PANELS[(self.nbBoxes + len(newBboxes)) \
																% COLORS_PANELS_NB])
							tmp.x = bbox.Rx + 1
							tmp.Rx = newBbox.Rx
							tmp.y = newBbox.y
							tmp.Ry = bbox.Ry
							newBboxes.append(tmp)
							# Cut newBbox
							newBbox.y = bbox.Ry + 1
							print(f"   newBbox corner cut {newBbox}")
							print(f"   new tmp bbox {tmp}")

					print(f"  newBbox cut {newBbox}")

				i += 1

			if not addNewBbox:
				idNewBbox += 1
				continue

			print("\nmerge part")
			i = 0
			while i < self.nbBoxes:
				bbox = self.bboxes[i]

				print(f"newBbox {newBbox}, bbox {bbox}")
				# Check merging of x or y
				if (newBbox.y == bbox.y and newBbox.Ry == bbox.Ry and\
						newBbox.x <= bbox.Rx + 1 and newBbox.Rx >= bbox.x - 1) or\
					(newBbox.x == bbox.x and newBbox.Rx == bbox.Rx and\
						newBbox.y <= bbox.Ry + 1 and newBbox.Ry >= bbox.y - 1):
					print(" merge on x axis")
					newBbox.merge(bbox)
					print(f"  newBbox merged {newBbox}\n")
					self.bboxes.pop(i)
					self.nbBoxes -= 1
					i = 0
					continue

				i += 1

			idNewBbox += 1
			self.bboxes.append(newBbox)
			self.nbBoxes += 1

		end = time.time()

		return ((end - start) * 1000000, nbSave, self.nbBoxes)


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


	def draw(self, win):
		for bbox in self.bboxes:
			x = (bbox.x) * TILE_SIZE + OFFSET[0] - 1
			y = (bbox.y) * TILE_SIZE + OFFSET[1] - 1
			w = (bbox.Rx - bbox.x + 1) * TILE_SIZE + 1
			h = (bbox.Ry - bbox.y + 1) * TILE_SIZE + 1
			pg.draw.rect(win, (255, 255, 255), (x, y, w, h), 1)


	def clear(self):
		self.bboxes.clear()
		self.nbBoxes = 0


	def copy(self):
		bboxManager = BboxManager()
		for bbox in self.bboxes:
			bboxManager.bboxes.append(bbox.copy())
		bboxManager.nbBoxes = self.nbBoxes
		return bboxManager


	def getListPosition(self):
		lst = []

		for bbox in self.bboxes:
			for y in range(bbox.y, bbox.Ry + 1):
				for x in range(bbox.x, bbox.Rx + 1):
					lst.append((x, y))

		return lst


	def printBbox(self):
		print("\nbboxes :")
		for bbox in self.bboxes:
			print(bbox)
