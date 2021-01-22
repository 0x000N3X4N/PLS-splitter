#include <iostream>
#include <Windows.h>
#include "Split_PLS.h"


int main(int argc, char* argv[]) {
  if (argc == 3) {
    LPWSTR* szArglist;
    int nArgs;
    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    if (NULL == szArglist) {
      std::wcerr << L"CommandLineToArgvW failed" << std::endl;
      return -1;
    }
    std::wstring sPathToFile = szArglist[1];
    CSplit_PLS pls(sPathToFile);
    pls.exec(atoi(argv[2]));

    LocalFree(szArglist);
  }
  else std::wcerr << L"Error arguments, @template: $name_of_executable $path_to_PLS_file $count_of_splits.";
  system("pause");
  return 0;
}