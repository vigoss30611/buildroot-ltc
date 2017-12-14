/**
*******************************************************************************
@file PerfTime.cpp

@brief Performance of functions. Implementation

@copyright Imagination Technologies Ltd. All Rights Reserved.

@license Strictly Confidential.
No part of this software, either material or conceptual may be copied or
distributed, transmitted, transcribed, stored in a retrieval system or
translated into any human or computer language in any form by any means,
electronic, mechanical, manual or other-wise, or disclosed to third
parties without the express written permission of
Imagination Technologies Limited,
Unit 8, HomePark Industrial Estate,
King's Langley, Hertfordshire,
WD4 8LZ, U.K.

******************************************************************************/
#include "ispc/PerfTime.h"

#include <img_defs.h>  // IMG_I64PR
#include <felixcommon/userlog.h>

#include <stack>
#include <vector>
#include <cstring>
#include <cstdarg>

#define LOG_TAG "PERF_time"

#define MAX_DEPTH (20)

#define US_IN_S (1000000LL)  // 10^6
#ifndef WIN32
#include <stdint.h>
#include <time.h>
#include <sys/time.h>  // gettimeofday
#define CLOCKS_PER_USEC (CLOCKS_PER_SEC/US_IN_S)
#else
#include <stdint.h>
#include <Windows.h>
#define FILETIME_EPOCH (116444736000000000i64)
#define FILETIME_TO_S_DIV (10000000)  // 10^(9-2) ns because of interval
#define FILETIME_TO_US_MULT 10
#define FILETIME_TO_US_MOD (1000000)  // 10^6 us in a second
#endif

#ifdef ISPC_HAS_LOGPERF
static void getTime_us(int64_t *tusec)
{
#ifdef WIN32
    FILETIME t;
    ULARGE_INTEGER d;
    uint64_t v;
    uint64_t tsec;

    GetSystemTimeAsFileTime(&t);

    d.LowPart = t.dwLowDateTime;
    d.HighPart = t.dwHighDateTime;

    v = d.QuadPart;  // in 100 ns interval
    v -= FILETIME_EPOCH;

    tsec = (int64_t)(v / FILETIME_TO_S_DIV);
    *tusec = (int64_t)((v * FILETIME_TO_US_MULT) % FILETIME_TO_US_MOD);
    *tusec += tsec * US_IN_S;
#else
#if 0
    struct timeval t;
    gettimeofday(&t, NULL);
    *tusec = t.tv_usec + t.tv_sec * US_IN_S;
#else
    *tusec = clock()/CLOCKS_PER_USEC;
#endif
#endif
}

typedef struct perf_time_node_tag {
    std::string loggingName;
    std::string file_name;
    std::string function_name;
    int64_t time_in;
    int64_t time_out;
    int64_t time_average;
    int hit_count;
} perf_time_node_t, *p_perf_time_node_t;

typedef struct perfTime_tag {
    std::string loggingName;
    bool verbose;
} perfTime_t, *p_perfTime_t;

static std::stack<perf_time_node_t> perf_time_stack;
static std::vector<perf_time_node_t> perf_hit_list;
static std::list<perfTime_t> perfTime;
#endif /* ISPC_HAS_LOGPERF */

bool ISPC::LOG_Perf_Available(void)
{
#ifdef ISPC_HAS_LOGPERF
    return true;
#else
    return false;
#endif
}

int nameRegistered(const std::string &name, bool *verbose) {
#if ISPC_HAS_LOGPERF
    //    LOG_PERF("searching for name %s\n", name.c_str());
    *verbose = false;
    for (std::list<perfTime_t>::iterator iter = perfTime.begin();
        iter != perfTime.end(); ++iter) {
        // LOG_DEBUG("name on the list %s\n", iter->c_str());
        if (iter->loggingName == name) {
            // LOG_DEBUG("name %s found on the list \n", iter->c_str());
            *verbose = iter->verbose;
            return 1;
        }
    }
    //    LOG_PERF("name %s NOT found \n", name.c_str());
#endif /* ISPC_HAS_LOGPERF */
    return 0;
}

void ISPC::LOG_Perf_Register(const char *loggingName, bool verbose) {
#ifdef ISPC_HAS_LOGPERF
    // LOG_DEBUG("adding name %s\n", module->getLoggingName());
    perfTime_t perfTimeNode = { loggingName, verbose };
    perfTime.push_back(perfTimeNode);
#endif /* ISPC_HAS_LOGPERF */
}

void ISPC::LOG_Perf_Register(const ISPC::ModuleBase* module, bool verbose) {
#ifdef ISPC_HAS_LOGPERF
    // LOG_DEBUG("adding name %s\n", module->getLoggingName());
    perfTime_t perfTimeNode = { module->getLoggingName(), verbose };
    perfTime.push_back(perfTimeNode);
#endif /* ISPC_HAS_LOGPERF */
}

void ISPC::LOG_Perf_In(const char *loggingName, const char *file,
    const char *function, unsigned int line)
{
#ifdef ISPC_HAS_LOGPERF
    bool verbose;
    // LOG_DEBUG("Logging for name %s from file %s function %s \n",
    // loggingName, file, function);
    if (!nameRegistered(loggingName, &verbose)) return;

    // update hit list
    perf_time_node_t time_node;
    getTime_us(&time_node.time_in);
    time_node.file_name.assign(file);
    time_node.function_name.assign(function);
    time_node.loggingName.assign(loggingName);

    std::vector<perf_time_node_t>::iterator iter = perf_hit_list.begin();
    // check if present
    while (iter != perf_hit_list.end()) {
        if (time_node.file_name == iter->file_name      &&
            time_node.function_name == iter->function_name)
        {
            iter->time_in = time_node.time_in;
            iter->time_out = 0;
            iter->hit_count++;
            break;
        }
        iter++;
    }

    if (iter == perf_hit_list.end()) {
        // add to hit list
        time_node.hit_count = 1;
        time_node.time_average = 0;
        perf_hit_list.push_back(time_node);
        iter = perf_hit_list.end();
        iter--;
    } else {
        time_node.hit_count = iter->hit_count;
        time_node.time_in = iter->time_in;
        time_node.time_average = iter->time_average;
        time_node.hit_count = iter->hit_count;
    }
    time_node.time_out = iter->time_out;

    // log current time
    if (verbose) {
        const char *fct_name = time_node.function_name.c_str();
        LOG_Perf(fct_name, line, loggingName, "IN (%d)\n",
            time_node.hit_count);
    }

    // put time on the perf stack
    perf_time_stack.push(time_node);
#endif /* ISPC_HAS_LOGPERF */
}

void ISPC::LOG_Perf_Out(const char *loggingName, const char *file,
    const char *function, unsigned int line)
{
#ifdef ISPC_HAS_LOGPERF
    bool verbose;
    // LOG_DEBUG("Logging for name %s from file %s function %s \n",
    // loggingName, file, function);
    if (!nameRegistered(loggingName, &verbose)) return;

    perf_time_node_t stack_top;
    perf_time_node_t time_node;
    getTime_us(&time_node.time_out);
    time_node.file_name.assign(file);
    time_node.function_name.assign(function);

    // LOG_DEBUG(stack_top.function_name.c_str(), line, log_tag,
    // "current time sec = %ld, usec = %ld \n",
    // time_node.time_in.tv_sec,
    // time_node.time_in.tv_usec);

    std::vector<perf_time_node_t>::iterator iter = perf_hit_list.begin();
    // check if present
    while (iter != perf_hit_list.end()) {
        if (time_node.file_name == iter->file_name
            && time_node.function_name == iter->function_name)
        {
            int64_t d;
            iter->time_out = time_node.time_out;
            time_node.time_in = iter->time_in;
            d = (iter->time_out - iter->time_in);

            iter->time_average =
                ((iter->hit_count - 1)*iter->time_average + d)
                / iter->hit_count;
            time_node.time_average = iter->time_average;
            time_node.hit_count = iter->hit_count;
            break;
        }
        iter++;
    }

    while (!perf_time_stack.empty()) {
        stack_top = perf_time_stack.top();
        if (time_node.function_name == stack_top.function_name
            && time_node.file_name == stack_top.file_name)
        {
            if (verbose) {
                const char *fct_name = stack_top.function_name.c_str();
                // may not work properly for recursive functions
                LOG_Perf(fct_name, line, loggingName, "OUT (%d)\n",
                    stack_top.hit_count);
            }
            perf_time_stack.pop();
            break;
        }
        else {
            if (verbose) {
                // LOG_DEBUG(loggingName, 0, time_node.function_name.c_str(),
                // "Duration not logged \n");
            }
            perf_time_stack.pop();
        }
    }
#endif /* ISPC_HAS_LOGPERF */
}

void ISPC::LOG_Perf_Summary() {
#ifdef ISPC_HAS_LOGPERF
    perf_time_node_t time_node;
    std::vector<perf_time_node_t>::iterator iter = perf_hit_list.begin();
    if (iter == perf_hit_list.end())
    {
        // no elemets - return without printing perf summary header
        return;
    }
    LOG_Perf("", 0, "", "************** PERF SUMMARY ************\n");
    // check if present
    while (iter != perf_hit_list.end()) {
        LOG_Perf(iter->function_name.c_str(), 0, "",
            "%s: last duration = %"IMG_I64PR"d [us] ; "\
            "average duration = %"IMG_I64PR"d [us] ; hit count = %d\n",
            iter->loggingName.c_str(),
            iter->time_out - iter->time_in,
            iter->time_average,
            iter->hit_count);
        iter++;
    }
#endif /* ISPC_HAS_LOGPERF */
}
