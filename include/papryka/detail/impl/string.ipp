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
 * @file        string.ipp
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @copyright   (c) <www.sequenceresearch.com>
 */
std::string &ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
std::string &rtrim(std::string &s)
{
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
std::string &trim(std::string &s)
{
        return ltrim(rtrim(s));
}

void tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters)
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);

    // Find first non-delimiter.
    std::string::size_type pos = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));

        // Skip delimiters.
        lastPos = str.find_first_not_of(delimiters, pos);

        // Find next non-delimiter.
        pos = str.find_first_of(delimiters, lastPos);
    }
}


void toupper(std::string& str)
{
    for (size_t i=0; i < str.length(); ++i)
        str[i] = std::toupper(str[i]);
}

void tolower(std::string& str)
{
    for (size_t i=0; i < str.length(); ++i)
        str[i] = std::toupper(str[i]);
}

size_t compare(const char* lhs, const char* rhs, size_t n)
{
    return (n) ? strncmp(lhs, rhs, n) : strcmp(lhs, rhs);
}

size_t compare_no_case(const char* lhs, const char* rhs, size_t n)
{
    return (n) ? strncasecmp(lhs, rhs, n) : strcasecmp(lhs, rhs);
}
