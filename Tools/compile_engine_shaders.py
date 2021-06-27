import os

BASE_PATH = "../Forge/src/Assets/Shaders/"
LIBRARY_BASE_PATH = "../Forge/src/Renderer/Shaders/"

SHADER_FILES = [
    ["LitColor.shader", "LitColor.h"],
    ["LitTexture.shader", "LitTexture.h"],
    ["DefaultColorAnimated.shader", "DefaultColorAnimated.h"],
    ["LitTextureAnimated.shader", "LitTextureAnimated.h"],
    ["DefaultShadow.shader", "DefaultShadow.h"],
    ["DefaultPointShadow.shader", "DefaultPointShadow.h"],
    ["DefaultPick.shader", "DefaultPick.h"],
    ["PBRColor.shader", "PBRColor.h"],
    ["PBRTexture.shader", "PBRTexture.h"],

    ["PostProcessing/Bloom.shader", "PostProcessing/Bloom.h"],
    ["PostProcessing/BloomCombine.shader", "PostProcessing/BloomCombine.h"],
    ["PostProcessing/Blur.shader", "PostProcessing/Blur.h"],
    ["PostProcessing/HDR.shader", "PostProcessing/HDR.h"],
    ["PostProcessing/Dither.shader", "PostProcessing/Dither.h"],
]

SHADER_LIBRARY_FILES = [
    ["LightingUtils.shader", "LightingUtils.h"],
    ["Lighting.shader", "Lighting.h"],
    ["Shadows.shader", "Shadows.h"],
    ["Clipping.shader", "Clipping.h"],
    ["PBRUtils.shader", "PBRUtils.h"],
    ["PBR.shader", "PBR.h"],
    ["Constants.shader", "Constants.h"],
]

SHADER_TYPE_MAP = {
    "vertex": "vertexShaderSource",
    "geometry": "geometryShaderSource",
    "fragment": "fragmentShaderSource",
}

def parse_single_shader(source_lines: list[str], line_endings: str) -> str:
    result = "{}".format(line_endings)
    for line in source_lines:
        result += "\"{}\\n\"{}".format(line.replace('"', "\\\"").rstrip(), line_endings)
    return result

def parse_shader_source(source_lines: list[str]) -> str:
    line_groups = []
    current_line_group = None
    for line in source_lines:
        if line.startswith("#shader"):
            shader_type = line[len("#shader "):].strip().lower()
            if shader_type in SHADER_TYPE_MAP:
                current_line_group = (SHADER_TYPE_MAP[shader_type], [])
                line_groups.append(current_line_group)
        elif current_line_group is not None:
            current_line_group[1].append(line)
    result = ""
    for group in line_groups:
        result += "std::string {} ={};\n".format(group[0], parse_single_shader(group[1], "\n\t")[:-2])
    return result

if __name__ == "__main__":
    for shader_file, header_file in SHADER_FILES:
        full_shader_file = os.path.join(BASE_PATH, shader_file)
        full_header_file = os.path.join(BASE_PATH, header_file)
        with open(full_shader_file, "r") as f:
            lines = f.readlines()
            with open(full_header_file, "w") as w:
                w.write(parse_shader_source(lines))

    for shader_file, header_file in SHADER_LIBRARY_FILES:
        full_shader_file = os.path.join(LIBRARY_BASE_PATH, shader_file)
        full_header_file = os.path.join(LIBRARY_BASE_PATH, header_file)
        with open(full_shader_file, "r") as f:
            lines = f.readlines()
            with open(full_header_file, "w") as w:
                w.write(parse_single_shader(lines, "\n")[1:])
