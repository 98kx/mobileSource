#ifndef _ARGCARGV_H
#define _ARGCARGV_H

extern wchar_t * _ppszArgv[];

int __cdecl _ConvertCommandLineToArgcArgv(const wchar_t *lpszSysCmdLine);

#endif //ARGCARGV_H
