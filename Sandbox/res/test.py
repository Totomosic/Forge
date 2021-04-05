import json

with open("Dragon - Copy/scene.gltf", "r") as f:
    data = json.load(f)
    for node in data["nodes"]:
        if "rotation" in node:
            del node["rotation"]
        if "translation" in node:
            del node["translation"]
        if "scale" in node:
            del node["scale"]
    
with open("Dragon - Copy/scene.gltf", "w") as f:
    json.dump(data, f, indent=4)