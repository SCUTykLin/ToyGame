//
// Created by 15813 on 2026/7/14.
//

#ifndef TOYPROJECTS_ROGUELIKE_VECTOR2D_H
#define TOYPROJECTS_ROGUELIKE_VECTOR2D_H

#include <iostream>

namespace roguelike::calculate {
    class Vector2D {
    public:
        float x;
        float y;

        Vector2D();

        Vector2D(float x, float y);

        Vector2D &Add(const Vector2D &vec);
        Vector2D &Add(const float& scaler);

        Vector2D &Subtract(const Vector2D &vec);
        Vector2D &Subtract(const float& scaler);

        Vector2D &Multiply(const Vector2D &vec);
        Vector2D &Multiply(const float& scaler);

        Vector2D &Divide(const Vector2D &vec);
        Vector2D &Divide(const float& scaler);

        friend Vector2D operator+(Vector2D &v1, Vector2D &v2);
        friend Vector2D operator+(Vector2D &v1, const float& scaler);
        friend Vector2D operator+(const float& scaler, Vector2D &v1);

        friend Vector2D operator-(Vector2D &v1, Vector2D &v2);
        friend Vector2D operator-(Vector2D &v1, const float& scaler);
        friend Vector2D operator-(const float& scaler, Vector2D &v1);

        friend Vector2D operator*(Vector2D &v1, Vector2D &v2);
        friend Vector2D operator*(Vector2D &v1, const float& scaler);
        friend Vector2D operator*(const float& scaler, Vector2D &v1);

        friend Vector2D operator/(Vector2D &v1, Vector2D &v2);
        friend Vector2D operator/(Vector2D &v1, const float& scaler);
        friend Vector2D operator/(const float& scaler, Vector2D &v1);

        Vector2D &operator+=(const Vector2D &vec);
        Vector2D &operator+=(const float& scaler);

        Vector2D &operator-=(const Vector2D &vec);
        Vector2D &operator-=(const float& scaler);

        Vector2D &operator*=(const Vector2D &vec);
        Vector2D &operator*=(const float& scaler);

        Vector2D &operator/=(const Vector2D &vec);
        Vector2D &operator/=(const float& scaler);

        friend std::ostream& operator<<(std::ostream& stream, const Vector2D& vec);

        Vector2D& Zero();
    };
}
#endif //TOYPROJECTS_ROGUELIKE_VECTOR2D_H
