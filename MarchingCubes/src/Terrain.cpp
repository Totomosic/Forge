#include "Terrain.h"
#include "MarchTables.h"

#include <iostream>

Terrain::Terrain(const glm::vec3& position, float surfaceLevel)
	: m_Position(position), m_SurfaceLevel(surfaceLevel)
{}

Ref<Mesh> Terrain::GenerateMesh(const glm::vec3& size, const glm::ivec3& resolution, float heightScale) const
{
	std::vector<Triangle> triangles = MarchingCubes(size, resolution, heightScale);

	uint32_t* indices = new uint32_t[triangles.size() * 3];
	for (size_t i = 0; i < triangles.size(); i++)
	{
		indices[i * 3 + 0] = i * 3 + 0;
		indices[i * 3 + 1] = i * 3 + 1;
		indices[i * 3 + 2] = i * 3 + 2;
	}

	Ref<VertexArray> vao = VertexArray::Create();
	BufferLayout layout = {
		{ ShaderDataType::Float3 },
		{ ShaderDataType::Float3 },
		{ ShaderDataType::Float2 },
	};

	float* vertexData = new float[triangles.size() * 3 * (3 + 3 + 2)];
	float* ptr = vertexData;

	for (size_t i = 0; i < triangles.size(); i++)
	{
		glm::vec3 a = triangles[i].Vertices[1] - triangles[i].Vertices[0];
		glm::vec3 b = triangles[i].Vertices[2] - triangles[i].Vertices[0];
		glm::vec3 normal = glm::normalize(glm::cross(a, b));
		for (int k = 0; k < 3; k++)
		{
			*ptr++ = triangles[i].Vertices[k].x;
			*ptr++ = triangles[i].Vertices[k].y;
			*ptr++ = triangles[i].Vertices[k].z;
			*ptr++ = normal.x;
			*ptr++ = normal.y;
			*ptr++ = normal.z;
			*ptr++ = triangles[i].UVs[k].x;
			*ptr++ = triangles[i].UVs[k].y;
		}
	}

	Ref<VertexBuffer> vbo = VertexBuffer::Create(vertexData, triangles.size() * 3 * layout.GetStride(), layout);
	Ref<IndexBuffer> ibo = IndexBuffer::Create(indices, triangles.size() * 3 * sizeof(uint32_t));

	delete[] vertexData;
	delete[] indices;

	vao->AddVertexBuffer(vbo);
	vao->SetIndexBuffer(ibo);

	return CreateRef<Mesh>(vao);
}

Ref<Mesh> Terrain::GeneratePointsMesh(const glm::vec3& size, const glm::ivec3& resolution, float heightScale) const
{
	std::vector<Triangle> triangles = MarchingCubes(size, resolution, heightScale);

	uint32_t* indices = new uint32_t[triangles.size() * 3];
	for (size_t i = 0; i < triangles.size(); i++)
	{
		indices[i * 3 + 0] = i * 3 + 0;
		indices[i * 3 + 1] = i * 3 + 1;
		indices[i * 3 + 2] = i * 3 + 2;
	}

	Ref<VertexArray> vao = VertexArray::Create();
	BufferLayout layout = {
		{ ShaderDataType::Float3 },
		{ ShaderDataType::Float3 },
		{ ShaderDataType::Float2 },
	};

	float* vertexData = new float[triangles.size() * 3 * (3 + 3 + 2)];
	float* ptr = vertexData;

	for (size_t i = 0; i < triangles.size(); i++)
	{
		glm::vec3 a = triangles[i].Vertices[1] - triangles[i].Vertices[0];
		glm::vec3 b = triangles[i].Vertices[2] - triangles[i].Vertices[0];
		glm::vec3 normal = glm::normalize(glm::cross(a, b));
		for (int k = 0; k < 3; k++)
		{
			*ptr++ = triangles[i].Vertices[k].x;
			*ptr++ = triangles[i].Vertices[k].y;
			*ptr++ = triangles[i].Vertices[k].z;
			*ptr++ = normal.x;
			*ptr++ = normal.y;
			*ptr++ = normal.z;
			*ptr++ = triangles[i].UVs[k].x;
			*ptr++ = triangles[i].UVs[k].y;
		}
	}

	Ref<VertexBuffer> vbo = VertexBuffer::Create(vertexData, triangles.size() * 3 * layout.GetStride(), layout);
	Ref<IndexBuffer> ibo = IndexBuffer::Create(indices, triangles.size() * 3 * sizeof(uint32_t));

	delete[] vertexData;
	delete[] indices;

	vao->AddVertexBuffer(vbo);
	vao->SetIndexBuffer(ibo);

	return CreateRef<Mesh>(vao);
}

std::vector<Triangle> Terrain::MarchingCubes(const glm::vec3& size, const glm::ivec3& resolution, float heightScale) const
{
	SimplexNoise simplex;
	int xPoints = resolution.x + 1;
	int yPoints = resolution.y + 1;
	int zPoints = resolution.z + 1;

	auto getIndex = [xPoints, yPoints, zPoints](int x, int y, int z)
	{
		return x + y * xPoints + z * xPoints * yPoints;
	};

	auto getNeighbours = [](int i, int j, int k, glm::ivec3 neighbours[8])
	{
		neighbours[0] = { i, j, k };
		neighbours[1] = { i + 1, j, k };
		neighbours[2] = { i + 1, j, k + 1 };
		neighbours[3] = { i, j, k + 1 };
		neighbours[4] = { i, j + 1, k };
		neighbours[5] = { i + 1, j + 1, k };
		neighbours[6] = { i + 1, j + 1, k + 1 };
		neighbours[7] = { i, j + 1, k + 1 };
	};

	float* marchingCubes = new float[size_t(xPoints) * size_t(yPoints) * size_t(zPoints)];
	float heightPerPoint = float(size.y) / float(resolution.y - 1);
	float scale = 50.0f;

	auto getPosition = [&](const glm::ivec3& pos)
	{
		return glm::vec4{ float(pos.x) / float(resolution.x) * size.x - size.x / 2, float(pos.y) / float(resolution.y) * size.y, float(pos.z) / float(resolution.z) * size.z - size.z / 2, marchingCubes[getIndex(pos.x, pos.y, pos.z)] };
	};

	for (int i = 0; i < xPoints; i++)
	{
		for (int j = 0; j < yPoints; j++)
		{
			for (int k = 0; k < zPoints; k++)
			{
				float noise = simplex.fractal(4, float(i) / scale + m_Position.x, float(j) / scale + m_Position.y, float(k) / scale + m_Position.z);
				float value = float(j) * -heightPerPoint + noise * heightScale;
				marchingCubes[getIndex(i, j, k)] = value;
			}
		}
	}

	auto calculateLookupIndex = [marchingCubes, &getIndex, this](glm::ivec3 neighbours[8])
	{
		uint32_t index = 0;
		for (int i = 0; i < 8; i++)
		{
			if (marchingCubes[getIndex(neighbours[i].x, neighbours[i].y, neighbours[i].z)] >= m_SurfaceLevel)
			{
				index |= (1 << i);
			}
		}
		return index;
	};

	auto interpolate = [this](const glm::vec4& a, const glm::vec4& b)
	{
		return glm::vec3(a) + (glm::vec3(b) - glm::vec3(a)) * (m_SurfaceLevel - a.w) / (b.w - a.w);
	};

	auto interpolateUVs = [](int a, int b)
	{
		glm::vec2 aUv = { 0.0f, 0.0f };
		glm::vec2 bUv = { 0.0f, 0.0f };
		return (aUv + bUv) / 2.0f;
	};

	std::vector<Triangle> triangles;
	for (int i = 0; i < xPoints - 1; i++)
	{
		for (int j = 0; j < yPoints - 1; j++)
		{
			for (int k = 0; k < zPoints - 1; k++)
			{
				glm::ivec3 neighbours[8];
				getNeighbours(i, j, k, neighbours);
				uint32_t lookupIndex = calculateLookupIndex(neighbours);

				for (int i = 0; triangulation[lookupIndex][i] != -1; i += 3)
				{
					int a0 = cornerIndexAFromEdge[triangulation[lookupIndex][i]];
					int b0 = cornerIndexBFromEdge[triangulation[lookupIndex][i]];

					int a1 = cornerIndexAFromEdge[triangulation[lookupIndex][i + 1]];
					int b1 = cornerIndexBFromEdge[triangulation[lookupIndex][i + 1]];

					int a2 = cornerIndexAFromEdge[triangulation[lookupIndex][i + 2]];
					int b2 = cornerIndexBFromEdge[triangulation[lookupIndex][i + 2]];

					Triangle triangle;

					triangle.Vertices[0] = interpolate(getPosition(neighbours[a0]), getPosition(neighbours[b0]));
					triangle.Vertices[1] = interpolate(getPosition(neighbours[a1]), getPosition(neighbours[b1]));
					triangle.Vertices[2] = interpolate(getPosition(neighbours[a2]), getPosition(neighbours[b2]));

					triangle.UVs[0] = interpolateUVs(a0, b0);
					triangle.UVs[1] = interpolateUVs(a1, b1);
					triangle.UVs[2] = interpolateUVs(a2, b2);
					triangles.push_back(triangle);
				}
			}
		}
	}

	delete[] marchingCubes;
	return triangles;
}
