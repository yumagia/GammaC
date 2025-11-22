#ifndef MATH_INCLUDED
#define MATH_INCLUDED

#include <math.h>

// Default is origin
struct Vec3f {
public:
	Vec3f() {}
	Vec3f(float x, float y, float z) : x(x), y(y), z(z) {}
	Vec3f(const Vec3f &v2) : x(v2.x), y(v2.y), z(v2.z) {}

	float Length() {
		return sqrt(x * x + y * y + z * z);
	}

	float Normalize();
	void Negate();
	Vec3f Cross(const Vec3f &v2) const;
	float Dot(const Vec3f &v2) const;

	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

	Vec3f operator+(const Vec3f &v2) const;
	Vec3f operator-(const Vec3f &v2) const;

	// We want symmetry for these operators

	friend Vec3f operator*(float lhs, const Vec3f &rhs);
	friend Vec3f operator*(const Vec3f &lhs, float rhs);
	friend Vec3f operator/(float lhs, const Vec3f &rhs);
	friend Vec3f operator/(const Vec3f &lhs, float rhs);
};


// Floating-point quaternion in (w, x, y, z) convention
// Default is identity
struct Quaternion {
public:
	Quaternion() {}		// Will be the indentity quaternion
    Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}
	Quaternion(const Quaternion &q2) : w(q2.w), x(q2.x), y(q2.y), z(q2.z) {}

	Quaternion(const Vec3f &axis, float angle); // Create from an axis-angle

	Quaternion GetConjugate();
	void Normalize();
	Vec3f RotateVector(const Vec3f &v) const;

	float w = 1.f;
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	
	Quaternion operator*(const Quaternion &q2) const;
};

// R, G, and B float values between 0-1
// Default is black
struct Color {
public:
    Color() {}
    Color(float r, float g, float b) : r(r), g(g), b(b) {}
	Color(const Color &c2) : r(c2.r), g(c2.g), b(c2.b) {}
	
	float r = 0.f;
	float g = 0.f;
	float b = 0.f;

	Color operator+(const Color &v2) const;
	Color operator-(const Color &v2) const;
	Color operator*(const Color &v2) const;
	Color operator/(const Color &v2) const;

	// We want symmetry for these operators

	friend Color operator*(float lhs, const Color &rhs);
	friend Color operator*(const Color &lhs, float rhs);
	friend Color operator/(float lhs, const Color &rhs);
	friend Color operator/(const Color &lhs, float rhs);
};

#endif
