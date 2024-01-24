#pragma warning( disable : 4244 )

#include "vector.hpp"
#include <iostream>
#include <algorithm>

int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);

constexpr const Vector& Vector::operator-(const Vector& other) const noexcept
{
    return Vector{ x - other.x, y - other.y, z - other.z };
}

constexpr const Vector& Vector::operator+(const Vector& other) const noexcept
{
    return Vector{ x + other.x, y + other.y, z + other.z };
}

constexpr const Vector& Vector::operator/(const float factor) const noexcept
{
    return Vector{ x / factor, y / factor, z / factor };
}

constexpr const Vector& Vector::operator*(const float factor) const noexcept
{
    return Vector{ x * factor, y * factor, z * factor };
}

const bool Vector::WTS(view_matrix_t viewMatrix, Vector& in, Vector& out)
{
    out.x = viewMatrix[0][0] * in.x + viewMatrix[0][1] * in.y + viewMatrix[0][2] * in.z + viewMatrix[0][3];
    out.y = viewMatrix[1][0] * in.x + viewMatrix[1][1] * in.y + viewMatrix[1][2] * in.z + viewMatrix[1][3];
    float width = viewMatrix[3][0] * in.x + viewMatrix[3][1] * in.y + viewMatrix[3][2] * in.z + viewMatrix[3][3];

    if (width < 0.01f) {
        return false;
    }

    float inverseWidth = 1.f / width;

    out.x *= inverseWidth;
    out.y *= inverseWidth;

    float x = screenWidth * 0.5f;
    float y = screenHeight * 0.5f;

    x += 0.5f * out.x * screenWidth + 0.5f;
    y -= 0.5f * out.y * screenHeight + 0.5f;

    out.x = x;
    out.y = y;

    return true;
}

float Vector::CalculateDistance(const Vector& point)
{
    float dx = point.x - x;
    float dy = point.y - y;

    return sqrt(pow(dx, 2) + pow(dy, 2));
}

// ================================================
// Vector 2
// ================================================

constexpr const Vector2& Vector2::operator-(const Vector2& other) const noexcept
{
    return Vector2{ x - other.x, y - other.y };
}

constexpr const Vector2& Vector2::operator+(const Vector2& other) const noexcept
{
    return Vector2{ x + other.x, y + other.y };
}

constexpr const Vector2& Vector2::operator/(const float factor) const noexcept
{
    return Vector2{ x / factor, y / factor };
}

constexpr const Vector2& Vector2::operator*(const float factor) const noexcept
{
    return Vector2{ x * factor, y * factor };
}

Vector2 Vector2::AimbotAimCalculation(Vector bonePos, Vector localPos, Vector2 viewAngle, int fFlags)
{
    float yaw, pitch, distance, fov, deltaX, deltaY, deltaZ;

    deltaX = bonePos.x - localPos.x;
    deltaY = bonePos.y - localPos.y;

    if (fFlags == 65667)
        deltaZ = (bonePos.z - 43.f) - localPos.z;
    else
        deltaZ = (bonePos.z - 63.f) - localPos.z;

    distance = sqrt(pow(deltaX, 2) + pow(deltaY, 2));
    yaw = atan2f(deltaY, deltaX) * 180 / M_PI - viewAngle.y;
    pitch = -atan(deltaZ / distance) * 180 / M_PI - viewAngle.x;

    Vector2 aimPos{ pitch, yaw };

    return aimPos;
}

float Vector2::AimbotFovCalculation(Vector2 targetPos, Vector2 eyePos)
{
    float fov;

    Vector2 delta = targetPos - eyePos;

    fov = sqrt(pow(delta.x, 2) + pow(delta.y, 2));

    return fov * 2.5;
}

Vector2 Vector2::AngleToScreenOffset(float angleX, float angleY, float previousX, float previousY, float fov)
{
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    float fovRad = fov * (M_PI / 180.0f);

    double thetaX = (angleX - previousX) * (M_PI / 180.0f);
    double offsetX = (width * tan(thetaX)) / (2 * tan(fovRad / 2));

    double thetaY = (angleY - previousY) * (M_PI / 180.0f);
    double offsetY = (height * tan(thetaY)) / (2 * tan(fovRad / 2));

    return Vector2(-offsetX, offsetY);
}