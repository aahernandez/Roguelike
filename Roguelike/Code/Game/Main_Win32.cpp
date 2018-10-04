#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/App.hpp"
#include "Engine/Memory/JobSystem.hpp"

#define UNUSED(x) (void)(x);

void Initialize()
{
	JobSystemStartup();
	LogSystemStartup();
	g_theApp = new App();
}

void Shutdown()
{
	delete g_theApp;
	g_theApp = nullptr;

	LogSystemShutdown();
	JobSystemShutdown();
}

int WINAPI WinMain(HINSTANCE applicationInstanceHandle, HINSTANCE previousInstanceHandle, LPSTR commandLineString, int showCommand)
{

	UNUSED(commandLineString);
	UNUSED(applicationInstanceHandle);
	UNUSED(previousInstanceHandle);
	UNUSED(showCommand);
	Initialize();
	
	while (!g_theApp->IsQuitting())
	{
		g_theApp->RunFrame();
	}
	Shutdown();
	return 0;
}