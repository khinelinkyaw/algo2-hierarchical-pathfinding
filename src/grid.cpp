#include "grid.h"
#include "raylib.h"
#include "structs.h"

void Grid::Draw() const
{
    int cellRowSize{ m_Dimensions.y / m_Rows };
    int cellColSize{ m_Dimensions.x / m_Cols };

    for (int index{ 0 }; index < m_Rows; ++index)
    {
        vec2 startPos{m_Position.x, m_Position.y + (index * cellRowSize)};
        vec2 endPos{m_Position.x + m_Dimensions.x, m_Position.y + (index * cellRowSize)};
        DrawLine(
            startPos.x,
            startPos.y,
            endPos.x,
            endPos.y,
            LIME
        );
    }

    for (int index{ 0 }; index < m_Cols; ++index)
    {
        vec2 startPos{ m_Position.x + (index * cellColSize), m_Position.y };
        vec2 endPos{ m_Position.x + (index * cellColSize), m_Position.y + m_Dimensions.y };
        DrawLine(
            startPos.x,
            startPos.y,
            endPos.x,
            endPos.y,
            LIME
        );
    }
}

Grid::Grid(int rows, int cols, int posX, int posY, int width, int height)
    : m_Rows{rows}
    , m_Cols{cols}
    , m_Position{posX, posY}
    , m_Dimensions{width, height}
{
}
