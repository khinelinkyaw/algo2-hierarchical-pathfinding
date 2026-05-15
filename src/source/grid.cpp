#include <cell.h>
#include <grid.h>
#include <structs.h>

#include <raylib.h>

#include <cmath>
#include <connection.h>
#include <vector>

using namespace HP;

Cell* Grid::GetCell(int cellId)
{
    return m_Cells.FindCell([cellId](Cell const& cell)
    {
        return cell.GetId() == cellId;
    });
}

Cell* Grid::GetCell(int rowIndex, int colIndex)
{
    int cellIndex{ (rowIndex * m_Cells.GetColSize()) + colIndex };

    if (cellIndex < m_Cells.GetSize())
    {
        return &m_Cells.GetCellRef(cellIndex);
    }
    return nullptr;
}

Cell* Grid::GetCell(float worldX, float worldY)
{
    auto [colIndex, rowIndex] { ConvertWorldToCellIndex(worldX, worldY) };

    return GetCell(rowIndex, colIndex);
}


vec2<int> Grid::GetCellCenter(Cell const& cell) const
{
    int cellId{ cell.GetId() };

    return GetCellCenter(cellId);
}

vec2<int> Grid::GetCellCenter(int cellId) const
{
    int cellCol{ cellId % m_Cells.GetColSize() };
    int cellRow{ static_cast<int>(cellId / m_Cells.GetColSize()) };

    return vec2<int>(
        static_cast<int>(m_Position.x + ((cellCol * m_CellWidth) + m_CellWidth / 2.f)),
        static_cast<int>(m_Position.y + ((cellRow * m_CellHeight) + m_CellHeight / 2.f))
    );
}

std::vector<Connection*> Grid::FindConnectionsFromCell(int cellId)
{
    std::vector<Connection*> result{};

    for (auto& connection : m_Connections)
    {
        if (connection.GetFromCell() == cellId)
        {
            result.push_back(&connection);
        }
    }
    return result;
}

void Grid::CreateNewConnection(int cellAId, int cellBId)
{
    auto cellA{ GetCell(cellAId) };
    auto cellB{ GetCell(cellBId) };

    if (cellA == nullptr or cellB == nullptr)
    {
        return;
    }

    if (cellA->GetCellType() == CellType::Obstacle or cellB->GetCellType() == CellType::Obstacle)
    {
        return;
    }

    m_Connections.emplace_back(cellAId, cellBId);
}

vec2<int> Grid::ConvertWorldToCellIndex(float worldX, float worldY) const
{
    return vec2<int>(
        static_cast<int>(floor((worldX - m_Position.x) / m_CellWidth)),
        static_cast<int>(floor((worldY - m_Position.y) / m_CellHeight))
    );
}

void Grid::SubdivideCellsIntoRegions()
{
    m_CellRegions.clear();

    float numRegionsX{ std::ceil(static_cast<float>(m_Cells.GetColSize()) / REGION_SIZE) };
    float numRegionsY{ std::ceil(static_cast<float>(m_Cells.GetRowSize()) / REGION_SIZE) };

    for (int regionY{ 0 }; regionY < numRegionsY; ++regionY)
    {
        for (int regionX{ 0 }; regionX < numRegionsX; ++regionX)
        {
            m_CellRegions.push_back(m_Cells.GetSubMatrix(regionX * REGION_SIZE, regionY * REGION_SIZE, REGION_SIZE, REGION_SIZE));
        }
    }
}

void Grid::MouseClicked()
{
    auto mousePos{ GetMousePosition() };

    Rectangle gridRect{
        static_cast<float>(m_Position.x),
        static_cast<float>(m_Position.y),
        static_cast<float>(m_Dimensions.x),
        static_cast<float>(m_Dimensions.y)
    };

    if (CheckCollisionPointRec(mousePos, gridRect))
    {
        auto [mouseGridX, mouseGridY] { ConvertWorldToCellIndex(mousePos.x, mousePos.y) };

        int cellIndex{ (mouseGridY * m_Cells.GetColSize()) + mouseGridX };
        CellType currentCellType{ m_Cells.GetCell(cellIndex).GetCellType()};

        switch (currentCellType)
        {
        case CellType::Empty:
            m_Cells.GetCell(cellIndex).SetCellType(CellType::Obstacle);
            break;
        case CellType::Obstacle:
            m_Cells.GetCell(cellIndex).SetCellType(CellType::Empty);
            break;
        }

        GenerationConnections();
    }
}

void Grid::Draw() const
{
    for (int index{ 0 }; index < static_cast<int>(m_Cells.GetSize()); ++index)
    {
        int cellRow{ static_cast<int>(index / m_Cells.GetColSize()) };
        int cellCol{ index % m_Cells.GetColSize() };

        if (m_Cells.GetCell(index).GetCellType() == CellType::Obstacle)
        {
            DrawRectangle(
                m_Position.x + (cellCol * m_CellWidth),
                m_Position.y + (cellRow * m_CellHeight),
                m_CellWidth,
                m_CellHeight,
                { 190, 33, 55, 100 }
            );
        }

        //Rectangle targetRect{
        //    static_cast<float>(m_Position.x + (cellCol * m_CellWidth)),
        //    static_cast<float>(m_Position.y + (cellRow * m_CellHeight)),
        //    static_cast<float>(m_CellWidth),
        //    static_cast<float>(m_CellHeight),
        //};

        //GuiLabel(targetRect, std::to_string(index).c_str());
    }

    for (int index{ 0 }; index < m_Cells.GetRowSize(); ++index)
    {
        vec2 startPos{m_Position.x, m_Position.y + (index * m_CellHeight)};
        vec2 endPos{m_Position.x + m_Dimensions.x, m_Position.y + (index * m_CellHeight)};
        DrawLine(
            startPos.x,
            startPos.y,
            endPos.x,
            endPos.y,
            {200,200,200,100}
        );
    }

    for (int index{ 0 }; index < m_Cells.GetColSize(); ++index)
    {
        vec2 startPos{ m_Position.x + (index * m_CellWidth), m_Position.y };
        vec2 endPos{ m_Position.x + (index * m_CellWidth), m_Position.y + m_Dimensions.y };
        DrawLine(
            startPos.x,
            startPos.y,
            endPos.x,
            endPos.y,
            { 200,200,200,100 }
        );
    }

    for (auto const& region : m_CellRegions)
    {
        auto topLeftCell{ region.GetCell(0, 0) };
        auto bottomRightCell{ region.GetCell(region.GetRowSize() - 1, region.GetColSize() - 1) };

        if (topLeftCell == nullptr or bottomRightCell == nullptr)
        {
            continue;
        }

        auto topLeftPos{ GetCellCenter(topLeftCell->GetId()) };
        auto bottomRightPos{ GetCellCenter(bottomRightCell->GetId()) };

        Rectangle targetRect{
            static_cast<float>(topLeftPos.x - m_CellWidth / 2.f) + DRAW_MARGIN,
            static_cast<float>(topLeftPos.y - m_CellHeight / 2.f) + DRAW_MARGIN,
            static_cast<float>((bottomRightPos.x - topLeftPos.x) + m_CellWidth) - (DRAW_MARGIN * 2.f),
            static_cast<float>((bottomRightPos.y - topLeftPos.y) + m_CellHeight) - (DRAW_MARGIN * 2.f),
        };

        DrawRectangleLinesEx(targetRect, 2.f, ORANGE);
    }

    //for (auto const& cell : m_Cells)
    //{
    //    auto cellCenter{ GetCellCenter(cell) };

    //    DrawCircle(
    //        cellCenter.x,
    //        cellCenter.y,
    //        5.f,
    //        ORANGE
    //    );
    //}

    //for (auto const& connection : m_Connections)
    //{
    //    auto [cellAId, cellBId] { connection.GetConnectedCells() };

    //    auto cellACenter{ GetCellCenter(cellAId) };
    //    auto cellBCenter{ GetCellCenter(cellBId) };

    //    DrawLine(
    //        cellACenter.x,
    //        cellACenter.y,
    //        cellBCenter.x,
    //        cellBCenter.y,
    //        ORANGE
    //    );
    //}
}
 
Grid::Grid(int rows, int cols, int posX, int posY, int width, int height)
    //: m_Rows{rows}
    //, m_Cols{cols}
    : m_Dimensions{width, height}
    , m_CellWidth{ m_Dimensions.x / cols }
    , m_CellHeight{ m_Dimensions.y / rows }
    , m_Cells{ rows, cols }
    , m_Position{posX, posY}
{
    //int totalCells{ rows * cols };
    //m_Cells.reserve(totalCells);

    //for (int index{ 0 }; index < totalCells; ++index)
    //{
    //    m_Cells.emplace_back(index, CellType::Empty);
    //}

    for (int index{ 0 }; index < m_Cells.GetSize(); ++index)
    {
        m_Cells.SetCell(index, Cell{index, CellType::Empty});
    }

    GenerationConnections();
    SubdivideCellsIntoRegions();
}

void Grid::GenerationConnections()
{
    m_Connections.clear();

    for (int index{ 0 }; index < m_Cells.GetSize(); ++index)
    {
        if (index - 1 >= 0 and index % m_Cells.GetColSize() != 0)
        {
            CreateNewConnection(index, index - 1);
        }
        if (index - m_Cells.GetColSize() >= 0)
        {
            CreateNewConnection(index, index - m_Cells.GetColSize());
        }
        if (index + 1 < m_Cells.GetSize() and (index + 1) % m_Cells.GetColSize() != 0)
        {
            CreateNewConnection(index, index + 1);
        }
        if (index + m_Cells.GetColSize() < m_Cells.GetSize())
        {
            CreateNewConnection(index, index + m_Cells.GetColSize());
        }
    }
}
