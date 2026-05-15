#ifndef SEARCH_REGION_H
#define SEARCH_REGION_H

#include <grid.h>
#include <cell.h>

#include <raylib.h>

namespace HP
{
    class SearchSpace final
    {
    private:
        Grid* m_pGrid{};
        Matrix<Cell*> m_Blocks{};
        Rectangle m_BoundingBox{};
    };
}

#endif