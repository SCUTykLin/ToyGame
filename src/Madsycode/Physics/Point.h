//
// Created by 15813 on 2026/7/19.
//

#ifndef TOYPROJECTS_Madsycode_POINT_H
#define TOYPROJECTS_Madsycode_POINT_H

#include <iostream>
#include <sstream>

namespace Madsycode::Physics{
    class Point{
    public:
        float X, Y;
    public:
        Point(float X=0, float Y=0) : X(X), Y(Y) {}
        inline Point operator+(const Point& p2) const {
            return Point(X + p2.X, Y + p2.Y);
        }

        inline friend Point operator+=(Point& p1, const Point& p2){
            p1.X += p2.X;
            p1.Y += p2.Y;
            return p1;
        }

        inline Point operator-(const Point& p2) const {
            return Point(X - p2.X, Y - p2.Y);
        }

        inline friend Point operator-=(Point& p1, const Point& p2){
            p1.X -= p2.X;
            p1.Y -= p2.Y;
            return p1;
        }

        inline Point operator*(const float scalar) const{
            return Point(X * scalar, Y * scalar);
        }

        std::string toString() {
            std::ostringstream oss;
            oss << "(X, Y) = (" << X << " " << Y << ")";
            return oss.str();
        }

    };
}

#endif //TOYPROJECTS_Madsycode_POINT_H
