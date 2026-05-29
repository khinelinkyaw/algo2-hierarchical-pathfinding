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
        bool m_Active{ true };

    public:
        std::pair<int, int> GetConnectedCells() const;
        int GetToCell() const { return m_ToCell; }
        int GetFromCell() const { return m_FromCell; }

        void SetWeight(float weight) { m_Weight = weight; }
        float GetWeight() const { return m_Weight; }

        void SetActive(bool active) { m_Active = active; }
        bool GetActive() const { return m_Active; }

        Connection(int fromCell, int toCell);

        bool operator==(Connection const& other) const;
        bool operator<(Connection const& other) const;
    };
}

#endif