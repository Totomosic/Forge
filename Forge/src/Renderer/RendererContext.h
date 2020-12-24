#pragma once
#include "CameraData.h"
#include "Shader.h"

namespace Forge
{

	constexpr const char ProjectionMatrixUniformName[] = "u_ProjectionMatrix";
	constexpr const char ViewMatrixUniformName[] = "u_ViewMatrix";
	constexpr const char ProjViewMatrixUniformName[] = "u_ProjViewMatrix";
	constexpr const char ModelMatrixUniformName[] = "u_ModelMatrix";


	struct FORGE_API ShaderRequirements
	{
	public:
		bool ProjectionMatrix;
		bool ViewMatrix;
		bool ProjViewMatrix;
		bool ModelMatrix;
	};

	class FORGE_API RendererContext
	{
	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjViewMatrix;

		std::unordered_map<const Shader*, ShaderRequirements> m_RequirementsMap;

	public:
		void SetCamera(const CameraData& camera);
		void Reset();

		ShaderRequirements GetShaderRequirements(const Ref<Shader>& shader);
		void BindShader(const Ref<Shader>& shader, const ShaderRequirements& requirements);
	};

}
