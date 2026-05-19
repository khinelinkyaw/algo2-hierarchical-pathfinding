#ifndef CELL_H
#define CELL_H

namespace HP
{
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
        int m_Id{ INVALID_CELL_ID };
        int m_RegionId{ INVALID_CELL_ID };
        CellType m_CellType{ CellType::Empty };

    public:
        int GetId() const;
        CellType GetCellType() const;
        void SetCellType(CellType cellType);

        int GetRegionId() const { return m_RegionId; }
        void SetRegionId(int regionId) { m_RegionId = regionId; }

        Cell() = default;
        Cell(int id, CellType cellType);
    };
}

#endif