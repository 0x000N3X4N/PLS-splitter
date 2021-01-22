#pragma once


#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <iterator>
#include <filesystem>


typedef unsigned long long ulong_64_t;


class CSplit_PLS {
  std::experimental::filesystem::path m_pathToFile;
  std::vector<std::wstring> m_vContentOfFile;
  bool m_isValid;
  ulong_64_t m_nEntries;

  bool isValid() const;
  bool initEntries();
public:
  explicit CSplit_PLS(std::wstring& sPathToFile);
  bool exec(ulong_64_t cntOf);
};