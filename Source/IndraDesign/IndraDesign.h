// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "icore.h"

/**
* Here we create our logs for Indra's Net
* Each log comes with:
* Name                       - Identifier, creates a class by the same name
* Default Verbosity          - Not sure to be honest, the debug.h macros certainly don't allow for defaults
* Max Compile Time Verbosity - Just like it sounds. Anything with a higher value than the one provided here will be discarded
*/

//General Log
DECLARE_LOG_CATEGORY_EXTERN(Indra, Display, All);
 
//Logging during game startup
DECLARE_LOG_CATEGORY_EXTERN(IndraInit, Display, Log);

//Logging for a that troublesome system
DECLARE_LOG_CATEGORY_EXTERN(IndraGeometry, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(IndraMaterials, Display, All);
DECLARE_LOG_CATEGORY_EXTERN(IndraLasers, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(IndraEvents, Display, Log);
DECLARE_LOG_CATEGORY_EXTERN(DebugEvents, Display, All);
 
//Logging for Critical Errors that must always be addressed
DECLARE_LOG_CATEGORY_EXTERN(IndraCriticalErrors, Display, All);

