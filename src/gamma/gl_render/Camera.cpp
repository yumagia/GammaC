#include "Camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

glm::mat4 Camera::GetMatrix() {
	Vec3f lookAtVec = orientation.RotateVector(Vec3f(0.f, 0.f, 1.f));
	glm::mat4 trans = glm::lookAt(	glm::vec3(lookAtVec.x, lookAtVec.y, lookAtVec.z),
									glm::vec3(position.x, position.y, position.z),
									glm::vec3(0.f, 1.f, 0.f)							);

	return trans;
}

void Camera::AxisRotate(const Vec3f &axis, float angle) {
	Quaternion rotation = Quaternion(axis, angle);
	orientation = rotation * orientation;
}

void Camera::AnglesRotate(float dx, float dy) {
	rotationX += dy;
	rotationY += dx;

	Quaternion quatPitch = Quaternion(Vec3f(1.f, 0.f, 0.f), rotationX);
	orientation = quatPitch * Quaternion();
	Quaternion quatYaw = Quaternion(Vec3f(0.f, 1.f, 0.f), rotationY);
	orientation = quatYaw * orientation;
}

void Camera::Move(int axis, float delta) {
	if(axis = 0) {
		position.x += delta;
	}
	else if(axis = 1) {
		position.y += delta;
	}
	else if(axis = 2) {
		position.z += delta;
	}
}