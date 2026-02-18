#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#undef max
#undef min

// Check if Thumbnail is a macro
#ifdef Thumbnail
#error "Thumbnail is defined as a macro!"
#endif

enum class ImageIdDomain : unsigned char {
    Thumbnail = 0x06,
};

int test() {
    return (int)ImageIdDomain::Thumbnail;
}
