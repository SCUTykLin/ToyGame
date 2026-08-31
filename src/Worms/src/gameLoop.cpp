//
// Created by YangKai.Lin on 2026/8/30.
//
#include "olcConsoleGameEngine.h"

namespace Worms {

    class OneLoneCoder_Worms : public olcConsoleGameEngine {
    public:
        OneLoneCoder_Worms() {
            m_sAppName = L"Worms";
        }
    private:
        int nMapWidth = 1024;
        int nMapHeight = 512;
        unsigned char* map = nullptr;

        virtual bool OnUserCreate() override {
            map = new unsigned char[nMapWidth * nMapHeight];
            memset(map, 0, nMapWidth * nMapHeight * sizeof(unsigned char));
            return true;
        }

        virtual bool OnUserUpdate(float fElapsedTime) override {
            return true;
        }

        void CreateMap() {
            float* fSurface = new float[nMapWidth];
            float* fNoiseSeed = new float[nMapWidth];

            for (int i = 0; i < nMapWidth; i++) {
                fNoiseSeed[i] = (float)rand() / (float)RAND_MAX;
            }
            fNoiseSeed[0] = 0.5f;
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