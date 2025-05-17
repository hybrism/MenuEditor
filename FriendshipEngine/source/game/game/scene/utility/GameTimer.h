#pragma once
#include <engine/text/text.h>
#include <string>
#include <array>

#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <sstream>

struct SceneUpdateContext;
class SceneManager;

class GameTimer
{
public:
	void Init();
	void Update(SceneUpdateContext& aContext);
	void Render();
	void ResetTimer();

	std::string GetTimeAsString();

private:
	std::string Display_Elapsed_Time() const;
	
	double myTimer = 0;
	Text myRenderedText;

	std::array<Text, 4> myRecords;

	SceneManager* mySceneManager;
	std::chrono::steady_clock::time_point start_time;
	std::chrono::steady_clock::time_point pause_start_time;

	double total_paused_duration;

	bool myFirstStart = true;
	bool myIsPaused = false;
};