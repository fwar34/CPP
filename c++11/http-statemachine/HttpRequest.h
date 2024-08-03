#pragma once
#include <string>
#include <unordered_map>
#include <variant>
#include <sstream>

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
    std::string ToString() const
    {
        std::ostringstream oss;
        oss << "method: " << requestLine.method << ", url: " << requestLine.url << ", version: " << requestLine.version << "\n";
        for (auto item : headerValues) {
            oss << item.first << ":";
            if (std::holds_alternative<int>(item.second)) {
                oss << std::get<int>(item.second) << "\n";
            } else {
                oss << std::get<std::string>(item.second) << "\n";
            }
        }
        oss << "content[" << content << "]";

        return oss.str();
    }
};