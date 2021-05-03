#include "Ocean.h"

FFT::FFT(uint32_t dimension)
	: m_Dimension(dimension), m_Pi2(PI * PI), m_Log2N(log(dimension) / log(2))
{
	m_Reversed = std::make_unique<uint32_t[]>(m_Dimension);
	for (int i = 0; i < m_Dimension; i++)
		m_Reversed[i] = Reverse(uint32_t(i));

	int pow2 = 1;
	m_T = std::make_unique<std::unique_ptr<std::complex<float>[]>[]>(m_Log2N);
	for (int i = 0; i < m_Log2N; i++)
	{
		m_T[i] = std::make_unique<std::complex<float>[]>(pow2);
		for (int j = 0; j < pow2; j++)
		{
			m_T[i][j] = T(j, pow2 * 2);
		}
		pow2 *= 2;
	}

	m_C[0] = std::make_unique<std::complex<float>[]>(m_Dimension);
	m_C[1] = std::make_unique<std::complex<float>[]>(m_Dimension);
	m_Which = 0;
}

uint32_t FFT::Reverse(uint32_t i) const
{
	uint32_t res = 0;
	for (int j = 0; j < m_Log2N; j++)
	{
		res = (res << 1) + (i & 1);
		i >>= 1;
	}
	return res;
}

std::complex<float> FFT::T(uint32_t x, uint32_t n)
{
	return { cos(m_Pi2 * x / n), sin(m_Pi2 * x / n) };
}

void FFT::FastFourierTransform(const std::complex<float>* input, std::complex<float>* output, int stride, int offset)
{
	for (int i = 0; i < m_Dimension; i++)
	{
		m_C[m_Which][i] = input[m_Reversed[i] * stride + offset];
	}
	int loops = m_Dimension >> 1;
	int size = 1 << 1;
	int sizeOver2 = 1;
	int w = 0;

	for (int i = 1; i <= m_Log2N; i++)
	{
		m_Which ^= 1;
		for (int j = 0; j < loops; j++)
		{
			for (int k = 0; k < sizeOver2; k++)
			{
				m_C[m_Which][size * j + k] = m_C[m_Which ^ 1][size * j + k] + m_C[m_Which ^ 1][size * j + sizeOver2 + k] * m_T[w][k];
			}
			for (int k = sizeOver2; k < size; k++)
			{
				m_C[m_Which][size * j + k] = m_C[m_Which ^ 1][size * j - sizeOver2 + k] - m_C[m_Which ^ 1][size * j + k] * m_T[w][k - sizeOver2];
			}
		}
		loops >>= 1;
		size <<= 1;
		sizeOver2 <<= 1;
		w++;
	}
	for (int i = 0; i < m_Dimension; i++)
	{
		output[i * stride + offset] = m_C[m_Which][i];
	}
}

Ocean::Ocean(int dimension, float amplitude, const glm::vec2& wind, float length)
	: m_Gravity(9.81f), m_Dimension(dimension), m_DimensionPlusOne(dimension + 1), m_PhillipsAmplitude(amplitude), m_Wind(wind), m_Length(length), m_FFT(dimension)
{
	m_HTilde = std::make_unique<std::complex<float>[]>(dimension * dimension);
	m_HTildeSlopeX = std::make_unique<std::complex<float>[]>(dimension * dimension);
	m_HTildeSlopeZ = std::make_unique<std::complex<float>[]>(dimension * dimension);
	m_HTildeDx = std::make_unique<std::complex<float>[]>(dimension * dimension);
	m_HTildeDz = std::make_unique<std::complex<float>[]>(dimension * dimension);

	m_Vertices = std::make_unique<OceanVertex[]>(m_DimensionPlusOne * m_DimensionPlusOne);
	m_Indices = std::make_unique<uint32_t[]>(m_DimensionPlusOne * m_DimensionPlusOne * 6);

	for (int m = 0; m < m_DimensionPlusOne; m++)
	{
		for (int n = 0; n < m_DimensionPlusOne; n++)
		{
			int index = m * m_DimensionPlusOne + n;
			std::complex<float> hTilde0 = HTilde0(n, m);
			std::complex<float> hTilde0Mk = Conjugate(HTilde0(-n, -m));

			m_Vertices[index].Tilde0.x = hTilde0.real();
			m_Vertices[index].Tilde0.y = hTilde0.imag();
			m_Vertices[index].Tilde0mk.x = hTilde0Mk.real();
			m_Vertices[index].Tilde0mk.y = hTilde0Mk.imag();

			m_Vertices[index].OriginalPosition.x = m_Vertices[index].Position.x = (n - m_Dimension / 2.0f) * m_Length / m_Dimension;
			m_Vertices[index].OriginalPosition.y = m_Vertices[index].Position.y = 0.0f;
			m_Vertices[index].OriginalPosition.z = m_Vertices[index].Position.z = (m - m_Dimension / 2.0f) * m_Length / m_Dimension;

			m_Vertices[index].Normal = { 0, 1, 0 };
		}
	}

	m_IndexCount = 0;
	for (int m = 0; m < m_Dimension; m++)
	{
		for (int n = 0; n < m_Dimension; n++)
		{
			int index = m * m_DimensionPlusOne + n;
			m_Indices[m_IndexCount++] = index;
			m_Indices[m_IndexCount++] = index + m_DimensionPlusOne;
			m_Indices[m_IndexCount++] = index + m_DimensionPlusOne + 1;
			m_Indices[m_IndexCount++] = index;
			m_Indices[m_IndexCount++] = index + m_DimensionPlusOne + 1;
			m_Indices[m_IndexCount++] = index + 1;
		}
	}

	BufferLayout layout = {
		{ ShaderDataType::Float3 },
		{ ShaderDataType::Float3 },
		{ ShaderDataType::Float3 },
		{ ShaderDataType::Float3 },
		{ ShaderDataType::Float3 },
	};
	m_VBO = VertexBuffer::Create(m_Vertices.get(), m_DimensionPlusOne * m_DimensionPlusOne * sizeof(OceanVertex), layout);
	m_IBO = IndexBuffer::Create(m_Indices.get(), m_DimensionPlusOne * m_DimensionPlusOne * 6 * sizeof(uint32_t));
	m_VAO = VertexArray::Create();
	m_VAO->AddVertexBuffer(m_VBO);
	m_VAO->SetIndexBuffer(m_IBO);
	m_Mesh = CreateRef<Mesh>(m_VAO);
}

float Ocean::Dispersion(int n, int m)
{
	float w0 = 2.0f * PI / 200.0f;
	float kx = PI * (2.0f * n - m_Dimension) / m_Length;
	float kz = PI * (2.0f * m - m_Dimension) / m_Length;
	return floorf(sqrtf(m_Gravity * sqrtf(kx * kx + kz * kz)) / w0) * w0;
}

float Ocean::Phillips(int n, int m)
{
	glm::vec2 k = { PI * (2.0f * n - m_Dimension) / m_Length, PI * (2.0f * m - m_Dimension) / m_Length };
	float kLength = glm::length(k);
	if (kLength < 0.000001f)
		return 0.0f;
	float kLength2 = kLength * kLength;
	float kLength4 = kLength2 * kLength2;
	float kDotW = glm::dot(glm::normalize(k), glm::normalize(m_Wind));
	float kDotW2 = kDotW * kDotW;
	float wLength = glm::length(m_Wind);
	float l = wLength * wLength / m_Gravity;
	float l2 = l * l;
	float damping = 0.001f;
	float dampedL2 = l2 * damping * damping;

	return m_PhillipsAmplitude * exp(-1.0f / (kLength2 * l2)) / kLength4 * kDotW2 * exp(-kLength2 * dampedL2);
}

std::complex<float> Ocean::HTilde0(int n, int m)
{
	// m_Random.seed(size_t(n) * size_t(m) * 12041240ULL + 12904712094709ULL);
	std::normal_distribution<float> dist;
	return std::complex<float>{ dist(m_Random), dist(m_Random) } * sqrtf(Phillips(n, m) / 2.0f);
}

std::complex<float> Ocean::HTilde(float t, int n, int m)
{
	int index = m * m_DimensionPlusOne + n;
	std::complex<float> hTilde0(m_Vertices[index].Tilde0.x, m_Vertices[index].Tilde0.y);
	std::complex<float> hTilde0mk(m_Vertices[index].Tilde0mk.x, m_Vertices[index].Tilde0mk.y);

	float omegaT = Dispersion(n, m) * t;
	
	std::complex<float> c0(cos(omegaT), sin(omegaT));
	std::complex<float> c1(cos(omegaT), -sin(omegaT));
	return hTilde0 * c0 + hTilde0mk * c1;
}

ComplexVectorNormal Ocean::HeightDisplacementNormal(const glm::vec2& x, float t)
{
	std::complex<float> h(0.0f, 0.0f);
	glm::vec2 d(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 0.0f, 0.0f);

	std::complex<float> c, res, htildeC;
	glm::vec2 k;
	float kx, kz, kLength, kDotx;

	for (int m = 0; m < m_Dimension; m++)
	{
		kz = 2.0f * PI * (m - m_Dimension / 2.0f) / m_Length;
		for (int n = 0; n < m_Dimension; n++)
		{
			kx = 2.0f * PI * (n - m_Dimension / 2.0f) / m_Length;
			k = { kx, kz };
			kLength = glm::length(k);
			kDotx = glm::dot(k, x);
			
			c = std::complex<float>(cos(kDotx), sin(kDotx));
			htildeC = HTilde(t, n, m) * c;

			h += htildeC;
			normal += glm::vec3{ -kx * htildeC.imag(), 0.0f, -kz * htildeC.imag() };
			if (kLength < 0.000001)
				continue;
			d += glm::vec2(kx / kLength * htildeC.imag(), kz / kLength * htildeC.imag());
		}
	}

	normal = glm::normalize(glm::vec3{ 0.0f, 1.0f, 0.0f } - normal);
	ComplexVectorNormal cvn;
	cvn.Height = h;
	cvn.Displacement = d;
	cvn.Normal = normal;
	return cvn;
}

void Ocean::EvaluateWaves(float t)
{
	float lambda = -1.0f;
	ComplexVectorNormal hdn;

	for (int m = 0; m < m_Dimension; m++)
	{
		for (int n = 0; n < m_Dimension; n++)
		{
			int index = m * m_DimensionPlusOne + n;
			glm::vec2 x = { m_Vertices[index].Position.x, m_Vertices[index].Position.z };
			hdn = HeightDisplacementNormal(x, t);
			m_Vertices[index].Position.y = hdn.Height.real();
			m_Vertices[index].Position.x = m_Vertices[index].OriginalPosition.x + lambda * hdn.Displacement.x;
			m_Vertices[index].Position.z = m_Vertices[index].OriginalPosition.z + lambda * hdn.Displacement.y;
			m_Vertices[index].Normal = hdn.Normal;
			if (n == 0 && m == 0)
			{
				m_Vertices[index + m_Dimension + m_DimensionPlusOne * m_Dimension].Position.y = hdn.Height.real();
				m_Vertices[index + m_Dimension + m_DimensionPlusOne * m_Dimension].Position.x = m_Vertices[index + m_Dimension + m_DimensionPlusOne * m_Dimension].OriginalPosition.x + lambda * hdn.Displacement.x;
				m_Vertices[index + m_Dimension + m_DimensionPlusOne * m_Dimension].Position.z = m_Vertices[index + m_Dimension + m_DimensionPlusOne * m_Dimension].OriginalPosition.z + lambda * hdn.Displacement.y;
				m_Vertices[index + m_Dimension + m_DimensionPlusOne * m_Dimension].Normal = hdn.Normal;
			}
			if (n == 0)
			{
				m_Vertices[index + m_Dimension].Position.y = hdn.Height.real();
				m_Vertices[index + m_Dimension].Position.x = m_Vertices[index + m_Dimension].OriginalPosition.x + lambda * hdn.Displacement.x;
				m_Vertices[index + m_Dimension].Position.z = m_Vertices[index + m_Dimension].OriginalPosition.z + lambda * hdn.Displacement.y;
				m_Vertices[index + m_Dimension].Normal = hdn.Normal;
			}
			if (m == 0)
			{
				m_Vertices[index + m_DimensionPlusOne * m_Dimension].Position.y = hdn.Height.real();
				m_Vertices[index + m_DimensionPlusOne * m_Dimension].Position.x = m_Vertices[index + m_DimensionPlusOne * m_Dimension].OriginalPosition.x + lambda * hdn.Displacement.x;
				m_Vertices[index + m_DimensionPlusOne * m_Dimension].Position.z = m_Vertices[index + m_DimensionPlusOne * m_Dimension].OriginalPosition.z + lambda * hdn.Displacement.y;
				m_Vertices[index + m_DimensionPlusOne * m_Dimension].Normal = hdn.Normal;
			}
		}
	}

	m_VBO->SetData(m_Vertices.get(), m_DimensionPlusOne * m_DimensionPlusOne * sizeof(OceanVertex));
}

void Ocean::EvaluateWavesFFT(float t)
{
	float lambda = -1.0f;
	for (int m = 0; m < m_Dimension; m++)
	{
		float kz = PI * (2.0f * m - m_Dimension) / m_Length;
		for (int n = 0; n < m_Dimension; n++)
		{
			float kx = PI * (2.0f * n - m_Dimension) / m_Length;
			float len = sqrtf(kx * kx + kz * kz);
			int index = m * m_Dimension + n;

			m_HTilde[index] = HTilde(t, n, m);
			m_HTildeSlopeX[index] = m_HTilde[index] * std::complex<float>{ 0, kx };
			m_HTildeSlopeZ[index] = m_HTilde[index] * std::complex<float>{ 0, kz };
			if (len < 0.0000001f)
			{
				m_HTildeDx[index] = std::complex<float>(0.0f, 0.0f);
				m_HTildeDz[index] = std::complex<float>(0.0f, 0.0f);
			}
			else
			{
				m_HTildeDx[index] = m_HTilde[index] * std::complex<float>(0.0f, -kx / len);
				m_HTildeDz[index] = m_HTilde[index] * std::complex<float>(0.0f, -kz / len);
			}
		}
	}

	for (int m = 0; m < m_Dimension; m++)
	{
		m_FFT.FastFourierTransform(m_HTilde.get(), m_HTilde.get(), 1, m * m_Dimension);
		m_FFT.FastFourierTransform(m_HTildeSlopeX.get(), m_HTildeSlopeX.get(), 1, m * m_Dimension);
		m_FFT.FastFourierTransform(m_HTildeSlopeZ.get(), m_HTildeSlopeZ.get(), 1, m * m_Dimension);
		m_FFT.FastFourierTransform(m_HTildeDx.get(), m_HTildeDx.get(), 1, m * m_Dimension);
		m_FFT.FastFourierTransform(m_HTildeDz.get(), m_HTildeDz.get(), 1, m * m_Dimension);
	}
	for (int n = 0; n < m_Dimension; n++)
	{
		m_FFT.FastFourierTransform(m_HTilde.get(), m_HTilde.get(), m_Dimension, n);
		m_FFT.FastFourierTransform(m_HTildeSlopeX.get(), m_HTildeSlopeX.get(), m_Dimension, n);
		m_FFT.FastFourierTransform(m_HTildeSlopeZ.get(), m_HTildeSlopeZ.get(), m_Dimension, n);
		m_FFT.FastFourierTransform(m_HTildeDx.get(), m_HTildeDx.get(), m_Dimension, n);
		m_FFT.FastFourierTransform(m_HTildeDz.get(), m_HTildeDz.get(), m_Dimension, n);
	}

	float signs[] = { 1.0f, -1.0f };

	for (int m = 0; m < m_Dimension; m++)
	{
		for (int n = 0; n < m_Dimension; n++)
		{
			int index = m * m_Dimension + n;
			int index1 = m * m_DimensionPlusOne + n;

			float sign = signs[(n + m) & 1];
			m_HTilde[index] = m_HTilde[index] * sign;

			m_Vertices[index1].Position.y = m_HTilde[index].real();
			m_HTildeDx[index] = m_HTildeDx[index] * sign;
			m_HTildeDz[index] = m_HTildeDz[index] * sign;
			m_Vertices[index1].Position.x = m_Vertices[index1].OriginalPosition.x + m_HTildeDx[index].real() * lambda;
			m_Vertices[index1].Position.z = m_Vertices[index1].OriginalPosition.z + m_HTildeDz[index].real() * lambda;

			m_HTildeSlopeX[index] = m_HTildeSlopeX[index] * sign;
			m_HTildeSlopeZ[index] = m_HTildeSlopeZ[index] * sign;
			m_Vertices[index1].Normal = glm::normalize(glm::vec3{ -m_HTildeSlopeX[index].real(), 1.0f, -m_HTildeSlopeZ[index].real() });

			if (m == 0 && n == 0)
			{
				m_Vertices[index1 + m_Dimension + m_DimensionPlusOne * m_Dimension].Position.y = m_HTilde[index].real();
				m_Vertices[index1 + m_Dimension + m_DimensionPlusOne * m_Dimension].Position.x = m_Vertices[index1 + m_Dimension + m_DimensionPlusOne * m_Dimension].OriginalPosition.x + m_HTildeDx[index].real() * lambda;
				m_Vertices[index1 + m_Dimension + m_DimensionPlusOne * m_Dimension].Position.z = m_Vertices[index1 + m_Dimension + m_DimensionPlusOne * m_Dimension].OriginalPosition.z + m_HTildeDz[index].real() * lambda;
				m_Vertices[index1 + m_Dimension + m_DimensionPlusOne * m_Dimension].Normal = m_Vertices[index1].Normal;
			}
			if (n == 0)
			{
				m_Vertices[index1 + m_Dimension].Position.y = m_HTilde[index].real();
				m_Vertices[index1 + m_Dimension].Position.x = m_Vertices[index1 + m_Dimension].OriginalPosition.x + m_HTildeDx[index].real() * lambda;
				m_Vertices[index1 + m_Dimension].Position.z = m_Vertices[index1 + m_Dimension].OriginalPosition.z + m_HTildeDz[index].real() * lambda;
				m_Vertices[index1 + m_Dimension].Normal = m_Vertices[index1].Normal;
			}
			if (m == 0)
			{
				m_Vertices[index1 + m_DimensionPlusOne * m_Dimension].Position.y = m_HTilde[index].real();
				m_Vertices[index1 + m_DimensionPlusOne * m_Dimension].Position.x = m_Vertices[index1 + m_DimensionPlusOne * m_Dimension].OriginalPosition.x + m_HTildeDx[index].real() * lambda;
				m_Vertices[index1 + m_DimensionPlusOne * m_Dimension].Position.z = m_Vertices[index1 + m_DimensionPlusOne * m_Dimension].OriginalPosition.z + m_HTildeDz[index].real() * lambda;
				m_Vertices[index1 + m_DimensionPlusOne * m_Dimension].Normal = m_Vertices[index1].Normal;
			}
		}
	}

	m_VBO->SetData(m_Vertices.get(), m_DimensionPlusOne * m_DimensionPlusOne * sizeof(OceanVertex));
}
