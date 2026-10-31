#include "qbvkGenerator.h"
#include <cmath>
#include <iostream>

namespace Math::Display {
    qbvkGenerator::qbvkGenerator() {
        m_numEdges = 1;
    }

    qbvkGenerator::~qbvkGenerator() {}

    void qbvkGenerator::SetStartPoint(float xPos, float yPos) {
        m_startX = xPos;
        m_startY = yPos;
    }

    void qbvkGenerator::SetEndPoint(float xPos, float yPos) {
        m_endX = xPos;
        m_endY = yPos;
    }

    void qbvkGenerator::AddNode(float angle) {
        m_nodeAngle.push_back(angle);
        m_numEdges = 2 + m_nodeAngle.size();
    }

    bool qbvkGenerator::GetEdge(float *x1, float *y1, float *x2, float *y2, int index) {
        if (index > (m_numEdges - 1)){return false;}
        *x1 = m_x1[index];
        *y1 = m_y1[index];
        *x2 = m_x2[index];
        *y2 = m_y2[index];
        return true;
    }

    bool qbvkGenerator::Compute() {
        m_x1.clear();
        m_y1.clear();
        m_x2.clear();
        m_y2.clear();

        m_x1.push_back(m_startX);
        m_y1.push_back(m_startY);

        if (m_numEdges == 1) {
            m_x2.push_back(m_endX);
            m_y2.push_back(m_endY);
        }else {
            float numSegs = 2.0f;
            float cumulativeAngle = 0.0f;
            for (auto &i : m_nodeAngle) {
                cumulativeAngle += i;
                numSegs += fabs(cos(cumulativeAngle * M_PI / 180.0f));
            }

            float initLength = sqrt((m_endX - m_startX) * (m_endX - m_startX) + (m_endY - m_startY) * (m_endY - m_startY));
            float edgeLength = initLength / numSegs;

            m_turtle.MoveTo(m_startX, m_startY);
            m_turtle.LookAt(m_endX, m_endY);
            m_turtle.SetLength(edgeLength);
            m_turtle.Step();
            m_x2.push_back(m_turtle.GetX());
            m_y2.push_back(m_turtle.GetY());

            for (auto &currentAngle : m_nodeAngle) {
                m_x1.push_back(m_turtle.GetX());
                m_y1.push_back(m_turtle.GetY());

                m_turtle.RotateLeft(currentAngle);

                m_turtle.Step();

                m_x2.push_back(m_turtle.GetX());
                m_y2.push_back(m_turtle.GetY());
            }

            m_x1.push_back({m_turtle.GetX()});
            m_y1.push_back({m_turtle.GetY()});
            m_x2.push_back({m_endX});
            m_y2.push_back({m_endY});
        }
        return true;
    }

    int qbvkGenerator::NumEdges() {
        return m_numEdges;
    }
}
