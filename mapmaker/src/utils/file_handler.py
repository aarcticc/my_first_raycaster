def load_map(file_path):
    import json
    with open(file_path, 'r') as file:
        return json.load(file)

def save_map(file_path, map_data):
    import json
    with open(file_path, 'w') as file:
        json.dump(map_data, file, indent=4)