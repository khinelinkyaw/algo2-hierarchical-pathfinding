#ifndef GRID_H
#define GRID_H

#include <cell.h>
#include <connection.h>
#include <constants.h>
#include <matrix.h>
#include <structs.h>
#include <graph/graph.h>

#include <vector>

namespace HP
{
    class Grid : public Graph
    {
    private:
        std::vector<Connection> m_Connections{};

    protected:
        vec2<int> m_Dimensions{};
        int m_CellWidth{};
        int m_CellHeight{};
        Matrix<Cell> m_Cells{ 0, 0 };
        vec2<int> m_Position{};

        vec2<int> ConvertWorldToCellIndex(float worldX, float worldY) const;
        std::vector<Connection*> GetConnectionFromCells(std::vector<Cell*> const& cells);

    public:
        std::vector<Cell*> GetConnectedCells(int cellId) override;
        std::vector<Connection*> GetConnectionsFromCell(int cellId) override;

        std::vector<Cell*> GetCellsFromRegion(int regionId) override;
        void CreateConnection(int cellAId, int cellBId) override;
        void CreateConnection(Cell* cellA, Cell* cellB) override;

        std::vector<Cell>& GetCells() { return m_Cells.GetData(); }

        Cell* GetCell(int cellId) override;
        Cell* GetCell(float worldX, float worldY);

        vec2<int> GetCellCenter(Cell const& cell) const;
        vec2<int> GetCellCenter(int cellId) const;
        vec2<int> GetCellPosition(int cellId) const override;

        std::vector<Connection*> GetExternalConnectionFromRegion(int regionId);

        std::vector<Connection*> FindCommonConnections(std::vector<Connection*> connectionsA, std::vector<Connection*> connectionsB);
        std::vector<Cell*> GetCellsFromConnections(std::vector<Connection*> connections);

        void MouseClicked();
        virtual void Draw() const;

        int GetSize() const { return m_Cells.GetSize(); }

        void GenerationConnections();

        Grid() = default;
        Grid(int rows, int cols, int posX = 0, int posY = 0, int width = Consts::SCREEN_WIDTH, int height = Consts::SCREEN_HEIGHT);
        virtual ~Grid() = default;
    };
}

#endif
