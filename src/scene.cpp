#include "grid.h"
#include "scene.h"

#include "raylib.h"

void Scene::Update()
{
    m_Grid.Update();
}

void Scene::Draw() const
{
    ClearBackground(RAYWHITE);
    m_Grid.Draw();
}
