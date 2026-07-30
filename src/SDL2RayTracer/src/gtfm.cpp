//
// Created by YangKai.Lin on 2026/7/26.
//

#include "gtfm.h"

#include <iomanip>
#include <glm/ext/matrix_transform.hpp>
#include "ray_math.h"
#include <iostream>

namespace RayTracing {
    GTform::GTform() {
        m_fwdtfm = glm::mat4x4(1.0f);
        m_bcktfm = glm::mat4x4(1.0f);
    }

    GTform::~GTform() {
    }

    GTform::GTform(const glm::mat4x4 &fwd, const glm::mat4x4 &bck) {
        // Verify that the inputs are 4x4.
        m_fwdtfm = fwd;
        m_bcktfm = bck;
    }

    void GTform::SetTransform(const glm::vec3 &translation, const glm::vec3 &rotation, const glm::vec3 &scale) {
        // Define a matrix for each component of the transform.
        glm::mat4x4 translationMatrix{0};
        glm::mat4x4 rotationMatrixX{0};
        glm::mat4x4 rotationMatrixY{0};
        glm::mat4x4 rotationMatrixZ{0};
        glm::mat4x4 scaleMatrix{0};

        // Set these to identity.
        SetIdentity(translationMatrix);
        SetIdentity(rotationMatrixX);
        SetIdentity(rotationMatrixY);
        SetIdentity(rotationMatrixZ);
        SetIdentity(scaleMatrix);

        // Populate these with the appropriate values.
        // First the translation matrix.
        translationMatrix = glm::translate(translationMatrix, translation);

        // Rotate
        rotationMatrixZ[0][0] = glm::cos(rotation.z);
        rotationMatrixZ[1][0] = -glm::sin(rotation.z);
        rotationMatrixZ[0][1] = glm::sin(rotation.z);
        rotationMatrixZ[1][1] = glm::cos(rotation.z);

        rotationMatrixY[0][0] = glm::cos(rotation.y);
        rotationMatrixY[2][0] = glm::sin(rotation.y);
        rotationMatrixY[0][2] = -glm::sin(rotation.y);
        rotationMatrixY[2][2] = glm::cos(rotation.y);

        rotationMatrixX[1][1] = glm::cos(rotation.x);
        rotationMatrixX[1][2] = glm::sin(rotation.x);
        rotationMatrixX[2][1] = -glm::sin(rotation.x);
        rotationMatrixX[2][2] = glm::cos(rotation.x);

        scaleMatrix = glm::scale(scaleMatrix, scale);

        // Combine to give the final forward transform matrix.
        m_fwdtfm = translationMatrix * scaleMatrix * rotationMatrixX * rotationMatrixY * rotationMatrixZ;

        // backward
        m_bcktfm = glm::inverse(m_fwdtfm);
    }

    glm::mat4x4 GTform::GetForward() {
        return m_fwdtfm;
    }

    glm::mat4x4 GTform::GetBackward() {
        return m_bcktfm;
    }

    glm::vec3 GTform::Apply(const glm::vec3 &inputVector, bool dirFlags) {
        glm::vec4 tempData{inputVector.x, inputVector.y, inputVector.z, 1.0f};
        if (dirFlags) {
            tempData = this->GetForward() * tempData;
        }else {
            tempData = this->GetBackward() * tempData;
        }
        glm::vec3 outputVector{tempData.x, tempData.y, tempData.z};
        return outputVector;
    }

    void GTform::PrintVector(const glm::vec3 &vector) {
        std::cout << std::fixed << std::setprecision(3) << vector.x << ' ' << vector.y << ' ' << vector.z << std::endl;
    }

    void GTform::Print(const glm::mat4x4 &matrix) {
        showMatrix(matrix);
    }

    Ray GTform::Apply(const Ray &inputRay, bool dirFlags) {
        // Create an output objects.
        Ray outputRay;

        if (dirFlags) {
            // Apply the forward tranform.
            outputRay.m_point1 = this->Apply(inputRay.m_point1, FWDTFORM);
            outputRay.m_point2 = this->Apply(inputRay.m_point2, FWDTFORM);
            outputRay.m_lab = outputRay.m_point2 - outputRay.m_point1;
        }else {
            // Apply the backward tranform.
            outputRay.m_point1 = this->Apply(inputRay.m_point1, BCKTFORM);
            outputRay.m_point2 = this->Apply(inputRay.m_point2, BCKTFORM);
            outputRay.m_lab = outputRay.m_point2 - outputRay.m_point1;
        }
        return outputRay;
    }

    GTform operator*(const GTform &lfs, const GTform &rhs) {
        auto fwdResult = lfs.m_fwdtfm * rhs.m_bcktfm;
        auto bckResult = glm::inverse(fwdResult);
        GTform finalResult{fwdResult, bckResult};
        return finalResult;
    }

    GTform & GTform::operator=(const GTform &rhs) {
        m_fwdtfm = rhs.m_fwdtfm;
        m_bcktfm = rhs.m_bcktfm;
        return *this;
    }

    void GTform::PrintMatrix(bool dirFlag) {
        if (dirFlag) {
            Print(m_fwdtfm);
        }else {
            Print(m_bcktfm);
        }
    }
}
