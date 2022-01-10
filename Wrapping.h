#pragma once

#include <AssFileSystem.h>

#include <corecrt_io.h>
FILE *winrt_open_file(const char *path) {
  wchar_t *wpath;
  int wlen;
  FILE *f;
  HANDLE hfile;
  int file_osfhandle;
  CREATEFILE2_EXTENDED_PARAMETERS ext_params = {0};
  ext_params.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
  ext_params.dwSize = sizeof(ext_params);

  wlen = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);

  wpath = (wchar_t *)malloc(wlen * sizeof(wchar_t));

  if (!MultiByteToWideChar(CP_UTF8, 0, path, -1, wpath, wlen)) {
    free(wpath);
    return NULL;
  }

  hfile = CreateFile2(wpath, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING,
                      &ext_params);
  free(wpath);
  if (hfile == INVALID_HANDLE_VALUE) {
    return NULL;
  }

  file_osfhandle = _open_osfhandle((intptr_t)hfile, 0);

  if (file_osfhandle == -1) {
    CloseHandle(hfile);
    return NULL;
  }

  f = _fdopen(file_osfhandle, "rb");
  return f;
}

#include <codecvt>
#include <cvt/wstring>
#include <sstream>
namespace WindowsRuntimeComponent {
public
ref class Wrapping sealed {
private:
  Wrapping(){};

public:
    static Platform::String^
  GetFile(Platform::String^ instr) {
    stdext::cvt::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    std::string stringUTF8 = convert.to_bytes(instr->Data());
    const char *nstring = stringUTF8.c_str();
    // ass open file
    FILE *file = ass_open_file(nstring, FN_EXTERNAL);
    if (file == NULL) {
      return ref new Platform::String(L"ERROR");
    }
    // only a sample: read file and return it as String^
    std::vector<char> clist;
    while (!feof(file)) {
      auto c = getc(file);
      clist.push_back(c);
    }
    fclose(file);
    auto cstring = std::string(clist.data());
    return ref new Platform::String(
        std::wstring(cstring.begin(), cstring.end()).c_str());
  }

  static Platform::String^
  GetDictionary(Platform::String^ instr) {
    //auto nstring = convert_platform_string_to_cstr(instr);
    stdext::cvt::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    std::string stringUTF8 = convert.to_bytes(instr->Data());
    const char *nstring = stringUTF8.c_str();
    auto assdir = new ASS_Dir;
    // ass open dir
    if (!ass_open_dir(assdir, nstring)) {
      return ref new Platform::String(L"ERROR");
    }
    // only a sample: read dir and return it as String^
    std::wostringstream stream;
    while (true)
    {
      auto name = ass_read_dir(assdir);
      if (!name) {
        break;
      }
      size_t size = sizeof(wchar_t);
      ASS_StringView ass_name = {name, strlen(name)};
      if (!check_add_size_wtf8to16(&size, ass_name.len))
        return ref new Platform::String(L"NULL");
      wchar_t *wname = (wchar_t *)malloc(size);
      if (!wname)
        return ref new Platform::String(L"NULL");
      wchar_t *end = convert_wtf8to16(wname, ass_name);
      if (end) {
        *end = L'\0';
        stream << wname << std::endl;
      }
    }
    auto result_string = ref new Platform::String(stream.str().c_str());
    ass_close_dir(assdir);
    //delete assdir;
    return result_string;
  }
};
} // namespace WindowsRuntimeComponent
