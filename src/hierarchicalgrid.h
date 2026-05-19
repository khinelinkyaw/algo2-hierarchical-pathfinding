#ifndef HIERARCHICALGRID_H
#define HIERARCHICALGRID_H

#include <cell.h>
#include <connection.h>
#include <grid.h>
#include <matrix.h>
#include <constants.h>

#include <set>
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
        std::unordered_map<int, Matrix<Cell*>> m_CellRegions{};
        std::vector<Cell*> m_AbstractGraph{};
        std::vector<Connection> m_AbstractConnections{};

        void SubdivideCellsIntoRegions();
        void BuildAbstractGraph();

        void BuildAbstractInterRegion();

        std::set<Cell*> GetCellsOfRegion(int regionId);
        std::vector<Cell*> GetAbstractCellsOfRegion(int regionId);

        void SetConnectionsToCell(Cell* cell, std::vector<Cell*> toConnCells);
        void CreateAbstractConnection(Cell* cellA, Cell* cellB);
        void CreateAbstractConnection(int cellAId, int cellBId);
        std::set<Connection*> GetExternalConnectionFromRegion(int regionId);

    public:
        void Draw() const override;
        void DrawAbstractConnections() const;

        HierarchicalGrid() = default;
        HierarchicalGrid(int rows, int cols, int posX = 0, int posY = 0, int width = Consts::SCREEN_WIDTH, int height = Consts::SCREEN_HEIGHT);
        ~HierarchicalGrid() override = default;
    };
}

#endif