#include "Camera.h"
#include "graphics/GL.h"

using namespace library;

class Camera::Impl
{
public:
    vec3 position;
    vec3 lookAt;
    vec3 up{vec3(0.0f, 1.0f, 0.0f)};
    vec3 right;
    vec3 direction;
    mat4 view;
    mat4 projection;
    float rotateSpeed{0.005f};
    float strafeSpeed{5.0f};
    float fov;
    float aspectRatio{4.0f / 3.0f};
    float nearClip{0.1f};
    float farClip{10000.0f};

    float horizontalAngle{3.14f};
    float verticalAngle{0.0f};

    Plane planes[6];

    void update();
};

void Camera::Impl::update()
{
    direction = vec3(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

    right = vec3(sin(horizontalAngle - half_pi<float>()), 0, cos(horizontalAngle - half_pi<float>()));
    up = glm::cross(right, direction);

    projection = perspective(radians(fov), aspectRatio, nearClip, farClip);
    vec3 center = position + direction;
    view = glm::lookAt(position, center, up);

    // Calculate frustum planes
    vec3 cN = position + direction * nearClip;
    vec3 cF = position + direction * farClip;

    float Hnear = 2.0f * tan(radians(fov / 2.0f)) * nearClip;
    float Wnear = Hnear * aspectRatio;
    float Hfar = 2.0f * tan(radians(fov / 2.0f)) * farClip;
    float Wfar = Hfar * aspectRatio;
    float hHnear = Hnear / 2.0f;
    float hWnear = Wnear / 2.0f;
    float hHfar = Hfar / 2.0f;
    float hWfar = Wfar / 2.0f;

    vec3 farPts[4];
    vec3 nearPts[4];

    farPts[0] = cF + up * hHfar - right * hWfar;
    farPts[1] = cF - up * hHfar - right * hWfar;
    farPts[2] = cF - up * hHfar + right * hWfar;
    farPts[3] = cF + up * hHfar + right * hWfar;

    nearPts[0] = cN + up * hHnear - right * hWnear;
    nearPts[1] = cN - up * hHnear - right * hWnear;
    nearPts[2] = cN - up * hHnear + right * hWnear;
    nearPts[3] = cN + up * hHnear + right * hWnear;

    planes[0] = Plane::fromPoints(nearPts[3], nearPts[0], farPts[0]);
    planes[1] = Plane::fromPoints(nearPts[1], nearPts[2], farPts[2]);
    planes[2] = Plane::fromPoints(nearPts[0], nearPts[1], farPts[1]);
    planes[3] = Plane::fromPoints(nearPts[2], nearPts[3], farPts[2]);
    planes[4] = Plane::fromPoints(nearPts[0], nearPts[3], nearPts[2]);
    planes[5] = Plane::fromPoints(farPts[3], farPts[0], farPts[1]);

}

//=========================================================================
Camera::Camera()
    : impl{std::make_unique<Impl>()}
{
    setPosition(0, 0, 0);
    setPerspective(45.0f, 1.0f, impl->nearClip, impl->farClip);
    setLookAt(0, 0, 0);
}

Camera::~Camera()
{
}

void Camera::setPosition(vec3 position)
{
    impl->position = position;
    impl->update();
}

void Camera::setPosition(float x, float y, float z)
{
    setPosition(vec3(x, y, z));
}

void Camera::setDirection(vec3 direction)
{
    auto tmpPosition = impl->position;
    setPosition(vec3(0));
    setLookAt(direction);
    setPosition(tmpPosition);
}

void Camera::setDirection(float x, float y, float z)
{
    setDirection(vec3(x, y, z));
}

void Camera::setLookAt(vec3 point)
{
    if (point != impl->position) {
        vec3 direction = normalize(point - impl->position);
        impl->verticalAngle = asinf(direction.y);
        impl->horizontalAngle = atan2f(direction.x, direction.z);
        impl->update();
    }
}

void Camera::setLookAt(float x, float y, float z)
{
    setLookAt(vec3(x, y, z));
}

vec3 Camera::getPosition() const
{
    return impl->position;
}

vec3 Camera::getDirection() const
{
    return impl->direction;
}

void Camera::setAspectRatio(float ratio)
{
    impl->aspectRatio = ratio;
    impl->update();
}

mat4 Camera::getViewMatrix() const
{
    return impl->view;
}

mat4 Camera::getProjectionMatrix() const
{
    return impl->projection;
}

//=========================================================================
void Camera::setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane)
{
    impl->fov = fov;
    impl->aspectRatio = aspectRatio;
    impl->nearClip = nearPlane;
    impl->farClip = farPlane;
    impl->update();
}

float Camera::getFarClip() const
{
    return impl->farClip;
}

float Camera::getNearClip() const
{
    return impl->nearClip;
}

void Camera::rotate(float x, float y)
{
    impl->horizontalAngle += impl->rotateSpeed * -x;
    impl->verticalAngle += impl->rotateSpeed * -y;
    impl->update();
}

void Camera::setRotateSpeed(float speed)
{
    impl->rotateSpeed = speed;
}

void Camera::move(MovementType movement)
{
    switch (movement) {
        case FORWARD:
            impl->position += impl->direction * impl->strafeSpeed;
            break;

        case BACKWARD:
            impl->position -= impl->direction * impl->strafeSpeed;
            break;

        case LEFT:
            impl->position -= impl->right * impl->strafeSpeed;
            break;

        case RIGHT:
            impl->position += impl->right * impl->strafeSpeed;
            break;

        case UP:
            impl->position += impl->up * impl->strafeSpeed;
            break;

        case DOWN:
            impl->position -= impl->up * impl->strafeSpeed;
            break;
    }

    impl->update();
}

bool Camera::intersects(const AABB &aabb) const
{
    for (unsigned int i = 0; i < 6; i++) {
        if (impl->planes[i].getDistance(aabb.getPositive(impl->planes[i].n)) < 0)
            return false;
    }

    return true;
}

bool Camera::contains(const AABB &aabb) const
{
    for (size_t i = 0; i < 6; i++) {
        if (impl->planes[i].getDistance(aabb.getPositive(impl->planes[i].n)) < 0)
            return false;
        else if (impl->planes[i].getDistance(aabb.getNegative(impl->planes[i].n)) < 0)
            return false;
    }

    return true;
}

vec3 Camera::getRight() const
{
    return impl->right;
}

vec3 Camera::getUp() const
{
    return impl->up;
}

void Camera::setNearClip(float nearClip)
{
    impl->nearClip = nearClip;
}

void Camera::setFarClip(float farClip)
{
    impl->farClip = farClip;
}

void Camera::setStrafeSpeed(float speed)
{
    impl->strafeSpeed = speed;
}

vec3 Camera::pick(const ivec2 &mousePos)
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    GLfloat zCursor;
    GLint winX = mousePos.x;
    GLint winY = viewport[3] - mousePos.y;
    glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zCursor);

    vec3 pos = glm::unProject(vec3(winX, winY, zCursor), impl->view, impl->projection, ivec4(viewport[0], viewport[1], viewport[2], viewport[3]));

    return pos;
}