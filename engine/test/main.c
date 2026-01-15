#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#include "pytest.h"
#include "PYDyn.h"
#include "PYDynEx.h"

void set_charset()
{
#ifdef _WIN32
    system("chcp 65001");
#endif

    //CONSOLE_FONT_INFOEX info = { 0 };
    //info.cbSize = sizeof(info);
    //info.dwFontSize.Y = 16; // leave X as zero
    //info.FontWeight = FW_NORMAL;
    //wcscpy(info.FaceName, L"Consolas");
    //SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), NULL, &info);
}

int main()
{
    set_charset();
    // phrase_output_test();
  //  test_lex_write();
    //    test_dyn_reader();
    test_engine();

  // test_han();

  // test_lex_reader();
//  test_graph();

    return 0;
}

