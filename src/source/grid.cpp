#include <cell.h>
#include <grid.h>
#include <raylib.h>
#include <structs.h>

#include <algorithm>
#include <cmath>
#include <connection.h>
#include <iostream>

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

vec2<int> Grid::GetCellCenter(Cell const& cell) const
{
    int cellId{ cell.GetId() };

    return GetCellCenter(cellId);
}

vec2<int> Grid::GetCellCenter(int cellId) const
{
    int cellWidth{ m_Dimensions.x / m_Cols };
    int cellHeight{ m_Dimensions.y / m_Rows };

    int cellCol{ cellId % m_Cols };
    int cellRow{ static_cast<int>(cellId / m_Cols) };

    return vec2<int>(
        static_cast<int>(m_Position.x + ((cellCol * cellWidth) + cellWidth / 2.f)),
        static_cast<int>(m_Position.y + ((cellRow * cellHeight) + cellHeight / 2.f))
    );
}

void Grid::CreateNewConnection(int cellAId, int cellBId)
{
    Connection newConnection{ cellAId, cellBId };

    auto iter{ std::ranges::find(m_Connections, newConnection) };

    if (iter == m_Connections.end())
    {
        m_Connections.push_back(newConnection);
    }
}

void Grid::Update()
{
    int cellWidth{ m_Dimensions.y / m_Rows };
    int cellHeight{ m_Dimensions.x / m_Cols };

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
            int mouseGridPosX{ static_cast<int>(floor((mousePos.x - m_Position.x)/cellHeight)) };
            int mouseGridPosY{ static_cast<int>(floor((mousePos.y - m_Position.y)/cellWidth)) };

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
    int cellWidth{ m_Dimensions.y / m_Rows };
    int cellHeight{ m_Dimensions.x / m_Cols };

    for (int index{ 0 }; index < static_cast<int>(m_Cells.size()); ++index)
    {
        if (m_Cells[index].GetCellType() == CellType::Obstacle)
        {
            int cellRow{ static_cast<int>(index / m_Cols) };
            int cellCol{ index % m_Cols };

            DrawRectangle(
                m_Position.x + (cellCol * cellHeight),
                m_Position.y + (cellRow * cellWidth),
                cellHeight,
                cellWidth,
                RED
            );
        }
    }

    for (int index{ 0 }; index < m_Rows; ++index)
    {
        vec2 startPos{m_Position.x, m_Position.y + (index * cellWidth)};
        vec2 endPos{m_Position.x + m_Dimensions.x, m_Position.y + (index * cellWidth)};
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
        vec2 startPos{ m_Position.x + (index * cellHeight), m_Position.y };
        vec2 endPos{ m_Position.x + (index * cellHeight), m_Position.y + m_Dimensions.y };
        DrawLine(
            startPos.x,
            startPos.y,
            endPos.x,
            endPos.y,
            LIME
        );
    }

    for (auto const& cell : m_Cells)
    {
        auto cellCenter{ GetCellCenter(cell) };

        DrawCircle(
            cellCenter.x,
            cellCenter.y,
            5.f,
            ORANGE
        );
    }

    for (auto const& connection : m_Connections)
    {
        auto [cellAId, cellBId] { connection.GetConnectedCells() };

        auto cellACenter{ GetCellCenter(cellAId) };
        auto cellBCenter{ GetCellCenter(cellBId) };

        DrawLine(
            cellACenter.x,
            cellACenter.y,
            cellBCenter.x,
            cellBCenter.y,
            ORANGE
        );
    }
}

Grid::Grid(int rows, int cols, int posX, int posY, int width, int height)
    : m_Rows{rows}
    , m_Cols{cols}
    , m_Dimensions{width, height}
    , m_CellWidth{ m_Dimensions.x / m_Cols }
    , m_CellHeight{ m_Dimensions.y / m_Rows }
    , m_Position{posX, posY}
{
    int totalCells{ rows * cols };
    m_Cells.reserve(totalCells);

    for (int index{ 0 }; index < totalCells; ++index)
    {
        m_Cells.emplace_back(index, CellType::Empty);

        if (index - 1 >= 0 and index % m_Cols != 0)
        {
            CreateNewConnection(index, index - 1);
        }
        if (index - m_Cols >= 0)
        {
            CreateNewConnection(index, index - m_Cols);
        }
        if (index + 1 < totalCells and (index + 1) % m_Cols != 0)
        {
            CreateNewConnection(index, index + 1);
        }
        if (index + m_Cols < totalCells)
        {
            CreateNewConnection(index, index + m_Cols);
        }
    }
}
