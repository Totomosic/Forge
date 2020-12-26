#pragma once
#include "Model.h"
#include "Texture.h"

namespace Forge
{

	class FORGE_API GraphicsCache
	{
	private:
		static Ref<Shader> s_DefaultColorShader;
		static Ref<Shader> s_DefaultTextureShader;
		static Ref<Shader> s_LitColorShader;
		static Ref<Shader> s_LitTextureShader;

		static Ref<Mesh> s_SquareMesh;
		static Ref<Mesh> s_CubeMesh;

	public:
		static void Init();

		// Models
		inline static Ref<Mesh> SquareMesh() { return s_SquareMesh; }
		inline static Ref<Mesh> CubeMesh() { return s_CubeMesh; }
		
		// Materials
		static Ref<Material> DefaultColorMaterial(const Color& color = COLOR_WHITE);
		static Ref<Material> DefaultTextureMaterial(const Ref<Texture>& texture);
		static Ref<Material> LitColorMaterial(const Color& color = COLOR_WHITE);
		static Ref<Material> LitTextureMaterial(const Ref<Texture>& texture);

		// Shaders
		inline static Ref<Shader> DefaultColorShader() { return s_DefaultColorShader; }
		inline static Ref<Shader> DefaultTextureShader() { return s_DefaultTextureShader; }
		inline static Ref<Shader> LitColorShader() { return s_LitColorShader; }
		inline static Ref<Shader> LitTextureShader() { return s_LitTextureShader; }

	private:
		static void CreateDefaultColorShader();
		static void CreateDefaultTextureShader();
		static void CreateLitColorShader();
		static void CreateLitTextureShader();

		static void CreateSquareMesh();
		static void CreateCubeMesh();
	};

}
