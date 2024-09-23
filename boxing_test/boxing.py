import pygame as pg
import math
import time
import sys

from define import	SCREEN_SIZE, OFFSET, GRID_SIZE, GRID_W, GRID_H,\
					TILE_SIZE, TILE_BG_SIZE, TILE_EMPTY, TILE_FULL
# from bboxManager1 import BboxManager
from bboxManager2 import BboxManager
# from bboxManager3 import BboxManager


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

		self.displayBorder = False


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
				self.bboxManager.update(tile_x, tile_y)
				while len(self.history) > self.currentIdHistory + 1:
					self.history.pop()
				self.history.append((copyGrid(self.grid), self.bboxManager.copy()))
				self.currentIdHistory += 1

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

		if self.keyboardState[pg.K_DOWN]:
			print()

		pg.display.set_caption(str(self.clock.get_fps()))


	def render(self):
		"""
		This is the method where all graphic update will be done
		"""
		# We clean our screen with one color
		self.win.fill((0, 0, 0))

		drawText(self.win, f"Nb bboxes : {self.bboxManager.nbBoxes}",
					(10, 10), (255, 255, 255))

		for y in range(GRID_SIZE):
			for x in range(GRID_SIZE):
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
		# Pygame quit
		pg.quit()
		sys.exit()


Game().run() # Start game
