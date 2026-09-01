//
// Created by YangKai.Lin on 2026/8/30.
//
#include "olcConsoleGameEngine.h"

namespace Worms {

    class cPhysicsObject {
    public:
        float px = 0.0f;
        float py = 0.0f;
        float vx = 0.0f;
        float vy = 0.0f;
        float ax = 0.0f;
        float ay = 0.0f;

        float radius = 4.0f;
        bool bStable = false;

        cPhysicsObject(float x = 0.0f, float y = 0.0f) {
            px = x;
            py = y;
        }

        virtual void Draw(olcConsoleGameEngine* engine, float fOffsetX, float fOffsetY) = 0;
    };

    class cDummy : public cPhysicsObject {
    public:
        cDummy(float x = 0.0f, float y = 0.0f) : cPhysicsObject(x, y) {

        }

        void Draw(olcConsoleGameEngine *engine, float fOffsetX, float fOffsetY) override {
            engine->DrawWireFrameModel(vecModel, px - fOffsetX, py - fOffsetY, atan2f(vy, vx), radius, FG_WHITE);
        }
    private:
        static std::vector<std::pair<float, float>> vecModel;
    };

    std::vector<std::pair<float, float>> DefineDummy(){
        std::vector<std::pair<float, float>> vecModel;
        vecModel.emplace_back(0.0f, 0.0f);
        for (int i = 0; i < 10; ++i) {
            vecModel.emplace_back(cosf(i / 9.0f * 2.0f * 3.14159f), sinf(i / 9.0f * 2.0f * 3.14159f));
        }
        return vecModel;
    }

    std::vector<std::pair<float, float>> cDummy::vecModel = DefineDummy();

    class OneLoneCoder_Worms : public olcConsoleGameEngine {
    public:
        OneLoneCoder_Worms() {
            m_sAppName = L"Worms";
        }
    private:
        int nMapWidth = 1024;
        int nMapHeight = 512;
        unsigned char* map = nullptr;

        float fCameraPosX = 0.0f;
        float fCameraPosY = 0.0f;

        std::list<cPhysicsObject*> listObjects;

        virtual bool OnUserCreate() override {
            map = new unsigned char[nMapWidth * nMapHeight];
            memset(map, 0, nMapWidth * nMapHeight * sizeof(unsigned char));
            CreateMap();
            return true;
        }

        virtual bool OnUserUpdate(float fElapsedTime) override {
            if (m_keys[L'M'].bReleased) {
                CreateMap();
            }

            if (m_mouse[2].bReleased) {
                cDummy* p = new cDummy(m_mousePosX + fCameraPosX, m_mousePosY + fCameraPosY);
                listObjects.push_back(p);
            }

            float fMapScrollSpeed = 400.0f;
            if (m_mousePosX < 5) fCameraPosX -= fMapScrollSpeed * fElapsedTime;
            if (m_mousePosX > ScreenWidth() - 5) fCameraPosX += fMapScrollSpeed * fElapsedTime;
            if (m_mousePosY < 5) fCameraPosY -= fMapScrollSpeed * fElapsedTime;
            if (m_mousePosY > ScreenHeight() - 5) fCameraPosY += fMapScrollSpeed * fElapsedTime;

            if (fCameraPosX < 0) fCameraPosX = 0;
            if (fCameraPosX >= nMapWidth - ScreenWidth()) fCameraPosX = nMapWidth - ScreenWidth();
            if (fCameraPosY < 0) fCameraPosY = 0;
            if (fCameraPosY >= nMapHeight - ScreenHeight()) fCameraPosY = nMapHeight - ScreenHeight();

            for (int x = 0; x < ScreenWidth(); ++x) {
                for (int y = 0; y < ScreenHeight(); ++y) {
                    switch (map[(y + (int)fCameraPosY) * nMapWidth + (x + (int)fCameraPosX)]) {
                        case 0:
                            Draw(x, y, PIXEL_SOLID, FG_CYAN);
                            break;
                        case 1:
                            Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
                            break;
                    }
                }
            }

            for (auto &p : listObjects) {
                p->Draw(this, fCameraPosX, fCameraPosY);
            }

            return true;
        }

        void CreateMap() {
            float* fSurface = new float[nMapWidth];
            float* fNoiseSeed = new float[nMapWidth];

            for (int i = 0; i < nMapWidth; i++) {
                fNoiseSeed[i] = (float)rand() / (float)RAND_MAX;
            }
            fNoiseSeed[0] = 0.5f;
            PerlinNoise1D(nMapWidth, fNoiseSeed, 8, 2.0f, fSurface);

            for (int x = 0; x < nMapWidth; x++) {
                for (int y = 0; y < nMapHeight; y++) {
                    if (y >= fSurface[x] * nMapHeight) {
                        map[y * nMapWidth + x] = 1.0f;
                    }else {
                        map[y * nMapWidth + x] = 0.0f;
                    }
                }
            }

            delete[] fSurface;
            delete[] fNoiseSeed;
        }

        void PerlinNoise1D(int nCount, float *fSeed, int nOctaves, float fBias, float *fOutput){
            // Used 1D Perlin Noise
            for (int x = 0; x < nCount; x++)
            {
                float fNoise = 0.0f;
                float fScaleAcc = 0.0f;
                float fScale = 1.0f;

                for (int o = 0; o < nOctaves; o++)
                {
                    int nPitch = nCount >> o;
                    int nSample1 = (x / nPitch) * nPitch;
                    int nSample2 = (nSample1 + nPitch) % nCount;
                    float fBlend = (float)(x - nSample1) / (float)nPitch;
                    float fSample = (1.0f - fBlend) * fSeed[nSample1] + fBlend * fSeed[nSample2];
                    fScaleAcc += fScale;
                    fNoise += fSample * fScale;
                    fScale = fScale / fBias;
                }

                // Scale to seed range
                fOutput[x] = fNoise / fScaleAcc;
            }
        }
    };

    void gameLoop() {
        OneLoneCoder_Worms worms;
        worms.ConstructConsole(256, 160, 6, 6);
        worms.Start();
    }
}