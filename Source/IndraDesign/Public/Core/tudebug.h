#pragma once
#include "debug.h"
#include <vector>

//static uint64 logFlags;
inline uint64 make_bitmask(std::vector<std::pair<int,int>> a_args){
    uint64 result = 0;
    for(auto p : a_args){
        for(int i = p.first; i <= p.second; ++i){
            result |= (1ull << (i - 1));
        }
    }
    return result;
}
#pragma warning (disable: 4005)
#define debugEvent(flag,level,Format,...)                       if(logFlags & (1ULL << (flag-1))) IN_LOG(DebugEvents,level,true,Format,##__VA_ARGS__);
#define debugInfo(flag,CategoryName,Format,...)                 if(logFlags & (1ULL << (flag-1))) IN_LOG(CategoryName,Info,true,Format,##__VA_ARGS__);
#define debugError(flag,CategoryName,Format,...)                if(logFlags & (1ULL << (flag-1))) IN_LOG(CategoryName,Error,true,Format,##__VA_ARGS__);
#define debugFatal(flag,CategoryName,Format,...)                if(logFlags & (1ULL << (flag-1))) IN_LOG(CategoryName,Fatal,true,Format,##__VA_ARGS__);
#define debugWarning(flag,CategoryName,Format,...)              if(logFlags & (1ULL << (flag-1))) IN_LOG(CategoryName,Warning,true,Format,##__VA_ARGS__);
#define debugVerbose(flag,CategoryName,Format,...)              if(logFlags & (1ULL << (flag-1))) IN_LOG(CategoryName,Verbose,false,Format,##__VA_ARGS__);
#define debugVeryVerbose(flag,CategoryName,Format,...)          if(logFlags & (1ULL << (flag-1))) IN_LOG(CategoryName,VeryVerbose,false,Format,##__VA_ARGS__);

#define debugInfoC(flag,CategoryName,Color,Format,...)          if(logFlags & (1ULL << (flag-1))) IN_LOGC(CategoryName,Info,Color,true,Format,##__VA_ARGS__);
#define debugVerboseC(flag,CategoryName,Color,Format,...)       if(logFlags & (1ULL << (flag-1))) IN_LOGC(CategoryName,Verbose,Color,false,Format,##__VA_ARGS__);
#define debugVeryVerboseC(flag,CategoryName,Color,Format,...)   if(logFlags & (1ULL << (flag-1))) IN_LOGC(CategoryName,VeryVerbose,Color,false,Format,##__VA_ARGS__);
