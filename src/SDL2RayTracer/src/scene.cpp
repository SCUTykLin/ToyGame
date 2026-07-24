//
// Created by YangKai.Lin on 2026/7/24.
//

#include "scene.h"

namespace RayTracing {
    Scene::Scene() {
        m_camera.SetPosition(glm::vec3(0, -10.0, 0));
        m_camera.SetLookAt(glm::vec3(0, 0, 0));
        m_camera.SetUp(glm::vec3(0, 0, 1.0));
        m_camera.SetHorzSize(0.25);
        m_camera.SetAspect(16.0 / 9.0);
        m_camera.UpdateCameraGeometry();
    }

    bool Scene::Render(qbImage &outputImage) {
        int xSize = outputImage.GetXSize();
        int ySize = outputImage.GetYSize();

        // Loop over each pixel in our image
        Ray cameraRay;
        glm::vec3 intPoint{0, 0, 0};
        glm::vec3 localNormal{0, 0, 0};
        glm::vec3 localColor{0, 0, 0};
        double xFact = 1.0 / (static_cast<double>(xSize) / 2.0);
        double yFact = 1.0 / (static_cast<double>(ySize) / 2.0);
        double minDist = 1e6;
        double maxDist = 0.0;

        for (int x = 0; x < xSize; ++x) {
            for (int y = 0; y < ySize; ++y) { // Normalize the x and y coordinates.
                double normX = (static_cast<double>(x) * xFact) - 1.0; // normX is in [-1, 1]
                double normY = (static_cast<double>(y) * yFact) - 1.0;

                // Generate the ray for this pixel.
                m_camera.GenerateRay(normX, normY, cameraRay);

                // Test
                bool validInt = m_testSphere.TestIntersections(cameraRay, intPoint, localNormal, localColor);
                if (validInt) {
                    double dist = glm::length(intPoint - cameraRay.m_point1);
                    if (dist > maxDist) maxDist = dist;
                    if (dist < minDist) minDist = dist;
                    outputImage.SetPixel(x, y, 255.0 - ((dist - 9.0) / 0.94605) * 255, 0, 0);
                }else {
                    outputImage.SetPixel(x, y, 0, 0, 0);
                }
            }
        }

        return true;
    }
}