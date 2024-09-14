cmake_minimum_required(VERSION 3.6)
project (effectiveModernCpp)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCE *.cpp)
foreach(source ${SOURCE})
	message("source: ${source}")
	string(REGEX MATCH ".*\/(.*)\.cpp" out ${source})
	set(tmp_depend_source ${CMAKE_MATCH_0})
	set(tmp_target ${CMAKE_MATCH_1})
	add_executable(${tmp_target} ${tmp_depend_source})
endforeach()
