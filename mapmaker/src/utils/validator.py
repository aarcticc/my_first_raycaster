def validate_map_structure(map_data):
    if not isinstance(map_data, dict):
        return False, "Map data must be a dictionary."

    if "grid" not in map_data:
        return False, "Map data must contain a 'grid' key."

    grid = map_data["grid"]

    if not isinstance(grid, list):
        return False, "'grid' must be a list."

    if not all(isinstance(row, list) for row in grid):
        return False, "Each row in 'grid' must be a list."

    row_length = len(grid[0])
    if not all(len(row) == row_length for row in grid):
        return False, "All rows in 'grid' must have the same length."

    return True, "Map structure is valid."


def validate_map_borders(map_data):
    grid = map_data["grid"]
    width = len(grid[0])
    height = len(grid)

    if any(cell != 1 for cell in grid[0]) or any(cell != 1 for cell in grid[height - 1]):
        return False, "Top and bottom borders must be solid walls."

    for row in grid:
        if row[0] != 1 or row[width - 1] != 1:
            return False, "Left and right borders must be solid walls."

    return True, "Map borders are valid."


def validate_map(map_data):
    structure_valid, structure_message = validate_map_structure(map_data)
    if not structure_valid:
        return structure_message

    borders_valid, borders_message = validate_map_borders(map_data)
    if not borders_valid:
        return borders_message

    return "Map is valid."