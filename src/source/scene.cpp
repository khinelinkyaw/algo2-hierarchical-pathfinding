#include <graph/grid.h>
#include <scene.h>
#include <pathfinding/astar.h>
#include <cell.h>
#include <ui.h>

#include <raylib.h>

#include <memory>
#include <vector>

using namespace HP;

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
        std::vector<Cell*> cellPath{ m_Grid->FindPath(startCell, m_DestCell) };
        auto floatPath{ AStar::ConvertToFloatPath(cellPath, m_Grid.get()) };

        if (!floatPath.empty()) m_Agent.SetPath(floatPath);
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

    UI::Instance().Draw();
}

HP::Scene::Scene()
{
}
