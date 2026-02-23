#pragma once

#include <chrono>

namespace GammaEngine {
	class Clock {
		public: 
			Clock();
			~Clock() {}
			void Reset();
			float DeltaTime();
			float GetElapsedTime() const;

		private:
			std::chrono::time_point<std::chrono::high_resolution_clock> startTicks;
			std::chrono::time_point<std::chrono::high_resolution_clock> lastTicks;
	};
}