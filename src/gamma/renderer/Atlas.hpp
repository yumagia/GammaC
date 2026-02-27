#pragma once

#include <vector>

namespace GammaEngine {
	class Atlas {
		public:
			Atlas();
			~Atlas();

			void Initialize();
			bool UploadTexture(unsigned int width, unsigned int height, unsigned char *texture);

			void WriteImageFile();

		private:
			std::vector<unsigned char> atlas_;
			std::vector<unsigned int> allocatedHeight_;

			unsigned int allocatedX_, allocatedY_;
	};
}