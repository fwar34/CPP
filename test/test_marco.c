#include <stdio.h>
#include <string.h>

#define STRUCT_FIELD_SIZE(structType, fieldName) sizeof(((structType*)0)->fieldName)

typedef struct {
    int a;
    char* b;
} Test;

int main(int argc, char const *argv[])
{
    printf("sizeof(a) = %zd, sizeof(b) = %zd\n", STRUCT_FIELD_SIZE(Test, a), STRUCT_FIELD_SIZE(Test, b));
    char* str = "";
    printf("%d\n", *str);
    char* str2 = NULL;
    printf("%p\n", str2);
    printf("%zu\n", strlen(str));
    return 0;
}

