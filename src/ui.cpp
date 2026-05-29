#include "constants.h"
#include "pathfinding/astar.h"
#include "ui.h"

#include <raygui.h>
#include <raylib.h>

#include "pathfinding/heuristics.h"
#include <format>
#include <string>

void HP::UI::UpdatePath(AStar::PathResult pathResult)
{
    if (pathResult.pathFound)
    {
        m_StringMap[UIElement::ValidPath] = "Valid path: True";
    }
    else
    {
        m_StringMap[UIElement::ValidPath] = "Valid path: False";
    }

    m_StringMap[UIElement::NodeLength] = "Node length: " + std::to_string(pathResult.pathNodeLength);
    m_StringMap[UIElement::RegionsTraversed] = "Regions traversed: " + std::to_string(pathResult.regionsTraversed);
    m_StringMap[UIElement::PathCost] = "Path cost: " + std::format("{:.0f}", pathResult.totalCost);
}

void HP::UI::Draw()
{
    DrawRectangle(
        POSITION.x,
        POSITION.y,
        Consts::SCREEN_WIDTH,
        Consts::UI_HEIGHT,
        DARKGRAY
    );

    DrawText(
        m_StringMap.find(UIElement::ValidPath)->second.c_str(),
        POSITION.x + PADDING,
        POSITION.y + PADDING,
        25,
        ORANGE
    );

    DrawText(
        m_StringMap.find(UIElement::NodeLength)->second.c_str(),
        POSITION.x + PADDING,
        POSITION.y + (PADDING * 2) +FONT_SIZE,
        25,
        ORANGE
    );

    DrawText(
        m_StringMap.find(UIElement::RegionsTraversed)->second.c_str(),
        POSITION.x + PADDING + COLUMN_SPACING,
        POSITION.y + PADDING,
        25,
        ORANGE
    );

    DrawText(
        m_StringMap.find(UIElement::PathCost)->second.c_str(),
        POSITION.x + PADDING + COLUMN_SPACING,
        POSITION.y + (PADDING * 2) + FONT_SIZE,
        25,
        ORANGE
    );

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        if (CheckCollisionPointRec(GetMousePosition(), DROPDOWN_BOUNDS))
        {
            m_DropdownOpen = !m_DropdownOpen;
        }
    }

    int selectedItem{ GuiDropdownBox(DROPDOWN_BOUNDS, m_DropdownItems.c_str(), &m_ActiveDropdownItem, m_DropdownOpen) };

    if (selectedItem == 1)
    {
        m_DropdownOpen = false;

        auto newHeuristics{ m_Heuristics[m_ActiveDropdownItem] };

        // There is definitely a better way to do this, but it works for now.
        if (newHeuristics == "Manhattan")
        {
            AStar::SetHeuristic<Manhattan>();
        }
        else if (newHeuristics == "Euclidean")
        {
            AStar::SetHeuristic<Euclidean>();
        }
        else if (newHeuristics == "SqEuclidean")
        {
            AStar::SetHeuristic<SqEuclidean>();
        }
        else if (newHeuristics == "Octile")
        {
            AStar::SetHeuristic<Octile>();
        }
        else if (newHeuristics == "Chebyshev")
        {
            AStar::SetHeuristic<Chebyshev>();
        }
    }
}
