#include <iostream>
#include <cstring>

size_t ret[4096];
void CharCount(const char* str)
{
    size_t len = strlen(str);
    for (size_t i = 0; i < len;  ++i) {
        ret[str[i]]++;
    }

    for (size_t i = 0; i < 4096; ++i) {
        if (ret[i]) {
            std::cout << ret[i] << std::endl;
        }
    }
}

void CharCount2(const char* str)
{
    size_t len = strlen(str);
    size_t count[len] = {0};
    for (size_t i = 0; i < len; ++i) {
        for (size_t j = i + 1; j < len; j++) {
            if (str[i] == str[j]) {
                
            }
        }
    }
}

int main()
{
    const char* str = "123321@@aa..cc*&^^&*";
    CharCount(str);

    return 0;
}