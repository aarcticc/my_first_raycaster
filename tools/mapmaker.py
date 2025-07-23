#!/usr/bin/env python3
import tkinter as tk
from tkinter import filedialog
import json
import os

class MapMaker:
    def __init__(self, root):
        self.root = root
        self.root.title("Raycaster Map Maker")
        
        # Map dimensions
        self.MAP_WIDTH = 20
        self.MAP_HEIGHT = 20
        self.CELL_SIZE = 30
        
        # Initialize empty map
        self.map_data = [[0 for _ in range(self.MAP_WIDTH)] for _ in range(self.MAP_HEIGHT)]
        # Set borders to walls (1)
        for x in range(self.MAP_WIDTH):
            self.map_data[0][x] = 1
            self.map_data[self.MAP_HEIGHT-1][x] = 1
        for y in range(self.MAP_HEIGHT):
            self.map_data[y][0] = 1
            self.map_data[y][self.MAP_WIDTH-1] = 1
        
        # Create toolbar
        self.toolbar = tk.Frame(root)
        self.toolbar.pack(side="top", fill="x")
        
        # Create tools
        tk.Button(self.toolbar, text="Save", command=self.save_map).pack(side="left")
        tk.Button(self.toolbar, text="Load", command=self.load_map).pack(side="left")
        tk.Button(self.toolbar, text="Clear", command=self.clear_map).pack(side="left")
        
        # Create canvas
        self.canvas = tk.Canvas(root, 
                              width=self.MAP_WIDTH*self.CELL_SIZE,
                              height=self.MAP_HEIGHT*self.CELL_SIZE)
        self.canvas.pack()
        
        # Bind mouse events
        self.canvas.bind("<Button-1>", self.on_click)
        self.canvas.bind("<B1-Motion>", self.on_drag)
        self.canvas.bind("<Button-3>", self.on_right_click)
        self.canvas.bind("<B3-Motion>", self.on_right_drag)
        
        # Draw initial grid
        self.draw_grid()
    
    def draw_grid(self):
        self.canvas.delete("all")
        for y in range(self.MAP_HEIGHT):
            for x in range(self.MAP_WIDTH):
                x1 = x * self.CELL_SIZE
                y1 = y * self.CELL_SIZE
                x2 = x1 + self.CELL_SIZE
                y2 = y1 + self.CELL_SIZE
                
                # Different colors for different wall types
                if self.map_data[y][x] == 0:
                    color = "white"
                else:
                    color = f"#{self.map_data[y][x]}00000"
                
                self.canvas.create_rectangle(x1, y1, x2, y2, 
                                          fill=color, outline="gray")
    
    def on_click(self, event):
        x = event.x // self.CELL_SIZE
        y = event.y // self.CELL_SIZE
        if 0 <= x < self.MAP_WIDTH and 0 <= y < self.MAP_HEIGHT:
            if x in (0, self.MAP_WIDTH-1) or y in (0, self.MAP_HEIGHT-1):
                return  # Don't allow modifying borders
            current = self.map_data[y][x]
            self.map_data[y][x] = (current + 1) % 5 or 1  # Cycle through 1-4
            self.draw_grid()
    
    def on_right_click(self, event):
        x = event.x // self.CELL_SIZE
        y = event.y // self.CELL_SIZE
        if 0 <= x < self.MAP_WIDTH and 0 <= y < self.MAP_HEIGHT:
            if x in (0, self.MAP_WIDTH-1) or y in (0, self.MAP_HEIGHT-1):
                return  # Don't allow modifying borders
            self.map_data[y][x] = 0
            self.draw_grid()
    
    def on_drag(self, event):
        self.on_click(event)
    
    def on_right_drag(self, event):
        self.on_right_click(event)
    
    def save_map(self):
        filename = filedialog.asksaveasfilename(
            defaultextension=".json",
            initialdir="maps",
            title="Save Map",
            filetypes=[("JSON files", "*.json")]
        )
        if filename:
            with open(filename, 'w') as f:
                json.dump({"grid": self.map_data}, f, indent=2)
    
    def load_map(self):
        filename = filedialog.askopenfilename(
            initialdir="maps",
            title="Load Map",
            filetypes=[("JSON files", "*.json")]
        )
        if filename:
            with open(filename, 'r') as f:
                data = json.load(f)
                self.map_data = data["grid"]
                self.draw_grid()
    
    def clear_map(self):
        for y in range(1, self.MAP_HEIGHT-1):
            for x in range(1, self.MAP_WIDTH-1):
                self.map_data[y][x] = 0
        self.draw_grid()

if __name__ == "__main__":
    root = tk.Tk()
    app = MapMaker(root)
    root.mainloop()
