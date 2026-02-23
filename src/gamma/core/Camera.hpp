#pragma once

#include "Transform.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace GammaEngine {
	class Camera {
		public:
			Camera() {}
			~Camera() {}

			Transform GetTransform();

			void Update(float deltaTime);

		private:
			Transform transform_;
	};
}