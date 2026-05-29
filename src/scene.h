#ifndef SCENE_H
#define SCENE_H

#include <cell.h>
#include <agent.h>
#include <graph/hierarchicalgrid.h>

#include <memory>

namespace HP
{
    class Scene final
    {
    private:
        std::unique_ptr<HierarchicalGrid> m_Grid{ std::make_unique<HierarchicalGrid>(30,30) };
        Agent m_Agent{};
        Cell* m_DestCell{};

    public:
        void Update();
        void RefreshPathForAgent();
        void Draw() const;

        Scene();
    };
}

#endif
