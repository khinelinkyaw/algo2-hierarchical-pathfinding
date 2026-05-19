#ifndef GRID_H
#define GRID_H

#include <cell.h>
#include <connection.h>
#include <constants.h>
#include <matrix.h>
#include <structs.h>

#include <set>
#include <vector>

namespace HP
{
    class Grid
    {
    private:
        std::vector<Connection> m_Connections{};

    protected:
        vec2<int> m_Dimensions{};
        int m_CellWidth{};
        int m_CellHeight{};
        Matrix<Cell> m_Cells{ 0, 0 };
        vec2<int> m_Position{};

        void CreateNewConnection(int cellAId, int cellBId);
        vec2<int> ConvertWorldToCellIndex(float worldX, float worldY) const;
        std::vector<Connection*> FindCommonConnections(std::set<Connection*> connectionsA, std::set<Connection*> connectionsB);
        std::vector<Cell*> GetCellsFromConnections(std::vector<Connection*> connections);
        std::vector<Connection*> GetConnectionFromCells(std::set<Cell*> const& cells);
        bool CheckTwoCells(int cellAId, int cellBId);

    public:
        std::vector<Cell>& GetCells() { return m_Cells.GetData(); }

        Cell* GetCell(int cellId);
        Cell* GetCell(float worldX, float worldY);

        vec2<int> GetCellCenter(Cell const& cell) const;
        vec2<int> GetCellCenter(int cellId) const;
        vec2<int> GetCellPosition(int cellId) const;

        std::vector<Connection*> FindConnectionsFromCell(int cellId);
        std::vector<Cell*> FindConnectedCells(int cellId);

        void MouseClicked();
        virtual void Draw() const;

        void GenerationConnections();

        Grid() = default;
        Grid(int rows, int cols, int posX = 0, int posY = 0, int width = Consts::SCREEN_WIDTH, int height = Consts::SCREEN_HEIGHT);
        virtual ~Grid() = default;
    };
}

#endif
