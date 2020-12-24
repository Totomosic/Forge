#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Forge.h"

using namespace Forge;

int main()
{
	ForgeInstance::Init();

	bool running = true;

	WindowProps props;
	Window window(props);

	window.Events.Close.AddEventListener([&](const WindowClose& evt)
	{
		running = false;
		return true;
	});

	float vertices[3 * 3] = {
		-0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
	};
	
	
	Ref<VertexArray> vao = VertexArray::Create();
	Ref<VertexBuffer> vbo = VertexBuffer::Create(vertices, sizeof(vertices), BufferLayout::Default());
	vao->AddVertexBuffer(vbo);

	Ref<Shader> shader = Shader::CreateFromFile("Default.shader");
	shader->Bind();

	while (running)
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		vao->Bind();
		glDrawArrays(GL_TRIANGLES, 0, 3);

		window.Update();
	}

	return 0;
}
