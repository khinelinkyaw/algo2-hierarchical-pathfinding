#ifndef ABSTRACT_GRAPH_H
#define ABSTRACT_GRAPH_H

#include <cell.h>
#include <connection.h>
#include <graph/graph.h>
#include <structs.h>

#include <vector>

namespace HP
{
    class HierarchicalGrid;
    class AbstractGraph final : public Graph
    {
    private:
        HierarchicalGrid* m_pHGrid{ nullptr };
        std::vector<Connection> m_Connections{};
        std::vector<Cell*> m_Cells{};

        void SetConnectionsToCell(Cell* cell, std::vector<Cell*> toConnCells);

        std::vector<int> GetRegionIds() const;

        void BuildInterRegion();
        void BuildIntraRegion();

    public:
        std::vector<Cell*> GetConnectedCells(int cellId) override;
        std::vector<Connection*> GetConnectionsFromCell(int cellId) override;

        std::vector<Cell*> GetCellsFromRegion(int regionId) override;
        void CreateConnection(int cellAId, int cellBId) override;
        void CreateConnection(Cell* cellA, Cell* cellB) override;

        Cell* GetCell(int cellId) override;
        vec2<int> GetCellPosition(int cellId) const override;

        std::vector<Cell*>& GetCells() { return m_Cells; }

        void BuildAbstractGraph();

        void Draw() const;

        AbstractGraph(HierarchicalGrid* pHGrid);
        ~AbstractGraph() override = default;
    };
}

#endif