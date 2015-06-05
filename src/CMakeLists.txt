include_directories(${CMAKE_SOURCE_DIR}/include)
add_executable(pgmTest pgmTest.cpp)
add_executable(2dPathing main2D.cpp distance2D.cpp)
add_executable(3dPathing main.cpp distance.cpp)
target_compile_features(2dPathing PUBLIC cxx_decltype_auto)
target_compile_features(3dPathing PUBLIC cxx_decltype_auto)

