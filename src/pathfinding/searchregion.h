#ifndef SEARCH_REGION_H
#define SEARCH_REGION_H

#include <grid.h>
#include <cell.h>

#include <raylib.h>

class SearchSpace final
{
private:
    Grid* m_pGrid{};
    MyMatrix<Cell*> m_Blocks{};
    Rectangle m_BoundingBox{};

};

#endif
