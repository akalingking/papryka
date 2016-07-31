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
 * @file        main.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        July 2, 2016 11:00 PM
 * @copyright   (c) 2016-2027 <www.sequenceresearch.com>
 */
#include <gtest/gtest.h>

int main(int argc, char* argv[])
{   
    ::testing::InitGoogleTest(&argc, argv);   
  	return RUN_ALL_TESTS();
}
