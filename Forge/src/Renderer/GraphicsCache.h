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
		static Ref<Shader> s_DefaultShadowShader;
		static Ref<Shader> s_DefaultPointShadowShader;
		static std::unordered_map<int, Ref<Shader>> s_DefaultColorAnimatedShaders;
		static std::unordered_map<int, Ref<Shader>> s_LitTextureAnimatedShaders;

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
		static Ref<Material> AnimatedDefaultColorMaterial(int maxJoints, const Color& color = COLOR_WHITE);
		static Ref<Material> AnimatedLitTextureMaterial(int maxJoints, const Ref<Texture>& texture);

		// Shaders
		inline static Ref<Shader> DefaultColorShader() { return s_DefaultColorShader; }
		inline static Ref<Shader> DefaultTextureShader() { return s_DefaultTextureShader; }
		inline static Ref<Shader> LitColorShader() { return s_LitColorShader; }
		inline static Ref<Shader> LitTextureShader() { return s_LitTextureShader; }
		inline static Ref<Shader> AnimatedDefaultColorShader(int maxJoints) { return CreateDefaultColorAnimatedShader(maxJoints); }
		inline static Ref<Shader> AnimatedLitTextureShader(int maxJoints) { return CreateLitTextureAnimatedShader(maxJoints); }

		inline static Ref<Shader> DefaultShadowShader() { return s_DefaultShadowShader; }
		inline static Ref<Shader> DefaultPointShadowShader() { return s_DefaultPointShadowShader; }

	private:
		static void CreateDefaultColorShader();
		static void CreateDefaultTextureShader();
		static void CreateLitColorShader();
		static void CreateLitTextureShader();
		static Ref<Shader> CreateDefaultColorAnimatedShader(int maxJoints);
		static Ref<Shader> CreateLitTextureAnimatedShader(int maxJoints);
		static void CreateDefaultShadowShader();
		static void CreateDefaultPointShadowShader();

		static void CreateSquareMesh();
		static void CreateCubeMesh();
	};

}
