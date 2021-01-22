#include "Split_PLS.h"


CSplit_PLS::CSplit_PLS(std::wstring& sPathToFile) : m_isValid(false) {
  std::wifstream f_in(sPathToFile);
  if (f_in.is_open()) {
    m_pathToFile = sPathToFile;
    std::wstring tmp;

    while (!f_in.eof()) {
      std::getline(f_in, tmp);
      m_vContentOfFile.push_back(tmp);
    }
    std::wcout << m_vContentOfFile[m_vContentOfFile.size() - 2] << std::endl;
  }
}

bool CSplit_PLS::isValid() const {
  return (m_vContentOfFile[0] == L"[playlist]" && 
    m_vContentOfFile[1].find(L"File1=") != std::wstring::npos && 
    m_vContentOfFile[m_vContentOfFile.size() - 4].find(L"Title") != std::wstring::npos &&
    m_vContentOfFile[m_vContentOfFile.size() - 2].find(L"NumberOfEntries=") != std::wstring::npos &&
    m_vContentOfFile[m_vContentOfFile.size() - 1] == L"Version=2");
}

bool CSplit_PLS::initEntries() {
  auto offset = m_vContentOfFile[m_vContentOfFile.size() - 2].find(L"=");
  std::wstring wsEntries = m_vContentOfFile[m_vContentOfFile.size() - 2].substr(offset + 1);
  m_nEntries = std::wcstoul(wsEntries.c_str(), nullptr, 0);
  return true;
}

bool CSplit_PLS::exec(ulong_64_t cntOfTracksOnSplit) {
  initEntries();
  m_isValid = isValid();
  
  try {
    if (m_isValid && m_nEntries > cntOfTracksOnSplit) {
      size_t nCountOfFullSplits = m_nEntries / cntOfTracksOnSplit;
      size_t nRemainder = m_nEntries % cntOfTracksOnSplit;

      std::wcout << L"Count of fill splits: " << nCountOfFullSplits << std::endl
                 << L"Remaining tracks: " << nRemainder << std::endl;

      std::wstring pathToSplittedFile;

      pathToSplittedFile = m_pathToFile.parent_path().wstring() + L"\\splitter";
      if (!std::experimental::filesystem::exists(pathToSplittedFile)) {
        if (!std::experimental::filesystem::create_directory(pathToSplittedFile))
          throw std::runtime_error("Erorr, cannot create directory! \
Please check user privileges or try running the program as administrator");
      }

      ulong_64_t line = 1;
      for (size_t i = 0; i < nCountOfFullSplits; i++) {
        pathToSplittedFile = m_pathToFile.parent_path().wstring() + L"\\splitter\\" + 
          m_pathToFile.stem().wstring() + L"_splitted" + std::to_wstring(i) + m_pathToFile.extension().wstring();
        std::wofstream f_out(pathToSplittedFile);
        if (!f_out.is_open()) throw std::runtime_error("Erorr, cannot create splitted file! \
Please check user privileges or try running the program as administrator");
        f_out << L"[playlist]" << std::endl;
       
        for (size_t j = 1; j <= cntOfTracksOnSplit; j++) {
          auto offset = m_vContentOfFile[line].find(L"=");
          std::wstring rhs = m_vContentOfFile[line].substr(offset);
          f_out << L"File" << std::to_wstring(j) << rhs << std::endl;
          line++;

          offset = m_vContentOfFile[line].find(L"=");
          rhs = m_vContentOfFile[line].substr(offset);
          f_out << L"Title" << std::to_wstring(j) << rhs << std::endl;
          line++;

          offset = m_vContentOfFile[line].find(L"=");
          rhs = m_vContentOfFile[line].substr(offset);
          f_out << L"Length" << std::to_wstring(j) << rhs << std::endl;
          line++;
        }
        f_out << L"NumberOfEntries=" << std::to_wstring(cntOfTracksOnSplit) << std::endl;
        f_out << L"Version=2";
        f_out.close();
      }
      
      std::experimental::filesystem::path pathToRemainingFiles = pathToSplittedFile;
      pathToRemainingFiles.replace_filename(m_pathToFile.stem().wstring() + L"_remaning.pls");
      std::wofstream f_remaining_out(pathToRemainingFiles.wstring());
      if (!f_remaining_out.is_open()) throw std::runtime_error("Erorr, cannot create file with remaining tracks! \
Please check user privileges or try running the program as administrator");
      f_remaining_out << L"[playlist]" << std::endl;
      for (ulong_64_t i = 1; i <= nRemainder; i++) {
        auto offset = m_vContentOfFile[line].find(L"=");
        std::wstring rhs = m_vContentOfFile[line].substr(offset);
        f_remaining_out << L"File" << std::to_wstring(i) << rhs << std::endl;
        line++;

        offset = m_vContentOfFile[line].find(L"=");
        rhs = m_vContentOfFile[line].substr(offset);
        f_remaining_out << L"Title" << std::to_wstring(i) << rhs << std::endl;
        line++;

        offset = m_vContentOfFile[line].find(L"=");
        rhs = m_vContentOfFile[line].substr(offset);
        f_remaining_out << L"Length" << std::to_wstring(i) << rhs << std::endl;
        line++;
      }

      f_remaining_out << L"NumberOfEntries=" << std::to_wstring(nRemainder) << std::endl;
      f_remaining_out << L"Version=2";
      f_remaining_out.close();
    }
    else if (!m_isValid)
      throw std::invalid_argument("Error, this is not valid PLS file format.");
    else if (m_nEntries < cntOfTracksOnSplit)
      throw std::logic_error("Logic error, count of tracks in playlist less than \
count of tracks if one split");
  }
  catch (const std::invalid_argument& e) {
    std::wcerr << e.what() << std::endl;
  }
  catch (const std::logic_error& e) {
    std::wcerr << e.what() << std::endl;
  }
  catch (const std::runtime_error& e) {
    std::wcerr << e.what() << std::endl;
  }
  return true;
}
