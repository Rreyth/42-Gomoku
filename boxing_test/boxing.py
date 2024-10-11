import pygame as pg
import math
import time
import sys

from define import	SCREEN_SIZE, OFFSET, GRID_SIZE, GRID_W, GRID_H,\
					TILE_SIZE, TILE_BG_SIZE, TILE_EMPTY, TILE_FULL, TILE_COLORS, TILE_COLORS_IN_BBOX
from bboxManager import BboxManager


def drawText(surface, text, pos, color=(0, 0, 0), size=24, font=None, align="left"):
	font = pg.font.SysFont(font, size)
	img = font.render(text, True, color)
	if (align == "center"):
		placement = img.get_rect(center=pos)
		surface.blit(img, placement)
	else:
		surface.blit(img, pos)


def copyGrid(grid: list[list[int]]) -> list[list[int]]:
	newGrid = []
	for line in grid:
		newGrid.append(line.copy())

	return newGrid


def generateNewTestSequence(lenght):
	import random
	lst = []
	for _ in range(lenght):
		tmp = (random.randint(0, GRID_SIZE - 1), random.randint(0, GRID_SIZE - 1))
		if tmp not in lst:
			lst.append(tmp)

	print(lst)

# testSequence = [(1, 4), (1, 17), (8, 5), (12, 16), (16, 18), (11, 3), (11, 9), (14, 5), (14, 17), (5, 4), (9, 18), (9, 0), (4, 4), (6, 13), (1, 0), (5, 10), (12, 3), (17, 3), (12, 13), (14, 9), (18, 15), (6, 5), (13, 2), (2, 18), (12, 9), (17, 4), (11, 14), (5, 8), (6, 11), (4, 7), (9, 9), (15, 1), (4, 18), (16, 7), (5, 9), (14, 1), (8, 3), (10, 13), (3, 7), (9, 11), (18, 8), (16, 0), (7, 18), (15, 14), (11, 12), (11, 5)]
# testSequence = [(2, 6), (16, 0), (3, 8), (3, 16), (13, 4), (9, 3), (4, 7), (0, 11), (7, 16), (18, 5), (14, 2), (6, 16), (3, 7), (11, 7), (3, 6), (9, 4), (16, 7), (10, 11), (16, 8), (17, 5), (14, 12), (10, 17), (0, 5), (11, 4), (11, 16), (9, 5), (1, 7), (12, 13), (9, 2), (0, 3), (13, 14), (5, 17), (13, 11), (6, 6), (3, 17), (13, 17), (8, 13), (1, 18), (2, 13), (2, 8), (15, 13), (14, 16), (2, 12), (13, 8), (7, 14), (8, 6), (12, 15), (1, 3), (10, 0), (3, 14), (0, 9), (11, 11), (13, 12), (14, 10), (17, 3), (13, 9), (13, 5), (8, 10), (13, 10), (4, 15), (18, 17), (15, 16), (9, 1), (15, 10), (5, 6), (2, 1), (10, 5), (10, 1), (2, 17), (6, 5), (15, 12), (12, 8), (5, 2), (2, 10), (6, 0), (7, 9), (16, 3), (11, 2), (18, 15), (2, 7), (11, 18), (14, 13), (2, 0), (12, 3), (11, 0), (8, 9), (0, 8), (0, 12), (10, 15), (15, 1), (14, 9), (6, 3), (4, 4), (14, 8), (17, 15), (12, 1), (16, 13), (10, 6), (18, 13), (8, 2), (10, 9), (13, 15), (14, 15), (6, 10), (3, 15), (6, 9), (17, 10), (1, 2), (1, 0), (0, 7), (8, 15), (11, 14), (16, 11), (8, 3), (16, 15), (13, 0), (9, 0), (0, 18), (9, 7), (9, 9), (4, 9), (10, 18), (3, 0), (9, 10), (7, 5), (17, 13), (0, 2)]

testSequence = [(9, 9), (8, 8)]
# testSequence = [(9, 9), (8, 8), (9, 8), (9, 7), (9, 10), (8, 7)]
# testSequence = [(9, 9), (8, 8), (9, 8), (9, 7), (10, 8), (7, 9), (11, 7)]
# testSequence = [(9, 9), (8, 8), (8, 9), (7, 9), (7, 10), (6, 10), (10, 9), (9, 7), (5, 11)]


class Game:
	def __init__(self):
		"""
		This method define all variables needed by the program
		"""
		# Start of pygame
		pg.init()

		# We remove the toolbar of the window's height
		self.winSize = SCREEN_SIZE
		# We create the window
		self.win = pg.display.set_mode(self.winSize, pg.RESIZABLE)

		self.clock = pg.time.Clock() # The clock be used to limit our fps
		self.fps = 60

		self.last = time.time()

		self.runMainLoop = True

		self.grid = []
		for _ in range(GRID_SIZE):
			self.grid.append([TILE_EMPTY] * GRID_SIZE)

		self.tileTarget = None

		self.bboxManager = BboxManager()
		self.history = []
		self.waitInput = 0
		self.currentIdHistory = 0
		self.history.append((copyGrid(self.grid), self.bboxManager.copy()))

		self.perfHistory = []

		self.displayBorder = False
		self.displayMixedBbox = False
		self.runTestSequence = False
		self.testSequenceId = 0


	def run(self):
		"""
		This method is the main loop of the game
		"""
		# Game loop
		while self.runMainLoop:
			self.input()
			self.tick()
			self.render()
			self.clock.tick(self.fps)


	def input(self):
		"""
		The method catch user's inputs, as key presse or a mouse click
		"""
		# We check each event
		for event in pg.event.get():
			# If the event it a click on the top right cross, we quit the game
			if event.type == pg.QUIT:
				self.quit()

		self.keyboardState = pg.key.get_pressed()
		self.mouseState = pg.mouse.get_pressed()
		self.mousePos = pg.mouse.get_pos()

		# Press espace to quit
		if self.keyboardState[pg.K_ESCAPE]:
			self.quit()


	def tick(self):
		"""
		This is the method where all calculations will be done
		"""
		tmp = time.time()
		delta = tmp - self.last
		self.last = tmp

		if self.waitInput > 0:
			self.waitInput = max(0, self.waitInput - delta)

		grid_x_left = OFFSET[0]
		grid_y_left = OFFSET[1]
		grid_x_right = grid_x_left + GRID_W
		grid_y_right = grid_y_left + GRID_H
		mouse_x = self.mousePos[0]
		mouse_y = self.mousePos[1]

		# Over
		if mouse_x >= grid_x_left and mouse_x <= grid_x_right\
			and mouse_y >= grid_y_left and mouse_y <= grid_y_right:
			tile_x = int(mouse_x - grid_x_left) // TILE_SIZE
			tile_y = int(mouse_y - grid_y_left) // TILE_SIZE

			# Place tile
			if self.mouseState[0] and self.grid[tile_y][tile_x] == TILE_EMPTY:
				self.grid[tile_y][tile_x] = TILE_FULL
				perf = self.bboxManager.update(tile_x, tile_y)
				while len(self.history) > self.currentIdHistory + 1:
					self.history.pop()
				self.history.append((copyGrid(self.grid), self.bboxManager.copy()))
				self.currentIdHistory += 1

				if perf[1] == 0:
					timePerBbox = perf[0]
				else:
					timePerBbox = perf[0] / perf[1]

				typeMove = '='
				if perf[1] < perf[2]:
					typeMove = '+'
				elif perf[1] > perf[2]:
					typeMove = '-'

				print(f"took {perf[0]:6.2f} us for {perf[1]:02}->{perf[2]:02} bbox ({timePerBbox:3.2f} us per bbox)")
				self.perfHistory.append((perf[0], timePerBbox, typeMove))

		# Clear grid
		if self.keyboardState[pg.K_SPACE]:
			self.grid = []
			for _ in range(GRID_SIZE):
				self.grid.append([TILE_EMPTY] * GRID_SIZE)

			self.bboxManager.clear()
			self.history.clear()
			self.currentIdHistory = 0
			self.history.append((copyGrid(self.grid), self.bboxManager.copy()))
			print("\n\n\n")

		# Go back in history
		if self.keyboardState[pg.K_LEFT] and self.waitInput == 0\
				and self.currentIdHistory > 0:
			self.currentIdHistory -= 1
			savedData = self.history[self.currentIdHistory]
			self.grid = copyGrid(savedData[0])
			self.bboxManager = savedData[1].copy()
			self.waitInput = 0.1

		# Go forward in history
		elif self.keyboardState[pg.K_RIGHT] and self.waitInput == 0\
				and self.currentIdHistory < len(self.history) - 1:
			self.currentIdHistory += 1
			savedData = self.history[self.currentIdHistory]
			self.grid = copyGrid(savedData[0])
			self.bboxManager = savedData[1].copy()
			self.waitInput = 0.1

		if self.keyboardState[pg.K_UP] and self.waitInput == 0:
			self.displayBorder = not self.displayBorder
			self.waitInput = 0.2

		if self.keyboardState[pg.K_d] and self.waitInput == 0:
			self.displayMixedBbox = not self.displayMixedBbox
			self.waitInput = 0.2

		if self.keyboardState[pg.K_p] and self.waitInput == 0:
			self.bboxManager.printBbox()
			self.waitInput = 0.2

		if self.keyboardState[pg.K_DOWN]:
			print()

		if self.keyboardState[pg.K_t] and self.waitInput == 0:
			self.runTestSequence = not self.runTestSequence
			self.displayBorder = self.runTestSequence
			self.waitInput = 0.2

		if self.runTestSequence and self.waitInput == 0:
			if self.testSequenceId < len(testSequence):
				x, y = testSequence[self.testSequenceId]
				self.testSequenceId += 1
				self.grid[y][x] = TILE_FULL
				perf = self.bboxManager.update(x, y)
				self.waitInput = 0.02
				while len(self.history) > self.currentIdHistory + 1:
					self.history.pop()
				self.history.append((copyGrid(self.grid), self.bboxManager.copy()))
				self.currentIdHistory += 1

				if perf[1] == 0:
					timePerBbox = perf[0]
				else:
					timePerBbox = perf[0] / perf[1]

				typeMove = '='
				if perf[1] < perf[2]:
					typeMove = '+'
				elif perf[1] > perf[2]:
					typeMove = '-'

				# print(f"took {perf[0]:6.2f} us for {perf[1]:02}->{perf[2]:02} bbox ({timePerBbox:3.2f} us per bbox)")
				self.perfHistory.append((perf[0], timePerBbox, typeMove))
			else:
				self.runTestSequence = False

		pg.display.set_caption(str(self.clock.get_fps()))


	def render(self):
		"""
		This is the method where all graphic update will be done
		"""
		# We clean our screen with one color
		self.win.fill((0, 0, 0))

		drawText(self.win, f"Nb bboxes : {self.bboxManager.nbBoxes}",
					(10, 10), (255, 255, 255))

		lst = self.bboxManager.getListPosition()

		for i in range(GRID_SIZE):
			drawText(self.win,
					str(i),
					(((i + 0.5) * TILE_SIZE + OFFSET[0]),
					((-0.5) * TILE_SIZE + OFFSET[1])),
					(250, 250, 250),
					24, None, "center")
			drawText(self.win,
					str(i),
					(((-0.5) * TILE_SIZE + OFFSET[0]),
					((i + 0.5) * TILE_SIZE + OFFSET[1])),
					(250, 250, 250),
					24, None, "center")

		for y in range(GRID_SIZE):
			for x in range(GRID_SIZE):

				if self.displayMixedBbox:
					if (x, y) in lst:
						colors = TILE_COLORS_IN_BBOX
					else:
						colors = TILE_COLORS
				else:
					colors = self.bboxManager.getColors(x, y)
				pg.draw.rect(self.win, colors[self.grid[y][x]],
								(x * TILE_SIZE + OFFSET[0],
								y * TILE_SIZE + OFFSET[1],
								TILE_BG_SIZE, TILE_BG_SIZE))

				count = self.bboxManager.countBboxCover(x, y)
				if count > 1:
					drawText(self.win, str(count),
			  				((x + 0.5) * TILE_SIZE + OFFSET[0],
		  					(y + 0.5) * TILE_SIZE + OFFSET[1]),
							(250, 250, 250),
							24, None, "center")

		if self.displayBorder:
			self.bboxManager.draw(self.win)

		# We update the drawing.
		# Before the function call, any changes will be not visible
		pg.display.update()


	def quit(self):
		"""
		This is the quit method
		"""
		sumTime = 0
		sumTimePerAddBbox = 0
		nbAddBbox = 0
		sumTimePerRemoveBbox = 0
		nbRemoveBbox = 0
		sumTimePerBbox = 0

		for perf in self.perfHistory:
			sumTime += perf[0]
			sumTimePerBbox += perf[1]

			if perf[2] == '+':
				sumTimePerAddBbox += perf[0]
				nbAddBbox += 1
			if perf[2] == '-':
				sumTimePerRemoveBbox += perf[0]
				nbRemoveBbox += 1

		nbMove = len(self.perfHistory)
		print(f"\n\nTime on average of {nbMove} moves :")
		if nbMove != 0:
			print(f" - general : {sumTime / nbMove:.2f} us")
			print(f" - per bbox : {sumTimePerBbox / nbMove:.2f} us")
		else:
			print(f" - general : -nan us")
			print(f" - per bbox : -nan us")
		if nbAddBbox != 0:
			print(f" - per add bbox : {sumTimePerAddBbox / nbAddBbox:.2f} us")
		else:
			print(f" - per add bbox : -nan us")
		if nbRemoveBbox != 0:
			print(f" - per remove bbox : {sumTimePerRemoveBbox / nbRemoveBbox:.2f} us")
		else:
			print(f" - per remove bbox : -nan us")

		# Pygame quit
		pg.quit()
		sys.exit()

# generateNewTestSequence(150)

Game().run() # Start game
