#ifndef HIERARCHICALGRID_H
#define HIERARCHICALGRID_H

#include <cell.h>
#include <graph/grid.h>
#include <graph/abstractgraph.h>
#include <matrix.h>
#include <constants.h>

#include <unordered_map>

namespace HP
{
    class HierarchicalGrid final : public Grid
    {
    private:
        static int constexpr REGION_SIZE{ 3 };
        static float constexpr DRAW_MARGIN{ 15.f };

        Grid m_RegionGrid{};
        AbstractGraph m_AbstractGraph{ this };
        std::unordered_map<int, Matrix<Cell*>> m_CellRegions{};

        void SubdivideCellsIntoRegions();
        
    public:
        void Draw() const override;

        Grid GetRegionGrid() { return m_RegionGrid; }

        void GenerationConnections() override;

        HierarchicalGrid() = default;
        HierarchicalGrid(int rows, int cols, int posX = 0, int posY = 0, int width = Consts::SCREEN_WIDTH, int height = Consts::SCREEN_HEIGHT);
        ~HierarchicalGrid() override = default;
    };
}

#endif