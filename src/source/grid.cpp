#include <grid.h>
#include <raylib.h>
#include <structs.h>
#include <cell.h>

#include <algorithm>
#include <cmath>

Cell* Grid::GetCell(int cellId)
{
    auto iter{ std::ranges::find_if(m_Cells, [cellId](Cell const& cell)
    {
        return cell.GetId() == cellId;
    })};

    if (iter != m_Cells.end())
    {
        return &*iter;
    }
    return nullptr;
}

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

            int cellIndex{ (mouseGridPosY * m_Cols) + mouseGridPosX };
            CellType currentCellType{ m_Cells[cellIndex].GetCellType()};

            switch (currentCellType)
            {
            case CellType::Empty:
                m_Cells[cellIndex].SetCellType(CellType::Obstacle);
                break;
            case CellType::Obstacle:
                m_Cells[cellIndex].SetCellType(CellType::Empty);
                break;
            }
        }
    }
}

void Grid::Draw() const
{
    int cellRowSize{ m_Dimensions.y / m_Rows };
    int cellColSize{ m_Dimensions.x / m_Cols };

    for (int index{ 0 }; index < static_cast<int>(m_Cells.size()); ++index)
    {
        if (m_Cells[index].GetCellType() == CellType::Obstacle)
        {
            int cellRow{ static_cast<int>(index / m_Cols) };
            int cellCol{ index % m_Cols };

            DrawRectangle(
                m_Position.x + (cellCol * cellColSize),
                m_Position.y + (cellRow * cellRowSize),
                cellColSize,
                cellRowSize,
                RED
            );
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
    int totalCells{ rows * cols };
    m_Cells.reserve(totalCells);
    m_Connections.reserve(totalCells);

    for (int index{ 0 }; index < totalCells; ++index)
    {
        m_Cells.emplace_back(index, CellType::Empty);

        if (index - 1 >= 0)
        {
            m_Connections.emplace_back(index, index - 1);
        }
        if (index - m_Rows >= 0)
        {
            m_Connections.emplace_back(index, index - m_Rows);
        }
        if (index + 1 < totalCells)
        {
            m_Connections.emplace_back(index, index + 1);
        }
        if (index + m_Rows < totalCells)
        {
            m_Connections.emplace_back(index, index + m_Rows);
        }
    }
}
