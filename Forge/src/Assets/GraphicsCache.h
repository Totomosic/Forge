#pragma once
#include "Renderer/Model.h"
#include "Renderer/Texture.h"

namespace Forge
{

	FORGE_API enum class AssetLocationSource
	{
		Generated,
		File,
	};

	FORGE_API enum class AssetLocationType
	{
		Texture2D,
		TextureCube,
		Mesh,
		Shader,
	};

	struct FORGE_API AssetLocation
	{
	public:
		std::string Path;
		AssetLocationSource Source;
		AssetLocationType Type = AssetLocationType::Texture2D;
	};

	inline bool operator==(const AssetLocation& left, const AssetLocation& right)
	{
		return left.Source == right.Source && left.Type == right.Type && left.Path == right.Path;
	}

	inline bool operator!=(const AssetLocation& left, const AssetLocation& right)
	{
		return !(left == right);
	}

	class FORGE_API GraphicsCache
	{
	private:
		static Ref<Shader> s_DefaultColorShader;
		static Ref<Shader> s_DefaultTextureShader;
		static Ref<Shader> s_LitColorShader[2];
		static Ref<Shader> s_LitTextureShader[2];
		static Ref<Shader> s_DefaultShadowShader;
		static Ref<Shader> s_DefaultPointShadowShader;
		static Ref<Shader> s_DefaultPickShader;
		static std::unordered_map<int, Ref<Shader>> s_DefaultColorAnimatedShaders;
		static std::unordered_map<int, Ref<Shader>> s_LitTextureAnimatedShaders;

		static Ref<Mesh> s_SquareMesh;
		static Ref<Mesh> s_CubeMesh;
		static Ref<Mesh> s_SphereMesh;

		static std::unordered_map<AssetLocation, std::weak_ptr<Shader>> s_Shaders;
		static std::unordered_map<AssetLocation, std::weak_ptr<Mesh>> s_Meshes;
		static std::unordered_map<AssetLocation, std::weak_ptr<Texture2D>> s_Texture2Ds;
		static std::unordered_map<AssetLocation, std::weak_ptr<TextureCube>> s_TextureCubes;
		static std::unordered_map<void*, AssetLocation> s_AssetLocations;

	public:
		static void Init();

		template<typename T>
		static AssetLocation GetAssetLocation(const Ref<T>& asset)
		{
			return s_AssetLocations.at((void*)asset.get());
		}

		template<typename T>
		static Ref<T> GetAsset(const AssetLocation& location)
		{
			if constexpr (std::is_same_v<T, Texture2D>)
			{
				FORGE_ASSERT(location.Type == AssetLocationType::Texture2D, "Invalid location");
				auto it = s_Texture2Ds.find(location);
				if (it != s_Texture2Ds.end())
					return it->second.lock();
				return nullptr;
			}
			if constexpr (std::is_same_v<T, TextureCube>)
			{
				FORGE_ASSERT(location.Type == AssetLocationType::TextureCube, "Invalid location");
				auto it = s_TextureCubes.find(location);
				if (it != s_TextureCubes.end())
					return it->second.lock();
				return nullptr;
			}
			if constexpr (std::is_same_v<T, Mesh>)
			{
				FORGE_ASSERT(location.Type == AssetLocationType::Mesh, "Invalid location");
				if (location.Source == AssetLocationSource::Generated)
					HandleGeneratedMesh(location);
				auto it = s_Meshes.find(location);
				if (it != s_Meshes.end())
					return it->second.lock();
				return nullptr;
			}
			if constexpr (std::is_same_v<T, Shader>)
			{
				FORGE_ASSERT(location.Type == AssetLocationType::Shader, "Invalid location");
				if (location.Source == AssetLocationSource::Generated)
					HandleGeneratedShader(location);
				auto it = s_Shaders.find(location);
				if (it != s_Shaders.end())
					return it->second.lock();
				return nullptr;
			}
		}

		static Ref<Texture2D> LoadTexture2D(const std::string& filename);
		static Ref<TextureCube> LoadTextureCube(
			const std::string& front,
			const std::string& back,
			const std::string& left,
			const std::string& right,
			const std::string& bottom,
			const std::string& top
		);
		static Ref<Mesh> LoadMesh(const std::string& filename);
		static Ref<Shader> LoadShader(const std::string& filename);

		// Meshes
		inline static Ref<Mesh> SquareMesh() { CreateSquareMesh(); return s_SquareMesh; }
		inline static Ref<Mesh> CubeMesh() { CreateCubeMesh(); return s_CubeMesh; }
		inline static Ref<Mesh> SphereMesh() { CreateSphereMesh(); return s_SphereMesh; }
		static Ref<Mesh> GridMesh(int xVertices, int zVertices);
		
		// Materials
		static Ref<Material> DefaultColorMaterial(const Color& color = COLOR_WHITE);
		static Ref<Material> DefaultTextureMaterial(const Ref<Texture>& texture);
		static Ref<Material> LitColorMaterial(const Color& color = COLOR_WHITE);
		static Ref<Material> LitTextureMaterial(const Ref<Texture>& texture);
		static Ref<Material> AnimatedDefaultColorMaterial(int maxJoints, const Color& color = COLOR_WHITE);
		static Ref<Material> AnimatedLitTextureMaterial(int maxJoints, const Ref<Texture>& texture);

		// Shaders
		inline static Ref<Shader> DefaultColorShader() { CreateDefaultColorShader(); return s_DefaultColorShader; }
		inline static Ref<Shader> DefaultTextureShader() { CreateDefaultTextureShader(); return s_DefaultTextureShader; }
		inline static Ref<Shader> LitColorShader(bool useShadows) { CreateLitColorShader(); return s_LitColorShader[useShadows ? 1 : 0]; }
		inline static Ref<Shader> LitTextureShader(bool useShadows) { CreateLitTextureShader(); return s_LitTextureShader[useShadows ? 1 : 0]; }
		inline static Ref<Shader> AnimatedDefaultColorShader(int maxJoints) { return CreateDefaultColorAnimatedShader(maxJoints); }
		inline static Ref<Shader> AnimatedLitTextureShader(int maxJoints) { return CreateLitTextureAnimatedShader(maxJoints); }

		inline static Ref<Shader> DefaultShadowShader() { CreateDefaultShadowShader(); return s_DefaultShadowShader; }
		inline static Ref<Shader> DefaultPointShadowShader() { CreateDefaultPointShadowShader(); return s_DefaultPointShadowShader; }
		inline static Ref<Shader> DefaultPickShader() { CreateDefaultPickShader(); return s_DefaultPickShader; }

		// Models
		inline static Ref<Model> SquareModel(float width, float height, const Ref<Material>& material)
		{
			Ref<Model> model = Model::Create(SquareMesh(), material);
			model->GetSubModels()[0].Transform = glm::scale(glm::mat4(1.0f), { width, height, 1.0f });
			return model;
		}

	private:
		static void CreateDefaultColorShader();
		static void CreateDefaultTextureShader();
		static void CreateLitColorShader();
		static void CreateLitTextureShader();
		static Ref<Shader> CreateDefaultColorAnimatedShader(int maxJoints);
		static Ref<Shader> CreateLitTextureAnimatedShader(int maxJoints);
		static void CreateDefaultShadowShader();
		static void CreateDefaultPointShadowShader();
		static void CreateDefaultPickShader();

		static void CreateSquareMesh();
		static void CreateCubeMesh();
		static void CreateSphereMesh();

		static void HandleGeneratedShader(const AssetLocation& location);
		static void HandleGeneratedMesh(const AssetLocation& location);

		template<typename T>
		static void RegisterNewAsset(AssetLocation location, const Ref<T>& asset, std::unordered_map<AssetLocation, std::weak_ptr<T>>& map)
		{
			if constexpr (std::is_same_v<T, Shader>)
				location.Type = AssetLocationType::Shader;
			if constexpr (std::is_same_v<T, Mesh>)
				location.Type = AssetLocationType::Mesh;
			if constexpr (std::is_same_v<T, Texture2D>)
				location.Type = AssetLocationType::Texture2D;
			if constexpr (std::is_same_v<T, TextureCube>)
				location.Type = AssetLocationType::TextureCube;
			map[location] = asset;
			s_AssetLocations[(void*)asset.get()] = location;
		}
	};

}

namespace std
{

	template<>
	struct hash<Forge::AssetLocation>
	{
	public:
		size_t operator()(const Forge::AssetLocation& location) const
		{
			return std::hash<std::string>{}(location.Path);
		}
	};

}
