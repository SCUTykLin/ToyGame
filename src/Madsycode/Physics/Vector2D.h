//
// Created by YangKai.Lin on 2026/7/18.
//

#ifndef TOYPROJECTS_Madsycode_VECTOR2D_H
#define TOYPROJECTS_Madsycode_VECTOR2D_H
#include <ostream>
#include <type_traits>
#include <sstream>

namespace Madsycode::Physics {
    template <typename T>
    class Vector2D {
    public:
        T X, Y;
        Vector2D(T x, T y) : X(x), Y(y) {}
        Vector2D() : X(0.0f), Y(0.0f) {}

        template <typename U>
        inline auto operator+(const Vector2D<U>& v2) const -> Vector2D<std::common_type_t<T, U>> {
            using type  = std::common_type_t<T, U>;
            return Vector2D(static_cast<type>(X + v2.X), static_cast<type>(Y + v2.Y));
        }

        template <typename U>
        inline auto operator-(const Vector2D<U>& v2) const -> Vector2D<std::common_type_t<T, U>> {
            using type  = std::common_type_t<T, U>;
            return Vector2D(static_cast<type>(X - v2.X), static_cast<type>(Y - v2.Y));
        }

        template <typename U>
        inline auto operator*(const U scalar) const -> Vector2D<std::common_type_t<T, U>> {
            using type  = std::common_type_t<T, U>;
            return Vector2D(static_cast<type>(X) * static_cast<type>(scalar),
                static_cast<type>(static_cast<type>(Y) * static_cast<type>(scalar)));
        }

        std::string toString() {
            std::ostringstream oss;
            oss << "(X, Y) = (" << X << " " << Y << "), TYPE = " << typeid(T).name();
            return oss.str();
        }
    };
}

#endif //TOYPROJECTS_Madsycode_VECTOR2D_H