#pragma once

#include "Math.hpp"
#include "Frustum.hpp"

#include <glm/glm.hpp>

namespace GammaEngine {
	class Camera {
		public:
			Camera() {}
			~Camera() {}

			void UpdateMatrices();

			const Vec3f GetPosition() const;
			void SetPosition(const Vec3f position);
			const Quaternion GetRotation() const;
			void SetRotation(const Quaternion rotation);

			const glm::mat4& GetViewMatrix() const;
			const glm::mat4& GetProjectionMatrix() const;

			bool GetProjectionMatrixDirty() const;
			void SetProjectionMatrixDirty(bool dirty);
			void SetPerspective(float fov, float aspectRatio, float near, float far);

			bool BoxVisible(Vec3f cornerPoint, Vec3f extent);

			void Update(float deltaTime);

		private:
			Vec3f position_{0.f, 0.f, 0.f};
			Quaternion rotation_{1.f, 0.f, 0.f, 0.f};
			glm::mat4 viewMatrix_{1.f};

			float near_, far_;
			float fov_;
			float aspectRatio_{1.f};
			bool projectionMatrixDirty_{true};
			glm::mat4 projectionMatrix_{1.f};
			Frustum frustum_;
	};
}