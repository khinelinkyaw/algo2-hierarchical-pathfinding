#ifndef GRID_H
#define GRID_H

#include <cell.h>
#include <connection.h>
#include <constants.h>
#include <structs.h>
#include <matrix.h>

#include <raylib.h>

#include <vector>

class Grid final
{
private:
    //int m_Rows{};
    //int m_Cols{};
    vec2<int> m_Dimensions{};
    int m_CellWidth{};
    int m_CellHeight{};
    //std::vector<Cell> m_Cells{};
    MyMatrix<Cell> m_Cells{ 0, 0 };
    std::vector<Connection> m_Connections{};
    vec2<int> m_Position{};

    void CreateNewConnection(int cellAId, int cellBId);
    vec2<int> ConvertWorldToCellIndex(float worldX, float worldY) const;

public:
    Cell* GetCell(int cellId);
    Cell* GetCell(int rowIndex, int colIndex);
    Cell* GetCell(float worldX, float worldY);
    std::vector<Cell*> GetCellsInRegion(Rectangle const& region);

    vec2<int> GetCellCenter(Cell const& cell) const;
    vec2<int> GetCellCenter(int cellId) const;
    std::vector<Connection*> FindConnectionsFromCell(int cellId);

    void MouseClicked();
    void Draw() const;

    Grid() = default;
    Grid(int rows, int cols, int posX = 0, int posY = 0, int width = Consts::SCREEN_WIDTH, int height = Consts::SCREEN_HEIGHT);
    void GenerationConnections();
};

#endif
