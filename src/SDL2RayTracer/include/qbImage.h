//
// Created by YangKai.Lin on 2026/7/23.
//

#ifndef TOYPROJECTS_SDL_RAYTRACING_QBIMAGE_H
#define TOYPROJECTS_SDL_RAYTRACING_QBIMAGE_H

#include <SDL2/SDL.h>
#include <vector>
#include <string>

namespace RayTracing {
    class qbImage {
    public:
        qbImage();
        ~qbImage();
        void Initialize(const int xSize, const int ySize, SDL_Renderer* pRenderer);
        void SetPixel(const int x, const int y, const double red, const double green, const double blue);
        void Display();

        int GetXSize() const;
        int GetYSize() const;
    private:
        Uint32 ConvertColor(const double red, const double green, const double blue);
        void InitTexture();

    private:
        std::vector<std::vector<double>> m_rChannel;
        std::vector<std::vector<double>> m_gChannel;
        std::vector<std::vector<double>> m_bChannel;

        int m_xSize, m_ySize;

        SDL_Renderer* m_pRenderer;
        SDL_Texture* m_pTexture;
    };
}


#endif //TOYPROJECTS_SDL_RAYTRACING_QBIMAGE_H