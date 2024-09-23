SCREEN_SIZE = (1280, 720)
# SCREEN_SIZE = (1920, 1080)

TILE_EMPTY = 0
TILE_FULL = 1
TILE_SIZE = 33
# TILE_SIZE = 51
TILE_BG_SIZE = TILE_SIZE - 1
TILE_COLORS = [(100, 100, 100), (255, 255, 255)]

GRID_SIZE = 19
GRID_W = GRID_SIZE * TILE_SIZE
GRID_H = GRID_SIZE * TILE_SIZE

OFFSET = (SCREEN_SIZE[0] / 2 - GRID_W / 2,
			SCREEN_SIZE[1] / 2 - GRID_H / 2)

COLORS_PANELS = [
	((100, 0, 0), (200, 0, 0), "Red"),
	((0, 100, 0), (0, 200, 0), "Green"),
	((0, 0, 100), (0, 0, 200), "Blue"),
	((100, 100, 0), (200, 200, 0), "Yellow"),
	((0, 100, 100), (0, 200, 200), "Cyan"),
	((100, 0, 100), (200, 0, 200), "Purple"),
	((175, 47, 0), (255, 127, 0), "Orange"),
	((139, 69, 19), (239, 169, 119), "Brown"),
	((50, 50, 50), (0, 0, 0), "Black"),
]
COLORS_PANELS_NB = len(COLORS_PANELS)
