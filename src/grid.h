#ifndef GRID_H
#define GRID_H

#include <structs.h>
#include <constants.h>
#include <cell.h>
#include <connection.h>

#include <vector>

class Grid final
{
private:
    int m_Rows{};
    int m_Cols{};
    std::vector<Cell> m_Cells{};
    std::vector<Connection> m_Connections{};
    vec2<int> m_Position{};
    vec2<int> m_Dimensions{};

    Cell* GetCell(int cellId);
public:
    void Update();
    void Draw() const;

    Grid() = default;
    Grid(int rows, int cols, int posX = 0, int posY = 0, int width = Consts::SCREEN_WIDTH, int height = Consts::SCREEN_HEIGHT);
};

#endif
