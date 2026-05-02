#ifndef SCENE_H
#define SCENE_H

#include "grid.h"
#include "agent.h"

class Scene final
{
private:
    Grid m_Grid{};
    Agent m_Agent{};

public:
    void SetGrid(Grid const& grid) { m_Grid = grid; }
    void SetGrid(Grid&& grid) { m_Grid = grid; }

    void Update();
    void Draw() const;
};

#endif
