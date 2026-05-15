#ifndef GRID_H
#define GRID_H

#include <cell.h>
#include <connection.h>
#include <constants.h>
#include <structs.h>
#include <matrix.h>

#include <raylib.h>

#include <vector>

namespace HP
{
    class Grid final
    {
    private:
        static int constexpr REGION_SIZE{ 4 };
        static float constexpr DRAW_MARGIN{ 15.f };

        vec2<int> m_Dimensions{};
        int m_CellWidth{};
        int m_CellHeight{};
        Matrix<Cell> m_Cells{ 0, 0 };
        std::vector<Matrix<Cell*>> m_CellRegions{};
        std::vector<Connection> m_Connections{};
        vec2<int> m_Position{};

        void CreateNewConnection(int cellAId, int cellBId);
        vec2<int> ConvertWorldToCellIndex(float worldX, float worldY) const;
        void SubdivideCellsIntoRegions();

    public:
        Cell* GetCell(int cellId);
        Cell* GetCell(float worldX, float worldY);

        vec2<int> GetCellCenter(Cell const& cell) const;
        vec2<int> GetCellCenter(int cellId) const;
        std::vector<Connection*> FindConnectionsFromCell(int cellId);

        void MouseClicked();
        void Draw() const;

        Grid() = default;
        Grid(int rows, int cols, int posX = 0, int posY = 0, int width = Consts::SCREEN_WIDTH, int height = Consts::SCREEN_HEIGHT);
        void GenerationConnections();
    };
}

#endif
