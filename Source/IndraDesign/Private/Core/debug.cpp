#include "debug.h"
#include "icore.h"
#include <locale>
#include <codecvt>
#include <string>

DColor DColor::Black    {FColor::Black,  COLOR_BLACK};
DColor DColor::Red      {FColor::Red,    COLOR_RED};
DColor DColor::Green    {FColor::Green,  COLOR_GREEN};
DColor DColor::Blue     {FColor::Blue,   COLOR_BLUE};
DColor DColor::Yellow   {FColor::Yellow, COLOR_YELLOW};
DColor DColor::Cyan     {FColor::Cyan,   COLOR_CYAN};
DColor DColor::Purple   {FColor::Purple, COLOR_PURPLE};
DColor DColor::Default  {FColor::Silver, COLOR_GRAY};
DColor DColor::White    {FColor::White,  COLOR_WHITE};

//[defunct todo]try to optimize this function. manually convert characters perhaps.
/*std::wstring Debug::text(const char* msg){
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wide = converter.from_bytes(msg);
    return wide;
}*/

float Duration(const ELogVerbosity::Type &level) {
    switch (level) {
    case ELogVerbosity::Fatal:
    case ELogVerbosity::Error:
        return 20.f;
    case ELogVerbosity::Log:
        return 5.f;
    case ELogVerbosity::Verbose:
        return 0.5f;
    case ELogVerbosity::VeryVerbose:
        return 0.1f;
    default:
        return 1.5f;
    }
}

void Debug::printOnScreen(const FString &msg, float duration, const DColor &color, uint64 key){
    if (GEngine){
        GEngine->AddOnScreenDebugMessage(key,duration,color.ScreenColor,msg );
    }
}
void Debug::logLine(const FName &logName, const FString &msg, const DColor &color, ELogVerbosity::Type level, FileName file, int32 line){
    SET_WARN_COLOR(color.LogColor);
    if(level == ELogVerbosity::Fatal && UIndraLib::isPIEWorld()){
        logLine(logName,"Fatal: " + msg,ELogVerbosity::Log,file,line);
        UIndraLib::QuitGame();
    } else {
        logLine(logName,msg,level,file,line);
    }
    CLEAR_WARN_COLOR();
}
void Debug::logLine(const FName &logName, const FString &msg, ELogVerbosity::Type level, FileName file, int32 line){
    FMsg::Logf_Internal(file,line,logName,level,TEXT("%s"),*msg);
}

void Debug::log(const FName &logName, const FString &msg, ELogVerbosity::Type level, FileName file, int32 line, bool printToScreen, uint64 key){
    static FName eventLog = IndraEvents.GetCategoryName();
    static FName deventLog = DebugEvents.GetCategoryName();
    bool bEventLog = logName == eventLog || logName == deventLog;
    DColor &color = logName == deventLog ? DColor::Yellow : (bEventLog ? DColor::Purple : getSeverityColor(level));
    if(printToScreen || bEventLog){
        printOnScreen(msg,Duration(level),color,key);
    }
    logLine(logName,msg,color,level,file,line);
}
void Debug::log(const FName &logName, const FString &msg, const DColor &color, ELogVerbosity::Type level, FileName file, int32 line, bool printToScreen, uint64 key){
    static FName eventLog = IndraEvents.GetCategoryName();
    static FName deventLog = DebugEvents.GetCategoryName();
    if(logName == eventLog || logName == deventLog){
        printOnScreen(msg,Duration(level),DColor::Purple,key);
        logLine(logName,msg,DColor::Purple,level,file,line);
    } else {
        if(printToScreen){
            printOnScreen(msg,Duration(level),color,key);
        }
        logLine(logName,msg,color,level,file,line);
    }
}

FString Debug::who(UObject* WorldContextObject){
    FString result;
    UWorld* World = WorldContextObject->GetWorld();
    switch(World->GetNetMode()){
		case NM_Client:
			result = FString::Printf(TEXT("Client %d: "), GPlayInEditorID - 1);
			break;
		case NM_DedicatedServer:
		case NM_ListenServer:
			result = FString::Printf(TEXT("Server: "));
			break;
		case NM_Standalone:
			break;
	}
    return result;
}