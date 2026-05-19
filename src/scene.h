#ifndef SCENE_H
#define SCENE_H

#include "grid.h"
#include "agent.h"
#include <pathfinding/astar.h>

#include <memory>

namespace HP
{
    class Scene final
    {
    private:
        std::unique_ptr<Grid> m_Grid{ std::make_unique<HierarchicalGrid>(10,10) };
        Agent m_Agent{};
        Cell* m_DestCell{};

    public:
        void SetGrid(Grid const& grid);
        void SetGrid(std::unique_ptr<Grid>&& grid);

        void Update();
        void RefreshPathForAgent();
        void Draw() const;

        Scene();
    };
}

#endif
