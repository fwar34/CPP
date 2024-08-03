#pragma once
#include <string>
#include <unordered_map>
#include <variant>

struct RequestLine
{
    std::string method;
    std::string url;
    std::string version;
};

struct HttpRequest
{
    RequestLine requestLine;
    std::unordered_map<std::string, std::variant<int, std::string>> headerValues;
    std::string content;
};