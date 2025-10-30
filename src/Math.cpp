#include "Math.h"

float Vec3f::Normalize() {
	float l = Length();
	if (l > 0.0f) {
		x /= l;
		y /= l;
		z /= l;
	}
	return l;
}

void Vec3f::Negate() {
	x = -x;
	y = -y;
	z = -z;
}

Vec3f Vec3f::Cross(const Vec3f &v2) const {
	return Vec3f(	y*v2.z - z*v2.y,
					z*v2.x - x*v2.z,
					x*v2.y - y*v2.x	);
}

float Vec3f::Dot(const Vec3f &v2) const {
	return x*v2.x + y*v2.y + z*v2.z;
}

Vec3f Vec3f::operator+(const Vec3f &v2) const {
	return Vec3f(x + v2.x, y + v2.y, z + v2.z);
}

Vec3f Vec3f::operator-(const Vec3f &v2) const {
	return Vec3f(x - v2.x, y - v2.y, z - v2.z);
}

Vec3f operator*(float lhs, const Vec3f &rhs) {
	return Vec3f(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
}

Vec3f operator*(const Vec3f &lhs, float rhs) {
	return Vec3f(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}

Vec3f operator/(float lhs, const Vec3f &rhs) {
	return Vec3f(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z);
}

Vec3f operator/(const Vec3f &lhs, float rhs) {
	return Vec3f(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
}

// Create from an axis-angle
// Axis must be a normal(unit) vector
Quaternion::Quaternion(const Vec3f &axis, float angle) {
	float sinHA = sinf(angle/2);
	w = cosf(angle/2);
	x = axis.x * sinHA;
	y = axis.y * sinHA;
	z = axis.z * sinHA;
}


Quaternion Quaternion::GetConjugate() {
	return Quaternion(w, -x, -y, -z);
}

// Vector rotation
Vec3f Quaternion::RotateVector(const Vec3f &v) const {
	Quaternion qv = Quaternion(	x * v.x + y + z * v.z,
								w * v.x + (y * v.y - z * v.y),
								w * v.y + (z * v.x - x * v.z),
								w * v.z + (x * v.y - y * v.x)	);

	return Vec3f(	qv.w * x + (qv.x * w - qv.y * z) + qv.z * y,
					qv.w * y + (qv.y * w - qv.z * x) + qv.x * z,
					qv.w * x + (qv.z * w - qv.x * y) + qv.y * x	);
}

Quaternion Quaternion::operator*(const Quaternion &q2) const {
	return Quaternion(	w * q2.x + x * q2.w + y * q2.z - z * q2.y,
						w * q2.y - x * q2.z + y * q2.w + z * q2.x,
						w * q2.z + x * q2.y - y * q2.x + z * q2.w,
						w * q2.w - x * q2.x - y * q2.y - z * q2.z	);
}

Color Color::operator+(const Color &v2) const {
	return Color(r + v2.r, g + v2.g, b + v2.b);
}

Color Color::operator-(const Color &v2) const {
	return Color(r - v2.r, g - v2.g, b - v2.b);
}

Color Color::operator*(const Color &v2) const {
	return Color(r * v2.r, g * v2.g, b * v2.b);
}

Color Color::operator/(const Color &v2) const {
	return Color(r / v2.r, g / v2.g, b / v2.b);
}

Color operator*(float lhs, const Color &rhs) {
	return Color(lhs * rhs.r, lhs * rhs.g, lhs * rhs.b);
}

Color operator*(const Color &lhs, float rhs) {
	return Color(lhs.r * rhs, lhs.g * rhs, lhs.b * rhs);
}

Color operator/(float lhs, const Color &rhs) {
	return Color(lhs / rhs.r, lhs / rhs.g, lhs / rhs.b);
}

Color operator/(const Color &lhs, float rhs) {
	return Color(lhs.r / rhs, lhs.g / rhs, lhs.b / rhs);
}