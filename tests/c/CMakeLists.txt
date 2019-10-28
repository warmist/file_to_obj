project (test_c CXX C)

add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/test_c_payload.obj ${PROJECT_BINARY_DIR}/test_c_payload.hpp 
	COMMAND file_to_obj ${PROJECT_SOURCE_DIR}/main.cpp ${PROJECT_BINARY_DIR} test_c_payload payload payload_size)

add_executable(test_c main.cpp test_c_payload.obj)

include_directories(${PROJECT_BINARY_DIR})
