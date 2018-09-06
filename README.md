# file_to_obj
Converts any file to linkable x64 obj file for msvc. This way you don't need to compile C file (e.g. from xd) and you dont need to mess with resource files etc...

# Usage

For use with cmake:
* Put the included (in assets dir) `EmbedFileFTO.cmake` into your `CMAKE_MODULE_PATH`
* `include("cmake/EmbedFileFTO.cmake")` somewhere in your `cmakelists.txt`
* `embed_file( <LIST OF FILES TO EMBED> )`
* add to `add_executable(...)` optionaly: `${EMBEDDED_HDRS}` and mandatory: `${EMBEDDED_LIBS}`
* `include_directories(${PROJECT_BINARY_DIR})` because i forget if it already looks for headers there?
* in your cpp file `#include "<EMBEDED FILE NAME>.hpp"` and it will contain: `EMB_FILE_<FILE NAME>` and `EMB_FILE_SIZE_<FILE NAME>`
* Have fun!

# WHY?

Not sure... Probably sanity is getting low... And i liked the `ld` having that power and basically alternatives: [here](https://stackoverflow.com/questions/4864866/c-c-with-gcc-statically-add-resource-files-to-executable-library)
