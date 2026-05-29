#include <cell.h>
#include <graph/grid.h>
#include <pathfinding/astar.h>
#include <structs.h>

#include <raylib.h>

#include <algorithm>
#include <cmath>
#include <connection.h>
#include <set>
#include <vector>

using namespace HP;

Cell* HP::Grid::GetCell(int cellId)
{
    return m_Cells.GetCellPtr(cellId);
}

Cell* Grid::GetCell(float worldX, float worldY)
{
    auto [colIndex, rowIndex] { ConvertWorldToCellIndex(worldX, worldY) };

    return m_Cells.GetCellPtr(rowIndex, colIndex);
}

vec2<int> Grid::GetCellCenter(Cell const& cell) const
{
    int cellId{ cell.GetId() };

    return GetCellCenter(cellId);
}

vec2<int> Grid::GetCellCenter(int cellId) const
{
    auto [cellCol, cellRow] { GetCellPosition(cellId) };

    return vec2<int>(
        static_cast<int>(m_Position.x + ((cellCol * m_CellWidth) + m_CellWidth / 2.f)),
        static_cast<int>(m_Position.y + ((cellRow * m_CellHeight) + m_CellHeight / 2.f))
    );
}

vec2<int> HP::Grid::GetCellPosition(int cellId) const
{
    return vec2<int>(
        cellId % m_Cells.GetColSize(),
        static_cast<int>(cellId / m_Cells.GetColSize())
    );
}

std::vector<Connection*> Grid::GetConnectionsFromCell(int cellId)
{
    std::vector<Connection*> result{};

    for (auto& connection : m_Connections)
    {
        if (connection.GetFromCell() == cellId)
        {
            result.push_back(&connection);
        }
    }
    return result;
}

std::vector<Cell*> HP::Grid::GetCellsFromRegion(int regionId)
{
    std::vector<Cell*> result{};

    for (auto& cell : m_Cells.GetData())
    {
        if (cell.GetRegionId() == regionId)
        {
            result.push_back(&cell);
        }
    }

    return result;
}

std::vector<Connection*> HP::Grid::GetConnectionFromCells(std::vector<Cell*> const& cells)
{
    std::vector<Connection*> result{};

    for (auto cell : cells)
    {
        auto connections{ GetConnectionsFromCell(cell->GetId()) };
        result.insert(result.end(), connections.begin(), connections.end());
    }

    return result;
}

std::vector<Cell*> HP::Grid::GetConnectedCells(int cellId)
{
    std::vector<Cell*> result{};

    auto connections{ GetConnectionsFromCell(cellId) };

    for (auto& connection : connections)
    {
        result.push_back(m_Cells.GetCellPtr(connection->GetToCell()));
    }

    return result;
}

void Grid::CreateConnection(int cellAId, int cellBId)
{
    CreateConnection(GetCell(cellAId), GetCell(cellBId));
}

void HP::Grid::CreateConnection(Cell* cellA, Cell* cellB)
{
    if (cellA == nullptr or cellB == nullptr)
    {
        return;
    }

    if (cellA->GetCellType() == CellType::Obstacle or cellB->GetCellType() == CellType::Obstacle)
    {
        return;
    }

    Connection connection{ cellA->GetId(), cellB->GetId() };
    connection.SetWeight(AStar::GetHeuristicCost(*cellA, *cellB, this));
    m_Connections.push_back(connection);
}

vec2<int> Grid::ConvertWorldToCellIndex(float worldX, float worldY) const
{
    return vec2<int>(
        static_cast<int>(floor((worldX - m_Position.x) / m_CellWidth)),
        static_cast<int>(floor((worldY - m_Position.y) / m_CellHeight))
    );
}

std::vector<Connection*> HP::Grid::FindCommonConnections(std::vector<Connection*> connectionsA, std::vector<Connection*> connectionsB)
{
    std::vector<Connection*> result{};

    for (auto conn : connectionsA)
    {
        for (auto otherConn : connectionsB)
        {
            if ((conn->GetToCell() == otherConn->GetToCell() and conn->GetFromCell() == otherConn->GetFromCell())
                or
                (conn->GetFromCell() == otherConn->GetToCell() and conn->GetToCell() == otherConn->GetFromCell())
                )
            {
                //result.push_back(GetCell(conn->GetFromCell
                //result.push_back(GetCell(conn->GetToCell()));

                result.push_back(conn);
                result.push_back(otherConn);
            }
        }
    }

    return result;
}

std::vector<Cell*> HP::Grid::GetCellsFromConnections(std::vector<Connection*> connections)
{
    std::vector<Cell*> result{};
    std::set<int> cellIds{};

    for (auto connection : connections)
    {
        cellIds.insert(connection->GetFromCell());
        cellIds.insert(connection->GetToCell());
    }

    result.reserve(cellIds.size());

    for (int cellId : cellIds)
    {
        result.push_back(m_Cells.GetCellPtr(cellId));
    }

    return result;
}


void Grid::MouseClicked()
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

        int cellIndex{ (mouseGridY * m_Cells.GetColSize()) + mouseGridX };
        CellType currentCellType{ m_Cells.GetCell(cellIndex).GetCellType()};

        switch (currentCellType)
        {
        case CellType::Empty:
            m_Cells.GetCell(cellIndex).SetCellType(CellType::Obstacle);
            ChangeConnectionsActiveStateToCell(cellIndex, false);
            break;
        case CellType::Obstacle:
            m_Cells.GetCell(cellIndex).SetCellType(CellType::Empty);
            ChangeConnectionsActiveStateToCell(cellIndex, true);
            break;
        }

    }
}

void Grid::Draw() const
{
    DrawCellEffects();

    DrawCellBorders();

    //DrawConnections();
}

void HP::Grid::DrawConnections() const
{
    for (auto const& connection : m_Connections)
    {
        if (connection.GetActive() == false) continue;

        auto fromCell{ m_Cells.GetCellPtr(connection.GetFromCell()) };
        auto toCell{ m_Cells.GetCellPtr(connection.GetToCell()) };
        auto fromPos{ GetCellCenter(*fromCell) };
        auto toPos{ GetCellCenter(*toCell) };
        DrawLineEx(
            { static_cast<float>(fromPos.x), static_cast<float>(fromPos.y) },
            { static_cast<float>(toPos.x), static_cast<float>(toPos.y) },
            2.f,
            ORANGE
        );
    }
}

void HP::Grid::DrawCellBorders(Color color, float thickness) const
{
    for (int index{ 0 }; index < m_Cells.GetRowSize(); ++index)
    {
        Vector2 startPos{ static_cast<float>(m_Position.x), static_cast<float>(m_Position.y + (index * m_CellHeight)) };
        Vector2 endPos{ static_cast<float>(m_Position.x + m_Dimensions.x), static_cast<float>(m_Position.y + (index * m_CellHeight)) };

        DrawLineEx(
            startPos,
            endPos,
            thickness,
            color
        );
    }

    for (int index{ 0 }; index < m_Cells.GetColSize(); ++index)
    {
        Vector2 startPos{ static_cast<float>(m_Position.x + (index * m_CellWidth)), static_cast<float>(m_Position.y) };
        Vector2 endPos{ static_cast<float>(m_Position.x + (index * m_CellWidth)), static_cast<float>(m_Position.y + m_Dimensions.y)};
        DrawLineEx(
            startPos,
            endPos,
            thickness,
            color
        );
    }
}

void HP::Grid::DrawCellEffects() const
{
    for (int index{ 0 }; index < static_cast<int>(m_Cells.GetSize()); ++index)
    {
        int cellRow{ static_cast<int>(index / m_Cells.GetColSize()) };
        int cellCol{ index % m_Cells.GetColSize() };

        if (m_Cells.GetCell(index).GetCellType() == CellType::Obstacle)
        {
            DrawRectangle(
                m_Position.x + (cellCol * m_CellWidth),
                m_Position.y + (cellRow * m_CellHeight),
                m_CellWidth,
                m_CellHeight,
                { 190, 33, 55, 100 }
            );
        }
    }
}

void HP::Grid::SetCellWidth(int cellWidth)
{
    m_CellWidth = cellWidth;
    m_Dimensions.x = m_CellWidth * m_Cells.GetColSize();
}

void HP::Grid::SetCellHeight(int cellHeight)
{
    m_CellHeight = cellHeight;
    m_Dimensions.y = m_CellHeight * m_Cells.GetRowSize();
}
 
Grid::Grid(int rows, int cols, int posX, int posY, int width, int height)
    : m_Dimensions{width, height}
    , m_CellWidth{ m_Dimensions.x / cols }
    , m_CellHeight{ m_Dimensions.y / rows }
    , m_Cells{ rows, cols }
    , m_Position{posX, posY}
{
    for (int index{ 0 }; index < m_Cells.GetSize(); ++index)
    {
        m_Cells.SetCell(index, Cell{index, CellType::Empty});
    }

    GenerationConnections();
}

void Grid::GenerationConnections()
{
    m_Connections.clear();

    for (int index{ 0 }; index < m_Cells.GetSize(); ++index)
    {
        if (index - 1 >= 0 and index % m_Cells.GetColSize() != 0)
        {
            CreateConnection(index, index - 1);
        }
        if (index - m_Cells.GetColSize() >= 0)
        {
            CreateConnection(index, index - m_Cells.GetColSize());
        }
        if (index + 1 < m_Cells.GetSize() and (index + 1) % m_Cells.GetColSize() != 0)
        {
            CreateConnection(index, index + 1);
        }
        if (index + m_Cells.GetColSize() < m_Cells.GetSize())
        {
            CreateConnection(index, index + m_Cells.GetColSize());
        }
    }
}

std::vector<Connection*> HP::Grid::GetExternalConnectionFromRegion(int regionId)
{
    std::vector<Connection*> result{};

    auto cells{ GetCellsFromRegion(regionId) };
    auto connections{ GetConnectionFromCells(cells) };

    for (auto connection : connections)
    {
        auto toCell{ GetCell(connection->GetToCell()) };

        auto toCellIter{ std::ranges::find(cells, toCell) };

        if (toCellIter != cells.end()) continue;

        result.push_back(connection);
    }

    return result;
}
