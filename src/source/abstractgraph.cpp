#include <cell.h>
#include <connection.h>
#include <graph/abstractgraph.h>
#include <graph/hierarchicalgrid.h>
#include <pathfinding/astar.h>

#include <algorithm>
#include <vector>
#include <raylib.h>

using namespace HP;

void HP::AbstractGraph::SetConnectionsToCell(Cell* cell, std::vector<Cell*> toConnCells)
{
    for (auto connCell : toConnCells)
    {
        if (cell->GetId() == connCell->GetId()) continue;

        CreateConnection(cell, connCell);
        CreateConnection(connCell, cell);
    }
}

std::vector<Cell*> HP::AbstractGraph::GetConnectedCells(int cellId)
{
    std::vector<Cell*> result{};

    for (auto& connection : m_Connections)
    {
        if (connection.GetFromCell() == cellId)
        {
            result.push_back(GetCell(connection.GetToCell()));
        }
    }

    return result;
}

std::vector<Connection*> HP::AbstractGraph::GetConnectionsFromCell(int cellId)
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

std::vector<Cell*> HP::AbstractGraph::GetCellsFromRegion(int regionId)
{
    std::vector<Cell*> result{};

    for (auto cell : m_Cells)
    {
        if (cell->GetRegionId() == regionId)
        {
            result.push_back(cell);
        }
    }

    return result;
}

void HP::AbstractGraph::CreateConnection(int cellAId, int cellBId)
{
    CreateConnection(GetCell(cellAId), GetCell(cellBId));
}

void HP::AbstractGraph::CreateConnection(Cell* cellA, Cell* cellB)
{
    if (cellA != nullptr and cellB != nullptr)
    {
        Connection connection{ cellA->GetId(), cellB->GetId() };
        auto iter{ std::ranges::find(m_Connections, connection) };

        if (iter == m_Connections.end())
        {
            connection.SetWeight(AStar::FindPath(cellA, cellB, this, nullptr));
            m_Connections.push_back(connection);
        }
    }
}

Cell* HP::AbstractGraph::GetCell(int cellId)
{
    for (auto cell : m_Cells)
    {
        if (cell->GetId() == cellId)
        {
            return cell;
        }
    }
    return nullptr;
}

vec2<int> HP::AbstractGraph::GetCellPosition(int cellId) const
{
    return m_pHGrid->GetCellPosition(cellId);
}

std::vector<int> HP::AbstractGraph::GetRegionIds() const
{
    std::vector<int> regionIds{};

    for (Cell const* cell : m_Cells)
    {
        if (std::ranges::find(regionIds, cell->GetRegionId()) == regionIds.end())
        {
            regionIds.push_back(cell->GetRegionId());
        }
    }

    return regionIds;
}

void HP::AbstractGraph::BuildAbstractGraph()
{
    BuildInterRegion();
    BuildIntraRegion();
}

void HP::AbstractGraph::Draw() const
{
    for (auto& connection : m_Connections)
    {
        auto fromCellPos{ m_pHGrid->GetCellCenter(connection.GetFromCell()) };
        auto toCellPos{ m_pHGrid->GetCellCenter(connection.GetToCell()) };

        DrawLine(
            fromCellPos.x, fromCellPos.y,
            toCellPos.x, toCellPos.y,
            RED
        );
    }
}

void HP::AbstractGraph::BuildInterRegion()
{
    auto regionGrid{ m_pHGrid->GetRegionGrid() };
    auto regions{ regionGrid.GetCells() };

    for (auto& region : regions)
    {
        int regionId{ region.GetId() };
        auto connectedRegions{ regionGrid.GetConnectedCells(regionId) };

        auto regionExtConn{ m_pHGrid->GetExternalConnectionFromRegion(regionId) };

        for (auto connRegion : connectedRegions)
        {
            int neighborRegionId{ connRegion->GetId() };

            auto neighborExtConn{ m_pHGrid->GetExternalConnectionFromRegion(neighborRegionId) };

            auto commonConnections{ m_pHGrid->FindCommonConnections(regionExtConn, neighborExtConn) };

            auto commonCells{ m_pHGrid->GetCellsFromConnections(commonConnections) };

            for (auto cell : commonCells)
            {
                auto iter{ std::ranges::find(m_Cells, cell) };

                if (iter == m_Cells.end() and cell->GetRegionId() == regionId)
                {
                    m_Cells.push_back(cell);
                }
            }

            for (auto conn : commonConnections)
            {
                auto iter{ std::ranges::find(m_Connections, *conn) };

                if (iter != m_Connections.end()) continue;

                CreateConnection(
                    conn->GetFromCell(), conn->GetToCell());
            }
        }
    }
}

void HP::AbstractGraph::BuildIntraRegion()
{
    for (auto& regionId : GetRegionIds())
    {
        auto regionCells{ GetCellsFromRegion(regionId) };

        for (auto cell : regionCells)
        {
            SetConnectionsToCell(cell, regionCells);
        }
    }
}

HP::AbstractGraph::AbstractGraph(HierarchicalGrid* pHGrid)
    : m_pHGrid{ pHGrid }
{
}
