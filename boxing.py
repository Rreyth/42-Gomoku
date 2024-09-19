import pygame as pg
import math
import time
import sys

SCREEN_SIZE = (1280, 720)

TILE_EMPTY = 0
TILE_FULL = 1
TILE_SIZE = 33
TILE_BG_SIZE = 32
TILE_COLORS = [(100, 100, 100), (200, 200, 200)]

GRID_SIZE = 19
GRID_W = GRID_SIZE * TILE_SIZE
GRID_H = GRID_SIZE * TILE_SIZE

OFFSET = (SCREEN_SIZE[0] / 2 - GRID_W / 2,
			SCREEN_SIZE[1] / 2 - GRID_H / 2)


class Bbox:
	def __init__(self, x, y, colors):
		self.x = x
		self.y = y
		self.Rx = x
		self.Ry = y
		self.colors = colors


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


	def copy(self):
		bbox = Bbox(self.x, self.y, self.colors)
		bbox.Rx = self.Rx
		bbox.Ry = self.Ry
		return bbox



class Game:
	def __init__(self):
		"""
		This method define all variables needed by the program
		"""
		# Start of pygame
		pg.init()

		# We remove the toolbar of the window's height
		self.winSize = (SCREEN_SIZE)
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

		self.bbox = Bbox(-1, -1, [(100, 0, 0), (200, 0, 0)])


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

		grid_x_left = OFFSET[0]
		grid_y_left = OFFSET[1]
		grid_x_right = grid_x_left + GRID_W
		grid_y_right = grid_y_left + GRID_H
		mouse_x = self.mousePos[0]
		mouse_y = self.mousePos[1]

		# Over
		if (mouse_x >= grid_x_left and mouse_x <= grid_x_right
			and mouse_y >= grid_y_left and mouse_y <= grid_y_right):
			tile_x = int(mouse_x - grid_x_left) // TILE_SIZE
			tile_y = int(mouse_y - grid_y_left) // TILE_SIZE

			# Place tile
			if (self.mouseState[0] and self.grid[tile_y][tile_x] == TILE_EMPTY):
				self.grid[tile_y][tile_x] = TILE_FULL
				self.bbox.update(tile_x, tile_y)

			# Remove tile
			if (self.mouseState[2] and self.grid[tile_y][tile_x] == TILE_FULL):
				self.grid[tile_y][tile_x] = TILE_EMPTY

				self.bbox = Bbox(-1, -1, [(100, 0, 0), (200, 0, 0)])
				for y in range(GRID_SIZE):
					for x in range(GRID_SIZE):
						if (self.grid[y][x]):
							self.bbox.update(x, y)

		# Clear grid
		if (self.keyboardState[pg.K_SPACE]):
			self.grid = []
			for _ in range(GRID_SIZE):
				self.grid.append([TILE_EMPTY] * GRID_SIZE)

			self.bbox = Bbox(-1, -1, [(100, 0, 0), (200, 0, 0)])

		pg.display.set_caption(str(self.clock.get_fps()))


	def render(self):
		"""
		This is the method where all graphic update will be done
		"""
		# We clean our screen with one color
		self.win.fill((0, 0, 0))

		for y in range(GRID_SIZE):
			for x in range(GRID_SIZE):
				if (self.bbox.isInside(x, y)):
					colors = self.bbox.colors
				else:
					colors = TILE_COLORS

				pg.draw.rect(self.win, colors[self.grid[y][x]],
								(x * TILE_SIZE + OFFSET[0],
								y * TILE_SIZE + OFFSET[1],
								TILE_BG_SIZE, TILE_BG_SIZE))

		# We update the drawing.
		# Before the function call, any changes will be not visible
		pg.display.update()


	def quit(self):
		"""
		This is the quit method
		"""
		# Pygame quit
		pg.quit()
		sys.exit()


Game().run() # Start game
