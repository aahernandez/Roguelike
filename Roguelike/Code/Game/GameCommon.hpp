#pragma once

enum GameState_e
{
	NO_STATE = -1,
	MAIN_MENU,
	PLAYING,
	STATS_SCREEN,
	STEPPED_PATHING,
	STEPPED_MAP_GENERATION,
	NUM_GAME_STATES
};

class SimpleRenderer;
class InputSystem;
class AudioSystem;
class Config;

extern SimpleRenderer* g_theSimpleRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;
extern Config* g_theConfig;