//
// Created by YangKai.Lin on 2026/7/24.
//

#include "scene.h"
#include <iostream>
#include "ray_math.h"

namespace RayTracing {
    Scene::Scene(float fov, float width, float height) : fov(fov), width(width), height(height) {
        m_camera.SetPosition(glm::vec3(0, -10.0, -1.0));
        m_camera.SetLookAt(glm::vec3(0, 0, 0));
        m_camera.SetUp(glm::vec3(0, 0, 1.0));

        m_camera.SetLength(computeFocus(GetWidth(), GetFOVRad()));
        m_camera.SetHorzSize(GetWidth() / 2.0f);

        m_camera.SetAspect(16.0 / 9.0);
        m_camera.UpdateCameraGeometry();

        std::cout << m_camera.toString();

        // Construct a test sphere
        m_objectList.push_back(std::make_shared<Objsphere>());
        m_objectList.push_back(std::make_shared<Objsphere>());
        m_objectList.push_back(std::make_shared<Objsphere>());

        // Construct a test plane.
        m_objectList.push_back(std::make_shared<ObjPlane>());
        m_objectList.at(3)->m_baseColor = glm::vec3(0.5, 0.5, 0.5);

        // Define a transform for the plane.
        GTform planeMatrix;
        planeMatrix.SetTransform(glm::vec3{0.0, 0.0, 0.75},
            glm::vec3{0.0, 0.0, 0.0},
            glm::vec3{4.0, 4.0, 1.0});
        m_objectList.at(3)->SetTransformMatrix(planeMatrix);

        // Modify the spheres.
        GTform testMatrix1, testMatrix2, testMatrix3;
        testMatrix1.SetTransform(glm::vec3{-1.5, 0.0, 0.0},
            glm::vec3{0.0, 0.0, 0.0},
            glm::vec3{0.5, 0.5, 0.75});
        testMatrix2.SetTransform(glm::vec3{0.0, 0.0, 0.0},
            glm::vec3{0.0, 0.0, 0.0},
            glm::vec3{0.75, 0.5, 0.5});
        testMatrix3.SetTransform(glm::vec3{1.5, 0.0, 0.0},
            glm::vec3{0.0, 0.0, 0.0},
            glm::vec3{0.75, 0.75, 0.75});

        m_objectList.at(0)->SetTransformMatrix(testMatrix1);
        m_objectList.at(1)->SetTransformMatrix(testMatrix2);
        m_objectList.at(2)->SetTransformMatrix(testMatrix3);

        m_objectList.at(0)->m_baseColor = glm::vec3{0.25, 0.5, 0.8};
        m_objectList.at(1)->m_baseColor = glm::vec3{1.0, 0.5, 0.0};
        m_objectList.at(2)->m_baseColor = glm::vec3{1.0, 0.8, 0.0};

        // Construct a test light
        m_lightList.push_back(std::make_shared<Pointlight>());
        m_lightList.at(0)->m_location = glm::vec3(5.0, -10.0, -5.0);
        m_lightList.at(0)->m_color = glm::vec3(1.0, 1.0, 1.0);
    }

    void Scene::SetFOV(float fov){
        this->fov = fov;
    }

    float Scene::GetFOV() {
        return fov;
    }

    float Scene::GetFOVRad() {
        return fov * PI<float> / 180.0;
    }

    void Scene::SetWidth(float Width) {
        this->width = Width;
    }

    float Scene::GetWidth() {
        return width;
    }

    void Scene::SetHeight(float Height) {
        this->height = Height;
    }

    float Scene::GetHeight() {
        return height;
    }

    bool Scene::Render(qbImage &outputImage) {
        int xSize = outputImage.GetXSize();
        int ySize = outputImage.GetYSize();

        std::cout << "Start Rendering ..." << std::endl;

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

                // Test for intersections with all objects in the scene.
                std::shared_ptr<ObjectBase> closestObject;
                glm::vec3 closestPoint{0.0, 0.0, 0.0};
                glm::vec3 closestIntPoint{0.0, 0.0, 0.0};
                glm::vec3 closestLocalNormal{0.0, 0.0, 0.0};
                glm::vec3 closestLocalColor{0.0, 0.0, 0.0};
                float minDist = 1e6;
                bool intersectionFound = false;

                for (auto currentObject : m_objectList) {
                    bool validInt = currentObject->TestIntersections(cameraRay, intPoint, localNormal, localColor);
                    if (validInt) {
                        // Set the flag to indicate that we found an intersection.
                        intersectionFound = true;

                        // Compute the distance between the camera and the point of intersection.
                        double dist = glm::length(intPoint - cameraRay.m_point1);

                        // If this object is closer to the camera than any one that we have seen before, then store a reference to it.
                        if (dist < minDist) {
                            minDist = dist;
                            closestObject = currentObject;
                            closestIntPoint = intPoint;
                            closestLocalNormal = localNormal;
                            closestLocalColor = localColor;
                        }
                    }
                }
                // Compute the illumination for the closest object, assuming that there was a valid intersection.
                if (intersectionFound) {
                    // Compute the intensity of illumination.
                    double intensity;
                    glm::vec3 color{0.0, 0.0, 0.0};
                    double red = 0.0;
                    double green = 0.0;
                    double blue = 0.0;
                    bool validIllum = false;
                    bool illumFound = false;

                    for (auto currentLight : m_lightList) {
                        validIllum = currentLight->ComputeIllumination(closestIntPoint, closestLocalNormal, m_objectList, closestObject, color, intensity);
                        if (validIllum) {
                            illumFound = true;
                            red += color.x * intensity;
                            green += color.y * intensity;
                            blue += color.z * intensity;
                        }
                    }
                    if (illumFound) {
                        red *= closestLocalColor.x;
                        green *= closestLocalColor.y;
                        blue *= closestLocalColor.z;
                        outputImage.SetPixel(x, y, red, green, blue);
                    }
                }

            }
        }
        std::cout << "End Rendering ..." << std::endl;
        return true;
    }
}