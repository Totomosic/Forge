#pragma once
#include "Shader.h"
#include "Texture.h"
#include "RendererContext.h"

namespace Forge
{

	class FORGE_API UniformContainerBase
	{
	public:
		virtual ~UniformContainerBase() = default;
		virtual void Apply(const std::string& name, const Ref<Shader>& shader, RendererContext& context) const = 0;
	};

	template<typename T>
	class FORGE_API UniformContainer : public UniformContainerBase
	{
	public:
		T Value;

	public:
		UniformContainer(const T& value) : UniformContainerBase(),
			Value(value)
		{}

		virtual void Apply(const std::string& name, const Ref<Shader>& shader, RendererContext& context) const override
		{
			if constexpr (std::is_same_v<T, Ref<Texture2D>> || std::is_same_v<T, Ref<Texture>>)
			{
				int slot = context.BindTexture(Value);
				shader->SetUniform(name, slot);
			}
			else
				shader->SetUniform(name, Value);
		}
	};

	class FORGE_API UniformContext
	{
	private:
		std::unordered_map<std::string, std::unique_ptr<UniformContainerBase>> m_Uniforms;

	public:
		UniformContext();

		template<typename T>
		void AddUniform(const std::string& name, const T& value)
		{
			m_Uniforms[name] = std::make_unique<UniformContainer<T>>(value);
		}

		template<typename T>
		void UpdateUniform(const std::string& name, const T& value)
		{
			((UniformContainer<T>*)m_Uniforms[name].get())->Value = value;
		}

		void Apply(const Ref<Shader>& shader, RendererContext& context) const;

	};

	class FORGE_API Material
	{
	private:
		Ref<Shader> m_Shader;
		UniformContext m_Uniforms;

	public:
		Material();
		Material(const Ref<Shader>& shader);
		virtual ~Material() = default;

		inline const Ref<Shader>& GetShader() const { return m_Shader; }
		inline const UniformContext& GetUniforms() const { return m_Uniforms; }
		inline UniformContext& GetUniforms() { return m_Uniforms; }

		void Apply(RendererContext& context) const;

	};

}
