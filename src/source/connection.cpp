#include <cell.h>
#include <connection.h>
#include <utility>

std::pair<int, int> Connection::GetConnectedCells() const
{
    return std::pair<int, int>{m_FirstEnd, m_SecondEnd};
}

int Connection::GetConnectedToCell(int cellId) const
{
    if (cellId == m_FirstEnd)
    {
        return m_SecondEnd;
    }
    else if (cellId == m_SecondEnd)
    {
        return m_FirstEnd;
    }
    return Cell::INVALID_CELL_ID;
}

Connection::Connection(int firstCell, int secondCell)
    : m_FirstEnd{firstCell}
    , m_SecondEnd{secondCell}
{
}

bool Connection::operator==(Connection const& other) const
{
    return (
        (m_FirstEnd == other.m_FirstEnd and m_SecondEnd == other.m_SecondEnd) or
        (m_FirstEnd == other.m_SecondEnd and m_SecondEnd == other.m_FirstEnd)
        );
}

bool Connection::operator<(Connection const& other) const
{
    return m_FirstEnd < other.m_FirstEnd or (m_FirstEnd == other.m_FirstEnd and m_SecondEnd < other.m_SecondEnd);
}
