//
//	Header wrapper for stb_image
//

#include "../source/stb_image.h" // I don't know why, but msvc cannot find this header if added as PRIVATE in CMake
                                 //   so I added it as relative