#set(USE_ADDONS Freetype OpenAL OpenGL GLFW Static SSD1306 ENet Bullet Linus ImGui)
set(USE_ADDONS Assimp Bullet Core ENet Freetype GLFW ImGui OpenAL OpenGL SSD1306 Static)

File(GLOB_RECURSE   SOURCE_FILES Hei/Source/*.cpp )
File(GLOB_RECURSE   HEADER_FILES Hei/Source/*.h   )

list(FILTER SOURCE_FILES EXCLUDE REGEX Hei/Source/DedicatedServer/*.*)
list(FILTER HEADER_FILES EXCLUDE REGEX Hei/Source/DedicatedServer/*.*)

