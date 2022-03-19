#pragma once
#include <chrono>

struct Timer
{
	std::chrono::steady_clock::time_point last;

	float mark()
	{
		const auto old = last;
		last = std::chrono::steady_clock::now();
		const std::chrono::duration<float> frameTime = last - old;
		return frameTime.count();
	}
};