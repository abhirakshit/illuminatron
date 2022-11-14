// Fill out your copyright notice in the Description page of Project Settings.

#include "IndraDesign.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, IndraDesign, "IndraDesign" );

//General Log
DEFINE_LOG_CATEGORY(Indra);
 
//Logging during game startup
DEFINE_LOG_CATEGORY(IndraInit);
 
//Logging for some system
DEFINE_LOG_CATEGORY(IndraGeometry);
DEFINE_LOG_CATEGORY(IndraMaterials);
DEFINE_LOG_CATEGORY(IndraLasers);
DEFINE_LOG_CATEGORY(IndraEvents);
DEFINE_LOG_CATEGORY(DebugEvents);
 
//Logging for Critical Errors that must always be addressed
DEFINE_LOG_CATEGORY(IndraCriticalErrors);