#pragma once
#include "CameraData.h"
#include "Shader.h"
#include "Lighting.h"

namespace Forge
{

	constexpr const char ProjectionMatrixUniformName[] = "u_ProjectionMatrix";
	constexpr const char ViewMatrixUniformName[] = "u_ViewMatrix";
	constexpr const char ProjViewMatrixUniformName[] = "u_ProjViewMatrix";
	constexpr const char ModelMatrixUniformName[] = "u_ModelMatrix";

	constexpr const char LightSourceArrayBase[] = "u_LightSources";
	constexpr const char LightSourceArrayUniformName[] = "u_LightSources[0]";
	constexpr const char UsedLightSourcesUniformName[] = "u_UsedLightSources";

	struct FORGE_API ShaderRequirements
	{
	public:
		bool ProjectionMatrix;
		bool ViewMatrix;
		bool ProjViewMatrix;
		bool ModelMatrix;

		bool LightSources;
	};

	class FORGE_API RendererContext
	{
	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjViewMatrix;
		
		std::vector<LightSource> m_LightSources;

		std::unordered_map<const Shader*, ShaderRequirements> m_RequirementsMap;

	public:
		void SetCamera(const CameraData& camera);
		void SetLightSources(const std::vector<LightSource>& lights);
		void AddLightSource(const LightSource& light);
		void Reset();

		ShaderRequirements GetShaderRequirements(const Ref<Shader>& shader);
		void BindShader(const Ref<Shader>& shader, const ShaderRequirements& requirements);
	};

}
