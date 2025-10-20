#ifndef GIGA_VECTOR3_H
#define GIGA_VECTOR3_H

#include <array>
#include <cmath>
#include <format>
#include <initializer_list>
#include <string>
#include <vector>

namespace giga {

class Vector3 {
public:
    // Components
    double x;
    double y;
    double z;

    // Constructors
    Vector3& operator=(const Vector3& vector3) {
        this->x = vector3.x;
        this->y = vector3.y;
        this->z = vector3.z;

        return *this;
    }

    Vector3(double x, double y, double z): x{x}, y{y}, z{z} {}

    Vector3(const std::initializer_list<double>& list) {
        this->x = list.begin()[0];
        this->y = list.begin()[1];
        this->z = list.begin()[2];
    }

    Vector3(const std::vector<double>& vector) {
        this->x = vector[0];
        this->y = vector[1];
        this->z = vector[2];
    }

    Vector3(const std::array<double, 3>& array) {
        this->x = array[0];
        this->y = array[1];
        this->z = array[2];
    }

    // Operators
    Vector3 operator+(const Vector3& vector3) {
        return {this->x + vector3.x, this->y + vector3.y, this->z + vector3.z};
    }

    Vector3 operator-(const Vector3& vector3) {
        return {this->x - vector3.x, this->y - vector3.y, this->z - vector3.z};
    }

    Vector3 operator*(const Vector3& vector3) {
        return {this->x * vector3.x, this->y * vector3.y, this->z * vector3.z};
    }

    Vector3 operator/(const Vector3& vector3) {
        return {this->x / vector3.x, this->y / vector3.y, this->z / vector3.z};
    }

    Vector3& operator+=(const Vector3& vector3) {
        this->x += vector3.x;
        this->y += vector3.y;
        this->z += vector3.z;

        return *this;
    }

    Vector3& operator-=(const Vector3& vector3) {
        this->x -= vector3.x;
        this->y -= vector3.y;
        this->z -= vector3.z;

        return *this;
    }

    Vector3& operator*=(const Vector3& vector3) {
        this->x *= vector3.x;
        this->y *= vector3.y;
        this->z *= vector3.z;

        return *this;
    }

    Vector3& operator/=(const Vector3& vector3) {
        this->x /= vector3.x;
        this->y /= vector3.y;
        this->z /= vector3.z;

        return *this;
    }

    double length() {
        return std::sqrt(std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2));
    }

    double dot(const Vector3& other) {
        return ((x * other.x) + (y * other.y) + (z * other.z));
    }

    Vector3 normalize() {
        return {x / this->length(), y / this->length(), z / this->length()};
    }

    std::string string() {
        return std::format("{{{}, {}, {}}}", x, y, z);
    }
};

} // namespace giga

#endif // GIGA_VECTOR3_H
