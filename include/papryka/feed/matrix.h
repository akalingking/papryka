/**
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * @file        matrix.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        June 12, 2017 9:47 AM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com> 
 */
#pragma once
#include "timeseries.h"
#include "../detail/types.h"
#include <cstring>
#include <iostream>
#include <cassert>
#include <tuple>

namespace papryka {
    
template <typename _T>
_T** create_matrix(size_t row, size_t col) {
    _T** t = new _T*[row];
    for (size_t i = 0; i<row; ++i) {
        t[i] = new _T[col];
        memset(t[i], 0, col*sizeof(_T));
    }
    return t;
}

template <typename _T>
void destroy_matrix(_T**& t, size_t row, size_t col)
{
    for (size_t i=0; i< row;++i)
    {
        delete [] t[i];
        t[i] = nullptr;
    }
    delete [] t;
    t = nullptr;
}

template <typename _T>
void copy_matrix(_T**& lhs, const _T** rhs, size_t rows, size_t cols)
{
    for (size_t i=0; i< rows; ++i)
    	for (size_t j=0; i< cols; ++j)
    		lhs[i][j] = rhs[i][j];
}

template <typename _T>
void print(_T** x, unsigned int row, unsigned int col)
{
    std::cout << std::endl;
    for (unsigned int i=0; i<row; ++i) 
    {
        for (unsigned int j=0; j<col; ++j)
            std::cout << x[i][j] << " ";
        std::cout << std::endl;
    }
    std::cout.flush();
}

template <typename _T>
void print(_T* x, unsigned int row, unsigned int col) 
{
    for (unsigned int i=0; i<row; ++i) 
    {
        for (unsigned int j=0; j<col;++j)
            std::cout << (_T)*((x + i*col) + j) << " ";
        std::cout << std::endl;
    }
}

template <typename _T=real_t>
struct Matrix
{
    size_t rows;
    size_t cols;
    _T** data;
    Matrix(size_t rows, size_t cols);
    ~Matrix();
    const Matrix<_T>& operator=(const Matrix<_T>& rhs) {
    	if (data)
    		destroy_matrix(data, rows, cols);
    	rows = 0; cols = 0;
    	rows = rhs.rows, cols = rhs.cols;
    	data = create_matrix<_T>(rows, cols);
    	copy_matrix<_T>(this->data, rhs.data);
    	return *this;
    }
};

template <typename _T>
Matrix<_T>::Matrix(size_t rows, size_t cols) : rows(rows), cols(cols), data(nullptr) {
    data = create_matrix<_T>(rows, cols);
    assert(data != nullptr);
}

template <typename _T>
Matrix<_T>::~Matrix() {
    destroy_matrix<_T>(data, rows, cols);
    assert (data == nullptr);
}

template<typename _T, class Tuple, std::size_t... Is>
void tuple_2_ptr_(_T* ptr, const Tuple& t, std::index_sequence<Is...>) {
    using sink = int[];
    (void)sink {0, (void(ptr[Is] = std::get<Is>(t), int{}))...};

    //(void)swallow {0, (void(printf("%d, ", std::get<Is>(t)), int{}))...};
}

template <typename _T, class ...Args>
void tuple_2_ptr(_T* ptr, std::tuple<Args...> t) {
    tuple_2_ptr_(ptr, t, std::index_sequence_for<Args...>{});
}

template <typename _T=real_t, typename _K = std::enable_if_t<std::is_arithmetic<_T>::value, _T> >
Matrix<_T> ts_to_mat(const Timeseries<_T> &ts) {
    Matrix<_T> mat(ts.size(), ts.column_size());
    for (size_t i=0; i<ts.size(); ++i)
    {
    	const typename Timeseries<_T>::row_t& row = ts[i];
        mat.data[i][0] = std::get<1>(row);
    }
    return mat;
}

template <typename _T=real_t, typename _K = std::enable_if_t<!std::is_arithmetic<_T>::value, _T> >
Matrix<_T> ts_to_mat(const Timeseries<_T>& ts, _K* k=0) {
    Matrix<_T> mat(ts.size(), ts.column_size());
    for (size_t i=0; i<ts.size(); ++i)
    {
    	const typename Timeseries<_T>::row_t& row = ts[i];
        tuple_2_ptr(mat.data[i], std::get<1>(row));
    }
    return mat;
}

// implement other _T overloads

} // namespace

