#pragma once

#include <string>
#include <tuple>

// std::tuple<std::string, bool> SplitString(const std::string& str, unsigned char c);
std::vector<std::string> SplitString(const std::string& str, const std::string& cutStr);