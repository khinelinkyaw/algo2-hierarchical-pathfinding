#ifndef A_STAR_H
#define A_STAR_H

#include <cell.h>
#include <connection.h>
#include <graph/graph.h>
#include <graph/grid.h>
#include <pathfinding/heuristics.h>

#include <memory>
#include <structs.h>
#include <vector>

namespace HP
{
	class AStar final
	{
	public:
		struct PathResult
		{
            float totalCost = 0.f;
            bool pathFound = true;
            bool intraRegionPath = false;
            int pathNodeLength = 0;
            int regionsTraversed = 0;
		};

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
		static inline std::unique_ptr<Heuristic> m_Heuristic{ std::make_unique<Euclidean>() };

		static void BacktrackFullPath(std::vector<CellRecord> const& ClosedList, CellRecord const& startingCellRecord, std::vector<Cell*>& finalPath);

	public:
		template<typename T>
		static void SetHeuristic();

		static float GetHeuristicCost(Cell const& startCell, Cell const& endCell, Graph* pGraph);
        static float GetHeuristicCost(int startCellId, int endCellId, Graph* pGraph);
		static std::vector<vec2<float>> ConvertToFloatPath(std::vector<Cell*> const& cellPath, Grid* pGrid);
		static PathResult FindPath(Cell* const pStartCell, Cell* const pDestCell, Graph* pGraph, std::vector<Cell*>* finalPath);
	};

	template<typename T>
	inline void AStar::SetHeuristic()
	{
		m_Heuristic = std::make_unique<T>();
	}
}

#endif