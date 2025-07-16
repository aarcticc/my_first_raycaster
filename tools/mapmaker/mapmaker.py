import pygame
import json
import os

# Constants
CELL_SIZE = 32
MAP_WIDTH = 20
MAP_HEIGHT = 20
WINDOW_WIDTH = MAP_WIDTH * CELL_SIZE + 200  # Extra space for UI
WINDOW_HEIGHT = MAP_HEIGHT * CELL_SIZE
NUM_TEXTURES = 4

# Colors
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
GRAY = (128, 128, 128)

class MapMaker:
    def __init__(self):
        pygame.init()
        self.screen = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
        pygame.display.set_caption("Raycaster Map Maker")
        
        # Initialize map
        self.map = [[0 for _ in range(MAP_WIDTH)] for _ in range(MAP_HEIGHT)]
        # Set borders
        for x in range(MAP_WIDTH):
            self.map[0][x] = 1
            self.map[MAP_HEIGHT-1][x] = 1
        for y in range(MAP_HEIGHT):
            self.map[y][0] = 1
            self.map[y][MAP_WIDTH-1] = 1
            
        self.current_texture = 1
        self.enemies = []
        self.mode = "wall"  # wall or enemy
        self.font = pygame.font.Font(None, 24)

    def draw_grid(self):
        for y in range(MAP_HEIGHT):
            for x in range(MAP_WIDTH):
                rect = pygame.Rect(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE)
                pygame.draw.rect(self.screen, WHITE, rect, 1)
                
                # Draw walls
                if self.map[y][x] > 0:
                    color = (50 * self.map[y][x], 50 * self.map[y][x], 128)
                    pygame.draw.rect(self.screen, color, rect)
                    
        # Draw enemies
        for ex, ey in self.enemies:
            pos = (int(ex * CELL_SIZE + CELL_SIZE/2), int(ey * CELL_SIZE + CELL_SIZE/2))
            pygame.draw.circle(self.screen, RED, pos, CELL_SIZE//3)

    def draw_ui(self):
        # UI panel
        panel = pygame.Rect(MAP_WIDTH * CELL_SIZE, 0, 200, WINDOW_HEIGHT)
        pygame.draw.rect(self.screen, GRAY, panel)
        
        # Mode selection
        wall_text = self.font.render(f"Mode: {self.mode}", True, WHITE)
        self.screen.blit(wall_text, (MAP_WIDTH * CELL_SIZE + 10, 10))
        
        # Current texture
        if self.mode == "wall":
            tex_text = self.font.render(f"Texture: {self.current_texture}", True, WHITE)
            self.screen.blit(tex_text, (MAP_WIDTH * CELL_SIZE + 10, 40))

    def save_map(self, filename):
        data = {
            'map': self.map,
            'enemies': self.enemies
        }
        with open(filename, 'w') as f:
            json.dump(data, f)

    def load_map(self, filename):
        if os.path.exists(filename):
            with open(filename, 'r') as f:
                data = json.load(f)
                self.map = data['map']
                self.enemies = data['enemies']

    def run(self):
        running = True
        while running:
            self.screen.fill(BLACK)
            
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False
                    
                elif event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_s:
                        self.save_map("map.json")
                    elif event.key == pygame.K_l:
                        self.load_map("map.json")
                    elif event.key == pygame.K_SPACE:
                        self.mode = "wall" if self.mode == "enemy" else "enemy"
                    elif event.key in [pygame.K_1, pygame.K_2, pygame.K_3, pygame.K_4]:
                        self.current_texture = int(event.unicode)
                        
                elif event.type == pygame.MOUSEBUTTONDOWN:
                    x, y = event.pos
                    map_x = x // CELL_SIZE
                    map_y = y // CELL_SIZE
                    
                    if map_x < MAP_WIDTH and map_y < MAP_HEIGHT:
                        if self.mode == "wall":
                            if event.button == 1:  # Left click
                                self.map[map_y][map_x] = self.current_texture
                            elif event.button == 3:  # Right click
                                self.map[map_y][map_x] = 0
                        elif self.mode == "enemy":
                            if event.button == 1:
                                self.enemies.append([map_x + 0.5, map_y + 0.5])
                            elif event.button == 3:
                                # Remove closest enemy
                                click_pos = (map_x + 0.5, map_y + 0.5)
                                min_dist = float('inf')
                                remove_idx = -1
                                for i, (ex, ey) in enumerate(self.enemies):
                                    dist = ((ex - click_pos[0])**2 + (ey - click_pos[1])**2)**0.5
                                    if dist < min_dist:
                                        min_dist = dist
                                        remove_idx = i
                                if remove_idx >= 0 and min_dist < 1:
                                    self.enemies.pop(remove_idx)
            
            self.draw_grid()
            self.draw_ui()
            pygame.display.flip()
            
        pygame.quit()

if __name__ == "__main__":
    MapMaker().run()
