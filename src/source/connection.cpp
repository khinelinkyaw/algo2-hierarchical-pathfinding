#include <cell.h>
#include <connection.h>
#include <utility>

std::pair<int, int> Connection::GetConnectedCells() const
{
    return std::pair<int, int>{m_FromCell, m_ToCell};
}

Connection::Connection(int fromCell, int toCell)
    : m_FromCell{ fromCell }
    , m_ToCell{ toCell }
{
}

bool Connection::operator==(Connection const& other) const
{
    return m_FromCell == other.m_FromCell and m_ToCell == other.m_ToCell;
}

bool Connection::operator<(Connection const& other) const
{
    return m_FromCell < other.m_FromCell or (m_FromCell == other.m_FromCell and m_ToCell < other.m_ToCell);
}
