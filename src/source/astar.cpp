#include <graph/graph.h>
#include <graph/grid.h>
#include <pathfinding/astar.h>
#include <structs.h>

#include <algorithm>
#include <cstdlib>
#include <cell.h>
#include <vector>
#include <set>

using namespace HP;

float AStar::GetHeuristicCost(Cell const& startCell, Cell const& endCell, Graph* pGraph)
{
    return GetHeuristicCost(startCell.GetId(), endCell.GetId(), pGraph);
}

float HP::AStar::GetHeuristicCost(int startCellId, int endCellId, Graph* pGraph)
{
	auto toDest{
		pGraph->GetCellPosition(startCellId) - pGraph->GetCellPosition(endCellId)
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

AStar::PathResult AStar::FindPath(Cell* const pStartCell, Cell* const pDestCell, Graph* pGraph, std::vector<Cell*>* finalPath)
{
    PathResult result{};
	std::vector<CellRecord> OpenList{};
	std::vector<CellRecord> ClosedList{};

	CellRecord CurrentNodeRecord{};
	CurrentNodeRecord.pCell = pStartCell;
	CurrentNodeRecord.pConnection = nullptr;
	CurrentNodeRecord.costSoFar = 0.0f;
    CurrentNodeRecord.distanceToDest = GetHeuristicCost(*pStartCell, *pDestCell, pGraph);
	CurrentNodeRecord.CalculateEstimatedTotalCost();
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
			result.totalCost = CurrentNodeRecord.estimatedTotalCost;
			break;
		}

		auto Connections = pGraph->GetConnectionsFromCell(CurrentNodeId);

		for (auto Conn : Connections)
		{
			NeighborCellRecord.pCell = pGraph->GetCell(Conn->GetToCell());
			NeighborCellRecord.pConnection = Conn;
			NeighborCellRecord.costSoFar = CurrentNodeRecord.costSoFar + Conn->GetWeight();
			NeighborCellRecord.distanceToDest = GetHeuristicCost(*(NeighborCellRecord.pCell), *pDestCell, pGraph);
            NeighborCellRecord.CalculateEstimatedTotalCost();

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
			return RecordA.distanceToDest < RecordB.distanceToDest;
		}) };

		CurrentNodeRecord = *ClosestNodeIter;
        result.pathFound = false;
	}

	if (finalPath != nullptr)
	{
		BacktrackFullPath(ClosedList, CurrentNodeRecord, *finalPath);

		std::set<int> regionIds{};
		for (auto cell : *finalPath)
		{
            regionIds.insert(cell->GetRegionId());
		}

		if (regionIds.size() == 1) result.intraRegionPath = true;
	}

    return result;
}
