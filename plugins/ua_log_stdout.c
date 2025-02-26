//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (C) Copyright 2021 ABB. All rights reserved.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! @file    ua_log_stdout.c
//! @brief   STDOUT wrapping for open62541
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <open62541/plugin/log_stdout.h>
#include <open62541/types.h>

#include <oul.log.h>

#include <stdio.h>

static const char* apcCategoryNames[] =
{
    "open62541:network",
    "open62541:channel",
    "open62541:session",
    "open62541:server",
    "open62541:client",
    "open62541:userland",
    "open62541:securitypolicy",
    "open62541:eventloop",
    "open62541:pubsub",
    "open62541:discovery"
};

//----------------------------------------------------------------------------------------------------------------------------
//! @internal
static void UA_Log_Stdout_log(void* pContext, UA_LogLevel eLevel, UA_LogCategory eCategory, const char* pcMessage, va_list tArgs)
{
    UA_LogLevel eMinLevel = (UA_LogLevel) pContext;
    uint32_t u32Category = 1 << eCategory;
    if(eLevel >= eMinLevel)
    {
        uint32_t u32Severity = OUL_LOG_S_DEBUG;

        switch(eLevel)
        {
            case UA_LOGLEVEL_TRACE:
            case UA_LOGLEVEL_DEBUG:
            case UA_LOGLEVEL_INFO:
                u32Severity = OUL_LOG_S_DEBUG;
                break;
            case UA_LOGLEVEL_WARNING:
                u32Severity = OUL_LOG_S_WARNING;
                break;
            case UA_LOGLEVEL_ERROR:
            case UA_LOGLEVEL_FATAL:
                u32Severity = OUL_LOG_S_ERROR;
                break;
        }

        if(eCategory >= sizeof(apcCategoryNames) / sizeof(apcCategoryNames[0]))
        {
            OUL_LogFunction(OUL_LOG_S_ERROR, "UA", "Unknown", "Unknown category %d", u32Category);
        }
        else if (u32Category & UA_LOGFILTER)
        {
            OUL_LogVFunction(u32Severity, "UA", apcCategoryNames[eCategory], pcMessage, tArgs);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------
//! @internal
static void UA_Log_Stdout_clear(UA_Logger* ptLogger)
{
    UA_free(ptLogger);
}

const UA_Logger UA_Log_Stdout_ = {UA_Log_Stdout_log, NULL, UA_Log_Stdout_clear};
const UA_Logger *UA_Log_Stdout = &UA_Log_Stdout_;

//----------------------------------------------------------------------------------------------------------------------------
//! @internal
UA_Logger UA_Log_Stdout_withLevel(UA_LogLevel eMinLevel)
{
    UA_Logger tLogger = {UA_Log_Stdout_log, (void*)eMinLevel, UA_Log_Stdout_clear};
    return tLogger;
}

//----------------------------------------------------------------------------------------------------------------------------
//! @internal
UA_Logger* UA_Log_Stdout_new(UA_LogLevel eMinlevel)
{
    UA_Logger *ptLogger = (UA_Logger*)UA_malloc(sizeof(UA_Logger));
    if(NULL != ptLogger)
    {
        *ptLogger = UA_Log_Stdout_withLevel(eMinlevel);
        ptLogger->clear = UA_Log_Stdout_clear;
    }
    return ptLogger;
}
