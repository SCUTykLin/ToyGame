//
// Created by YangKai.Lin on 2026/10/31.
//
#include "qbvkInitiator.h"
#include <cmath>

namespace Math::Display {
    qbvkInitiator::qbvkInitiator() {

    }

    qbvkInitiator::~qbvkInitiator() {

    }

    void qbvkInitiator::AddVertex(float xPos, float yPos) {
        m_vertexX.push_back(xPos);
        m_vertexY.push_back(yPos);
    }

    int qbvkInitiator::NumEdge() const {
        if (m_vertexX.size() < 3) {
            return -1;
        }
        return m_vertexX.size();
    }

    bool qbvkInitiator::GetEdge(float *x1, float *y1, float *x2, float *y2, int index) {
        if (index >= m_vertexX.size()) {
            return false;
        }
        *x1 = m_vertexX[index];
        *y1 = m_vertexY[index];
        if (index < m_vertexX.size() - 1) {
            *x2 = m_vertexX[index + 1];
            *y2 = m_vertexY[index + 1];
        }else {
            *x2 = m_vertexX[0];
            *y2 = m_vertexY[0];
        }
        return true;
    }
}
