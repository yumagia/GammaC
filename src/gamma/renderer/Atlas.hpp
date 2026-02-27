#pragma once

namespace GammaEngine {
	class Atlas {
		public:
			Atlas();
			~Atlas();

			void Initialize();
		private:
			unsigned char *atlas_{nullptr};
	};
}