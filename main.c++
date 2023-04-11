#include <iostream>
#include <string>
#include <fstream>
#include <Windows.h>

std::wstring GetCurrentExecutableDirectory() {
    wchar_t buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
    return std::wstring(buffer).substr(0, pos);
}

void ReadPathsFromFile(const std::wstring& filename, std::wstring& hd_trabalho, std::wstring& hd_backup) {
    std::wifstream config_file(filename);
    if (config_file.is_open()) {
        std::getline(config_file, hd_trabalho);
        std::getline(config_file, hd_backup);
        config_file.close();
    }
    else {
        std::wcerr << L"Não foi possível abrir o arquivo de configuração: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }
}

void CopyFilesRecursive(const std::wstring& src, const std::wstring& dst) {
    WIN32_FIND_DATA ffd;
    std::wstring src_pattern = src + L"\\*";
    HANDLE hFind = FindFirstFile(src_pattern.c_str(), &ffd);

    if (hFind == INVALID_HANDLE_VALUE) {
        std::wcerr << L"FindFirstFile falhou (" << GetLastError() << L")" << std::endl;
        return;
    }

    do {
        if (wcscmp(ffd.cFileName, L".") == 0 || wcscmp(ffd.cFileName, L"..") == 0) continue;

        std::wstring src_path = src + L"\\" + ffd.cFileName;
        std::wstring dst_path = dst + L"\\" + ffd.cFileName;

        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            CreateDirectory(dst_path.c_str(), NULL);
            CopyFilesRecursive(src_path, dst_path);
        }
        else {
            CopyFile(src_path.c_str(), dst_path.c_str(), FALSE);
        }
    } while (FindNextFile(hFind, &ffd) != 0);

    FindClose(hFind);
}

std::wstring CurrentDateTime() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    wchar_t buffer[20];
    swprintf_s(buffer, L"%04d%02d%02d_%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    return buffer;
}

int main() {
    std::wstring hd_trabalho;
    std::wstring hd_backup;
    std::wstring config_file_path = GetCurrentExecutableDirectory() + L"\\config.txt";

    ReadPathsFromFile(config_file_path, hd_trabalho, hd_backup);

    std::wstring pasta_backup = hd_backup + L"\\" + CurrentDateTime();

    CreateDirectory(pasta_backup.c_str(), NULL);
    CopyFilesRecursive(hd_trabalho, pasta_backup);

    return 0;
}