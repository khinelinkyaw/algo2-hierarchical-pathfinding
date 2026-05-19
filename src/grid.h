#ifndef GRID_H
#define GRID_H

#include <cell.h>
#include <connection.h>
#include <constants.h>
#include <matrix.h>
#include <structs.h>

#include <set>
#include <unordered_map>
#include <vector>

namespace HP
{
    class Grid
    {
    protected:
        vec2<int> m_Dimensions{};
        int m_CellWidth{};
        int m_CellHeight{};
        Matrix<Cell> m_Cells{ 0, 0 };
        std::vector<Connection> m_Connections{};
        vec2<int> m_Position{};

        void CreateNewConnection(int cellAId, int cellBId);
        vec2<int> ConvertWorldToCellIndex(float worldX, float worldY) const;
        std::vector<Connection*> FindCommonConnections(std::set<Connection*> connectionsA, std::set<Connection*> connectionsB);
        std::vector<Cell*> GetCellsFromConnections(std::vector<Connection*> connections);
        std::vector<Connection*> GetConnectionFromCells(std::set<Cell*> const& cells);

    public:
        Cell* GetCell(int cellId);
        Cell* GetCell(float worldX, float worldY);

        vec2<int> GetCellCenter(Cell const& cell) const;
        vec2<int> GetCellCenter(int cellId) const;
        std::vector<Connection*> FindConnectionsFromCell(int cellId);
        std::vector<Cell*> FindConnectedCells(int cellId);

        void MouseClicked();
        virtual void Draw() const;

        void GenerationConnections();

        Grid() = default;
        Grid(int rows, int cols, int posX = 0, int posY = 0, int width = Consts::SCREEN_WIDTH, int height = Consts::SCREEN_HEIGHT);
        virtual ~Grid() = default;
    };

    class HierarchicalGrid final : public Grid
    {
    private:
        static int constexpr REGION_SIZE{ 4 };
        static float constexpr DRAW_MARGIN{ 15.f };

        Grid m_RegionGrid{};
        std::unordered_map<int, Matrix<Cell*>> m_CellRegions{};
        std::vector<Cell*> m_AbstractGraph{};
        std::vector<Connection> m_AbstractConnections{};

        void SubdivideCellsIntoRegions();

        void BuildAbstractGraph();

        std::set<Cell*> GetCellsOfRegion(int regionId);
        std::set<Connection*> GetExternalConnectionFromRegion(int regionId);

    public:
        void Draw() const override;

        HierarchicalGrid() = default;
        HierarchicalGrid(int rows, int cols, int posX = 0, int posY = 0, int width = Consts::SCREEN_WIDTH, int height = Consts::SCREEN_HEIGHT);
        ~HierarchicalGrid() override = default;
    };
}

#endif
