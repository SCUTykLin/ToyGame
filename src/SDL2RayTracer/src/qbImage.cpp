//
// Created by YangKai.Lin on 2026/7/23.
//

#include "qbImage.h"

namespace RayTracing {
    qbImage::qbImage() {
        m_xSize = 0;
        m_ySize = 0;
        m_pTexture = nullptr;
    }

    qbImage::~qbImage() {
        if (m_pTexture != nullptr) {
            SDL_DestroyTexture(m_pTexture);
        }
    }

    void qbImage::Initialize(const int xSize, const int ySize, SDL_Renderer *pRenderer) {
        m_rChannel.resize(xSize, std::vector<double>(ySize, 0.0));
        m_gChannel.resize(xSize, std::vector<double>(ySize, 0.0));
        m_bChannel.resize(xSize, std::vector<double>(ySize, 0.0));

        m_xSize = xSize;
        m_ySize = ySize;

        m_pRenderer = pRenderer;

        InitTexture();
    }

    void qbImage::SetPixel(const int x, const int y, const double red, const double green, const double blue) {
        m_rChannel.at(x).at(y) = red;
        m_gChannel.at(x).at(y) = green;
        m_bChannel.at(x).at(y) = blue;
    }

    void qbImage::UpdateTexture() {
        Uint32* tempPixels = new Uint32[m_xSize * m_ySize];
        memset(tempPixels, 0, m_xSize * m_ySize * sizeof(Uint32));
        for (int x = 0; x < m_xSize; x++) {
            for (int y = 0; y < m_ySize; y++) {
                tempPixels[y * m_xSize + x] = ConvertColor(m_rChannel.at(x).at(y), m_gChannel.at(x).at(y), m_bChannel.at(x).at(y));
            }
        }

        SDL_UpdateTexture(m_pTexture, nullptr, tempPixels, m_xSize * sizeof(Uint32));
        delete[] tempPixels;
    }

    void qbImage::Display() {

        ComputeMaxValues();

        Uint32* tempPixels = new Uint32[m_xSize * m_ySize];
        memset(tempPixels, 0, m_xSize * m_ySize * sizeof(Uint32));
        for (int x = 0; x < m_xSize; x++) {
            for (int y = 0; y < m_ySize; y++) {
                tempPixels[y * m_xSize + x] = ConvertColor(m_rChannel.at(x).at(y), m_gChannel.at(x).at(y), m_bChannel.at(x).at(y));
            }
        }

        SDL_UpdateTexture(m_pTexture, nullptr, tempPixels, m_xSize * sizeof(Uint32));
        delete[] tempPixels;

        // Copy texture to the renderer
        SDL_Rect srcRect, bounds;
        srcRect.x = 0;
        srcRect.y = 0;
        srcRect.w = m_xSize;
        srcRect.h = m_ySize;
        bounds = srcRect;
        SDL_RenderCopy(m_pRenderer, m_pTexture, &srcRect, &bounds);
    }

    int qbImage::GetXSize() const {
        return m_xSize;
    }

    int qbImage::GetYSize() const {
        return m_ySize;
    }

    Uint32 qbImage::ConvertColor(const double red, const double green, const double blue) {

        auto scale_fun = [this](double c){ return (c / this->m_overallMax) * 255;};

        // Convert the colors to unsigned integers.
        unsigned char r = static_cast<unsigned char>(scale_fun(red));
        unsigned char g = static_cast<unsigned char>(scale_fun(green));
        unsigned char b = static_cast<unsigned char>(scale_fun(blue));
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        Uint32 pixelColor = (b << 24) + (g << 16) + (r << 8) + 255;
#else
        Uint32 pixelColor = (255 << 24) + (r << 16) + (g << 8) + b;
#endif

        return pixelColor;
    }

    void qbImage::InitTexture() {
        Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
#else
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0xff000000;
#endif

        if (m_pTexture != nullptr) {
            SDL_DestroyTexture(m_pTexture);
        }

        SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, m_xSize, m_ySize, 32, rmask, gmask, bmask, amask);
        m_pTexture = SDL_CreateTextureFromSurface(m_pRenderer, tempSurface);
        SDL_FreeSurface(tempSurface);
    }

    void qbImage::ComputeMaxValues() {
        m_maxRed = 0.0;
        m_maxGreen = 0.0;
        m_maxBlue = 0.0;
        m_overallMax = 0.0;
        for (int x = 0; x < m_xSize; x++) {
            for (int y = 0; y < m_ySize; y++) {
                double redValue = m_rChannel.at(x).at(y);
                double greenValue = m_gChannel.at(x).at(y);
                double blueValue = m_bChannel.at(x).at(y);

                if (redValue > m_maxRed) {
                    m_maxRed = redValue;
                }

                if (greenValue > m_maxGreen) {
                    m_maxGreen = greenValue;
                }

                if (blueValue > m_maxBlue) {
                    m_maxBlue = blueValue;
                }

                if (m_maxRed > m_overallMax) {
                    m_overallMax = m_maxRed;
                }

                if (m_maxGreen > m_overallMax) {
                    m_overallMax = m_maxGreen;
                }

                if (m_maxBlue > m_overallMax) {
                    m_overallMax = m_maxBlue;
                }

            }
        }
    }
}
