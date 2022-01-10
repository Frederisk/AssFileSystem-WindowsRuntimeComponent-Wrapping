#include "pch.h"
//
//#include <assert.h>
//
//#include "AssFileSystem.h"
//
//static inline bool check_add_size(size_t *size, size_t amount) {
//  size_t res = *size + amount;
//  if (res < amount)
//    return false;
//  *size = res;
//  return true;
//}
//
//#define NAME_BUF_SIZE 256
//
//static bool alloc_path(ASS_Dir *dir, size_t size) {
//  if (size <= dir->max_path)
//    return true;
//  if (!check_add_size(&size, NAME_BUF_SIZE))
//    return false;
//  char *path = (char *)realloc(dir->path, size);
//  if (!path)
//    return false;
//  dir->path = path;
//  dir->max_path = size;
//  return true;
//}
//
//#if !defined(_WIN32) || defined(__CYGWIN__)
//
//#include <dirent.h>
//
//FILE *ass_open_file(const char *filename, FileNameSource hint) {
//  return fopen(filename, "rb");
//}
//
//bool ass_open_dir(ASS_Dir *dir, const char *path) {
//  dir->handle = NULL;
//  dir->path = NULL;
//  dir->name = NULL;
//
//  size_t len = strlen(path);
//  if (len && path[len - 1] == '/')
//    len--;
//
//  size_t size = NAME_BUF_SIZE + 2;
//  if (!check_add_size(&size, len))
//    return false;
//  dir->path = malloc(size);
//  if (!dir->path)
//    return false;
//  dir->max_path = size;
//  memcpy(dir->path, path, len);
//  dir->path[len] = '/';
//  dir->prefix = len + 1;
//
//  dir->handle = opendir(path);
//  if (dir->handle)
//    return true;
//
//  free(dir->path);
//  dir->path = NULL;
//  return false;
//}
//
//const char *ass_read_dir(ASS_Dir *dir) {
//  struct dirent *entry = readdir(dir->handle);
//  return dir->name = entry ? entry->d_name : NULL;
//}
//
//const char *ass_current_file_path(ASS_Dir *dir) {
//  size_t size = dir->prefix + 1, len = strlen(dir->name);
//  if (!check_add_size(&size, len) || !alloc_path(dir, size))
//    return NULL;
//  memcpy(dir->path + dir->prefix, dir->name, len + 1);
//  return dir->path;
//}
//
//void ass_close_dir(ASS_Dir *dir) {
//  if (dir->handle)
//    closedir(dir->handle);
//  free(dir->path);
//  dir->handle = NULL;
//  dir->path = NULL;
//}
//
//#else // Windows
//
//#ifndef ASS_DIRECTWRITE_H
//#define ASS_DIRECTWRITE_H
//
//#ifdef WINAPI_FAMILY
//#include <winapifamily.h>
//#define ASS_WINAPI_DESKTOP WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
//#else
//#define ASS_WINAPI_DESKTOP 1
//#endif
//
//// ASS_FontProvider *ass_directwrite_add_provider(ASS_Library *lib,
////                                               ASS_FontSelector *selector,
////                                               const char *config,
////                                               FT_Library ftlib);
//
//#endif
//
////#include "ass_directwrite.h" // for ASS_WINAPI_DESKTOP
//#include <windows.h>
//
//static const uint8_t wtf8_len_table[256] = {
//    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 0x
//    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 1x
//    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 2x
//    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 3x
//
//    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4x
//    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5x
//    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6x
//    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7x
//
//    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 8x
//    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 9x
//    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Ax
//    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bx
//
//    0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // Cx
//    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // Dx
//    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, // Ex
//    4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Fx
//};
//
//static const uint8_t wtf8_len4_range[5][2] = {
//    {0x90, 0x30}, {0x80, 0x40}, {0x80, 0x40}, {0x80, 0x40}, {0x80, 0x10}};
//
//static inline bool check_add_size_wtf8to16(size_t *size, size_t len) {
//  return len > SIZE_MAX / sizeof(wchar_t)
//             ? false
//             : check_add_size(size, len * sizeof(wchar_t));
//}
//
//// does not append zero termination implicitly
//// expects preallocated buffer (use check_add_size_wtf8to16() for size)
//static wchar_t *convert_wtf8to16(wchar_t *dst, ASS_StringView src) {
//  const char *str = src.str;
//  for (const char *end = str + src.len; str < end; str++) {
//    uint8_t ch = *str;
//    switch (wtf8_len_table[ch]) {
//    case 1: // 1 -> 1w
//      *dst++ = ch;
//      continue;
//
//    case 2: // 2 -> 1w
//      if (str + 1 < end) {
//        uint8_t next = *++str;
//        if ((next & 0xC0) != 0x80)
//          return NULL;
//        *dst++ = (ch & 0x1F) << 6 | (next & 0x3F);
//        continue;
//      }
//      return NULL;
//
//    case 3: // 3 -> 1w
//      if (str + 2 < end) {
//        ch &= 0xF;
//        uint8_t next1 = *++str, next2 = *++str;
//        if (next1 < (ch ? 0x80 : 0xA0) || next1 >= 0xC0 ||
//            (next2 & 0xC0) != 0x80)
//          return NULL;
//        *dst++ = ch << 12 | (next1 & 0x3F) << 6 | (next2 & 0x3F);
//        continue;
//      }
//      return NULL;
//
//    case 4: // 4 -> 2w
//      if (str + 3 < end) {
//        ch &= 0x7;
//        uint8_t next1 = *++str, next2 = *++str, next3 = *++str;
//        if ((uint8_t)(next1 - wtf8_len4_range[ch][0]) >=
//                wtf8_len4_range[ch][1] ||
//            (next2 & 0xC0) != 0x80 || (next3 & 0xC0) != 0x80)
//          return NULL;
//        *dst++ = 0xD800 | ((ch << 8 | (next1 & 0x3F) << 2) - 0x40) |
//                 (next2 & 0x3F) >> 4;
//        *dst++ = 0xDC00 | (next2 & 0xF) << 6 | (next3 & 0x3F);
//        continue;
//      }
//      return NULL;
//
//    default:
//      return NULL;
//    }
//  }
//  return dst;
//}
//
//static inline bool check_add_size_wtf16to8(size_t *size, size_t wlen) {
//  enum { max_bytes_per_wchar = 3 };
//  return wlen > SIZE_MAX / max_bytes_per_wchar
//             ? false
//             : check_add_size(size, max_bytes_per_wchar * wlen);
//}
//
//// does not append zero termination implicitly
//// expects preallocated buffer (use check_add_size_wtf16to8() for size)
//static char *convert_wtf16to8(char *dst, const wchar_t *src, size_t wlen) {
//  for (const wchar_t *end = src + wlen; src < end; src++) {
//    uint16_t wch = *src;
//    if (wch < 0x80) {
//      // 1w -> 1
//      *dst++ = wch;
//      continue;
//    }
//    if (wch < 0x800) {
//      // 1w -> 2
//      *dst++ = 0xC0 | wch >> 6;
//      *dst++ = 0x80 | (wch & 0x3F);
//      continue;
//    }
//    if (wch >= 0xD800 && wch < 0xDC00 && src + 1 < end) {
//      uint16_t next = src[1];
//      if (next >= 0xDC00 && next < 0xE000) { // correctly paired surrogates
//        src++;                               // 2w -> 4
//        uint32_t full = (uint32_t)((wch & 0x3FF) + 0x40) << 10 | (next & 0x3FF);
//        *dst++ = 0xF0 | full >> 18;
//        *dst++ = 0x80 | ((full >> 12) & 0x3F);
//        *dst++ = 0x80 | ((full >> 6) & 0x3F);
//        *dst++ = 0x80 | (full & 0x3F);
//        continue;
//      }
//    }
//    // unpaired surrogates fall through here
//    // 1w -> 3
//    *dst++ = 0xE0 | wch >> 12;
//    *dst++ = 0x80 | ((wch >> 6) & 0x3F);
//    *dst++ = 0x80 | (wch & 0x3F);
//  }
//  return dst;
//}
//
//static FILE *open_file_wtf8(const char *filename) {
//  size_t size = sizeof(wchar_t);
//  ASS_StringView name = {filename, strlen(filename)};
//  if (!check_add_size_wtf8to16(&size, name.len))
//    return NULL;
//  wchar_t *wname = (wchar_t *)malloc(size);
//  if (!wname)
//    return NULL;
//  wchar_t *end = convert_wtf8to16(wname, name);
//  FILE *fp = NULL;
//  if (end) {
//    *end = L'\0';
//    fp = _wfopen(wname, L"rb");
//  }
//  free(wname);
//  return fp;
//}
//
//FILE *ass_open_file(const char *filename, FileNameSource hint) {
//  FILE *fp = open_file_wtf8(filename);
//  if (fp || hint == FN_DIR_LIST)
//    return fp;
//  return fopen(filename, "rb");
//}
//
//static const wchar_t dir_tail[] = L"\\*";
//
//static bool append_tail(wchar_t *wpath, size_t wlen) {
//  size_t offs = 0;
//  if (wlen == 2 && wpath[1] == L':')
//    offs = 1;
//  else if (wlen && (wpath[wlen - 1] == L'/' || wpath[wlen - 1] == L'\\'))
//    offs = 1;
//  memcpy(wpath + wlen, dir_tail + offs, sizeof(dir_tail) - offs);
//  return !offs;
//}
//
//static bool open_dir_wtf8(ASS_Dir *dir, ASS_StringView path) {
//  assert(dir->handle == INVALID_HANDLE_VALUE && !dir->path);
//
//  size_t size = sizeof(dir_tail);
//  if (!check_add_size_wtf8to16(&size, path.len))
//    return false;
//
//  wchar_t *wpath = (wchar_t *)malloc(size);
//  if (!wpath)
//    return false;
//
//  bool add_separator;
//  WIN32_FIND_DATAW data;
//  wchar_t *end = convert_wtf8to16(wpath, path);
//  if (end) {
//    add_separator = append_tail(wpath, end - wpath);
//    dir->handle = FindFirstFileExW(wpath, FindExInfoBasic, &data,
//                                   FindExSearchNameMatch, NULL, 0);
//  }
//  free(wpath);
//  if (dir->handle == INVALID_HANDLE_VALUE)
//    return false;
//
//  size = NAME_BUF_SIZE + 2;
//  size_t wlen = wcslen(data.cFileName);
//  if (!check_add_size(&size, path.len) ||
//      !check_add_size_wtf16to8(&size, wlen) ||
//      !(dir->path = (char *)malloc(size))) {
//    FindClose(dir->handle);
//    return false;
//  }
//  dir->max_path = size;
//  memcpy(dir->path, path.str, path.len);
//  if (add_separator)
//    dir->path[path.len++] = '\\';
//  *convert_wtf16to8(dir->path + path.len, data.cFileName, wlen) = '\0';
//  dir->prefix = path.len;
//  return true;
//}
//
//bool ass_open_dir(ASS_Dir *dir, const char *path) {
//  dir->handle = INVALID_HANDLE_VALUE;
//  dir->path = NULL;
//  dir->name = NULL;
//
//  size_t len = strlen(path);
//  if (open_dir_wtf8(dir, ASS_StringView{path, len}))
//    return true;
//
//  if (len > INT_MAX)
//    return false;
//
//  UINT cp = CP_ACP;
//#if ASS_WINAPI_DESKTOP
//  if (!AreFileApisANSI())
//    cp = CP_OEMCP;
//#endif
//  size_t wlen = MultiByteToWideChar(cp, 0, path, len, NULL, 0);
//  if (wlen > (SIZE_MAX - sizeof(dir_tail)) / sizeof(wchar_t))
//    return false;
//  wchar_t *wpath = (wchar_t *)malloc(wlen * sizeof(wchar_t) + sizeof(dir_tail));
//  if (!wpath)
//    return false;
//  MultiByteToWideChar(cp, 0, path, len, wpath, wlen);
//  bool add_separator = append_tail(wpath, wlen);
//
//  WIN32_FIND_DATAW data;
//  dir->handle = FindFirstFileExW(wpath, FindExInfoBasic, &data,
//                                 FindExSearchNameMatch, NULL, 0);
//  if (dir->handle == INVALID_HANDLE_VALUE) {
//    free(wpath);
//    return false;
//  }
//  size_t size = NAME_BUF_SIZE + 2, wlen1 = wcslen(data.cFileName);
//  if (!check_add_size_wtf16to8(&size, wlen) ||
//      !check_add_size_wtf16to8(&size, wlen1) ||
//      !(dir->path = (char *)malloc(size))) {
//    FindClose(dir->handle);
//    return false;
//  }
//  dir->max_path = size;
//  char *ptr = convert_wtf16to8(dir->path, wpath, wlen);
//  if (add_separator)
//    *ptr++ = '\\';
//  convert_wtf16to8(ptr, data.cFileName, wlen1 + 1);
//  dir->prefix = ptr - dir->path;
//  return true;
//}
//
//const char *ass_read_dir(ASS_Dir *dir) {
//  if (!dir->name) // first invocation
//    return dir->name = dir->path + dir->prefix;
//
//  WIN32_FIND_DATAW data;
//  while (FindNextFileW(dir->handle, &data)) {
//    size_t size = dir->prefix + 1, wlen = wcslen(data.cFileName);
//    if (!check_add_size_wtf16to8(&size, wlen) || !alloc_path(dir, size))
//      continue;
//    convert_wtf16to8(dir->path + dir->prefix, data.cFileName, wlen + 1);
//    return dir->name = dir->path + dir->prefix;
//  }
//  return NULL;
//}
//
//const char *ass_current_file_path(ASS_Dir *dir) { return dir->path; }
//
//void ass_close_dir(ASS_Dir *dir) {
//  if (dir->handle != INVALID_HANDLE_VALUE)
//    FindClose(dir->handle);
//  free(dir->path);
//  dir->handle = INVALID_HANDLE_VALUE;
//  dir->path = NULL;
//}
//
//#endif // Windows
