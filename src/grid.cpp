#include "grid.h"
#include "raylib.h"
#include "structs.h"

#include <cmath>

void Grid::Update()
{
    int cellRowSize{ m_Dimensions.y / m_Rows };
    int cellColSize{ m_Dimensions.x / m_Cols };

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        auto mousePos{ GetMousePosition() };

        Rectangle gridRect{
            static_cast<float>(m_Position.x),
            static_cast<float>(m_Position.y),
            static_cast<float>(m_Dimensions.x),
            static_cast<float>(m_Dimensions.y)
        };

        if (CheckCollisionPointRec(mousePos, gridRect))
        {
            int mouseGridPosX{ static_cast<int>(floor((mousePos.x - m_Position.x)/cellColSize)) };
            int mouseGridPosY{ static_cast<int>(floor((mousePos.y - m_Position.y)/cellRowSize)) };

            CellType& currentGridType{ m_Cells[mouseGridPosY][mouseGridPosX] };

            switch (currentGridType)
            {
            case Grid::CellType::Empty:
                currentGridType = CellType::Obstacle;
                break;
            case Grid::CellType::Obstacle:
                currentGridType = CellType::Empty;
                break;
            }
        }
    }
}

void Grid::Draw() const
{
    int cellRowSize{ m_Dimensions.y / m_Rows };
    int cellColSize{ m_Dimensions.x / m_Cols };

    for (int row{ 0 }; row < m_Rows; ++row)
    {
        for (int col{ 0 }; col < m_Cols; ++col)
        {
            if (m_Cells[row][col] == CellType::Obstacle)
            {
                DrawRectangle(
                    m_Position.x + (col * cellColSize),
                    m_Position.y + (row * cellRowSize),
                    cellColSize,
                    cellRowSize,
                    RED
                );
            }
        }
    }

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
    m_Cells.resize(rows);
    
    for (auto& row : m_Cells)
    {
        row.resize(cols, CellType::Empty );
    }
}
