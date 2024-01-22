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

Vector Vector::Clamp(Vector angle)
{
    Vector clamped;

    clamped.x = std::clamp(angle.x, -89.0f, 89.0f);
    clamped.y = std::clamp(angle.y, -180.0f, 180.0f);
    clamped.z = 0.0f;

    return clamped;
}

Vector Vector::Normalize(Vector angle)
{
    Vector normalized;

    normalized.x = std::fmod(angle.x + 180.0f, 360.0f) - 180.0f;
    normalized.y = std::fmod(angle.y + 180.0f, 360.0f) - 180.0f;
    normalized.z = 0.0f;

    return normalized;
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

void Vector2::AimAtPos(float x, float y)
{
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);
    int ScreenCenterY = height * 0.5f;
    int ScreenCenterX = width * 0.5f;

    float AimSpeed = 5;
    float TargetX = 0;
    float TargetY = 0;

    //X Axis
    if (x != 0)
    {
        if (x > ScreenCenterX)
        {
            TargetX = -(ScreenCenterX - x);
            TargetX /= AimSpeed;
            if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
        }

        if (x < ScreenCenterX)
        {
            TargetX = x - ScreenCenterX;
            TargetX /= AimSpeed;
            if (TargetX + ScreenCenterX < 0) TargetX = 0;
        }
    }

    //Y Axis

    if (y != 0)
    {
        if (y > ScreenCenterY)
        {
            TargetY = -(ScreenCenterY - y);
            TargetY /= AimSpeed;
            if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
        }

        if (y < ScreenCenterY)
        {
            TargetY = y - ScreenCenterY;
            TargetY /= AimSpeed;
            if (TargetY + ScreenCenterY < 0) TargetY = 0;
        }
    }

    TargetX /= 10;
    TargetY /= 10;

    //Mouse even't will round to 0 a lot of the time, so we can add this, to make it more accurrate on slow speeds.
    if (fabs(TargetX) < 1)
    {
        if (TargetX > 0)
        {
            TargetX = 1;
        }
        if (TargetX < 0)
        {
            TargetX = -1;
        }
    }
    if (fabs(TargetY) < 1)
    {
        if (TargetY > 0)
        {
            TargetY = 1;
        }
        if (TargetY < 0)
        {
            TargetY = -1;
        }
    }
    mouse_event(MOUSEEVENTF_MOVE, TargetX, TargetY, NULL, NULL);
}