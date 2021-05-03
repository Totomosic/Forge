#pragma once
#include "Forge.h"
using namespace Forge;
#include <complex>
#include <random>

struct OceanVertex
{
public:
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Tilde0;
	glm::vec3 Tilde0mk;
	glm::vec3 OriginalPosition;
};

struct ComplexVectorNormal
{
public:
	std::complex<float> Height;
	glm::vec2 Displacement;
	glm::vec3 Normal;
};

inline std::complex<float> Conjugate(const std::complex<float>& a)
{
	return std::complex<float>(a.real(), -a.imag());
}

class FFT
{
private:
	uint32_t m_Dimension;
	int m_Which;
	int m_Log2N;
	float m_Pi2;
	std::unique_ptr<uint32_t[]> m_Reversed;
	std::unique_ptr<std::unique_ptr<std::complex<float>[]>[]> m_T;
	std::unique_ptr<std::complex<float>[]> m_C[2];

public:
	FFT(uint32_t dimension);

	uint32_t Reverse(uint32_t i) const;
	std::complex<float> T(uint32_t x, uint32_t n);
	void FastFourierTransform(const std::complex<float>* input, std::complex<float>* output, int stride, int offset);
};

class Ocean
{
private:
	float m_Gravity;
	int m_Dimension;
	int m_DimensionPlusOne;
	float m_PhillipsAmplitude;
	glm::vec2 m_Wind;
	float m_Length;

	std::mt19937 m_Random;
	FFT m_FFT;

	std::unique_ptr<std::complex<float>[]> m_HTilde;
	std::unique_ptr<std::complex<float>[]> m_HTildeSlopeX;
	std::unique_ptr<std::complex<float>[]> m_HTildeSlopeZ;
	std::unique_ptr<std::complex<float>[]> m_HTildeDx;
	std::unique_ptr<std::complex<float>[]> m_HTildeDz;

	Ref<VertexArray> m_VAO;
	Ref<VertexBuffer> m_VBO;
	Ref<IndexBuffer> m_IBO;
	Ref<Mesh> m_Mesh;

	std::unique_ptr<OceanVertex[]> m_Vertices;
	std::unique_ptr<uint32_t[]> m_Indices;
	size_t m_IndexCount;

public:
	Ocean(int dimension, float amplitude, const glm::vec2& wind, float length);

	inline Ref<Mesh> GetMesh() const { return m_Mesh; }

	float Dispersion(int n, int m);
	float Phillips(int n, int m);
	std::complex<float> HTilde0(int n, int m);
	std::complex<float> HTilde(float t, int n, int m);
	ComplexVectorNormal HeightDisplacementNormal(const glm::vec2& x, float t);
	void EvaluateWaves(float t);
	void EvaluateWavesFFT(float t);
};
