#include <grid.h>
#include <scene.h>

#include <raylib.h>

void Scene::Update()
{
    m_Grid.Update();
    m_Agent.Update();
}

void Scene::Draw() const
{
    ClearBackground(RAYWHITE);
    m_Grid.Draw();
    m_Agent.Draw();
}
