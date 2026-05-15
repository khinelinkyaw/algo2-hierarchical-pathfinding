#ifndef AGENT_H
#define AGENT_H

#include "structs.h"

#include <vector>

namespace HP
{
    class Agent final
    {
    private:
        static float constexpr m_Margin{ 1.f };
        static float constexpr m_Radius{ 15.f };
        vec2<float> m_Position{ 100.f, 100.f };
        std::vector<vec2<float>> m_Path{};
        std::vector<vec2<float>>::iterator m_CurrentDest{};
        float m_Speed{ 10.f };

    public:
        void SetPath(std::vector<vec2<float>> const&);
        vec2<float> GetPosition() const;

        void Update();
        void Draw() const;
    };
}

#endif