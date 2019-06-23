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
 * @file        timeseries.cpp
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @copyright   (c) <www.sequenceresearch.com>
 */
#include <gtest/gtest.h>
#include <papryka/papryka.h>
#include <papryka/feed/timeseries.h>
#include <papryka/feed/matrix.h>
//#include <papryka/feed/bartimeseries.h>

using namespace papryka;

TEST(Feed, timeseries)
{
    typedef Timeseries<real_t> timeseries_t;
    timeseries_t::row_t row;
    timeseries_t ts(Frequency::Day);
    row = timeseries_t::row_t(Clock::now(), real_t(1.0));
    ts.push_back(row);

    row = timeseries_t::row_t(Clock::now(), real_t(3.0));
    ts.push_back(row);
    const timeseries_t::row_t& row2 = ts[1];
    EXPECT_EQ(ts.size(), 2);
    EXPECT_EQ(ts.column_size(), 1);
    EXPECT_EQ(std::get<1>(row2), real_t(3.0));
}

TEST(Feed, bartimeseries)
{
	typedef std::tuple<real_t, real_t, real_t, real_t, real_t> bar_t;
    typedef Timeseries<bar_t> timeseries_t;
	timeseries_t ts(Frequency::Day);
	timeseries_t::row_t row = timeseries_t::row_t(Clock::now(), bar_t(1,1,1,1,1));
	ts.push_back(row);
	EXPECT_EQ(ts.size(), 1);
	EXPECT_EQ(ts.column_size(), 5);

	row = timeseries_t::row_t(Clock::now(), bar_t(2,1,1,1,1));
	ts.push_back(row);
	EXPECT_EQ(ts.size(), 2);
	EXPECT_EQ(ts.column_size(), 5);
}

TEST(Feed, matrix_real)
{
	Matrix<real_t> mat(5,1);
	mat.data[0][0] =  1.0;
	mat.data[1][0] =  1.0;
	mat.data[2][0] =  1.0;
	mat.data[3][0] =  1.0;

	typedef Timeseries<real_t> timeseries_t;
	timeseries_t::row_t row;
	timeseries_t ts(Frequency::Day);

	row = timeseries_t::row_t(Clock::now(), real_t(1.0));
	ts.push_back(row);
	EXPECT_EQ(ts.size(), 1);

	row = timeseries_t::row_t(Clock::now(), real_t(3.0));
	ts.push_back(row);
	EXPECT_EQ(ts.size(), 2);


//	Matrix<real_t> mat2 = ts_to_mat(ts);
//	EXPECT_EQ(mat2.data[0][0], 1.0);
//	EXPECT_EQ(mat2.data[1][0], 3.0);
	Matrix<real_t> mat2(ts.size(), ts.column_size());
	ts_to_mat(ts, mat2);
	EXPECT_EQ(mat2.data[0][0], 1.0);
	EXPECT_EQ(mat2.data[1][0], 3.0);
}

TEST(Feed, matrix_tuple)
{
	typedef std::tuple<real_t, real_t, real_t, real_t, real_t> bar_t;
	typedef Timeseries<bar_t> timeseries_t;
	timeseries_t ts(Frequency::Day);
	timeseries_t::row_t row1 = timeseries_t::row_t(Clock::now(), bar_t(1,2,3,4,5));
	ts.push_back(row1);
	EXPECT_EQ(ts.size(), 1);
	timeseries_t::row_t row2 = timeseries_t::row_t(Clock::now(), bar_t(2,3,4,5,6));
	ts.push_back(row2);
	EXPECT_EQ(ts.size(), 2);

	Matrix<real_t> mat(ts.size(), ts.column_size());
	ts_to_mat(ts, mat);
	EXPECT_EQ(mat.data[0][0], 1.0);
	EXPECT_EQ(mat.data[0][1], 2.0);
	EXPECT_EQ(mat.data[0][2], 3.0);
	EXPECT_EQ(mat.data[0][3], 4.0);
	EXPECT_EQ(mat.data[0][4], 5.0);

	EXPECT_EQ(mat.data[1][0], 2.0);
	EXPECT_EQ(mat.data[1][1], 3.0);
	EXPECT_EQ(mat.data[1][2], 4.0);
	EXPECT_EQ(mat.data[1][3], 5.0);
	EXPECT_EQ(mat.data[1][4], 6.0);

}

TEST(Feed, matrix_bartimeseries)
{
}



