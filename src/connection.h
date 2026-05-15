#ifndef CONNECTION_H
#define CONNECTION_H

#include <cell.h>
#include <utility>

namespace HP
{
    class Connection final
    {
    private:
        int m_FromCell{ Cell::INVALID_CELL_ID };
        int m_ToCell{ Cell::INVALID_CELL_ID };
        float m_Weight{ 1.f };

    public:
        std::pair<int, int> GetConnectedCells() const;
        int GetToCell() const { return m_ToCell; }
        int GetFromCell() const { return m_FromCell; }
        float GetWeight() const { return m_Weight; }

        Connection(int fromCell, int toCell);

        bool operator==(Connection const& other) const;
        bool operator<(Connection const& other) const;
    };
}

#endif