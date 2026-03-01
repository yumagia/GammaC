#pragma once

#include "Math.hpp"

#include "glm/glm.hpp"

namespace GammaEngine {
	class Frustum {
		public:
			Frustum();
			~Frustum();

			void SetFrustum(float near, float far, float fov, float aspectRatio, Vec3f position, Quaternion rotation);
			bool ContainsBox(Vec3f cornerPoint, Vec3f extent);

		private:
			Vec3f leftNormal_, rightNormal_, bottomNormal_, topNormal_, nearNormal_, farNormal_;
			float leftDist_, rightDist_, bottomDist_, topDist_, nearDist_, farDist_;
	};
}