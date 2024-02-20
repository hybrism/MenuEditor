#pragma once
#include <chrono>

class Timer
{
public:
	Timer();
	Timer(const Timer& aTimer) = delete;
	Timer& operator=(const Timer& aTimer) = delete;
	void Update();
	const float& GetDeltaTime() const { return myDeltaTime; }
	const double& GetTotalTime() const { return myTotalTime; }
private:
	std::chrono::high_resolution_clock::time_point myLastTimePoint;
	double myTotalTime;
	float myDeltaTime;
};
