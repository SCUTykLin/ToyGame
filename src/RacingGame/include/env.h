//
// Created by 15813 on 2026/8/23.
//

#ifndef TOYPROJECTS_RACE_ENV_H
#define TOYPROJECTS_RACE_ENV_H

#include <SDL2/SDL.h>
#include <vector>

namespace Racing{
    typedef struct Env{
        // Sky Environment
        SDL_Vertex sky[4] = {
                {
                        {0, 0},
                        {25, 25, 112, 255},
                        {0, 0}
                },{
                        {1, 0},
                        {25, 25, 112, 255},
                        {0, 0}
                },{
                        {0, 0.5},
                        {100, 200, 255, 255},
                        {0, 0}
                },{
                        {1, 0.5},
                        {100, 200, 255, 255},
                        {0, 0}
                }
        };
        int skyIdx[6] = {0, 1, 2, 1, 2, 3};

        // Left & Right Arrow
        SDL_Vertex leftArrow_1[3] = {
                {
                        {},
                        {},
                        {0, 0}
                }
        };
        SDL_Vertex leftArrow_2[4];

        SDL_Vertex rightArrow_1[3];
        SDL_Vertex rightArrow_2[4];

        // Mountain Environment
        std::vector<SDL_Vertex> mountain;
        std::vector<int> mountainIdx;
        void GenerateScaleMount(int render_width, int render_height, float fTrackCurvature);
        void renderMountain(SDL_Renderer* renderer);

        // Grass Environment
        std::vector<SDL_Vertex> leftGrass;
        std::vector<int> leftGrassIdx;
        std::vector<SDL_Vertex> rightGrass;
        std::vector<int> rightGrassIdx;

        std::vector<SDL_Vertex> leftClip;
        std::vector<int> leftClipIdx;
        std::vector<SDL_Vertex> rightClip;
        std::vector<int> rightClipIdx;

        std::vector<SDL_Vertex> road;
        std::vector<int> roadIdx;
        std::vector<int> Idx;

        std::vector<SDL_Vertex> left_mark;
        std::vector<SDL_Vertex> right_mark;

        /**
         *
         * @param render_width 画布宽度
         * @param render_height 画布长度
         * @param fCurvature 曲率
         * @param fDistance 一共走了多少距离
         * @param nTrackSection 第几段
         *
         * 生成道路，道路两边的标识，草地
         */
        void GenerateScaleGrass(int render_width, int render_height, float fCurvature, float fDistance, int nTrackSection);

        void GenerateRoadMark(int render_width, int render_height, float fCurvature, float fDistance, float curDistance);

        void scaleEnv(int render_width, int render_height);
        void scaleSky(int render_width, int render_height);

        void renderEnv(SDL_Renderer* renderer);

        void renderSky(SDL_Renderer* renderer);
        void renderGrass(SDL_Renderer* renderer);
        void renderClip(SDL_Renderer* renderer);
        void renderRoad(SDL_Renderer* renderer);
        void renderMark(SDL_Renderer* renderer);
    }Env_t;
}

#endif //TOYPROJECTS_RACE_ENV_H
