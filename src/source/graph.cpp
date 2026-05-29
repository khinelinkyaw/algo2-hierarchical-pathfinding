#include <cell.h>
#include <connection.h>
#include <graph/graph.h>

#include <algorithm>

void HP::Graph::ChangeConnectionsActiveStateToCell(int cellId, bool state)
{
    auto connectionsFromCell{ GetConnectionsFromCell(cellId) };

    for (auto connection : connectionsFromCell)
    {
        auto neighborCell{ GetCell(connection->GetToCell()) };

        if (neighborCell->GetCellType() == CellType::Obstacle and state == true)
        {
            continue;
        }

        auto reverseConnection{ Connection{ connection->GetToCell(), connection->GetFromCell() } };

        if (auto iter{ std::ranges::find(m_Connections, *connection) }; iter != m_Connections.end())
        {
            iter->SetActive(state);
        }

        if (auto iter{ std::ranges::find(m_Connections, reverseConnection) }; iter != m_Connections.end())
        {
            iter->SetActive(state);
        }
    }
}
