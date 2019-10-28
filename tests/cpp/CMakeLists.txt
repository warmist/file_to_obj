project (test_cpp CXX C)

add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/test_cpp_payload.obj ${PROJECT_BINARY_DIR}/test_cpp_payload.hpp
	COMMAND file_to_obj -cpp ${PROJECT_SOURCE_DIR}/main.cpp ${PROJECT_BINARY_DIR} test_cpp_payload payload)

add_executable(test_cpp main.cpp test_cpp_payload.obj)

include_directories(${PROJECT_BINARY_DIR})