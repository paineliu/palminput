cmake_minimum_required(VERSION 3.12)
project(py_test C)

set(CMAKE_C_STANDARD 11)

include_directories(./include)
include_directories(./src/pinyin)

SET(LIB_PINYIN_SRC
        ./src/pinyin/PDLexCore.c
        ./src/pinyin/PDLexBeijing.c
        ./src/pinyin/PDLexBeijing.h
        ./src/pinyin/PDLexName.c
        ./src/pinyin/PDLexName.h
        ./src/pinyin/PYCand.c
        ./src/pinyin/PYCand.h
        ./src/pinyin/PYConst.h
        ./src/pinyin/PYDyn.c
        ./src/pinyin/PYDyn.h
        ./src/pinyin/PYDynEx.c
        ./src/pinyin/PYEngine.c
        ./src/pinyin/PYEngineEx.c
        ./src/pinyin/PYFix.c
        ./src/pinyin/PYFix.h
        ./src/pinyin/PYGraph.c
        ./src/pinyin/PYGraph.h
        ./src/pinyin/PYHan.c
        ./src/pinyin/PYHan.h
        ./src/pinyin/PYHanData.c
        ./src/pinyin/PYHanData.h
        ./src/pinyin/PYHanDef.h
        ./src/pinyin/PYKey.c
        ./src/pinyin/PYKey.h
        ./src/pinyin/PYLex.c
        ./src/pinyin/PYLex.h
        ./src/pinyin/PYLexEx.cpp
        ./src/pinyin/PYLexEx.h
        ./src/pinyin/PYName.c
        ./src/pinyin/PYName.h
        ./src/pinyin/PYPredict.c
        ./src/pinyin/PYPredict.h
        ./src/pinyin/PYName.h
        ./src/pinyin/PYPriDef.h
        ./src/pinyin/PYUtils.c
        ./src/pinyin/PYUtils.h
        )

SET(PINYIN_TEST_SRC
        ./test/main.c
        ./test/pytest.c
        ./test/pytest.h
        ./test/pytest_dyn.c
        ./test/pytest_engine.c
        ./test/pytest_graph.c
        ./test/pytest_han.c
        ./test/pytest_lexread.c
        ./test/pytest_lexwrite.c
        )

add_library(palmengine STATIC ${LIB_PINYIN_SRC})
target_compile_options(palmengine PRIVATE -fPIC)
# target_link_libraries(palmengine m)

add_executable(pe_test ${PINYIN_TEST_SRC})
target_link_libraries(pe_test palmengine)
