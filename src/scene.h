#ifndef SCENE_H
#define SCENE_H

#include <grid.h>
#include <cell.h>
#include <agent.h>
#include <hierarchicalgrid.h>

#include <memory>

namespace HP
{
    class Scene final
    {
    private:
        std::unique_ptr<Grid> m_Grid{ std::make_unique<HierarchicalGrid>(20,20) };
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
