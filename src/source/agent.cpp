#include <agent.h>
#include <raylib.h>
#include <structs.h>

#include <cstdlib>
 
void Agent::Update()
{
    if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
    {
        auto mousePos{ GetMousePosition() };

        m_Destination.x = mousePos.x;
        m_Destination.y = mousePos.y;
    }

    if (abs(m_Destination.x - m_Position.x) > m_Margin and abs(m_Destination.y - m_Position.y) > m_Margin)
    {
        vec2<float> m_Velocity{
            (m_Destination.x - m_Position.x) * m_Speed * GetFrameTime(),
            (m_Destination.y - m_Position.y) * m_Speed * GetFrameTime()
        };

        m_Position += m_Velocity;
    }
}

void Agent::Draw() const
{
    DrawCircle(
        static_cast<int>(m_Position.x),
        static_cast<int>(m_Position.y),
        m_Radius,
        BLUE);
}
