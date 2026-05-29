#include <cell.h>
#include <connection.h>
#include <graph/abstractgraph.h>
#include <graph/hierarchicalgrid.h>
#include <pathfinding/astar.h>

#include <algorithm>
#include <raylib.h>
#include <utility>
#include <vector>

using namespace HP;

void HP::AbstractGraph::SetConnectionsToCell(Cell* cell, std::vector<Cell*> toConnCells, bool intraRegion)
{
    for (auto connCell : toConnCells)
    {
        if (cell->GetId() == connCell->GetId()) continue;

        CreateConnection(cell, connCell, intraRegion);
        CreateConnection(connCell, cell, intraRegion);
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
        if (connection.GetFromCell() == cellId and GetCell(connection.GetToCell()) != nullptr)
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
    CreateConnection(cellA, cellB, false);
}

void HP::AbstractGraph::AddCell(Cell* cell)
{
    auto iter{ std::ranges::find(m_Cells, cell) };

    if (iter == m_Cells.end())
    {
        m_Cells.push_back(cell);
    }
}

void HP::AbstractGraph::CreateConnection(Cell* cellA, Cell* cellB, bool intraRegion)
{
    if (cellA == nullptr or cellB == nullptr) return;
    if (cellA->GetCellType() == CellType::Obstacle or cellB->GetCellType() == CellType::Obstacle) return;

    Connection connection{ cellA->GetId(), cellB->GetId() };
    if (std::ranges::find(m_Connections, connection) != m_Connections.end()) return;

    std::vector<Cell*> tempPath{};

    tempPath.clear();
    auto pathResult{ AStar::FindPath(cellA, cellB, m_pHGrid, &tempPath) };

    if (not pathResult.pathFound or (intraRegion == true and not pathResult.intraRegionPath))
    {
        return;
    }

    connection.SetWeight(pathResult.totalCost);
    m_Connections.push_back(connection);
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
        if (connection.GetActive() == false) continue;

        auto fromCellPos{ m_pHGrid->GetCellCenter(connection.GetFromCell()) };
        auto toCellPos{ m_pHGrid->GetCellCenter(connection.GetToCell()) };

        DrawLine(
            fromCellPos.x, fromCellPos.y,
            toCellPos.x, toCellPos.y,
            RED
        );
    }
}

AStar::PathResult HP::AbstractGraph::FindPath(Cell* const pStartCell, Cell* const pDestCell, std::vector<Cell*>* finalPath)
{
    AStar::PathResult result{};
    std::vector<Cell*> baseCells{ m_Cells };
    std::vector<Connection> baseConnections{ m_Connections };

    AddCell(pStartCell);
    AddCell(pDestCell);

    SetConnectionsToCell(pStartCell, GetCellsFromRegion(pStartCell->GetRegionId()), true);
    SetConnectionsToCell(pDestCell, GetCellsFromRegion(pDestCell->GetRegionId()), true);

    result = AStar::FindPath(pStartCell, pDestCell, this, finalPath);

    m_Cells = std::move(baseCells);
    m_Connections = std::move(baseConnections);
    return result;
}

void HP::AbstractGraph::ChangeConnectionsActiveStateToCell(int cellId, bool)
{
    auto cell{ GetCell(cellId) };

    if (cell == nullptr) return;

    //auto connections{ GetConnectionsFromCell(cellId) };
    auto connections{ m_pHGrid->GetConnectionsFromCell(cellId) };
    int cellRegion{ cell->GetRegionId() };
    bool interRegion{ false };
    
    for (auto connection : connections)
    {
        auto connectedCellRegion{ GetCell(connection->GetToCell())->GetRegionId() };

        if (connectedCellRegion != cellRegion)
        {
            interRegion = true;
        }
    }

    if (interRegion == true)
    {
        RemoveConnectionsOfRegion(cellRegion, RegionConnectionType::BothRegions);
        GenerateInterRegionConnections(cellRegion);
    }
    else
    {
        RemoveConnectionsOfRegion(cellRegion, RegionConnectionType::IntraRegion);
    }

    InterconnectAllCellsOfRegion(cellRegion);
}

void HP::AbstractGraph::BuildInterRegion()
{
    m_Connections.clear();
    m_Cells.clear();

    auto regionGrid{ m_pHGrid->GetRegionGrid() };
    auto regions{ regionGrid.GetCells() };

    for (auto& region : regions)
    {
        GenerateInterRegionConnections(region.GetId());
    }
}

void HP::AbstractGraph::GenerateInterRegionConnections(int regionId)
{
    auto regionGrid{ m_pHGrid->GetRegionGrid() };
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
            if (cell->GetCellType() == CellType::Obstacle) continue;

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

void HP::AbstractGraph::BuildIntraRegion()
{
    for (int regionId : GetRegionIds())
    {
        InterconnectAllCellsOfRegion(regionId);
    }
}

void HP::AbstractGraph::InterconnectAllCellsOfRegion(int regionId)
{
    auto regionCells{ GetCellsFromRegion(regionId) };

    for (auto cell : regionCells)
    {
        SetConnectionsToCell(cell, regionCells, true);
    }
}

void HP::AbstractGraph::RemoveConnectionsOfRegion(int regionId, RegionConnectionType regionConnectionType)
{
    std::vector<Connection> connectionsToRemove{};

    for (auto const& connection : m_Connections)
    {
        auto fromCell{ GetCell(connection.GetFromCell()) };
        auto toCell{ GetCell(connection.GetToCell()) };

        if (fromCell == nullptr or toCell == nullptr) continue;

        switch (regionConnectionType)
        {
        case RegionConnectionType::IntraRegion:
            if (fromCell->GetRegionId() == regionId and toCell->GetRegionId() == regionId)
            {
                connectionsToRemove.push_back(connection);
            }
            break;
        case RegionConnectionType::InterRegion:
            if ((fromCell->GetRegionId() == regionId and toCell->GetRegionId() != regionId)
                or
                (fromCell->GetRegionId() != regionId and toCell->GetRegionId() == regionId)
                )
            {
                connectionsToRemove.push_back(connection);
            }
            break;
        case RegionConnectionType::BothRegions:
            if (fromCell->GetRegionId() == regionId or toCell->GetRegionId() == regionId)
            {
                connectionsToRemove.push_back(connection);
            }
            break;
        }
    }

    for (auto const& connection : connectionsToRemove)
    {
        auto iter{ std::ranges::find(m_Connections, connection) };
        if (iter != m_Connections.end())
        {
            m_Connections.erase(iter);
        }
    }
}

HP::AbstractGraph::AbstractGraph(HierarchicalGrid* pHGrid)
    : m_pHGrid{ pHGrid }
{
}
