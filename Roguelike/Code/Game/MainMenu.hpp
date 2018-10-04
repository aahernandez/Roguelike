#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Core/RGBA.hpp"

class MainMenu
{
public:
	bool m_canResume;
	bool m_canStepMap;
	RGBA m_resumeColor;
	RGBA m_steppedMapColor;

	MainMenu();
	~MainMenu();

	void Initialize();
	void Update(float deltaSeconds);
	void Render() const;
	void Destroy();

	void KeyDown();
};