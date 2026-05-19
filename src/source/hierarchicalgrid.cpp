#include <graph/hierarchicalgrid.h>

#include <cell.h>
#include <graph/grid.h>

#include <cmath>
#include <connection.h>
#include <vector>
#include <utility>

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
}

HP::HierarchicalGrid::HierarchicalGrid(int rows, int cols, int posX, int posY, int width, int height)
    : Grid{ rows, cols, posX, posY, width, height }
{
    SubdivideCellsIntoRegions();
    m_AbstractGraph.BuildAbstractGraph();
}