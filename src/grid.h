#ifndef GRID_H
#define GRID_H

#include "structs.h"
#include "constants.h"

#include <vector>

class Grid final
{
private:
    enum class CellType
    {
        Empty,
        Obstacle
    };

    int m_Rows{};
    int m_Cols{};
    std::vector<std::vector<CellType>> m_Cells{};
    vec2<int> m_Position{};
    vec2<int> m_Dimensions{};

public:
    void Update();
    void Draw() const;

    Grid() = default;
    Grid(int rows, int cols, int posX = 0, int posY = 0, int width = Consts::SCREEN_WIDTH, int height = Consts::SCREEN_HEIGHT);
};

#endif
