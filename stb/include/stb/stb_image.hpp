//
//	Header wrapper for stb_image
//	Defines required macro so that I never forget to define it myself
//

#define STB_IMAGE_IMPLEMENTATION
#include "../source/stb_image.h" // I don't know why, but msvc cannot find this header if added as PRIVATE in CMake
								 //   so I added it as relative