#pragma once

#include <vector>

namespace GammaEngine {
	class Atlas {
		public:
			Atlas();
			~Atlas();

			void Initialize();
			bool UploadTexture(unsigned int width, unsigned int height, unsigned char *texture);

			void RemapUv(float& u, float& v);
			void WriteImageFile();

			unsigned int GetDimensions();
			unsigned char *GetData();
		private:
			std::vector<unsigned char> atlas_;
			std::vector<unsigned int> allocatedHeight_;

			unsigned int allocatedX_, allocatedY_;

			unsigned int dimensions_{1};
	};
}