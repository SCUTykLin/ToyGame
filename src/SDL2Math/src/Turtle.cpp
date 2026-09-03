//
// Created by 15813 on 2026/9/3.
//

#include "Turtle.h"

#include <cmath>

namespace Math::Display {
    Turtle::Turtle() {
        m_pRenderer = nullptr;
        m_x = 0.0f;
        m_y = 0.0f;
        m_r = 255.0f;
        m_g = 255.0f;
        m_b = 255.0f;
        m_alpha = 255.0f;
        m_angle = 0.0f;
        m_length = 1.0f;
        m_penDown = false;
    }

    void Turtle::SetRenderer(SDL_Renderer *m_pRenderer_) {
        m_pRenderer = m_pRenderer_;
    }

    void Turtle::PenUp() {
        m_penDown = false;
    }

    void Turtle::PenDown() {
        m_penDown = true;
    }

    void Turtle::SetPenColor(int r, int g, int b, int alpha) {
        m_r = r;
        m_g = g;
        m_b = b;
        m_alpha = alpha;
    }

    void Turtle::SetAngle(float angle_) {
        m_angle = angle_;
    }

    void Turtle::SetLength(float length_) {
        m_length = length_;
    }

    void Turtle::RotateRight(float angle) {
        m_angle += angle * (M_PI / 180.0f);
    }

    void Turtle::RotateLeft(float angle) {
        m_angle -= angle * (M_PI / 180.0f);
    }

    void Turtle::MoveTo(float xPos, float yPos) {
        m_x = xPos;
        m_y = yPos;
    }

    void Turtle::Step() {
        float startX = m_x;
        float startY = m_y;
        float endX = startX + (cos(m_angle) * m_length);
        float endY = startY + (sin(m_angle) * m_length);
        m_x = endX;
        m_y = endY;

        if (m_pRenderer) {
            SDL_SetRenderDrawColor(m_pRenderer, m_r, m_g, m_b, m_alpha);
            SDL_RenderDrawLine(m_pRenderer,
                static_cast<int>(round(startX)), static_cast<int>(round(startY)),
                static_cast<int>(round(endX)), static_cast<int>(round(endY)));
        }
    }
}
