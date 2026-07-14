//
// Created by 15813 on 2026/7/14.
//

#include "Vector2D.h"
namespace roguelike::calculate {
    Vector2D::Vector2D() : x(0.0f), y(0.0f) {
    }

    Vector2D::Vector2D(float x, float y) : x(x), y(y) {
    }

    Vector2D &Vector2D::Add(const Vector2D &vec) {
        this->x += vec.x;
        this->y += vec.y;
        return *this;
    }

    Vector2D &Vector2D::Subtract(const Vector2D &vec) {
        this->x -= vec.x;
        this->y -= vec.y;
        return *this;
    }

    Vector2D &Vector2D::Multiply(const Vector2D &vec) {
        this->x /= vec.x;
        this->y /= vec.y;
        return *this;
    }

    Vector2D &Vector2D::Divide(const Vector2D &vec) {
        this->x *= vec.x;
        this->y *= vec.y;
        return *this;
    }

    Vector2D operator+(Vector2D& v1, Vector2D& v2){
        return Vector2D(v1.x + v2.x, v1.y + v2.y);
    }

    Vector2D operator+(Vector2D& v1, const float& scaler){
        return Vector2D(v1.x + scaler, v1.y + scaler);
    }

    Vector2D operator+(const float& scaler, Vector2D& v1){
        return Vector2D(v1.x + scaler, v1.y + scaler);
    }

    Vector2D operator-(Vector2D& v1, Vector2D& v2){
        return Vector2D(v1.x - v2.x, v1.y - v2.y);
    }

    Vector2D operator-(Vector2D& v1, const float& scaler){
        return Vector2D(v1.x - scaler, v1.y - scaler);
    }

    Vector2D operator-(const float& scaler, Vector2D& v1){
        return Vector2D(scaler - v1.x, scaler - v1.y);
    }

    Vector2D operator*(Vector2D& v1, Vector2D& v2){
        return Vector2D(v1.x * v2.x, v1.y * v2.y);
    }

    Vector2D operator*(Vector2D& v1, const float& scaler){
        return Vector2D(v1.x * scaler, v1.y * scaler);
    }

    Vector2D operator*(const float& scaler, Vector2D& v1){
        return Vector2D(v1.x * scaler, v1.y * scaler);
    }

    Vector2D operator/(Vector2D& v1, Vector2D& v2){
        return Vector2D(v1.x / v2.x, v1.y / v2.y);
    }

    Vector2D operator/(Vector2D& v1, const float& scaler){
        return Vector2D(v1.x / scaler, v1.y / scaler);
    }

    Vector2D operator/(const float& scaler, Vector2D& v1){
        return Vector2D(scaler / v1.x, scaler / v1.y);
    }

    Vector2D &Vector2D::operator+=(const Vector2D &vec) {
        this->Add(vec);
        return *this;
    }

    Vector2D &Vector2D::operator-=(const Vector2D &vec) {
        this->Subtract(vec);
        return *this;
    }

    Vector2D &Vector2D::operator*=(const Vector2D &vec) {
        this->Multiply(vec);
        return *this;
    }

    Vector2D &Vector2D::operator/=(const Vector2D &vec) {
        this->Divide(vec);
        return *this;
    }

    std::ostream& operator<<(std::ostream& stream, const Vector2D& vec){
        stream << "[" << vec.x << ", " << vec.y << "]";
        return stream;
    }

    Vector2D &Vector2D::Add(const float &scaler) {
        this->x += scaler;
        this->y += scaler;
        return *this;
    }

    Vector2D &Vector2D::Subtract(const float &scaler) {
        this->x -= scaler;
        this->y -= scaler;
        return *this;
    }

    Vector2D &Vector2D::Multiply(const float &scaler) {
        this->x *= scaler;
        this->y *= scaler;
        return *this;
    }

    Vector2D &Vector2D::Divide(const float &scaler) {
        this->x /= scaler;
        this->y /= scaler;
        return *this;
    }

    Vector2D &Vector2D::operator+=(const float &scaler) {
        this->Add(scaler);
        return *this;
    }

    Vector2D &Vector2D::operator-=(const float &scaler) {
        this->Subtract(scaler);
        return *this;
    }

    Vector2D &Vector2D::operator*=(const float &scaler) {
        this->Multiply(scaler);
        return *this;
    }

    Vector2D &Vector2D::operator/=(const float &scaler) {
        this->Divide(scaler);
        return *this;
    }

    Vector2D &Vector2D::Zero() {
        this->x = 0;
        this->y = 0;
        return *this;
    }
}