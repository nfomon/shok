// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Util_h_
#define _Util_h_

/* Miscellaneous utilities */

#include <string>
#include <utility>
#include <vector>

namespace Util {

// Extract the first space-separated word from a string.
// Returns the pair <first word, rest of s>
std::pair<std::string,std::string> break_word(std::string s);

// Split words of a string by whitespace
std::vector<std::string> split(const std::string& s, char delim = ' ');

// Trim whitespace from the left of s
std::string ltrim_space(const std::string& s);

// Trim whitespace from the right of s
std::string rtrim_space(const std::string& s);

// Pad a string with a character on the left until it is of a minimum size
std::string pad_str(const std::string& s, char c, size_t len);

}

#endif // _Util_h_
