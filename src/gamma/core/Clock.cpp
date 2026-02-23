#include "Clock.hpp"

using namespace std::chrono;

namespace GammaEngine {
	Clock::Clock() {
		Reset();
	}

	void Clock::Reset() {
		startTicks = high_resolution_clock::now();
		lastTicks = startTicks;
	}

	float Clock::DeltaTime() {
		auto currentTicks = high_resolution_clock::now();
		auto deltaTime = duration_cast<duration<float>>(currentTicks - lastTicks).count();
		lastTicks = currentTicks;
		
		return deltaTime;
	}

	float Clock::GetElapsedTime() const {
		auto currentTicks = high_resolution_clock::now();
		return duration_cast<duration<float>>(currentTicks - startTicks).count();
	}
}