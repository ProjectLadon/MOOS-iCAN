#--------------------------------------------------------
# The CMakeLists.txt for:                       iCAN
# Author(s):                              
#--------------------------------------------------------

SET(SRC
  CAN.cpp
  CAN_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(iCAN ${SRC})

TARGET_LINK_LIBRARIES(iCAN
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

