#pragma once
#include <numbers>
#include <cmath>
#include "Windows.h"

extern int screenWidth;
extern int screenHeight;

#define M_PI 3.14159265358979323846264338327950288

struct view_matrix_t
{
    float* operator[](int index)
    {
        return matrix[index];
    }

    float matrix[4][4];
};

struct Vector
{
    constexpr Vector(
        const float x = 0.f,
        const float y = 0.f,
        const float z = 0.f) noexcept :
        x(x), y(y), z(z) { }

    constexpr const Vector& operator-(const Vector& other) const noexcept;
    constexpr const Vector& operator+(const Vector& other) const noexcept;
    constexpr const Vector& operator/(const float factor) const noexcept;
    constexpr const Vector& operator*(const float factor) const noexcept;

    const static bool WTS(view_matrix_t viewMatrix, Vector& in, Vector& out);

    // like you define it in vector.hpp, i get you may spell it wrong there but how do you not notice in .cpp??
    // if you do notice but just dont change it, come on man
    float CalculateDistance(const Vector& point);
    static Vector Clamp(Vector angle);
    static Vector Normalize(Vector angle);

    float x, y, z;
};

struct Vector2
{
    constexpr Vector2(
        const float x = 0.0f,
        const float y = 0.0f) noexcept :
        x(x), y(y) { };

    constexpr const Vector2& operator-(const Vector2& other) const noexcept;
    constexpr const Vector2& operator+(const Vector2& other) const noexcept;
    constexpr const Vector2& operator/(const float factor) const noexcept;
    constexpr const Vector2& operator*(const float factor) const noexcept;

    static void AimAtPos(float x, float y);
    static Vector2 AimbotAimCalculation(Vector bonePos, Vector localPos, Vector2 viewAngle, int fFlags);

    float x, y;
};
