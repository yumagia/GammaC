#pragma once

#include "Math.hpp"

struct Transform {
	Vec3f position_;
	Quaternion rotation_;
	Vec3f scale_;
};