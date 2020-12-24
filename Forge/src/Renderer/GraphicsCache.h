#pragma once
#include "Model.h"

namespace Forge
{

	class FORGE_API GraphicsCache
	{
	private:
		static Ref<Shader> s_DefaultColorShader;

		static Ref<Mesh> s_SquareMesh;


	public:
		static void Init();

		// Models
		inline static Ref<Mesh> SquareMesh() { return s_SquareMesh; }
		
		// Materials
		static Ref<Material> DefaultColorMaterial(const Color& color = COLOR_WHITE);

		// Shaders
		inline static Ref<Shader> DefaultColorShader() { return s_DefaultColorShader; }

	private:
		static void CreateDefaultColorShader();

		static void CreateSquareMesh();
	};

}
