//
// Created by YangKai.Lin on 2026/7/24.
//

#include "ray.h"
#include <sstream>

namespace RayTracing {
    Ray::Ray() {
        m_point1 = glm::vec3(0, 0, 0);
        m_point2 = glm::vec3(0, 0, 1.0);
        m_lab = m_point2 - m_point1;
    }

    Ray::Ray(const glm::vec3 &point1, const glm::vec3 &point2) : m_point1(point1), m_point2(point2) {
        m_lab = m_point2 - m_point1;
    }

    glm::vec3 Ray::GetPoint1() const {
        return m_point1;
    }

    glm::vec3 Ray::GetPoint2() const {
        return m_point2;
    }

    std::string Ray::toString() const {
        std::stringstream ss;
        ss << "Start Point: [" << m_point1.x << ", " << m_point1.y << ", " << m_point1.z << "]" << std::endl;
        ss << "End Point:   [" << m_point1.x << ", " << m_point1.y << ", " << m_point1.z << "]" << std::endl;
        ss << "Direction:   [" << m_lab.x << ", " << m_lab.y << ", " << m_lab.z << "]" << std::endl;
        return ss.str();
    }
}
