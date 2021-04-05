#pragma once
#include "SimplexNoise.h"
#include "Forge.h"
using namespace Forge;

struct Triangle
{
public:
	glm::vec3 Vertices[3];
	glm::vec2 UVs[3];
};

class Terrain
{
private:
	glm::vec3 m_Position;
	float m_SurfaceLevel;

public:
	Terrain(const glm::vec3& position, float surfaceLevel);

	Ref<Mesh> GenerateMesh(const glm::vec3& size, const glm::ivec3& resolution, float heightScale) const;
	Ref<Mesh> GeneratePointsMesh(const glm::vec3& size, const glm::ivec3& resolution, float heightScale) const;

private:
	std::vector<Triangle> MarchingCubes(const glm::vec3& size, const glm::ivec3& resolution, float heightScale) const;

};
