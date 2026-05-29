#ifndef GRAPH_H
#define GRAPH_H

#include <cell.h>
#include <connection.h>
#include <structs.h>

#include <vector>

namespace HP
{
    class Graph
    {
    protected:
        std::vector<Connection> m_Connections{};

    public:
        virtual std::vector<Cell*> GetConnectedCells(int cellId) = 0;
        virtual std::vector<Connection*> GetConnectionsFromCell(int cellId) = 0;

        virtual std::vector<Cell*> GetCellsFromRegion(int regionId) = 0;
        virtual void CreateConnection(int cellAId, int cellBId) = 0;
        virtual void CreateConnection(Cell* cellA, Cell* cellB) = 0;

        virtual Cell* GetCell(int cellId) = 0;
        virtual vec2<int> GetCellPosition(int cellId) const = 0;

        virtual void ChangeConnectionsActiveStateToCell(int cellId, bool state);

        virtual void Draw() const = 0;

        virtual ~Graph() = default;
    };
}

#endif