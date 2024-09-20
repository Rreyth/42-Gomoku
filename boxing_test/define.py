# SCREEN_SIZE = (1280, 720)
SCREEN_SIZE = (1920, 1080)

TILE_EMPTY = 0
TILE_FULL = 1
# TILE_SIZE = 33
TILE_SIZE = 51
TILE_BG_SIZE = TILE_SIZE - 1
TILE_COLORS = [(100, 100, 100), (200, 200, 200)]

GRID_SIZE = 19
GRID_W = GRID_SIZE * TILE_SIZE
GRID_H = GRID_SIZE * TILE_SIZE

OFFSET = (SCREEN_SIZE[0] / 2 - GRID_W / 2,
			SCREEN_SIZE[1] / 2 - GRID_H / 2)

COLORS_PANELS = [
	((100, 0, 0), (200, 0, 0)),
	((0, 100, 0), (0, 200, 0)),
	((0, 0, 100), (0, 0, 200)),
	((100, 100, 0), (200, 200, 0)),
	((0, 100, 100), (0, 200, 200)),
	((100, 0, 100), (200, 0, 200)),
	((100, 50, 0), (200, 150, 0)),
	((0, 100, 50), (0, 200, 150)),
	((50, 0, 100), (150, 0, 200)),
]
COLORS_PANELS_NB = len(COLORS_PANELS)
