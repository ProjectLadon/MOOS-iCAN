#--------------------------------------------------------
# The CMakeLists.txt for:                       iCAN
# Author(s):
#--------------------------------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

SET(XXD
  /usr/bin/xxd)

# List of schema files
set(JSON_SCHEMA_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/schema/channel_schema.json
)

# List of schema files
set(JSON_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/schema/channel_schema.h
)

FOREACH(LOOP_SCHEMA ${JSON_SCHEMA_FILES})
	get_filename_component(LOOP_DIR ${LOOP_SCHEMA} DIRECTORY)
	get_filename_component(LOOP_FILE_WE ${LOOP_SCHEMA} NAME_WE)
	SET(LOOP_HEADER ${LOOP_DIR}/${LOOP_FILE_WE}.h)
	add_custom_command(
		OUTPUT ${LOOP_HEADER}
		COMMAND	cd ${LOOP_DIR} && ${XXD} -i ${LOOP_FILE_WE}.json ${LOOP_FILE_WE}.h
		DEPENDS ${LOOP_SCHEMA})
ENDFOREACH(LOOP_SCHEMA)

SET(SRC
  CAN.cpp
  CAN_Info.cpp
  CAN_field.cpp
  main.cpp
  "${JSON_HEADER_FILES}"
)

ADD_EXECUTABLE(iCAN ${SRC})

TARGET_LINK_LIBRARIES(iCAN
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)
