#include "Frustum.hpp"

#include <iostream>

namespace GammaEngine {
	Frustum::Frustum() {

	}

	Frustum::~Frustum() {

	}

	void Frustum::SetFrustum(float near, float far, float fov, float aspectRatio, Vec3f position, Quaternion rotation) {
		// Left and right
		Vec3f normal = rotation.RotateVector(Vec3f(cos(fov) * aspectRatio, 0, -sin(fov)));		
		leftNormal_ = normal;
		leftDist_ = normal.Dot(position);
		normal = rotation.RotateVector(Vec3f(-cos(fov) * aspectRatio, 0, -sin(fov)));		
		leftNormal_ = normal;
		leftDist_ = normal.Dot(position);

		// Near and far
		normal = rotation.RotateVector(Vec3f(0, 0, -1));
		float dist = normal.Dot(position);
		nearDist_ = dist + near;
		farDist_ = -(dist + far);
		nearNormal_ = normal;
		farNormal_ = -1 * normal;

		// Bottom and top
		normal = rotation.RotateVector(Vec3f(0, cos(fov), -sin(fov)));		
		leftNormal_ = normal;
		leftDist_ = normal.Dot(position);
		normal = rotation.RotateVector(Vec3f(0, -cos(fov), -sin(fov)));		
		leftNormal_ = normal;
		leftDist_ = normal.Dot(position);
	}

	bool Frustum::ContainsBox(Vec3f cornerPoint, Vec3f extent) {
		// Left and right
		Vec3f near = cornerPoint + Vec3f(extent.x * (leftNormal_.x > 0), extent.y * (leftNormal_.y > 0), extent.z * (leftNormal_.z > 0));
		if((near.Dot(leftNormal_) - leftDist_) < 0) {
			return 0;
		}
		near = cornerPoint + Vec3f(extent.x * (rightNormal_.x > 0), extent.y * (rightNormal_.y > 0), extent.z * (rightNormal_.z > 0));
		if((near.Dot(rightNormal_) - rightDist_) < 0) {
			return 0;
		}

		// Near and far
		near = cornerPoint + Vec3f(extent.x * (nearNormal_.x > 0), extent.y * (nearNormal_.y > 0), extent.z * (nearNormal_.z > 0));
		if((near.Dot(nearNormal_) - nearDist_) < 0) {
			return 0;
		}
		near = cornerPoint + Vec3f(extent.x * (farNormal_.x > 0), extent.y * (farNormal_.y > 0), extent.z * (farNormal_.z > 0));
		if((near.Dot(farNormal_) - farDist_) < 0) {
			return 0;
		}

		// Bottom and top
		near = cornerPoint + Vec3f(extent.x * (bottomNormal_.x > 0), extent.y * (bottomNormal_.y > 0), extent.z * (bottomNormal_.z > 0));
		if((near.Dot(bottomNormal_) - bottomDist_) < 0) {
			return 0;
		}
		near = cornerPoint + Vec3f(extent.x * (topNormal_.x > 0), extent.y * (topNormal_.y > 0), extent.z * (topNormal_.z > 0));
		if((near.Dot(topNormal_) - topDist_) < 0) {
			return 0;
		}

		return 1;
	}
}