#ifndef HIERARCHICALGRID_H
#define HIERARCHICALGRID_H

#include <cell.h>
#include <constants.h>
#include <graph/abstractgraph.h>
#include <graph/grid.h>
#include <matrix.h>

#include <unordered_map>
#include <vector>

namespace HP
{
    class HierarchicalGrid final : public Grid
    {
    private:
        static int constexpr REGION_SIZE{ 3 };
        static float constexpr DRAW_MARGIN{ 15.f };

        Grid m_RegionGrid{};
        AbstractGraph m_AbstractGraph{ this };
        std::vector<Cell*> m_AbstractPath{};
        std::unordered_map<int, Matrix<Cell*>> m_CellRegions{};

        void SubdivideCellsIntoRegions();
        
    public:
        void Draw() const override;

        Grid GetRegionGrid() { return m_RegionGrid; }

        void GenerationConnections() override;

        std::vector<Cell*> FindPath(Cell* const pStartCell, Cell* const pDestCell);

        HierarchicalGrid() = default;
        HierarchicalGrid(int rows, int cols, int posX = 0, int posY = 0, int width = Consts::SCREEN_WIDTH, int height = Consts::SCREEN_HEIGHT);
        ~HierarchicalGrid() override = default;
    };
}

#endif