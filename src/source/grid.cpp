#include <cell.h>
#include <graph/grid.h>
#include <pathfinding/astar.h>
#include <structs.h>

#include <raylib.h>
#include <raygui.h>

#include <cmath>
#include <connection.h>
#include <vector>
#include <set>
#include <string>

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

std::vector<Connection*> HP::Grid::FindCommonConnections(std::set<Connection*> connectionsA, std::set<Connection*> connectionsB)
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
            break;
        case CellType::Obstacle:
            m_Cells.GetCell(cellIndex).SetCellType(CellType::Empty);
            break;
        }

        GenerationConnections();
    }
}

void Grid::Draw() const
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

        Rectangle targetRect{
            static_cast<float>(m_Position.x + (cellCol * m_CellWidth)),
            static_cast<float>(m_Position.y + (cellRow * m_CellHeight)),
            static_cast<float>(m_CellWidth),
            static_cast<float>(m_CellHeight),
        };

        GuiLabel(targetRect, std::to_string(index).c_str());
    }

    for (int index{ 0 }; index < m_Cells.GetRowSize(); ++index)
    {
        vec2 startPos{m_Position.x, m_Position.y + (index * m_CellHeight)};
        vec2 endPos{m_Position.x + m_Dimensions.x, m_Position.y + (index * m_CellHeight)};
        DrawLine(
            startPos.x,
            startPos.y,
            endPos.x,
            endPos.y,
            {200,200,200,100}
        );
    }

    for (int index{ 0 }; index < m_Cells.GetColSize(); ++index)
    {
        vec2 startPos{ m_Position.x + (index * m_CellWidth), m_Position.y };
        vec2 endPos{ m_Position.x + (index * m_CellWidth), m_Position.y + m_Dimensions.y };
        DrawLine(
            startPos.x,
            startPos.y,
            endPos.x,
            endPos.y,
            { 200,200,200,100 }
        );
    }

    //for (auto const& cell : m_Cells)
    //{
    //    auto cellCenter{ GetCellCenter(cell) };

    //    DrawCircle(
    //        cellCenter.x,
    //        cellCenter.y,
    //        5.f,
    //        ORANGE
    //    );
    //}

    //for (auto const& connection : m_Connections)
    //{
    //    auto [cellAId, cellBId] { connection.GetConnectedCells() };

    //    auto cellACenter{ GetCellCenter(cellAId) };
    //    auto cellBCenter{ GetCellCenter(cellBId) };

    //    DrawLine(
    //        cellACenter.x,
    //        cellACenter.y,
    //        cellBCenter.x,
    //        cellBCenter.y,
    //        ORANGE
    //    );
    //}
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
