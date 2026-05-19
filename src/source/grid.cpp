#include <cell.h>
#include <grid.h>
#include <structs.h>

#include <raylib.h>
#include <raygui.h>

#include <algorithm>
#include <cmath>
#include <connection.h>
#include <utility>
#include <vector>
#include <set>
#include <string>

using namespace HP;

Cell* HP::Grid::GetCell(int cellId)
{
    return m_Cells.GetCellPtr(cellId);
}

Cell* Grid::GetCell(float worldX, float worldY)
{
    auto [colIndex, rowIndex] { ConvertWorldToCellIndex(worldX, worldY) };

    return m_Cells.GetCellPtr(rowIndex, colIndex);
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

std::vector<Cell*> HP::Grid::FindConnectedCells(int cellId)
{
    std::vector<Cell*> result{};

    auto connections{ FindConnectionsFromCell(cellId) };

    for (auto& connection : connections)
    {
        result.push_back(m_Cells.GetCellPtr(connection->GetToCell()));
    }

    return result;
}

void Grid::CreateNewConnection(int cellAId, int cellBId)
{
    auto cellA{ m_Cells.GetCellPtr(cellAId) };
    auto cellB{ m_Cells.GetCellPtr(cellBId) };

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

void HierarchicalGrid::SubdivideCellsIntoRegions()
{
    m_CellRegions.clear();

    float numRegionsX{ std::ceil(static_cast<float>(m_Cells.GetColSize()) / REGION_SIZE) };
    float numRegionsY{ std::ceil(static_cast<float>(m_Cells.GetRowSize()) / REGION_SIZE) };

    m_RegionGrid = Grid{ static_cast<int>(numRegionsY), static_cast<int>(numRegionsX)};

    int regionId{ 0 };
    for (int regionY{ 0 }; regionY < numRegionsY; ++regionY)
    {
        for (int regionX{ 0 }; regionX < numRegionsX; ++regionX)
        {
            auto cellMatrix{ m_Cells.GetSubMatrix(regionY * REGION_SIZE, regionX * REGION_SIZE, REGION_SIZE, REGION_SIZE) };
            auto matrixData{ cellMatrix.GetData() };

            for (auto& cell : matrixData)
            {
                cell->SetRegionId(regionId);
            }

            m_CellRegions.insert({ regionId, std::move(cellMatrix)});

            ++regionId;
        }
    }
}

void HP::HierarchicalGrid::BuildAbstractGraph()
{
    // Loop through each region
    // 
    // Start out with empty vectors for each region (Use a map)
    // 
    // Get the border cells and their connected cells (Do 4 checks -> Up, Down, Left, Right)
    // 
    // Compare the cells to each other -> Find the maximum border
    //

    for (auto& region : m_CellRegions)
    {
        int regionId{ region.first };
        auto connectedRegions{ m_RegionGrid.FindConnectedCells(regionId) };

        auto regionExtConn{ GetExternalConnectionFromRegion(regionId) };

        for (auto connRegion : connectedRegions)
        {
            int neighborRegionId{ connRegion->GetId() };

            auto neighborExtConn{ GetExternalConnectionFromRegion(neighborRegionId) };

            auto commonConnections{ FindCommonConnections(regionExtConn, neighborExtConn) };

            auto commonCells{ GetCellsFromConnections(commonConnections) };

            for (auto conn : commonConnections)
            {
                auto iter{ std::ranges::find(m_AbstractConnections, *conn) };

                if (iter != m_AbstractConnections.end()) continue;

                m_AbstractConnections.push_back(Connection{ conn->GetFromCell(), conn->GetToCell() });
            }

            for (auto cell : commonCells)
            {
                if (cell->GetRegionId() == regionId)
                {
                    m_AbstractGraph.push_back(cell);
                }
            }
        }
    }
}

std::set<Cell*> HP::HierarchicalGrid::GetCellsOfRegion(int regionId)
{
    std::set<Cell*> result{};

    for (auto& cell : m_Cells.GetData())
    {
        if (cell.GetRegionId() == regionId)
        {
            result.insert(&cell);
        }
    }

    return result;
}

std::vector<Connection*> HP::Grid::GetConnectionFromCells(std::set<Cell*> const& cells)
{
    std::vector<Connection*> result{};

    for (auto cell : cells)
    {
        auto connections{ FindConnectionsFromCell(cell->GetId()) };
        result.insert(result.end(), connections.begin(), connections.end());
    }

    return result;
}

std::set<Connection*> HP::HierarchicalGrid::GetExternalConnectionFromRegion(int regionId)
{
    std::set<Connection*> result{};

    auto iter{ m_CellRegions.find(regionId) };
    
    if (iter != m_CellRegions.end())
    {
        auto cells{ GetCellsOfRegion(regionId) };
        auto connections{ GetConnectionFromCells(cells) };

        for (auto connection : connections)
        {
            auto toCell{ GetCell(connection->GetToCell()) };

            if (toCell == nullptr) continue;

            auto toCellIter{ std::ranges::find(cells, toCell) };

            if (toCellIter != cells.end()) continue;

            result.insert(connection);
        }
    }

    return result;
}

std::vector<Connection*> HP::Grid::FindCommonConnections(std::set<Connection*> connectionsA, std::set<Connection*> connectionsB)
{
    std::vector<Connection*> result{};

    for (auto conn : connectionsA)
    {
        for (auto otherConn : connectionsB)
        {
            if ((conn->GetToCell() == otherConn->GetToCell() and conn->GetFromCell() == otherConn->GetFromCell())
                or
                (conn->GetFromCell() == otherConn->GetToCell() and conn->GetToCell() == otherConn->GetFromCell())
                )
            {
                //result.push_back(GetCell(conn->GetFromCell
                //result.push_back(GetCell(conn->GetToCell()));

                result.push_back(conn);
                result.push_back(otherConn);
            }
        }
    }

    return result;
}

std::vector<Cell*> HP::Grid::GetCellsFromConnections(std::vector<Connection*> connections)
{
    std::vector<Cell*> result{};
    std::set<int> cellIds{};

    for (auto connection : connections)
    {
        cellIds.insert(connection->GetFromCell());
        cellIds.insert(connection->GetToCell());
    }

    result.reserve(cellIds.size());

    for (int cellId : cellIds)
    {
        result.push_back(m_Cells.GetCellPtr(cellId));
    }

    return result;
}

void HP::HierarchicalGrid::Draw() const
{
    Grid::Draw();

    //for (auto const& region : m_CellRegions)
    //{
    //    auto topLeftCell{ region.second.GetCell(0, 0) };
    //    auto bottomRightCell{ region.second.GetCell(region.second.GetRowSize() - 1, region.second.GetColSize() - 1) };

    //    if (topLeftCell == nullptr or bottomRightCell == nullptr)
    //    {
    //        continue;
    //    }

    //    auto topLeftPos{ GetCellCenter(topLeftCell->GetId()) };
    //    auto bottomRightPos{ GetCellCenter(bottomRightCell->GetId()) };

    //    Rectangle targetRect{
    //        static_cast<float>(topLeftPos.x - m_CellWidth / 2.f) + DRAW_MARGIN,
    //        static_cast<float>(topLeftPos.y - m_CellHeight / 2.f) + DRAW_MARGIN,
    //        static_cast<float>((bottomRightPos.x - topLeftPos.x) + m_CellWidth) - (DRAW_MARGIN * 2.f),
    //        static_cast<float>((bottomRightPos.y - topLeftPos.y) + m_CellHeight) - (DRAW_MARGIN * 2.f),
    //    };

    //    DrawRectangleLinesEx(targetRect, 2.f, ORANGE);
    //}

    for (auto const& connection : m_AbstractConnections)
    {
        auto [cellAId, cellBId] { connection.GetConnectedCells() };
        auto cellACenter{ GetCellCenter(cellAId) };
        auto cellBCenter{ GetCellCenter(cellBId) };
        DrawLine(
            cellACenter.x,
            cellACenter.y,
            cellBCenter.x,
            cellBCenter.y,
            ORANGE
        );
    }
}

HP::HierarchicalGrid::HierarchicalGrid(int rows, int cols, int posX, int posY, int width, int height)
    : Grid{ rows, cols, posX, posY, width, height }
{
    SubdivideCellsIntoRegions();
    BuildAbstractGraph();
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

        Rectangle targetRect{
            static_cast<float>(m_Position.x + (cellCol * m_CellWidth)),
            static_cast<float>(m_Position.y + (cellRow * m_CellHeight)),
            static_cast<float>(m_CellWidth),
            static_cast<float>(m_CellHeight),
        };

        GuiLabel(targetRect, std::to_string(index).c_str());
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
    : m_Dimensions{width, height}
    , m_CellWidth{ m_Dimensions.x / cols }
    , m_CellHeight{ m_Dimensions.y / rows }
    , m_Cells{ rows, cols }
    , m_Position{posX, posY}
{
    for (int index{ 0 }; index < m_Cells.GetSize(); ++index)
    {
        m_Cells.SetCell(index, Cell{index, CellType::Empty});
    }

    GenerationConnections();
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
