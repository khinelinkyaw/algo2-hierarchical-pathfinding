#include <cell.h>
#include <pathfinding/astar.h>
#include <structs.h>

#include <algorithm>
#include <cstdlib>
//#include <pathfinding/heuristics.h>
#include <vector>

float AStar::GetHeuristicCost(Cell const& startCell, Cell const& endCell) const
{
	auto toDest{
		m_Grid->GetCellCenter(startCell) - m_Grid->GetCellCenter(endCell)
	};
	return static_cast<float>(abs(static_cast<float>(toDest.x)) + abs(static_cast<float>(toDest.y)));
}

std::vector<Cell*> AStar::BacktrackFullPath(std::vector<CellRecord> const& ClosedList, CellRecord const& startingCellRecord)
{
	std::vector<Cell*> Path{};
	CellRecord currentCellRecord{ startingCellRecord };

	while (currentCellRecord.pConnection != nullptr)
	{
		Path.push_back(currentCellRecord.pCell);

		auto NextNodeIter = std::ranges::find_if(ClosedList, [&currentCellRecord](CellRecord const& Record)
			{
				return currentCellRecord.pConnection->GetFromCell() == Record.pCell->GetId();
			});

		currentCellRecord = *NextNodeIter;
	}

	Path.push_back(currentCellRecord.pCell);
	std::ranges::reverse(Path);
	return Path;
}

std::vector<vec2<float>> AStar::ConvertToFloatPath(std::vector<Cell*> const& cellPath)
{
	std::vector<vec2<float>> floatPath{};
	floatPath.reserve(cellPath.size());

	for (auto cellPtr : cellPath)
	{
		auto cellCenter{ m_Grid->GetCellCenter(*cellPtr) };
		floatPath.emplace_back(
			static_cast<float>(cellCenter.x),
			static_cast<float>(cellCenter.y)
		);
	}

	return floatPath;
}

std::vector<Cell*> AStar::FindPath(Cell* const pStartCell, Cell* const pDestCell)
{
	std::vector<CellRecord> OpenList{};
	std::vector<CellRecord> ClosedList{};

	CellRecord CurrentNodeRecord{};
	CurrentNodeRecord.pCell = pStartCell;
	CurrentNodeRecord.pConnection = nullptr;
	CurrentNodeRecord.costSoFar = 0.0f;
	CurrentNodeRecord.estimatedTotalCost = GetHeuristicCost(*pStartCell, *pDestCell);
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
			break;
		}

		auto Connections = m_Grid->FindConnectionsFromCell(CurrentNodeId);

		for (auto Conn : Connections)
		{
			NeighborCellRecord.pCell = m_Grid->GetCell(Conn->GetToCell());
			NeighborCellRecord.pConnection = Conn;
			NeighborCellRecord.costSoFar = CurrentNodeRecord.costSoFar + Conn->GetWeight();
			NeighborCellRecord.estimatedTotalCost = NeighborCellRecord.costSoFar + GetHeuristicCost(*(NeighborCellRecord.pCell), *pDestCell);

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

	return BacktrackFullPath(ClosedList, CurrentNodeRecord);
}
