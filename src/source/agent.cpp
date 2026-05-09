#include <agent.h>
#include <raylib.h>
#include <structs.h>

#include <cstdlib>
#include <iterator>
#include <vector>
 
void Agent::SetPath(std::vector<vec2<float>> const& path)
{
    m_Path = path;
    m_Position = *m_Path.begin();
    m_CurrentDest = m_Path.begin();
}

vec2<float> Agent::GetPosition() const
{
    return m_Position;
}

void Agent::Update()
{
    if (m_Path.empty() or m_CurrentDest == m_Path.end())
    {
        return;
    }

    if (abs(m_CurrentDest->x - m_Position.x) > m_Margin or abs(m_CurrentDest->y - m_Position.y) > m_Margin)
    {
        vec2<float> m_Velocity{
            (m_CurrentDest->x - m_Position.x) * m_Speed * GetFrameTime(),
            (m_CurrentDest->y - m_Position.y) * m_Speed * GetFrameTime()
        };

        m_Position += m_Velocity;
    }
    else if (m_CurrentDest != m_Path.end())
    {
        ++m_CurrentDest;
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
