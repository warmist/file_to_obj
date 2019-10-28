SET(FTO_PATH ${PROJECT_SOURCE_DIR}/cmake/file_to_obj.exe)
SET(EMBEDDED_LIBS)
SET(EMBEDDED_HDRS)
function(embed_file files)

	foreach(v ${files})
		get_filename_component(filename ${v} NAME_WE)

		string(CONCAT out_hdr ${filename} ".hpp")
		string(CONCAT out_obj ${filename} ".obj")

		string(MAKE_C_IDENTIFIER ${filename} filename_c)
		add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/${out_obj} ${out_hdr} COMMAND ${FTO_PATH} ${v} ${PROJECT_BINARY_DIR} ${filename} EMB_FILE_${filename_c} EMB_FILE_SIZE_${filename_c})

		list(APPEND EMBEDDED_LIBS ${PROJECT_BINARY_DIR}/${out_obj})
		list(APPEND EMBEDDED_HDRS ${out_hdr})

		set(EMBEDDED_HDRS "${EMBEDDED_HDRS}" PARENT_SCOPE)
		set(EMBEDDED_LIBS "${EMBEDDED_LIBS}" PARENT_SCOPE)
	endforeach()
endfunction()

function(embed_file_cpp files)

	foreach(v ${files})
		get_filename_component(filename ${v} NAME_WE)

		string(CONCAT out_hdr ${filename} ".hpp")
		string(CONCAT out_obj ${filename} ".obj")

		string(MAKE_C_IDENTIFIER ${filename} filename_c)
		add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/${out_obj} ${out_hdr} COMMAND ${FTO_PATH} -cpp ${v} ${PROJECT_BINARY_DIR} ${filename} EMB_FILE_${filename_c})

		list(APPEND EMBEDDED_LIBS ${PROJECT_BINARY_DIR}/${out_obj})
		list(APPEND EMBEDDED_HDRS ${out_hdr})

		set(EMBEDDED_HDRS "${EMBEDDED_HDRS}" PARENT_SCOPE)
		set(EMBEDDED_LIBS "${EMBEDDED_LIBS}" PARENT_SCOPE)
	endforeach()
endfunction()