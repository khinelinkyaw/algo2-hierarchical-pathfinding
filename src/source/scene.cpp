#include <grid.h>
#include <scene.h>

#include <raylib.h>
#include <utility>

void Scene::SetGrid(Grid const& grid)
{
    m_Grid = grid;
    m_Astar.SetGrid(&m_Grid);
}

void Scene::SetGrid(Grid&& grid)
{
    m_Grid = std::move(grid);
    m_Astar.SetGrid(&m_Grid);
}

void Scene::Update()
{
    if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
    {
        auto mousePos{ GetMousePosition() };

        auto [startCellPosX, startCellPosY] { m_Agent.GetPosition() };
        auto startCell{ m_Grid.GetCell(startCellPosX, startCellPosY) };
        auto destCell{ m_Grid.GetCell(mousePos.x, mousePos.y) };

        if (startCell != nullptr and destCell != nullptr)
        {
            auto cellPath{ m_Astar.FindPath(startCell, destCell) };
            auto floatPath{ m_Astar.ConvertToFloatPath(cellPath) };
            m_Agent.SetPath(floatPath);
        }
    }

    m_Grid.Update();
    m_Agent.Update();
}

void Scene::Draw() const
{
    ClearBackground(RAYWHITE);
    m_Grid.Draw();
    m_Agent.Draw();
}
