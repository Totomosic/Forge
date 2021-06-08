#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Forge.h"
using namespace Forge;

int main()
{
	ForgeInstance::Init();

	WindowProps props;
	props.Title = "Editor";
	Application app(props);

	Scene& scene = app.CreateScene();

	while (!app.ShouldExit())
	{
		Timestep ts = app.GetTimestep();

		app.OnUpdate();
	}

	return 0;
}
