#ifndef UI_H
#define UI_H

#include <constants.h>
#include <pathfinding/astar.h>
#include <pathfinding/heuristics.h>
#include <structs.h>

#include <raylib.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace HP
{
    enum class UIElement
    {
        ValidPath,
        NodeLength,
        RegionsTraversed,
        PathCost
    };

    class UI final
    {
    private:
        static int constexpr PADDING{ 15 };
        static int constexpr FONT_SIZE{ 25 };
        static int constexpr COLUMN_SPACING{ 300 };
        static inline vec2<int> const POSITION{ 0, Consts::SCREEN_HEIGHT - Consts::UI_HEIGHT };
        static inline Rectangle const DROPDOWN_BOUNDS{
            static_cast<float>(POSITION.x + (PADDING * 2) + (COLUMN_SPACING * 2)),
            static_cast<float>(POSITION.y + PADDING),
            static_cast<float>(COLUMN_SPACING),
            static_cast<float>(FONT_SIZE)
        };

        std::unordered_map<UIElement, std::string> m_StringMap{
            {UIElement::ValidPath, "Valid path: False"},
            {UIElement::NodeLength, "Node length: 0"},
            {UIElement::RegionsTraversed, "Regions traversed: 0"},
            {UIElement::PathCost, "Path cost: 0"}
        };

        std::vector<std::string> m_Heuristics{
            Manhattan::Name,
            Euclidean::Name,
            SqEuclidean::Name,
            Octile::Name,
            Chebyshev::Name
        };

        std::string m_DropdownItems{
            m_Heuristics[0] + ";" + m_Heuristics[1] + ";" + m_Heuristics[2] + ";" + m_Heuristics[3] + ";" + m_Heuristics[4]
        };

        int m_ActiveDropdownItem{ 0 };
        bool m_DropdownOpen{ false };


    public:
        static UI& Instance()
        {
            static UI instance{};
            return instance;
        }

        void UpdatePath(AStar::PathResult pathResult);
        void Draw();
        void DrawHeuristicDropdown();
        void DrawStats();
    };
}

#endif