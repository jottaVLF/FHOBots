#include "Vector2D.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {
double safeAcosRatio(double numerator, double denominator)
{
    if(denominator == 0)
        return 0;

    double ratio = numerator / denominator;
    ratio = std::max(-1.0, std::min(1.0, ratio));
    return acos(ratio);
}
}

Vector2D::Vector2D()
{}

Vector2D::Vector2D(double _x, double _y) : x(_x), y(_y)
{}

Vector2D::Vector2D(double angle) : x(cos(angle)), y(sin(angle))
{}

Vector2D Vector2D::operator+(Vector2D& other)
{
    return Vector2D(x + other.x, y + other.y);
}

Vector2D Vector2D::operator-(Vector2D& other)
{
    return Vector2D(x - other.x, y - other.y);
}

double Vector2D::operator*(Vector2D& other)
{
    return x * other.x + y * other.y;
}

Vector2D& Vector2D::operator+=(Vector2D& other)
{
    x += other.x;
    y += other.y;
    return *this;
}

Vector2D& Vector2D::operator-=(Vector2D& other)
{
    x -= other.x;
    y -= other.y;
    return *this;
}

Vector2D Vector2D::operator*(double scalar)
{
    return Vector2D(x * scalar, y * scalar);
}

Vector2D Vector2D::operator*=(double scalar)
{
    x *= scalar;
    y *= scalar;
    return *this;
}

Vector2D Vector2D::operator*(int scalar)
{
    return Vector2D(x * scalar, y * scalar);
}

Vector2D Vector2D::operator*=(int scalar)
{
    x *= scalar;
    y *= scalar;
    return *this;
}

Vector2D Vector2D::operator/(double scalar)
{
    if(scalar != 0)
        return Vector2D(x / scalar, y / scalar);

    return Vector2D(0, 0);
}

Vector2D Vector2D::operator/=(double scalar)
{
    if(scalar != 0)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    return Vector2D(0, 0);
}

Vector2D Vector2D::operator/(int scalar)
{
    if(scalar != 0)
        return Vector2D(x / scalar, y / scalar);

    return Vector2D(0, 0);
}

Vector2D Vector2D::operator/=(int scalar)
{
    if(scalar != 0)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    return Vector2D(0, 0);
}

double Vector2D::operator||(Vector2D& other)
{
    /// Utilizo produto vetorial para saber se é negativo ou positivo a diferença de angulo
    double vetorial_product = x * other.y - other.x * y;
   /// Utilizo produto escalar pra achar a direfença de angulo
    double scalar_product = safeAcosRatio((*this) * other, magnitude() * other.magnitude());
    return vetorial_product >= 0 ? scalar_product : -1 * scalar_product;
}

double Vector2D::operator&&(Vector2D& other)
{
    return x * other.y - other.x * y;
}

std::ostream& operator<<(std::ostream& os, Vector2D& v)
{
    os << "(x: " << v.x << ", y:" << v.y << ")";
    return  os;
}

double Vector2D::magnitude()
{
    return hypot(x,y);
}

double Vector2D::angle()
{
    return atan2(y,x);
}

void Vector2D::normalize()
{
    double mag = magnitude();
    if(mag != 0)
        (*this) /= mag;
}


void Vector2D::set(double _x, double _y)
{
    x = _x;
    y = _y;
}

double Vector2D::angleBetween(Vector2D other) {
    return safeAcosRatio(this->x * other.x + this->y * other.y, this->magnitude() * other.magnitude());
}
