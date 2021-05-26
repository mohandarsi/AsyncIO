#pragma once

#include "spdlog/spdlog.h"

namespace AsyncFileIO
{

#define LOGGING_CRASH ((void)(*(volatile char*)0 = 0))

#define EAT_STREAM_PARAMETERS ((void) 0)

#define CHECK(condition) \
  if(!(condition)) { \
  	SPDLOG_ERROR("Failed condition check {}",condition); \
    LOGGING_CRASH; }\
   else {  EAT_STREAM_PARAMETERS; }
	
}