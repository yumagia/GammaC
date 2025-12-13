#ifndef CAMERA_INCLUDED
#define CAMERA_INCLUDED

#include "Math.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
    void AxisRotate(const Vec3f &axis, float angle);
    void AnglesRotate(float dx, float dy);

    glm::mat4 GetMatrix();

    float       rotationX, rotationY;

    Quaternion  orientation;
    Vec3f       position;
};

#endif