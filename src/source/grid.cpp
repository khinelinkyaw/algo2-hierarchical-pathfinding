#include <cell.h>
#include <grid.h>
#include <raylib.h>
#include <structs.h>

#include <algorithm>
#include <cmath>
#include <connection.h>
#include <vector>

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

Cell* Grid::GetCell(int rowIndex, int colIndex)
{
    int cellIndex{ (rowIndex * m_Cols) + colIndex };

    if (cellIndex < m_Cols * m_Rows)
    {
        return &m_Cells[cellIndex];
    }
    return nullptr;
}

Cell* Grid::GetCell(float worldX, float worldY)
{
    auto [rowIndex, colIndex] { ConvertWorldToCellIndex(worldX, worldY) };

    return GetCell(rowIndex, colIndex);
}

vec2<int> Grid::GetCellCenter(Cell const& cell) const
{
    int cellId{ cell.GetId() };

    return GetCellCenter(cellId);
}

vec2<int> Grid::GetCellCenter(int cellId) const
{
    int cellCol{ cellId % m_Cols };
    int cellRow{ static_cast<int>(cellId / m_Cols) };

    return vec2<int>(
        static_cast<int>(m_Position.x + ((cellCol * m_CellWidth) + m_CellWidth / 2.f)),
        static_cast<int>(m_Position.y + ((cellRow * m_CellHeight) + m_CellHeight / 2.f))
    );
}

std::vector<Connection*> Grid::FindConnectionsFromCell(int cellId)
{
    std::vector<Connection*> result{};

    for (auto& connection : m_Connections)
    {
        if (connection.GetConnectedToCell(cellId) != Cell::INVALID_CELL_ID)
        {
            result.push_back(&connection);
        }
    }
    return result;
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

vec2<int> Grid::ConvertWorldToCellIndex(float worldX, float worldY) const
{
    return vec2<int>(
        static_cast<int>(floor((worldX - m_Position.x) / m_CellWidth)),
        static_cast<int>(floor((worldY - m_Position.y) / m_CellHeight))
    );
}

void Grid::Update()
{
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
            auto [mouseGridX, mouseGridY] { ConvertWorldToCellIndex(mousePos.x, mousePos.y) };

            int cellIndex{ (mouseGridY * m_Cols) + mouseGridX };
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
    for (int index{ 0 }; index < static_cast<int>(m_Cells.size()); ++index)
    {
        if (m_Cells[index].GetCellType() == CellType::Obstacle)
        {
            int cellRow{ static_cast<int>(index / m_Cols) };
            int cellCol{ index % m_Cols };

            DrawRectangle(
                m_Position.x + (cellCol * m_CellWidth),
                m_Position.y + (cellRow * m_CellHeight),
                m_CellWidth,
                m_CellHeight,
                RED
            );
        }
    }

    for (int index{ 0 }; index < m_Rows; ++index)
    {
        vec2 startPos{m_Position.x, m_Position.y + (index * m_CellHeight)};
        vec2 endPos{m_Position.x + m_Dimensions.x, m_Position.y + (index * m_CellHeight)};
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
        vec2 startPos{ m_Position.x + (index * m_CellWidth), m_Position.y };
        vec2 endPos{ m_Position.x + (index * m_CellWidth), m_Position.y + m_Dimensions.y };
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
