#ifndef MY_MATRIX_H
#define MY_MATRIX_H

#include <vector>
#include <stdexcept>

namespace HP
{
    template<typename T>
    class Matrix final
    {
    private:
        int m_Rows{};
        int m_Cols{};
        std::vector<T> m_Data{};

    public:
        Matrix(int rows, int cols)
            : m_Rows{ rows }
            , m_Cols{ cols }
            , m_Data(rows* cols)
        {
        }

        void Resize(int newRows, int newCols)
        {
            m_Rows = newRows;
            m_Cols = newCols;
            m_Data.resize(newRows * newCols);
        }

        T GetCell(int row, int col) const
        {
            if (row < m_Rows and col < m_Cols)
            {
                return m_Data[(row * m_Cols) + col];
            }
            return T{};
        }

        T GetCell(int index) const
        {
            if (index < m_Rows * m_Cols)
            {
                return m_Data[index];
            }
            return T{};
        }

        T& GetCell(int row, int col)
        {
            if (row < m_Rows and col < m_Cols)
            {
                return m_Data[(row * m_Cols) + col];
            }
            throw std::out_of_range("Cell index out of range");
        }

        T& GetCell(int index)
        {
            if (index < m_Rows * m_Cols)
            {
                return m_Data[index];
            }
            throw std::out_of_range("Cell index out of range");
        }

        T* GetCellPtr(int row, int col)
        {
            if (row < m_Rows and col < m_Cols)
            {
                return &m_Data[(row * m_Cols) + col];
            }
            return nullptr;
        }

        T* GetCellPtr(int index)
        {
            if (index < m_Rows * m_Cols)
            {
                return &m_Data[index];
            }
            return nullptr;
        }

        void SetCell(int row, int col, T const& value)
        {
            if (row < m_Rows and col < m_Cols)
            {
                m_Data[(row * m_Cols) + col] = value;
            }
        }

        void SetCell(int index, T const& value)
        {
            if (index < m_Rows * m_Cols)
            {
                m_Data[index] = value;
            }
        }

        template<typename Func>
        T* FindCell(Func predicateFunc)
        {
            for (auto& currentCell : m_Data)
            {
                if (predicateFunc(currentCell))
                {
                    return &currentCell;
                }
            }
            return nullptr;
        }

        std::vector<T*> GetRow(int row)
        {
            if (row < m_Rows)
            {
                std::vector<T*> result{};
                for (int col{ 0 }; col < m_Cols; ++col)
                {
                    result.push_back(GetCellPtr(row, col));
                }
                return result;
            }
            return {};
        }

        std::vector<T*> GetCol(int col)
        {
            if (col < m_Cols)
            {
                std::vector<T*> result{};
                for (int row{ 0 }; row < m_Rows; ++row)
                {
                    result.push_back(GetCellPtr(row, col));
                }
                return result;
            }
            return {};
        }

        std::vector<T*> GetBorderCells()
        {
            std::vector<T*> result{};

            auto topRow{ GetRow(0) };
            auto bottomRow{ GetRow(m_Rows - 1) };
            auto leftCol{ GetCol(0) };
            auto rightCol{ GetCol(m_Cols - 1) };

            result.insert(result.end(), topRow.begin(), topRow.end());
            result.insert(result.end(), bottomRow.begin(), bottomRow.end());
            result.insert(result.end(), ++leftCol.begin(), --leftCol.end());
            result.insert(result.end(), ++rightCol.begin(), --rightCol.end());

            return result;
        }

        int GetRowSize() const
        {
            return m_Rows;
        }

        int GetColSize() const
        {
            return m_Cols;
        }

        int GetSize() const
        {
            return m_Rows * m_Cols;
        }

        std::vector<T>& GetData()
        {
            return m_Data;
        }

        std::vector<T> const& GetData() const
        {
            return m_Data;
        }

        Matrix<T*> GetSubMatrix(int startRow, int startCol, int numRows, int numCols)
        {
            if (startRow < m_Rows and startCol < m_Cols)
            {
                if (startRow + numRows > m_Rows)
                {
                    numRows = m_Rows - startRow;
                }

                if (startCol + numCols > m_Cols)
                {
                    numCols = m_Cols - startCol;
                }

                Matrix<T*> result(numRows, numCols);
                for (int row{ startRow }; row < startRow + numRows; ++row)
                {
                    for (int col{ startCol }; col < startCol + numCols; ++col)
                    {
                        result.SetCell(row - startRow, col - startCol, GetCellPtr(row, col));
                    }
                }
                return result;
            }
            return Matrix<T*>{0, 0};
        }
    };
}

#endif