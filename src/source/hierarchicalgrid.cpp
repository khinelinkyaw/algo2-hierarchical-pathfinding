#include <graph/hierarchicalgrid.h>

#include <cell.h>
#include <graph/grid.h>
#include <pathfinding/astar.h>
#include <structs.h>

#include <raylib.h>

#include <cmath>
#include <connection.h>
#include <vector>
#include <set>
#include <utility>
#include <algorithm>

using namespace HP;

void HierarchicalGrid::SubdivideCellsIntoRegions()
{
    m_CellRegions.clear();

    int numRegionsX{ static_cast<int>(std::ceil(static_cast<float>(m_Cells.GetColSize()) / REGION_SIZE)) };
    int numRegionsY{ static_cast<int>(std::ceil(static_cast<float>(m_Cells.GetRowSize()) / REGION_SIZE)) };

    m_RegionGrid = Grid{ numRegionsY, numRegionsX };

    int regionId{ 0 };
    for (int regionY{ 0 }; regionY < numRegionsY; ++regionY)
    {
        for (int regionX{ 0 }; regionX < numRegionsX; ++regionX)
        {
            auto cellMatrix{ m_Cells.GetSubMatrix(regionY * REGION_SIZE, regionX * REGION_SIZE, REGION_SIZE, REGION_SIZE) };
            auto matrixData{ cellMatrix.GetData() };

            for (auto& cell : matrixData)
            {
                cell->SetRegionId(regionId);
            }

            m_CellRegions.insert({ regionId, std::move(cellMatrix) });

            ++regionId;
        }
    }
}


void HP::HierarchicalGrid::BuildAbstractGraph()
{
    BuildAbstractInterRegion();

    for (auto& region : m_RegionGrid.GetCells())
    {
        int regionId{ region.GetId() };

        auto regionCells{ GetAbstractCellsOfRegion(regionId) };

        for (auto cell : regionCells)
        {
            SetConnectionsToCell(cell, regionCells);
        }
    }
}


void HP::HierarchicalGrid::BuildAbstractInterRegion()
{
    for (auto& region : m_CellRegions)
    {
        int regionId{ region.first };
        auto connectedRegions{ m_RegionGrid.GetConnectedCells(regionId) };

        auto regionExtConn{ GetExternalConnectionFromRegion(regionId) };

        for (auto connRegion : connectedRegions)
        {
            int neighborRegionId{ connRegion->GetId() };

            auto neighborExtConn{ GetExternalConnectionFromRegion(neighborRegionId) };

            auto commonConnections{ FindCommonConnections(regionExtConn, neighborExtConn) };

            auto commonCells{ GetCellsFromConnections(commonConnections) };

            for (auto conn : commonConnections)
            {
                auto iter{ std::ranges::find(m_AbstractConnections, *conn) };

                if (iter != m_AbstractConnections.end()) continue;

                CreateAbstractConnection(conn->GetFromCell(), conn->GetToCell());
            }

            for (auto cell : commonCells)
            {
                auto iter{ std::ranges::find(m_AbstractGraph, cell) };

                if (iter == m_AbstractGraph.end() and cell->GetRegionId() == regionId)
                {
                    m_AbstractGraph.push_back(cell);
                }
            }
        }
    }
}

std::set<Cell*> HP::HierarchicalGrid::GetCellsOfRegion(int regionId)
{
    std::set<Cell*> result{};

    for (auto& cell : m_Cells.GetData())
    {
        if (cell.GetRegionId() == regionId)
        {
            result.insert(&cell);
        }
    }

    return result;
}


std::vector<Cell*> HP::HierarchicalGrid::GetAbstractCellsOfRegion(int regionId)
{
    std::vector<Cell*> result{};

    for (auto cell : m_AbstractGraph)
    {
        if (cell->GetRegionId() == regionId)
        {
            result.push_back(cell);
        }
    }

    return result;
}


void HP::HierarchicalGrid::SetConnectionsToCell(Cell* cell, std::vector<Cell*> toConnCells)
{
    for (auto connCell : toConnCells)
    {
        if (cell->GetId() == connCell->GetId()) continue;

        CreateAbstractConnection(cell, connCell);
        CreateAbstractConnection(connCell, cell);
    }
}

void HP::HierarchicalGrid::CreateAbstractConnection(Cell* cellA, Cell* cellB)
{
    if (cellA != nullptr and cellB != nullptr)
    {
        Connection connection{ cellA->GetId(), cellB->GetId() };
        auto iter{ std::ranges::find(m_AbstractConnections, connection) };

        if (iter == m_AbstractConnections.end())
        {
            connection.SetWeight(AStar::FindPath(cellA, cellB, this, nullptr));
            m_AbstractConnections.push_back(connection);
        }
    }
}


void HP::HierarchicalGrid::CreateAbstractConnection(int cellAId, int cellBId)
{
    CreateAbstractConnection(GetCell(cellAId), GetCell(cellBId));
}

std::vector<Connection*> HP::Grid::GetConnectionFromCells(std::set<Cell*> const& cells)
{
    std::vector<Connection*> result{};

    for (auto cell : cells)
    {
        auto connections{ GetConnectionsFromCell(cell->GetId()) };
        result.insert(result.end(), connections.begin(), connections.end());
    }

    return result;
}


std::set<Connection*> HP::HierarchicalGrid::GetExternalConnectionFromRegion(int regionId)
{
    std::set<Connection*> result{};

    auto iter{ m_CellRegions.find(regionId) };

    if (iter != m_CellRegions.end())
    {
        auto cells{ GetCellsOfRegion(regionId) };
        auto connections{ GetConnectionFromCells(cells) };

        for (auto connection : connections)
        {
            auto toCell{ GetCell(connection->GetToCell()) };

            if (toCell == nullptr) continue;

            auto toCellIter{ std::ranges::find(cells, toCell) };

            if (toCellIter != cells.end()) continue;

            result.insert(connection);
        }
    }

    return result;
}
void HP::HierarchicalGrid::Draw() const
{
    Grid::Draw();

    DrawAbstractConnections();

    for (auto const& cell : m_AbstractGraph)
    {
        auto cellCenter{ GetCellCenter(cell->GetId()) };
        DrawCircle(
            cellCenter.x,
            cellCenter.y,
            10.f,
            { 33, 190, 55, 100 }
        );
    }
}



void HP::HierarchicalGrid::DrawAbstractConnections() const
{
    for (auto const& connection : m_AbstractConnections)
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

HP::HierarchicalGrid::HierarchicalGrid(int rows, int cols, int posX, int posY, int width, int height)
    : Grid{ rows, cols, posX, posY, width, height }
{
    SubdivideCellsIntoRegions();
    BuildAbstractGraph();
}