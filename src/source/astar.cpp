#include <cell.h>
#include <pathfinding/astar.h>
#include <structs.h>

#include <algorithm>
#include <cstdlib>
//#include <pathfinding/heuristics.h>
#include <vector>

using namespace HP;

float AStar::GetHeuristicCost(Cell const& startCell, Cell const& endCell, Grid* pGrid)
{
    return GetHeuristicCost(startCell.GetId(), endCell.GetId(), pGrid);
}

float HP::AStar::GetHeuristicCost(int startCellId, int endCellId, Grid* pGrid)
{
	auto toDest{
		pGrid->GetCellPosition(startCellId) - pGrid->GetCellPosition(endCellId)
	};
	return static_cast<float>(abs(static_cast<float>(toDest.x)) + abs(static_cast<float>(toDest.y)));
}

void AStar::BacktrackFullPath(std::vector<CellRecord> const& ClosedList, CellRecord const& startingCellRecord, std::vector<Cell*>& finalPath)
{
	CellRecord currentCellRecord{ startingCellRecord };

	while (currentCellRecord.pConnection != nullptr)
	{
		finalPath.push_back(currentCellRecord.pCell);

		auto NextNodeIter = std::ranges::find_if(ClosedList, [&currentCellRecord](CellRecord const& Record)
			{
				return currentCellRecord.pConnection->GetFromCell() == Record.pCell->GetId();
			});

		currentCellRecord = *NextNodeIter;
	}

	finalPath.push_back(currentCellRecord.pCell);
	std::ranges::reverse(finalPath);
}

std::vector<vec2<float>> AStar::ConvertToFloatPath(std::vector<Cell*> const& cellPath, Grid* pGrid)
{
	std::vector<vec2<float>> floatPath{};
	floatPath.reserve(cellPath.size());

	for (auto cellPtr : cellPath)
	{
		auto cellCenter{ pGrid->GetCellCenter(*cellPtr) };
		floatPath.emplace_back(
			static_cast<float>(cellCenter.x),
			static_cast<float>(cellCenter.y)
		);
	}

	return floatPath;
}

float AStar::FindPath(Cell* const pStartCell, Cell* const pDestCell, Grid* pGrid, std::vector<Cell*>* finalPath)
{
	float totalCost{};
	std::vector<CellRecord> OpenList{};
	std::vector<CellRecord> ClosedList{};

	CellRecord CurrentNodeRecord{};
	CurrentNodeRecord.pCell = pStartCell;
	CurrentNodeRecord.pConnection = nullptr;
	CurrentNodeRecord.costSoFar = 0.0f;
	CurrentNodeRecord.estimatedTotalCost = GetHeuristicCost(*pStartCell, *pDestCell, pGrid);
	OpenList.push_back(CurrentNodeRecord);

	CellRecord NeighborCellRecord{};
	auto FindNeighborNodeId{ [&NeighborCellRecord](CellRecord const& InnerRecord)
	{
		return	InnerRecord.pCell->GetId() == NeighborCellRecord.pCell->GetId();
	} };

	while (!OpenList.empty())
	{
		auto NodeRecordIter = std::min_element(OpenList.begin(), OpenList.end());
		CurrentNodeRecord = *NodeRecordIter;
		auto CurrentNodeId{ CurrentNodeRecord.pCell->GetId() };

		if (CurrentNodeRecord.pCell == pDestCell)
		{
			ClosedList.push_back(CurrentNodeRecord);
			OpenList.clear();
            totalCost = CurrentNodeRecord.estimatedTotalCost;
			break;
		}

		auto Connections = pGrid->FindConnectionsFromCell(CurrentNodeId);

		for (auto Conn : Connections)
		{
			NeighborCellRecord.pCell = pGrid->GetCell(Conn->GetToCell());
			NeighborCellRecord.pConnection = Conn;
			NeighborCellRecord.costSoFar = CurrentNodeRecord.costSoFar + Conn->GetWeight();
			NeighborCellRecord.estimatedTotalCost = NeighborCellRecord.costSoFar + GetHeuristicCost(*(NeighborCellRecord.pCell), *pDestCell, pGrid);

			auto OpenListIter = std::ranges::find_if(OpenList, FindNeighborNodeId);
			auto ClosedListIter = std::ranges::find_if(ClosedList, FindNeighborNodeId);

			if (OpenListIter != OpenList.end() and NeighborCellRecord.costSoFar < OpenListIter->costSoFar)
			{
				OpenList.erase(OpenListIter);
			}
			else if (ClosedListIter != ClosedList.end() and NeighborCellRecord.costSoFar < ClosedListIter->costSoFar)
			{
				ClosedList.erase(ClosedListIter);
			}
			else if (OpenListIter != OpenList.end() or ClosedListIter != ClosedList.end())
			{
				continue;
			}

			OpenList.push_back(NeighborCellRecord);
		}

		ClosedList.push_back(CurrentNodeRecord);
		std::erase(OpenList, CurrentNodeRecord);
	}

	auto GoalNodeIter = std::ranges::find_if(ClosedList, [&pDestCell](CellRecord const& InnerRecord)
		{
			return InnerRecord.pCell->GetId() == pDestCell->GetId();
		});

	if (GoalNodeIter == ClosedList.end())
	{
		auto ClosestNodeIter{ std::ranges::min_element(ClosedList, [](CellRecord const& RecordA, CellRecord const& RecordB)
		{
			return RecordA.estimatedTotalCost < RecordB.estimatedTotalCost;
		}) };

		CurrentNodeRecord = *ClosestNodeIter;
	}

	if (finalPath != nullptr)
	{
		BacktrackFullPath(ClosedList, CurrentNodeRecord, *finalPath);
	}
    return totalCost;
}
