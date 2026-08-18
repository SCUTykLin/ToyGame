//
// Created by 15813 on 2026/8/17.
//

#include "olcConsoleGameEngine.h"

namespace Racing {
class OneLoneCoder_FormulaOLC : public olcConsoleGameEngine{
public:
    OneLoneCoder_FormulaOLC(){
        m_sAppName = L"Formula OLC";
    }
protected:
    virtual bool OnUserCreate(){
        vecTrack.push_back(std::make_pair(0.0f, 10.0f));		// Short section for start/finish line
        vecTrack.push_back(std::make_pair(0.0f, 200.0f));
        vecTrack.push_back(std::make_pair(1.0f, 200.0f));
        vecTrack.push_back(std::make_pair(0.0f, 400.0f));
        vecTrack.push_back(std::make_pair(-1.0f, 100.0f));
        vecTrack.push_back(std::make_pair(0.0f, 200.0f));
        vecTrack.push_back(std::make_pair(-1.0f, 200.0f));
        vecTrack.push_back(std::make_pair(1.0f, 200.0f));
        vecTrack.push_back(std::make_pair(0.0f, 200.0f));
        vecTrack.push_back(std::make_pair(0.2f, 500.0f));
        vecTrack.push_back(std::make_pair(0.0f, 200.0f));

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime){

        if (m_keys[VK_UP].bHeld){
            fDistance += 100.f * fElapsedTime;
        }

        Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ', 0);

        for (int y = 0; y < ScreenHeight() / 2; ++y){
            for (int x = 0; x < ScreenWidth(); ++x){
                float fPerspective = (float)y / (ScreenWidth() / 2.0f);
                float fMiddlePoint = 0.5f;
                float fRoadWidth = 0.1f + fPerspective * 0.8f;
                float fClipWidth = fRoadWidth * 0.15f;
                fRoadWidth *= 0.5f;

                int nLeftGrass = (fMiddlePoint - fRoadWidth - fClipWidth) * ScreenWidth();
                int nLeftClip = (fMiddlePoint - fRoadWidth) * ScreenWidth();

                int nRightGrass = (fMiddlePoint + fRoadWidth + fClipWidth) * ScreenWidth();
                int nRightClip = (fMiddlePoint + fRoadWidth) * ScreenWidth();

                int nRow = ScreenHeight() / 2 + y;
                int nGrassColour = sinf(20.0f *  powf(1.0f - fPerspective,3) + fDistance * 0.1f) > 0.0f ? FG_GREEN : FG_DARK_GREEN;
                int nClipColour = sinf(80.0f *  powf(1.0f - fPerspective, 2) + fDistance) > 0.0f ? FG_RED : FG_WHITE;

                if (x >= 0 && x < nLeftGrass){
                    Draw(x, nRow, PIXEL_SOLID, nGrassColour);
                }
                if (x >= nLeftGrass && x < nLeftClip){
                    Draw(x, nRow, PIXEL_SOLID, nClipColour);
                }
                if (x >= nLeftClip && x < nRightClip){
                    Draw(x, nRow, PIXEL_SOLID, FG_GREY);
                }
                if (x >= nRightClip && x < nRightGrass){
                    Draw(x, nRow, PIXEL_SOLID, nClipColour);
                }
                if (x >= nRightGrass && x < ScreenWidth()){
                    Draw(x, nRow, PIXEL_SOLID, nGrassColour);
                }
            }
        }

        int nCarPos = ScreenWidth() / 2 + ((int)(ScreenWidth() * fCarPos) / 2.0f) - 7;

        DrawStringAlpha(nCarPos, 80, L"   ||####||   ");
        DrawStringAlpha(nCarPos, 81, L"      ##      ");
        DrawStringAlpha(nCarPos, 82, L"     ####     ");
        DrawStringAlpha(nCarPos, 83, L"     ####     ");
        DrawStringAlpha(nCarPos, 84, L"|||  ####  |||");
        DrawStringAlpha(nCarPos, 85, L"|||########|||");
        DrawStringAlpha(nCarPos, 86, L"|||  ####  |||");

        return true;
    }

private:
    float fCarPos = 0.0f;
    float fDistance = 0.0f;
    std::vector<std::pair<float, float>> vecTrack; // curvature, distance
};

    void gameLoop(){
        OneLoneCoder_FormulaOLC game;
        game.ConstructConsole(160, 100, 8, 8);
        game.Start();
    }
}