#ifndef CELL_H
#define CELL_H

enum class CellType
{
    Empty,
    Obstacle
};

class Cell final
{
public:
    static int constexpr INVALID_CELL_ID{ -1 };
private:
    int m_Id{INVALID_CELL_ID};
    CellType m_CellType{ CellType::Empty };

public:
    int GetId() const;
    CellType GetCellType() const;
    void SetCellType(CellType cellType);

    Cell() = default;
    Cell(int id, CellType cellType);
};

#endif