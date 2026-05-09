#ifndef SCENE_H
#define SCENE_H

#include "grid.h"
#include "agent.h"
#include <pathfinding/astar.h>

class Scene final
{
private:
    Grid m_Grid{};
    Agent m_Agent{};
    AStar m_Astar{};

public:
    void SetGrid(Grid const& grid);
    void SetGrid(Grid&& grid);

    void Update();
    void Draw() const;
};

#endif
