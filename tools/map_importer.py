#!/usr/bin/env python3

import re
import os
import sys
from pathlib import Path

def extract_map_data(content):
    # Find the map array definition
    map_match = re.search(r'int map\[MAP_HEIGHT\]\[MAP_WIDTH\]\s*=\s*{([^}]+)};', content)
    if not map_match:
        return None
    return map_match.group(1).strip()

def extract_enemy_data(content):
    # Find the enemies array definition
    enemy_match = re.search(r'Enemy enemies\[MAX_ENEMIES\]\s*=\s*{([^}]+)};', content)
    if not enemy_match:
        return None
    return enemy_match.group(1).strip()

def extract_dimensions(content):
    # Find MAP_WIDTH and MAP_HEIGHT definitions
    width_match = re.search(r'#define MAP_WIDTH\s+(\d+)', content)
    height_match = re.search(r'#define MAP_HEIGHT\s+(\d+)', content)
    if not width_match or not height_match:
        return None, None
    return width_match.group(1), height_match.group(1)

def update_map_h(map_h_path, width, height):
    if not width or not height:
        return False
    
    with open(map_h_path, 'r') as f:
        content = f.read()
    
    # Replace dimensions
    content = re.sub(r'#define MAP_WIDTH\s+\d+', f'#define MAP_WIDTH {width}', content)
    content = re.sub(r'#define MAP_HEIGHT\s+\d+', f'#define MAP_HEIGHT {height}', content)
    
    with open(map_h_path, 'w') as f:
        f.write(content)
    return True

def update_map_c(map_c_path, map_data):
    if not map_data:
        return False
        
    with open(map_c_path, 'r') as f:
        content = f.read()
    
    # Replace map array initialization
    new_content = re.sub(
        r'int map\[MAP_HEIGHT\]\[MAP_WIDTH\]\s*=\s*{[^}]+};',
        f'int map[MAP_HEIGHT][MAP_WIDTH] = {{\n{map_data}\n}};',
        content
    )
    
    with open(map_c_path, 'w') as f:
        f.write(new_content)
    return True

def update_enemy_c(enemy_c_path, enemy_data):
    if not enemy_data:
        return True  # Not having enemy data is okay
        
    with open(enemy_c_path, 'r') as f:
        content = f.read()
    
    # Find and replace the enemies initialization if it exists
    if 'Enemy enemies[MAX_ENEMIES] = {' in content:
        new_content = re.sub(
            r'Enemy enemies\[MAX_ENEMIES\]\s*=\s*{[^}]+};',
            f'Enemy enemies[MAX_ENEMIES] = {{\n{enemy_data}\n}};',
            content
        )
        
        with open(enemy_c_path, 'w') as f:
            f.write(new_content)
    return True

def main():
    if len(sys.argv) != 2:
        print("Usage: ./map_importer.py <generated_code.txt>")
        sys.exit(1)

    input_file = sys.argv[1]
    if not os.path.exists(input_file):
        print(f"Error: File {input_file} not found!")
        sys.exit(1)

    # Read the generated code
    with open(input_file, 'r') as f:
        content = f.read()

    # Get project root directory (up two levels from script location)
    project_root = Path(__file__).parent.parent
    
    # Define file paths
    map_h_path = project_root / "include" / "map.h"
    map_c_path = project_root / "src" / "map.c"
    enemy_c_path = project_root / "src" / "enemy.c"

    # Extract data
    width, height = extract_dimensions(content)
    map_data = extract_map_data(content)
    enemy_data = extract_enemy_data(content)

    # Validate paths
    for path in [map_h_path, map_c_path, enemy_c_path]:
        if not path.exists():
            print(f"Error: Required file {path} not found!")
            sys.exit(1)

    # Update files
    if not update_map_h(map_h_path, width, height):
        print("Error: Failed to update map.h!")
        sys.exit(1)

    if not update_map_c(map_c_path, map_data):
        print("Error: Failed to update map.c!")
        sys.exit(1)

    if not update_enemy_c(enemy_c_path, enemy_data):
        print("Error: Failed to update enemy.c!")
        sys.exit(1)

    print("Map data successfully imported!")
    print(f"Updated map dimensions: {width}x{height}")
    if enemy_data:
        print("Enemy data imported")

if __name__ == "__main__":
    main()
