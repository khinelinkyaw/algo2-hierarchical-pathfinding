#ifndef AGENT_H
#define AGENT_H

#include "structs.h"

class Agent final
{
private:
    static float constexpr m_Margin{ 1.f };
    static float constexpr m_Radius{ 15.f };
    vec2<float> m_Position{100.f, 100.f};
    vec2<float> m_Destination{};
    float m_Speed{ 1.f };

public:
    void Update();
    void Draw() const;
};

#endif