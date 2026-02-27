#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace GammaEngine {
	void Camera::Update(float deltaTime) {
		rotation_ = Quaternion(Vec3f(0, 1, 0), 1.f * deltaTime) * rotation_;
	}

	void Camera::UpdateMatrices() {
		glm::mat4 worldMatrix{1.f};
		worldMatrix = glm::translate(worldMatrix, glm::vec3(position_.x, position_.y, position_.z));
		worldMatrix = worldMatrix * glm::mat4_cast(glm::quat(rotation_.w, rotation_.x, rotation_.y, rotation_.z));
		
		viewMatrix_ = glm::inverse(worldMatrix);
	}

	const Vec3f Camera::GetPosition() const {
		return position_;
	}
	void Camera::SetPosition(const Vec3f position) {
		position_ = position;
	}

	const Quaternion Camera::GetRotation() const {
		return rotation_;
	}
	void Camera::SetRotation(const Quaternion rotation) {
		rotation_ = rotation;
	}

	const glm::mat4& Camera::GetViewMatrix() const {
		return viewMatrix_;
	}

	const glm::mat4& Camera::GetProjectionMatrix() const {
		return projectionMatrix_;
	}

	bool Camera::GetProjectionMatrixDirty() const {
        return projectionMatrixDirty_;
    }

    void Camera::SetProjectionMatrixDirty(bool dirty) {
        projectionMatrixDirty_ = dirty;
    }

    void Camera::SetPerspective(float fov, float aspectRatio, float near, float far) {
        projectionMatrix_ = glm::perspective(glm::radians(fov), aspectRatio, near, far);
        aspectRatio_ = aspectRatio;
    }
}