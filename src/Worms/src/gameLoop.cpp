//
// Created by YangKai.Lin on 2026/8/30.
//
#include <complex>
#include <variant>

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

        int nBounceBeforeDeath = -1;
        bool bDead = false;

        cPhysicsObject(float x = 0.0f, float y = 0.0f) {
            px = x;
            py = y;
        }

        virtual void Draw(olcConsoleGameEngine* engine, float fOffsetX, float fOffsetY) = 0;
        virtual int BounceDeathAction() = 0;
    };

    class cDummy : public cPhysicsObject {
    public:
        cDummy(float x = 0.0f, float y = 0.0f) : cPhysicsObject(x, y) {

        }

        void Draw(olcConsoleGameEngine *engine, float fOffsetX, float fOffsetY) override {
            engine->DrawWireFrameModel(vecModel, px - fOffsetX, py - fOffsetY, atan2f(vy, vx), radius, FG_WHITE);
        }

        int BounceDeathAction() override {
            return 0;
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

        virtual int BounceDeathAction() override{
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

    class cMissile : public cPhysicsObject{
        public:
	        cMissile(float x = 0.0f, float y = 0.0f, float _vx = 0.0f, float _vy = 0.0f) : cPhysicsObject(x, y){
		        radius = 2.5f;
		        fFriction = 0.5f;
		        vx = _vx;
		        vy = _vy;
		        bDead = false;
		        nBounceBeforeDeath = 1;
	        }

	        virtual void Draw(olcConsoleGameEngine *engine, float fOffsetX, float fOffsetY){
		        engine->DrawWireFrameModel(vecModel, px - fOffsetX, py - fOffsetY, atan2f(vy, vx), radius, FG_YELLOW);
	        }

	        virtual int BounceDeathAction(){
		        return 20;
	        }

    private:
	    static std::vector<std::pair<float, float>> vecModel;
    };

    std::vector<std::pair<float, float>> DefineMissile(){
        std::vector<std::pair<float, float>> vecModel;
        vecModel.push_back({ 0.0f, 0.0f });
        vecModel.push_back({ 1.0f, 1.0f });
        vecModel.push_back({ 2.0f, 1.0f });
        vecModel.push_back({ 2.5f, 0.0f });
        vecModel.push_back({ 2.0f, -1.0f });
        vecModel.push_back({ 1.0f, -1.0f });
        vecModel.push_back({ 0.0f, 0.0f });
        vecModel.push_back({ -1.0f, -1.0f });
        vecModel.push_back({ -2.5f, -1.0f });
        vecModel.push_back({ -2.0f, 0.0f });
        vecModel.push_back({ -2.5f, 1.0f });
        vecModel.push_back({ -1.0f, 1.0f });

        for (auto &v : vecModel){
            v.first /= 2.5f; v.second /= 2.5f;
        }
        return vecModel;
    }
    std::vector<std::pair<float, float>> cMissile::vecModel = DefineMissile();

    class cWorm : public cPhysicsObject {
    public:
        cWorm(float x = 0.0f, float y = 0.0f) : cPhysicsObject(x, y) {
            radius = 3.5f;
            fFriction = 0.2f;
            bDead = false;
            nBounceBeforeDeath = -1;

            if (sprWorm == nullptr) {
                sprWorm = new olcSprite(L"src/Worms/assets/worms1.spr");
            }
        }

        virtual void Draw(olcConsoleGameEngine *engine, float fOffsetX, float fOffsetY){
            engine->DrawPartialSprite(px - fOffsetX - radius, py - fOffsetY - radius, sprWorm, 0, 0, 8, 8);
        }

        virtual int BounceDeathAction(){
            return 0; // Nothing
        }

    public:
        float fShootAngle = 0.0f;

    private:
        static olcSprite *sprWorm;
    };

    olcSprite* cWorm::sprWorm = nullptr;

    class OneLoneCoder_Worms : public olcConsoleGameEngine {
    public:
        OneLoneCoder_Worms() {
            m_sAppName = L"Worms";
        }
    private:
        int nMapWidth = 1024;
        int nMapHeight = 512;
        unsigned char* map = nullptr;

        float fCameraPosX = 0.0f; // 相机左上角
        float fCameraPosY = 0.0f;
        float fCameraPosXTarget = 0.0f;
        float fCameraPosYTarget = 0.0f;

        std::list<std::unique_ptr<cPhysicsObject>> listObjects;
        cPhysicsObject* pObjectUnderControl = nullptr;
        cPhysicsObject* pCameraTrackingObject = nullptr;
        bool bEnergising = false;
        float fEnergyLevel = 0.0f;
        bool bFireWeapon = false;

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

            if (m_mouse[0].bReleased) {
                Boom(m_mousePosX + fCameraPosX, m_mousePosY + fCameraPosY, 10.0f);
            }

            if (m_mouse[2].bReleased) {
                cWorm* worm = new cWorm(m_mousePosX + fCameraPosX, m_mousePosY + fCameraPosY);
                pObjectUnderControl = worm;
                pCameraTrackingObject = worm;
                listObjects.push_back(std::unique_ptr<cPhysicsObject>(worm));
            }

            if (m_mouse[1].bReleased) {
                listObjects.push_back(std::make_unique<cMissile>(m_mousePosX + fCameraPosX, m_mousePosY + fCameraPosY));
            }

            float fMapScrollSpeed = 400.0f;
            if (m_mousePosX < 5) fCameraPosX -= fMapScrollSpeed * fElapsedTime;
            if (m_mousePosX > ScreenWidth() - 5) fCameraPosX += fMapScrollSpeed * fElapsedTime;
            if (m_mousePosY < 5) fCameraPosY -= fMapScrollSpeed * fElapsedTime;
            if (m_mousePosY > ScreenHeight() - 5) fCameraPosY += fMapScrollSpeed * fElapsedTime;

            if (pObjectUnderControl != nullptr) {
                if (pObjectUnderControl->bStable) {
                    if (m_keys[L'Z'].bPressed) {
                        float a = ((cWorm*)pObjectUnderControl)->fShootAngle;
                        pObjectUnderControl->vx = 4.0f * cosf(a);
                        pObjectUnderControl->vy = 8.0f * sinf(a);
                        pObjectUnderControl->bStable = false;
                    }
                    if (m_keys[L'A'].bPressed) {
                        cWorm* worm = (cWorm *)pObjectUnderControl;
                        worm->fShootAngle -= 1.0f * fElapsedTime;
                        if (worm->fShootAngle < -3.14159f) worm->fShootAngle += 3.14159f * 2.0f;
                    }
                    if (m_keys[L'S'].bPressed) {
                        cWorm* worm = (cWorm *)pObjectUnderControl;
                        worm->fShootAngle += 1.0f * fElapsedTime;
                        if (worm->fShootAngle > 3.14159f) worm->fShootAngle -= 3.14159f * 2.0f;
                    }
                    if (m_keys[VK_SPACE].bPressed) {
                        bEnergising = true;
                        bFireWeapon = false;
                        fEnergyLevel = 0.0f;
                    }
                    if (m_keys[VK_SPACE].bHeld) {
                        if (bEnergising) {
                            fEnergyLevel += 0.75f * fElapsedTime;
                            if (fEnergyLevel >= 1.0f) {
                                fEnergyLevel = 0.0f;
                            }
                        }
                    }

                    if (m_keys[VK_SPACE].bReleased) {
                        if (bEnergising) {
                            bFireWeapon = true;
                        }
                        bEnergising = false;
                    }
                }

                if (bFireWeapon) {
                    cWorm* worm = (cWorm *)pObjectUnderControl;

                    // Get Weapon Origin
                    float ox = worm->px;
                    float oy = worm->py;

                    // Get Weapon Direction
                    float dx = cosf(worm->fShootAngle);
                    float dy = sinf(worm->fShootAngle);

                    // Create Weapon Object
                    cMissile *m = new cMissile(ox, oy, dx * 40.0f * fEnergyLevel, dy * 40.0f * fEnergyLevel);
                    listObjects.push_back(std::unique_ptr<cMissile>(m));
                    pCameraTrackingObject = m;

                    bFireWeapon = false;
                    fEnergyLevel = 0.0f;
                    bEnergising = false;
                }
            }

            if (pCameraTrackingObject != nullptr) {
                // fCameraPosX = pCameraTrackingObject->px - ScreenWidth() / 2.0f; // (fCameraPosX, fCameraPosY)表示相机左上角
                // fCameraPosY = pCameraTrackingObject->py - ScreenHeight() / 2.0f;
                fCameraPosXTarget = pCameraTrackingObject->px - ScreenWidth() / 2.0f;
                fCameraPosYTarget = pCameraTrackingObject->py - ScreenHeight() / 2.0f;
                fCameraPosX += (fCameraPosXTarget - fCameraPosX) * 5.0f * fElapsedTime;
                fCameraPosY += (fCameraPosYTarget - fCameraPosY) * 5.0f * fElapsedTime;
            }

            if (fCameraPosX < 0) fCameraPosX = 0;
            if (fCameraPosX >= nMapWidth - ScreenWidth()) fCameraPosX = nMapWidth - ScreenWidth();
            if (fCameraPosY < 0) fCameraPosY = 0;
            if (fCameraPosY >= nMapHeight - ScreenHeight()) fCameraPosY = nMapHeight - ScreenHeight();

            for (int i = 0; i < 10; ++i) {
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

                        if (p->nBounceBeforeDeath > 0) {
                            p->bDead = (--p->nBounceBeforeDeath == 0);

                            if (p->bDead) {
                                int nResponse = p->BounceDeathAction();
                                if (nResponse > 0) {
                                    Boom(p->px, p->py, nResponse);
                                    pCameraTrackingObject = pObjectUnderControl;
                                }
                            }
                        }
                    }else{
                        p->px = fPotentialX;
                        p->py = fPotentialY;
                    }

                    if (fMagVelocity < 0.1f) p->bStable = true;
                }

                listObjects.remove_if([](auto& c){return c->bDead;});
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
                cWorm* worm = (cWorm*)pObjectUnderControl;
                if (p.get() == worm) {
                    float cx = worm->px + 8.0f * cosf(worm->fShootAngle) - fCameraPosX;
                    float cy = worm->py + 8.0f * sinf(worm->fShootAngle) - fCameraPosY;

                    // Draw "+" symbol
                    Draw(cx, cy, PIXEL_SOLID, FG_BLACK);
                    Draw(cx + 1, cy, PIXEL_SOLID, FG_BLACK);
                    Draw(cx - 1, cy, PIXEL_SOLID, FG_BLACK);
                    Draw(cx, cy + 1, PIXEL_SOLID, FG_BLACK);
                    Draw(cx, cy - 1, PIXEL_SOLID, FG_BLACK);

                    // Draws an Energy Bar, indicating how much energy should the weapon be
                    // fired with
                    for (int i = 0; i < 11 * fEnergyLevel; i++){
                        Draw(worm->px - 5 + i - fCameraPosX, worm->py - 12 - fCameraPosY, PIXEL_SOLID, FG_GREEN);
                        Draw(worm->px - 5 + i - fCameraPosX, worm->py - 11 - fCameraPosY, PIXEL_SOLID, FG_RED);
                    }
                }
            }

            return true;
        }

        void Boom(float fWorldX, float fWorldY, float fRadius) {
            auto CircleBresenham = [&](int xc, int yc, int r)
            {
                // Taken from wikipedia
                int x = 0;
                int y = r;
                int p = 3 - 2 * r;
                if (!r) return;

                auto drawline = [&](int sx, int ex, int ny)
                {
                    for (int i = sx; i < ex; i++)
                        if (ny >= 0 && ny < nMapHeight && i >= 0 && i < nMapWidth)
                            map[ny*nMapWidth + i] = 0;
                };

                while (y >= x)
                {
                    // Modified to draw scan-lines instead of edges
                    drawline(xc - x, xc + x, yc - y);
                    drawline(xc - y, xc + y, yc - x);
                    drawline(xc - x, xc + x, yc + y);
                    drawline(xc - y, xc + y, yc + x);
                    if (p < 0) p += 4 * x++ + 6;
                    else p += 4 * (x++ - y--) + 10;
                }
            };

            CircleBresenham(fWorldX, fWorldY, fRadius);

            for (auto& p : listObjects) {
                float dx = p->px - fWorldX;
                float dy = p->py - fWorldY;
                float fDist = sqrtf(dx * dx + dy * dy);
                if (fDist < 0.0001f) fDist = 0.0001f;
                if (fDist < fRadius) {
                    p->vx = (dx / fDist) * fRadius;
                    p->vy = (dy / fDist) * fRadius;
                    p->bStable = false;
                }
            }

            for (int i = 0; i < (int)fRadius; ++i) {
                listObjects.push_back(std::make_unique<cDebris>(fWorldX, fWorldY));
            }
        }

        void CreateMap() {
            auto* fSurface = new float[nMapWidth];
            auto* fNoiseSeed = new float[nMapWidth];

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