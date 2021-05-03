#pragma once
#include "Shader.h"
#include "Texture.h"

namespace Forge
{

	class RendererContext;

	FORGE_API enum class PolygonMode
	{
		Fill = GL_FILL,
		Line = GL_LINE,
	};

	struct FORGE_API RenderSettings
	{
	public:
		PolygonMode Mode = PolygonMode::Fill;
	};

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

		virtual void Apply(const std::string& name, const Ref<Shader>& shader, RendererContext& context) const override;
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

		void Clear();
		void Apply(const Ref<Shader>& shader, RendererContext& context) const;

	};

}

#include "RendererContext.h"

namespace Forge
{

	template<typename T>
	void UniformContainer<T>::Apply(const std::string& name, const Ref<Shader>& shader, RendererContext& context) const
	{
		if constexpr (std::is_same_v<T, Ref<Texture2D>> || std::is_same_v<T, Ref<Texture>> || std::is_same_v<T, Ref<RenderTexture>> || std::is_same_v<T, Ref<TextureCube>>)
		{
			int slot = context.BindTexture(Value);
			shader->SetUniform(name, slot);
		}
		else
			shader->SetUniform(name, Value);
	}

}
