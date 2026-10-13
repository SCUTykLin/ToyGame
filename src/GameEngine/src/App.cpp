//
// Created by 15813 on 2026/8/24.
//
#include "App.h"
#include "olcConsoleGameEngine.h"
#include <vector>

namespace olc3D::math {
    struct vec3d {
        float x, y, z;
    };

    struct triangle {
        vec3d p[3];
    };

    struct mesh {
        std::vector<triangle> tri;
    };
}

namespace olc3D {
    class olcEngine3D : public olcConsoleGameEngine {
    private:
        math::mesh meshCube;

    public:
        olcEngine3D() {
            m_sAppName = L"3D Engine By OlcEngine";
        }

        bool OnUserCreate() override {
            // 按照顺时针顺序存储
            meshCube.tri = {
                // SOUTH
                {0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f},
                {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f},

                // EAST
                {1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f},
                {1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f},

                // NORTH
                {1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f},
                {1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},

                // WEST
                {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
                {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},

                // TOP
                {0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
                {0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f},

                // BOTTOM
                {1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f},
                {1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
            };
            return true;
        }

        bool OnUserUpdate(float fElapsedTime) override {
            Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);
            // Draw Triangle
            for (auto tri : meshCube.tri) {

            }
            return true;
        }
    };

    void App::run() {
        olcEngine3D demo;
        if (demo.ConstructConsole(256, 240, 4, 4)) { demo.Start(); }
    }
}
