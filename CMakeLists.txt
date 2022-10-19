cmake_minimum_required(VERSION 3.5)

project(unienet LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Wt REQUIRED)

SET (WT_CONNECTOR "wthttp" CACHE STRING "Connector used (wthttp(YES) or wtfcgi(NO))")

add_executable(unienet
  app.h
  app.cpp
  session.h
  session.cpp
  user.h
  user.cpp
  event.h
  event.cpp
  eventdatabase.h
  eventdatabase.cpp
  markerwidget.h
  markerwidget.cpp
  markerinfowidget.h
  markerinfowidget.cpp
  markerformwidget.h
  markerformwidget.cpp
  style.css
  wt_config.xml
)

TARGET_LINK_LIBRARIES(unienet
  ${WT_CONNECTOR}
  wt
  Wt::Dbo
  Wt::DboMySQL
)

include_directories(/usr/local/include/Wt)

add_custom_command(
        TARGET unienet POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/wt_config.xml
                ${CMAKE_CURRENT_BINARY_DIR}/wt_config.xml)

add_custom_command(
        TARGET unienet POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/style.css
                        ${CMAKE_CURRENT_BINARY_DIR}/style.css)

file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
