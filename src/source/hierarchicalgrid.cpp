#include <graph/hierarchicalgrid.h>
#include <cell.h>
#include <graph/grid.h>
#include <connection.h>
#include <pathfinding/astar.h>

#include <raylib.h>

#include <cmath>
#include <utility>
#include <vector>

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

void HP::HierarchicalGrid::Draw() const
{
    Grid::Draw();
    m_AbstractGraph.Draw();

    for (int index{ 0 }; index < static_cast<int>(m_AbstractPath.size()) - 1; ++index)
    {
        auto cellA{ m_AbstractPath[index] };
        auto cellB{ m_AbstractPath[index + 1] };
        auto cellAPos{ GetCellCenter(*cellA) };
        auto cellBPos{ GetCellCenter(*cellB) };
        DrawLineEx(
            { static_cast<float>(cellAPos.x), static_cast<float>(cellAPos.y) },
            { static_cast<float>(cellBPos.x), static_cast<float>(cellBPos.y) },
            5.f,
            ORANGE
        );
    }
}

void HP::HierarchicalGrid::GenerationConnections()
{
    Grid::GenerationConnections();
    m_AbstractGraph.BuildAbstractGraph();
}

std::vector<Cell*> HP::HierarchicalGrid::FindPath(Cell* const pStartCell, Cell* const pDestCell)
{
    m_AbstractPath.clear();
    auto pathResult{ m_AbstractGraph.FindPath(pStartCell, pDestCell, &m_AbstractPath) };

    std::vector<Cell*> path{};

    if (pathResult.pathFound)
    {
        pathResult = AStar::FindPath(pStartCell, pDestCell, this, &path);
    }

    return path;
}

HP::HierarchicalGrid::HierarchicalGrid(int rows, int cols, int posX, int posY, int width, int height)
    : Grid{ rows, cols, posX, posY, width, height }
{
    SubdivideCellsIntoRegions();
    GenerationConnections();
}