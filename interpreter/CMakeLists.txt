cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_SOURCE_DIR}/includes)

if(WIN32)
	set(mlang_shared_LIBS )
else(WIN32)
	set(mlang_shared_LIBS dl)
endif(WIN32)

add_library(mlang_shared SHARED
	runtime.cpp
	values/boolean.cpp
	values/string.cpp
	values/function.cpp
	values/number.cpp
	values/base.cpp
	values/null.cpp
	values/map.cpp
	ops/base.cpp
	ops/push.cpp
	ops/pushs.cpp
	ops/pop.cpp
	ops/pushb.cpp
	ops/pushmap.cpp
	ops/type_operator.cpp
)
set_target_properties(mlang_shared
	PROPERTIES OUTPUT_NAME mlang)
target_link_libraries(mlang_shared ${mlang_shared_LIBS})

add_executable(mlang
	main.cpp
)
target_link_libraries(mlang mlang_shared)
