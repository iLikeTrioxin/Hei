set(USE_ADDONS Assimp Freetype OpenAL OpenGL GLFW Static SSD1306 ENet Bullet)

File(GLOB_RECURSE   SOURCE_FILES Hei/Source/*.cpp )
File(GLOB_RECURSE   HEADER_FILES Hei/Source/*.h   )

list(FILTER SOURCE_FILES EXCLUDE REGEX Hei/Source/DedicatedServer/*.*)
list(FILTER HEADER_FILES EXCLUDE REGEX Hei/Source/DedicatedServer/*.*)

