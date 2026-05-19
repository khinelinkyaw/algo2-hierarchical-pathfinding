#include <grid.h>
#include <scene.h>

#include <memory>
#include <raylib.h>
#include <utility>

using namespace HP;

void Scene::SetGrid(Grid const& grid)
{
    m_Grid = std::make_unique<Grid>(grid);
    m_Astar.SetGrid(m_Grid.get());
}

void Scene::SetGrid(std::unique_ptr<Grid>&& grid)
{
    m_Grid = std::move(grid);
    m_Astar.SetGrid(m_Grid.get());
}

void Scene::Update()
{
    if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
    {
        auto mousePos{ GetMousePosition() };
        m_DestCell = m_Grid->GetCell(mousePos.x, mousePos.y);

        RefreshPathForAgent();
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        m_Grid->MouseClicked();
        RefreshPathForAgent();
    }

    m_Agent.Update();
}

void Scene::RefreshPathForAgent()
{
    auto [startCellPosX, startCellPosY] { m_Agent.GetPosition() };
    auto startCell{ m_Grid->GetCell(startCellPosX, startCellPosY) };

    if (startCell != nullptr and m_DestCell != nullptr)
    {
        auto cellPath{ m_Astar.FindPath(startCell, m_DestCell) };
        auto floatPath{ m_Astar.ConvertToFloatPath(cellPath) };
        m_Agent.SetPath(floatPath);
    }
}

void Scene::Draw() const
{
    ClearBackground(DARKBLUE);
    m_Grid->Draw();
    m_Agent.Draw();

    if (m_DestCell != nullptr)
    {
        auto cellCenter{ m_Grid->GetCellCenter(*m_DestCell) };

        DrawCircle(
            cellCenter.x,
            cellCenter.y,
            10.f,
            RED);
    }
}

HP::Scene::Scene()
{
    m_Astar.SetGrid(m_Grid.get());
}
