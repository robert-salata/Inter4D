#include "application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Application* application = new Application();

	while (!application->end)
		application->update();

	delete application;
	return 0;
}