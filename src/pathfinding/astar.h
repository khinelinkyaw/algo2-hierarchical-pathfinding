#ifndef A_STAR_H
#define A_STAR_H

#include <cell.h>
#include <connection.h>
#include <grid.h>
#include <pathfinding/heuristics.h>

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
			float estimatedTotalCost = 0.f; // f-cost (= costSoFar + h-cost)

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
		Grid* m_Grid;

		float GetHeuristicCost(Cell const& startCell, Cell const& endCell) const;
		std::vector<Cell*> BacktrackFullPath(std::vector<CellRecord> const& ClosedList, CellRecord const& startingCellRecord);

	public:
		void SetGrid(Grid* grid) { m_Grid = grid; }
		std::vector<vec2<float>> ConvertToFloatPath(std::vector<Cell*> const& cellPath);

		std::vector<Cell*> FindPath(Cell* const pStartCell, Cell* const pDestCell);
	};
}

#endif