#ifndef SCENE_H
#define SCENE_H

#include "grid.h"

class Scene final
{
private:
    Grid m_Grid{};

public:
    void SetGrid(Grid const& grid) { m_Grid = grid; }
    void SetGrid(Grid&& grid) { m_Grid = grid; }

    void Draw();
};

#endif
