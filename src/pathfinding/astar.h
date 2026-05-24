#ifndef A_STAR_H
#define A_STAR_H

#include <cell.h>
#include <connection.h>
#include <graph/graph.h>
#include <pathfinding/heuristics.h>
#include <graph/grid.h>

#include <structs.h>
#include <vector>

namespace HP
{
	class AStar final
	{
	public:
		struct CellRecord
		{
			Cell* pCell = nullptr;
			Connection* pConnection = nullptr;
			float costSoFar = 0.f; // accumulated g-costs of all the connections leading up to this one
            float distanceToDest = 0.f; // h-cost
			float estimatedTotalCost = 0.f; // f-cost (= costSoFar + h-cost)

			void CalculateEstimatedTotalCost()
			{
				estimatedTotalCost = costSoFar + distanceToDest;
            }

			bool operator==(const CellRecord& other) const
			{
				return pCell == other.pCell
					&& pConnection == other.pConnection
					&& costSoFar == other.costSoFar
					&& estimatedTotalCost == other.estimatedTotalCost;
			};

			bool operator<(const CellRecord& other) const
			{
				return estimatedTotalCost < other.estimatedTotalCost;
			};
		};

	private:
		HeuristicFunctions::Heuristic m_HeuristicFunction;

		static void BacktrackFullPath(std::vector<CellRecord> const& ClosedList, CellRecord const& startingCellRecord, std::vector<Cell*>& finalPath);

	public:
		static float GetHeuristicCost(Cell const& startCell, Cell const& endCell, Graph* pGraph);
        static float GetHeuristicCost(int startCellId, int endCellId, Graph* pGraph);
		static std::vector<vec2<float>> ConvertToFloatPath(std::vector<Cell*> const& cellPath, Grid* pGrid);
		static float FindPath(Cell* const pStartCell, Cell* const pDestCell, Graph* pGraph, std::vector<Cell*>* finalPath);
	};
}

#endif