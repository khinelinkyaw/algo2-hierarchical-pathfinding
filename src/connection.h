#ifndef CONNECTION_H
#define CONNECTION_H

#include <cell.h>
#include <utility>

class Connection final
{
private:
    int m_FirstEnd{ Cell::INVALID_CELL_ID };
    int m_SecondEnd{ Cell::INVALID_CELL_ID };

public:
    std::pair<int, int> GetConnectedCells() const;
    int GetConnectedToCell(int cellId) const;

    Connection(int firstCell, int secondCell);

    bool operator==(Connection const& other) const;
    bool operator<(Connection const& other) const;
};

#endif