//
// Created by YangKai.Lin on 2026/10/31.
//

#include "DragonCurve.h"

namespace Math::Display {
    DragonCurve::DragonCurve() {
        m_startX = 0.0f;
        m_startY = 0.0f;
        m_length = 1.0f;
        m_level = 1;
    }

    DragonCurve::~DragonCurve() {
    }

    void DragonCurve::SetStartPoint(float xPos, float yPos) {
        m_startX = xPos;
        m_startY = yPos;
    }

    void DragonCurve::SetLength(float length) {
        m_length = length;
    }

    void DragonCurve::SetLevel(int level) {
        m_level = level;
    }

    void DragonCurve::BuildTurns() {
        m_turnList.clear();
        m_turnList.reserve(1 << m_level);
        m_turnList.push_back(1);

        for (int i = 1; i < m_level; ++i) {
            int numTurns = static_cast<int>(m_turnList.size());
            m_turnList.push_back(1);
            for (int j = numTurns - 1; j >= 0; --j) {
                m_turnList.push_back(-m_turnList[j]);
            }
        }
    }

    bool DragonCurve::Compute() {
        m_x1.clear();
        m_y1.clear();
        m_x2.clear();
        m_y2.clear();

        if (m_level < 1) {
            return false;
        }

        BuildTurns();

        m_turtle.MoveTo(m_startX, m_startY);
        m_turtle.SetAngle(0.0f);
        m_turtle.SetLength(m_length);

        m_x1.push_back(m_startX);
        m_y1.push_back(m_startY);
        m_turtle.Step();
        m_x2.push_back(m_turtle.GetX());
        m_y2.push_back(m_turtle.GetY());

        for (auto &currentTurn: m_turnList) {
            m_x1.push_back(m_turtle.GetX());
            m_y1.push_back(m_turtle.GetY());

            m_turtle.RotateLeft(90.0f * static_cast<float>(currentTurn));

            m_turtle.Step();

            m_x2.push_back(m_turtle.GetX());
            m_y2.push_back(m_turtle.GetY());
        }

        return true;
    }

    int DragonCurve::NumEdges() {
        return static_cast<int>(m_x1.size());
    }

    bool DragonCurve::GetEdge(float *x1, float *y1, float *x2, float *y2, int index) {
        if (index < 0 || index >= NumEdges()) {
            return false;
        }
        *x1 = m_x1[index];
        *y1 = m_y1[index];
        *x2 = m_x2[index];
        *y2 = m_y2[index];
        return true;
    }
}
