#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Forge.h"
using namespace Forge;

#include "EditorLayer.h"

int main()
{
	ForgeInstance::Init();

	WindowProps props;
	props.Title = "Editor";
	Application app(props);

	app.EnableImGui();
	app.PushLayer<Editor::EditorLayer>();

	while (!app.ShouldExit())
	{
		Timestep ts = app.GetTimestep();

		app.OnUpdate();
	}

	return 0;
}
