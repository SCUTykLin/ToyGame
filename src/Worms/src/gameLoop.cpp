//
// Created by YangKai.Lin on 2026/8/30.
//
#include <complex>

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
        float fFriction = 0.8f;

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

    class cDebris : public cPhysicsObject {
    public:
        cDebris(float x = 0, float y = 0) : cPhysicsObject(x, y){
            vx = 10.0f * cosf(((float)rand() / (float)RAND_MAX) * 2.0f * 3.14159f);
            vy = 10.0f * sinf(((float)rand() / (float)RAND_MAX) * 2.0f * 3.14159f);
            radius = 1.0f;
            fFriction = 0.8f;
            nBounceBeforeDeath = 5; // After 5 bounces, dispose
        }
        virtual void Draw(olcConsoleGameEngine *engine, float fOffsetX, float fOffsetY){
            engine->DrawWireFrameModel(vecModel, px - fOffsetX, py - fOffsetY, atan2f(vy, vx), radius, FG_DARK_GREEN);
        }

        virtual int BounceDeathAction(){
            return 0; // Nothing, just fade
        }
    private:
        static std::vector<std::pair<float, float>> vecModel;
    };

    std::vector<std::pair<float, float>> DefineDebris() {
        std::vector<std::pair<float, float>> vecModel;
        vecModel.emplace_back(0.0f, 0.0f);
        vecModel.emplace_back(1.0f, 0.0f);
        vecModel.emplace_back(1.0f, 1.0f);
        vecModel.emplace_back(0.0f, 1.0f);
        return vecModel;
    }

    std::vector<std::pair<float, float>> cDebris::vecModel = DefineDebris();

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

        bool OnUserCreate() override {
            map = new unsigned char[nMapWidth * nMapHeight];
            memset(map, 0, nMapWidth * nMapHeight * sizeof(unsigned char));
            CreateMap();
            return true;
        }

        bool OnUserUpdate(float fElapsedTime) override {
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

            for (int i = 0; i < 10; ++i)
                for (auto& p : listObjects) {
                    p->ay += 2.0f;
                    p->vx += p->ax * fElapsedTime;
                    p->vy += p->ay * fElapsedTime;

                    float fPotentialX = p->px + p->vx * fElapsedTime;
                    float fPotentialY = p->py + p->vy * fElapsedTime;

                    p->ax = 0.0f;
                    p->ay = 0.0f;
                    p->bStable = false;

                    float fAngle = atan2f(p->vy, p->vx);
                    float fResponseX = 0;
                    float fResponseY = 0;
                    bool bCollision = false;

                    for (float r = fAngle - 3.14159f / 2.0f; r < fAngle + 3.14159f / 2.0f; r += 3.14159f / 8.0f) {
                        float fTestPosX = (p->radius) * cosf(r) + fPotentialX;
                        float fTestPosY = (p->radius) * sinf(r) + fPotentialY;

                        if (fTestPosX >= nMapWidth) fPotentialX = nMapWidth - 1;
                        if (fTestPosY >= nMapHeight) fPotentialY = nMapHeight - 1;
                        if (fTestPosX < 0) fTestPosX = 0;
                        if (fTestPosY < 0) fTestPosY = 0;
                        if (map[(int)fTestPosY * nMapWidth + (int)fTestPosX] != 0) {
                            fResponseX += fPotentialX - fTestPosX;
                            fResponseY += fPotentialY - fTestPosY;
                            bCollision = true;
                        }
                    }

                    float fMagVelocity = std::sqrtf(p->vx *p->vx + p->vy *p->vy);
                    float fMagRespone = std::sqrtf(fResponseX * fResponseX + fResponseY * fResponseY);
                    if (bCollision) {
                        p->bStable = true;

                        float dot = p->vx * (fResponseX / fMagRespone) + p->vy * (fResponseY / fMagRespone);
                        p->vx = p->fFriction * (-2.0f * dot * (fResponseX / fMagRespone) + p->vx);
                        p->vy = p->fFriction * (-2.0f * dot * (fResponseY / fMagRespone) + p->vy);
                    }else{
                        p->px = fPotentialX;
                        p->py = fPotentialY;
                    }

                    if (fMagVelocity < 0.1f) p->bStable = true;
                }

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
        worms.ConstructConsole(256, 160, 4, 4);
        worms.Start();
    }
}