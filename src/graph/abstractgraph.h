#ifndef ABSTRACT_GRAPH_H
#define ABSTRACT_GRAPH_H

#include <cell.h>
#include <connection.h>
#include <graph/graph.h>
#include <structs.h>
#include <pathfinding/astar.h>

#include <vector>

namespace HP
{
    enum class RegionConnectionType
    {
        InterRegion,
        IntraRegion,
        BothRegions
    };

    class HierarchicalGrid;
    class AbstractGraph final : public Graph
    {
    private:
        HierarchicalGrid* m_pHGrid{ nullptr };
        std::vector<Cell*> m_Cells{};

        void SetConnectionsToCell(Cell* cell, std::vector<Cell*> toConnCells, bool intraRegion);

        std::vector<int> GetRegionIds() const;

        void BuildInterRegion();
        void GenerateInterRegionConnections(int regionId);
        void BuildIntraRegion();

        void InterconnectAllCellsOfRegion(int regionId);

        void RemoveConnectionsOfRegion(int regionId, RegionConnectionType regionConnectionType);

        void AddCell(Cell* cell);
        void CreateConnection(Cell* cellA, Cell* cellB, bool intraRegion);

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

        void Draw() const override;

        AStar::PathResult FindPath(Cell* const pStartCell, Cell* const pDestCell, std::vector<Cell*>* finalPath);

        void ChangeConnectionsActiveStateToCell(int cellId, bool state) override;

        AbstractGraph(HierarchicalGrid* pHGrid);
        ~AbstractGraph() override = default;
    };
}

#endif