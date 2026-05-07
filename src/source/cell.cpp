#include <cell.h>

int Cell::GetId() const
{
    return m_Id;
}

CellType Cell::GetCellType() const
{
    return m_CellType;
}

void Cell::SetCellType(CellType cellType)
{
    m_CellType = cellType;
}

Cell::Cell(int id, CellType cellType)
    : m_Id{id}
    , m_CellType{cellType}
{

}
