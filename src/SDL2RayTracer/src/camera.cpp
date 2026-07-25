//
// Created by YangKai.Lin on 2026/7/24.
//

#include "camera.h"
#include <cmath>
#include <sstream>

namespace RayTracing {
    Camera::Camera() :
    m_cameraPosition(0.0, -10.0, 0.0),
    m_cameraLookAt(0.0, 0.0, 0.0),
    m_cameraUp(0.0, 0.0, 1.0),
    m_cameraLength(1.0),
    m_cameraHorzSize(1.0),
    m_cameraAspect(1.0){}

    void Camera::SetPosition(const glm::vec3 &newPosition) {
        m_cameraPosition = newPosition;
    }

    void Camera::SetLookAt(const glm::vec3 &newLookAt) {
        m_cameraLookAt = newLookAt;
    }

    void Camera::SetUp(const glm::vec3 &newUpVector) {
        m_cameraUp = newUpVector;
    }

    void Camera::SetLength(double newLength) {
        m_cameraLength = newLength;
    }

    void Camera::SetHorzSize(double newSize) {
        m_cameraHorzSize = newSize;
    }

    void Camera::SetAspect(double newAspect) {
        m_cameraAspect = newAspect;
    }

    glm::vec3 Camera::GetPosition() const {
        return m_cameraPosition;
    }

    glm::vec3 Camera::GetLookAt() const {
        return m_cameraLookAt;
    }

    glm::vec3 Camera::GetUp() const {
        return m_cameraUp;
    }

    glm::vec3 Camera::GetU() const {
        return m_projectionScreenU;
    }

    glm::vec3 Camera::GetV() const {
        return m_projectionScreenV;
    }

    glm::vec3 Camera::GetScreenCenter() const {
        return m_projectionScreenCenter;
    }

    double Camera::GetLength() const {
        return m_cameraLength;
    }

    double Camera::GetHorzSize() const {
        return m_cameraHorzSize;
    }

    double Camera::GetAspect() const {
        return m_cameraAspect;
    }

    bool Camera::GenerateRay(float proScreenX, float proScreenY, Ray& cameraRay) {
        // Compute the location of the screen point in the world coordinate.
        glm::vec3 screenWorldPart1 = m_projectionScreenCenter + (m_projectionScreenU * proScreenX);
        glm::vec3 screenWorldCoodinate = screenWorldPart1 + (m_projectionScreenV * proScreenY);

        cameraRay.m_point1 = m_cameraPosition;
        cameraRay.m_point2 = screenWorldCoodinate;
        cameraRay.m_lab = cameraRay.m_point2 - cameraRay.m_point1;
        return true;
    }
    void Camera::UpdateCameraGeometry() {
        // First, compute up vector from the camera position to the LookAt position.
        m_alignmentVector = m_cameraLookAt - m_cameraPosition;
        m_alignmentVector = glm::normalize(m_alignmentVector);

        // Second, compute the U and V vectors.
        m_projectionScreenU = glm::cross(m_alignmentVector, m_cameraUp);
        m_projectionScreenU = glm::normalize(m_projectionScreenU);
        m_projectionScreenV = glm::cross(m_projectionScreenU, m_alignmentVector);
        m_projectionScreenV = glm::normalize(m_projectionScreenV);

        // Thirdly, compute the position of the center point of the screen.
        m_projectionScreenCenter = m_cameraPosition + static_cast<float>(m_cameraLength) * m_alignmentVector;

        // Modify the U and V vectors to match this size and aspect ratio.
        m_projectionScreenU = m_projectionScreenU * static_cast<float>(m_cameraHorzSize);
        m_projectionScreenV = m_projectionScreenV * static_cast<float>(m_cameraHorzSize / m_cameraAspect);
    }

    std::string Camera::toString() const {
        std::stringstream ss;
        ss << "Camera Setting ===> " << "Camera Position: [" << m_cameraPosition.x << ", " << m_cameraPosition.y << ", " << m_cameraPosition.z << "]" << std::endl;
        ss << "Camera LookAt: [" << m_cameraLookAt.x << ", " << m_cameraLookAt.y << ", " << m_cameraLookAt.z << "]" << std::endl;
        ss << "Camera Alignment Vector: [" << m_alignmentVector.x << ", " << m_alignmentVector.y << ", " << m_alignmentVector.z << "]" << std::endl;
        ss << "Camera Projection Screen U: [" << m_projectionScreenU.x << ", " << m_projectionScreenU.y << ", " << m_projectionScreenU.z << "]" << std::endl;
        ss << "Camera Projection Screen V: [" << m_projectionScreenV.x << ", " << m_projectionScreenV.y << ", " << m_projectionScreenV.z << "]" << std::endl;
        ss << "Camera Projection Screen Centre: [" << m_projectionScreenCenter.x << ", " << m_projectionScreenCenter.y << ", " << m_projectionScreenCenter.z << "]" << std::endl;
        ss << "Camera Focus: [" << m_cameraLength << "]" << std::endl;
        ss << "Camera Horize Size: [" << m_cameraHorzSize << "]" << " <=== Camera Setting" << std::endl;
        return ss.str();
    }
}
