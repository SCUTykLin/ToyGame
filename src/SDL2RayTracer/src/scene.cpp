//
// Created by YangKai.Lin on 2026/7/24.
//

#include "scene.h"

namespace RayTracing {
    Scene::Scene() {
        m_camera.SetPosition(glm::vec3(0, -10.0, 0));
        m_camera.SetLookAt(glm::vec3(0, 0, 0));
    }

    bool Scene::Render(qbImage &outputImage) {
        int xSize = outputImage.GetXSize();
        int ySize = outputImage.GetYSize();


        return true;
    }
}