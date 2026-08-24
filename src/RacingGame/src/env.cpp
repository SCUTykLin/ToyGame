//
// Created by 15813 on 2026/8/23.
//
#include "env.h"

namespace Racing{
    void Env::renderSky(SDL_Renderer* renderer){
        SDL_RenderGeometry(renderer, nullptr, sky, 4, skyIdx, 6);
    }

    /**
     * Only call once !!!
     */
    void Env::scaleSky(int render_width, int render_height){
        sky[0].position.x *= render_width;
        sky[0].position.y *= render_height;

        sky[1].position.x *= render_width;
        sky[1].position.y *= render_height;

        sky[2].position.x *= render_width;
        sky[2].position.y *= render_height;

        sky[3].position.x *= render_width;
        sky[3].position.y *= render_height;
    }

    void Env::scaleEnv(int render_width, int render_height) {
        scaleSky(render_width, render_height);
        int count = 0;
        for (int y = 0; y < render_height / 2; ++y){
            count += 2;
            if (count > 3) {
                Idx.push_back(count - 1);
                Idx.push_back(count - 2);
                Idx.push_back(count - 4);
                Idx.push_back(count - 4);
                Idx.push_back(count - 3);
                Idx.push_back(count - 1);
            }
        }
    }

    void Env::GenerateScaleMount(int render_width, int render_height, float fTrackCurvature) {
        mountain.clear();
        mountainIdx.clear();

        SDL_Vertex first = {
                {0, (float(render_height) / 2)},
                {120, 85, 40, 255},
                {}
        };

        SDL_Vertex second = {
                {0, (float(render_height) / 2)},
                {120, 85, 40, 255},
                {}
        };

        mountain.push_back(first);
        mountain.push_back(second);

        for (int x = 1; x < render_width; x++){
            float nHillHeight = (int)(fabs(sinf(x * 0.01f + fTrackCurvature) * 16.0f));
            SDL_Vertex temp1 = {
                    {float(x), (float(render_height) / 2) - nHillHeight},
                    {120, 85, 40, 255},
                    {}
            };
            SDL_Vertex temp2 = {
                    {float(x), (float(render_height) / 2)},
                    {210, 180, 130, 255},
                    {}
            };
            mountain.push_back(temp1);
            mountain.push_back(temp2);
            int size = mountain.size();
            mountainIdx.push_back(size-1);
            mountainIdx.push_back(size-3);
            mountainIdx.push_back(size-2);
            mountainIdx.push_back(size-2);
            mountainIdx.push_back(size-4);
            mountainIdx.push_back(size-3);
        }
    }

    void Env::renderMountain(SDL_Renderer *renderer) {
        SDL_RenderGeometry(renderer, nullptr, mountain.data(), mountain.size(),
                           mountainIdx.data(), mountainIdx.size());
    }

    void Env::GenerateScaleGrass(int render_width, int render_height, float fCurvature, float fDistance, int nTrackSection) {
        const SDL_Color darkGreen = {26, 74, 58, 255};
        const SDL_Color lightGreen = {199, 233, 192, 255};
        const SDL_Color red = {255, 99, 71, 255};
        const SDL_Color lightGray = {200, 200, 200, 255};
        const SDL_Color white = {255, 255, 255, 255};
        const SDL_Color gray = {64, 64, 64, 255};

        leftGrass.clear();
        rightGrass.clear();
        leftClip.clear();
        rightClip.clear();
        road.clear();

        auto add_element_fun = [](float x1, float y1, SDL_Color r1,
                float x2, float y2, SDL_Color r2,
                std::vector<SDL_Vertex>& container)->void{
            container.push_back({
                {x1, y1},
                r1,
                {0, 0}
            });
            container.push_back({
                {x2, y2},
                r2,
                {0, 0}
            });
        };

        for (int y = 0; y < render_height / 2; ++y){
            float fPerspective = (float)y / (render_width / 2.0f);
            float fMiddlePoint = 0.5f + fCurvature * powf(1.0f - fPerspective, 3);
            float fRoadWidth = 0.1f + fPerspective * 0.8f;
            float fClipWidth = fRoadWidth * 0.15f;
            fRoadWidth *= 0.5f;

            float nLeftGrass = (fMiddlePoint - fRoadWidth - fClipWidth) * render_width;
            float nLeftClip = (fMiddlePoint - fRoadWidth) * render_width;

            float nRightGrass = (fMiddlePoint + fRoadWidth + fClipWidth) * render_width;
            float nRightClip = (fMiddlePoint + fRoadWidth) * render_width;

            float nRow = render_height / 2.0f + y;
            SDL_Color nGrassColour = sinf(20.0f *  powf(1.0f - fPerspective,3) + fDistance * 0.1f) > 0.0f ? lightGreen : darkGreen;
            SDL_Color nClipColour = sinf(80.0f *  powf(1.0f - fPerspective, 2) + fDistance) > 0.0f ? red : white;
            SDL_Color nRoadColour = (nTrackSection - 1) == 0 ? lightGray : gray;

            // left & right Grass
            add_element_fun(0, nRow, nGrassColour, nLeftGrass, nRow, nGrassColour, leftGrass);
            add_element_fun(nRightGrass, nRow, nGrassColour, 1.0f * render_width, nRow, nGrassColour, rightGrass);

            // left & right Clip
            add_element_fun(nLeftClip, nRow, nClipColour, nLeftGrass, nRow, nClipColour, leftClip);
            add_element_fun(nRightClip, nRow, nClipColour, nRightGrass, nRow, nClipColour, rightClip);

            // Road
            add_element_fun(nLeftClip, nRow, nRoadColour, nRightClip, nRow, nRoadColour, road);
        }
    }

    void Env::renderGrass(SDL_Renderer *renderer) {
        SDL_RenderGeometry(renderer, nullptr, leftGrass.data(), leftGrass.size(),
                           Idx.data(), Idx.size());
        SDL_RenderGeometry(renderer, nullptr, rightGrass.data(), rightGrass.size(),
                           Idx.data(), Idx.size());
    }

    void Env::renderClip(SDL_Renderer *renderer) {
        SDL_RenderGeometry(renderer, nullptr, leftClip.data(), leftClip.size(),
                           Idx.data(), Idx.size());
        SDL_RenderGeometry(renderer, nullptr, rightClip.data(), rightClip.size(),
                           Idx.data(), Idx.size());
    }

    void Env::renderRoad(SDL_Renderer *renderer) {
        SDL_RenderGeometry(renderer, nullptr, road.data(), road.size(),
                           Idx.data(), Idx.size());
    }

    void Env::renderEnv(SDL_Renderer *renderer) {
        renderSky(renderer);
        renderMountain(renderer);
        renderGrass(renderer);
        renderClip(renderer);
        renderRoad(renderer);
        renderMark(renderer);
    }

    void Env::GenerateRoadMark(int render_width, int render_height, float fCurvature, float fDistance, float curDistance) {

        left_mark.clear();
        right_mark.clear();

        const SDL_Color yellow = {255, 255, 0, 255};
        const SDL_Color gray = {240, 240, 240, 255};

        static bool add_arrow = false;
        static bool finish_arrow = true;
        static float curPos = render_height / 2;
        static float triangle_size = 1.5f;

        if (static_cast<int>(fDistance) % 20 == 0 && !add_arrow && finish_arrow) {
            add_arrow = true;
            finish_arrow = false;
            curPos = render_height / 2;
        }
        else if(finish_arrow){
            add_arrow = false;
            finish_arrow = true;
            curPos = 0;
        }

        for (int y = 0; y < render_height / 2; ++y) {
            float nRow = render_height / 2.0f + y;
            float fPerspective = (float) y / (render_width / 2.0f);
            float fMiddlePoint = 0.5f + fCurvature * powf(1.0f - fPerspective, 3);
            float fRoadWidth = 0.1f + fPerspective * 0.8f;
            float size = fRoadWidth / 7.0f;
            fRoadWidth *= 0.5f;

            float yellow_mark_point = fMiddlePoint; // 中间的双形线
            float interval = size * 0.4f;
            float left_size  = size * 0.6f / 2;
            float right_point = (yellow_mark_point - interval / 2) * render_width;
            float left_point = (yellow_mark_point - interval / 2 - left_size) * render_width;

            left_mark.push_back({
                                        {left_point, nRow},
                                        yellow,
                                        {0, 0}
                                });
            left_mark.push_back({
                                        {right_point, nRow},
                                        yellow,
                                        {0, 0}
                                });

            right_mark.push_back({
                                        {left_point + interval * render_width, nRow},
                                        yellow,
                                        {0, 0}
                                });
            right_mark.push_back({
                                        {right_point + interval * render_width, nRow},
                                        yellow,
                                        {0, 0}
                                });
        }
    }

    void Env::renderMark(SDL_Renderer *renderer) {
        SDL_RenderGeometry(renderer, nullptr, left_mark.data(), left_mark.size(),
                           Idx.data(), Idx.size());
        SDL_RenderGeometry(renderer, nullptr, right_mark.data(), right_mark.size(),
                           Idx.data(), Idx.size());
    }
}