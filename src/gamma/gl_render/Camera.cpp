#include "Camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

glm::mat4 Camera::GetMatrix() {
	Vec3f lookAtVec = orientation.RotateVector(Vec3f(0.f, 0.f, 1.f));
	std::cout << lookAtVec.x << std::endl;
	std::cout << lookAtVec.y << std::endl;
	std::cout << lookAtVec.z << std::endl;

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

	std::cout << orientation.w << std::endl;
	std::cout << orientation.x << std::endl;
	std::cout << orientation.y << std::endl;
	std::cout << orientation.z << std::endl;
}