#pragma once
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include <UObject/UObjectBase.h> 
#include <GenericPlatform/GenericPlatformMisc.h>

#include <cwchar>
#include <utility>

using FileName = const ANSICHAR*;

#define Log Log
#define Info Log
#define Error Error
#define Fatal Fatal
#define Warning Warning
#define Verbose Verbose
#define VeryVerbose VeryVerbose
struct DColor{
    friend class Debug;
    FColor ScreenColor;
    const TCHAR* LogColor;
    

    static DColor Default; //gray

private:
    static DColor Purple;
public:
    static DColor Green;
    static DColor Blue;

    static DColor Black;
    static DColor Cyan;
    static DColor White;
    static DColor Yellow;
    static DColor Red;
};

class Debug
{
private:
    static DColor& getSeverityColor( ELogVerbosity::Type level )
    {
        switch( level )
        {
            case ELogVerbosity::Type::VeryVerbose:
                return DColor::Default;
            case ELogVerbosity::Type::Verbose:
                return DColor::Cyan;
            case ELogVerbosity::Type::Log:
                return DColor::White;
            case ELogVerbosity::Type::Warning:
                return DColor::Yellow;
            case ELogVerbosity::Type::Error:
            case ELogVerbosity::Type::Fatal:
                return DColor::Red;
        }
        return DColor::White;
    }
public:
    template<class T>
    static FString GetDebugName(T* p){
        if(std::is_base_of<AActor,T>()){
            return AActor::GetDebugName((AActor*)p);
		} else if(std::is_base_of<UActorComponent, T>()){
			return ((UActorComponent*)p)->GetReadableName();
		}
        return FString("name-unavailable");
    }
    template<class T>
    static auto ToString(T &obj){
        return obj.ToString().GetCharArray().GetData();
    }
    template<typename... Args>
    static FString sprintf(const wchar_t* format, Args... args){
        const size_t bsize = 512;
        wchar_t buffer[bsize];
        swprintf(buffer,bsize,format,std::forward<Args>(args)...);
        return FString(buffer);
    }
    static void printOnScreen(const FString &msg, float duration, const DColor &color = DColor::White, uint64 key = INDEX_NONE);
    static void logLine(const FName &logName, const FString &msg, const DColor &color, ELogVerbosity::Type level, FileName file, int32 line);
    static void logLine(const FName &logName, const FString &msg, ELogVerbosity::Type level, FileName file, int32 line);
    static void log(const FName &logName, const FString &msg, ELogVerbosity::Type level, FileName file, int32 line, bool printToScreen = false, uint64 key = INDEX_NONE);
    static void log(const FName &logName, const FString &msg, const DColor &color, ELogVerbosity::Type level, FileName file, int32 line, bool printToScreen = false, uint64 key = INDEX_NONE);
    static FString who(UObject* WorldContextObject);

};


/**
* Logging Macros
*****************
*
* These macros were specifically made to improve quality of life for anybody writing or viewing debug log lines for this project.
* Features:
*   -Colored console output
*   -Printing to Screen
*   -Logging class::function[line]
*   -Auto widening of literal strings
*   -Compile time disabling of logs
* 
* todo list:
*   1) Add all verbosity levels
*   2) Add feature for only printing logs to file
*/

#define INVOCATIONLINE_STATIC ("(" + FString(__FUNCTION__) + ":" + FString::FromInt(__LINE__) + ") " )
#define INVOCATIONLINE ("[" + Debug::GetDebugName(this) + "](" + FString(__FUNCTION__) + ":" + FString::FromInt(__LINE__) + ") " )
#define LOGLINETEXT_STATIC(Format,...) (INVOCATIONLINE_STATIC + Debug::sprintf( L##Format, ##__VA_ARGS__ ))
#define LOGLINETEXT(Format,...) (INVOCATIONLINE + Debug::sprintf( L##Format, ##__VA_ARGS__ ))


/**
* IndraNet BASE Logging Macro
*/
#define IN_STATIC_LOG(CategoryName,Verbosity,PrintToScreen,Format,...) \
{ \
    static_assert((ELogVerbosity::Verbosity & ELogVerbosity::VerbosityMask) < ELogVerbosity::NumVerbosity && ELogVerbosity::Verbosity > 0, "Invalid verbosity provided."); \
    if(ELogVerbosity::Verbosity <= FLogCategory##CategoryName::CompileTimeVerbosity){ \
        Debug::log(CategoryName.GetCategoryName(),LOGLINETEXT_STATIC( Format, ##__VA_ARGS__ ),ELogVerbosity::Type::Verbosity,__FILE__,__LINE__,PrintToScreen); \
    } \
}
/**
* IndraNet BASE Logging Macro
*/
#define IN_LOG(CategoryName,Verbosity,PrintToScreen,Format,...) \
{ \
    static_assert((ELogVerbosity::Verbosity & ELogVerbosity::VerbosityMask) < ELogVerbosity::NumVerbosity && ELogVerbosity::Verbosity > 0, "Invalid verbosity provided."); \
    if(ELogVerbosity::Verbosity <= FLogCategory##CategoryName::CompileTimeVerbosity){ \
        Debug::log(CategoryName.GetCategoryName(),LOGLINETEXT( Format, ##__VA_ARGS__ ),ELogVerbosity::Type::Verbosity,__FILE__,__LINE__,PrintToScreen); \
    } \
}
/**
* IndraNet BASE Colored Logging Macro
*/
#define IN_LOGC(CategoryName,Verbosity,Color,PrintToScreen,Format,...) \
{ \
    static_assert((ELogVerbosity::Verbosity & ELogVerbosity::VerbosityMask) < ELogVerbosity::NumVerbosity && ELogVerbosity::Verbosity > 0, "Invalid verbosity provided."); \
    if(ELogVerbosity::Verbosity <= FLogCategory##CategoryName::CompileTimeVerbosity){ \
        Debug::log(CategoryName.GetCategoryName(),LOGLINETEXT( Format, ##__VA_ARGS__ ),Color,ELogVerbosity::Type::Verbosity,__FILE__,__LINE__,PrintToScreen); \
    } \
}

/**
* IndraNet BASE FString'ified Logging Macro
*/
#define IN_LOGF(CategoryName,Verbosity,PrintToScreen,msg) \
{ \
    static_assert((ELogVerbosity::Verbosity & ELogVerbosity::VerbosityMask) < ELogVerbosity::NumVerbosity && ELogVerbosity::Verbosity > 0, "Invalid verbosity provided."); \
    if(ELogVerbosity::Verbosity <= FLogCategory##CategoryName::CompileTimeVerbosity){ \
        Debug::log(CategoryName.GetCategoryName(),msg,ELogVerbosity::Type::Verbosity,__FILE__,__LINE__,PrintToScreen); \
    } \
}

#define logInfo(CategoryName,Format,...) IN_LOG(CategoryName,Info,false,Format,##__VA_ARGS__);
#define logError(CategoryName,Format,...) IN_LOG(CategoryName,Error,true,Format,##__VA_ARGS__);
#define logFatal(CategoryName,Format,...) IN_LOG(CategoryName,Fatal,true,Format,##__VA_ARGS__);
#define logWarning(CategoryName,Format,...) IN_LOG(CategoryName,Warning,true,Format,##__VA_ARGS__);
#define logVerbose(CategoryName,Format,...) IN_LOG(CategoryName,Verbose,false,Format,##__VA_ARGS__);
#define logVeryVerbose(CategoryName,Format,...) IN_LOG(CategoryName,VeryVerbose,false,Format,##__VA_ARGS__);

#define logInfoC(CategoryName,Color,PrintToScreen,Format,...) IN_LOGC(CategoryName,Info,Color,PrintToScreen,Format,##__VA_ARGS__);
#define logErrorC(CategoryName,Color,PrintToScreen,Format,...) IN_LOGC(CategoryName,Error,Color,PrintToScreen,Format,##__VA_ARGS__);
#define logFatalC(CategoryName,Color,PrintToScreen,Format,...) IN_LOGC(CategoryName,Fatal,Color,PrintToScreen,Format,##__VA_ARGS__);
#define logWarningC(CategoryName,Color,PrintToScreen,Format,...) IN_LOGC(CategoryName,Warning,Color,PrintToScreen,Format,##__VA_ARGS__);
#define logVerboseC(CategoryName,Color,PrintToScreen,Format,...) IN_LOGC(CategoryName,Verbose,Color,PrintToScreen,Format,##__VA_ARGS__);
#define logVeryVerboseC(CategoryName,Color,PrintToScreen,Format,...) IN_LOGC(CategoryName,VeryVerbose,Color,PrintToScreen,Format,##__VA_ARGS__);

#define logInfoF(CategoryName,msg) IN_LOGF(CategoryName,Info,false,msg);
#define logErrorF(CategoryName,msg) IN_LOGF(CategoryName,Error,true,msg);
#define logFatalF(CategoryName,msg) IN_LOGF(CategoryName,Fatal,true,msg);
#define logWarningF(CategoryName,msg) IN_LOGF(CategoryName,Warning,true,msg);
#define logVerboseF(CategoryName,msg) IN_LOGF(CategoryName,Verbose,false,msg);
#define logVeryVerboseF(CategoryName,msg) IN_LOGF(CategoryName,VeryVerbose,false,msg);

#define LOGINITLEVEL(level) IN_LOGC(IndraInit,level,DColor::Green,false," ");
#define LOGINIT() LOGINITLEVEL(Info);
#define LOGEVENTINFO(level,Format,...) IN_LOG(IndraEvents,level,false,Format,##__VA_ARGS__);
#define LOGEVENT(level) LOGEVENTINFO(level,"");
#define LOGCALL(log) logVerboseC(log,DColor::Blue,false," ");

//placeholders - defined in tudebug.h (don't include that header to disable these log lines)
#define debugEvent(flag,level,Format,...)
#define debugInfo(flag,CategoryName,Format,...)
#define debugError(flag,CategoryName,Format,...)
#define debugFatal(flag,CategoryName,Format,...)
#define debugWarning(flag,CategoryName,Format,...)
#define debugVerbose(flag,CategoryName,Format,...)
#define debugVeryVerbose(flag,CategoryName,Format,...)

#define debugInfoC(flag,CategoryName,Color,Format,...)
#define debugVerboseC(flag,CategoryName,Color,Format,...)
#define debugVeryVerboseC(flag,CategoryName,Color,Format,...)

/** logLevelMsg
* Sometimes you really just need to log a ToString(), and when you do you're gonna need these.
* Unless of course I can figure out how to get the above macros to accept FStrings as part of the sprintf
* Which I did.. gonna keep this for posterity's sake. Despite having deleted the definitions themselves
*/
/*
#define logInfoMsg(CategoryName,Msg) 
#define logErrorMsg(CategoryName,Msg) 
#define logFatalMsg(CategoryName,Msg) 
#define logWarningMsg(CategoryName,Msg) 
#define logVerboseMsg(CategoryName,Msg) 

#define logInfoMsgC(CategoryName,Color,PrintToScreen,Msg) 
#define logErrorMsgC(CategoryName,Color,PrintToScreen,Msg) 
#define logFatalMsgC(CategoryName,Color,PrintToScreen,Msg) 
#define logWarningMsgC(CategoryName,Color,PrintToScreen,Msg) 
*/