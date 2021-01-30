/*
* Tencent is pleased to support the open source community by making Libco available.

* Copyright (C) 2014 THL A29 Limited, a Tencent company. All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License"); 
* you may not use this file except in compliance with the License. 
* You may obtain a copy of the License at
*
*	http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, 
* software distributed under the License is distributed on an "AS IS" BASIS, 
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
* See the License for the specific language governing permissions and 
* limitations under the License.
*/


#ifndef __CO_ROUTINE_INNER_H__

#include "co_routine.h"
#include "coctx.h"
struct stCoRoutineEnv_t;
struct stCoSpec_t
{
	void *value;
};

struct stStackMem_t
{
	stCoRoutine_t* occupy_co;       // 当前占用共享栈的协程对象指针
	int stack_size;                 // 
	char* stack_bp; //stack_buffer + stack_size
	char* stack_buffer;             // 共享栈起始地址

};

// 共享栈不是唯一的，有count个栈，每个栈大小为stack_size，alloc_idx是当前可用栈的索引
struct stShareStack_t
{
	unsigned int alloc_idx;         // 共享栈共享者计数？
	int stack_size;                 // 申请的共享栈大小
	int count;                      // 共享栈数量
	stStackMem_t** stack_array;     // 共享栈地址
};


// 协程控制块
struct stCoRoutine_t
{
	stCoRoutineEnv_t *env;  // 协程运行环境
	pfn_co_routine_t pfn;   // 协程执行的函数入口
	void *arg;              // 函数参数？
	coctx_t ctx;            // 协程上下文

	char cStart;            // 
	char cEnd;              //
	char cIsMain;           // 是否主线程标志?
	char cEnableSysHook;    // 是否开启hook
	char cIsShareStack;     // 是否启用共享栈

	void *pvEnv;            // ?

	//char sRunStack[ 1024 * 128 ];
	stStackMem_t* stack_mem; // 指向当前协程使用的共享栈


	//save satck buffer while confilct on same stack_buffer;
	char* stack_sp; 
	unsigned int save_size;
	char* save_buffer;

	stCoSpec_t aSpec[1024];

};



//1.env
// 初始化 gCoEnvPerThread 全局变量，每个线程一个实例
void 			co_init_curr_thread_env();
stCoRoutineEnv_t *	co_get_curr_thread_env();

//2.coroutine
// 释放协程占用内存，主要考虑释放共享栈
void    co_free( stCoRoutine_t * co );
// 将当前线程中执行的协程切换到下一个
void    co_yield_env(  stCoRoutineEnv_t *env );

//3.func



//-----------------------------------------------------------------------------------------------
// stCoRoutineEnv_t: env 每个线程一个实例  <----|
//                                              |
// stCoRoutineEnv_t --> stCoRoutine_t[128] --> env* 
//                                         --> pfn
//                                         --> arg
//                                         --> coctx_t*    --> *regs[] 寄存器
//                                                         --> ss_size && *ss_sp 栈顶和栈大小
//                                         --> some flag
//                                         --> pvEnv* ?
//                                         --> stack_mem 协程栈地址
//                                         --> stack_sp*
//                                         --> save_buffer*
//                                         --> save_size
//                                         --> stCoSpec_t[]
//                  --> callstacksize
//                  --> pendingCo
//                  --> occupyCo
//                  --> stCoEpoll_t*
                        // stCoEpoll_t: ctx
                        // stCoEpoll_t-->  epollfd
                        //            -->  epollsize
                        //            -->  timeoutlist -->
                        //            -->  activelist  -->
                        //            -->  epoll_result-->
                        //            -->  stTimeout_t --> stTimeoutItemLink_t[]  --> head(stTimeoutItem_t)
                        //                             --> size          |        --> tail(stTimeoutItem_t) --> maxtimeout
                        //                                               |                                  --> pre
                        //                             --> start         |                                  --> next
                        //                             --> startIdx      |----------------------<-------------> link
                        //                                                                                  --> 过期时间expiretime
                        //                                                                                  --> pfn即将处理
                        //                                                                                  --> pfn立即处理
                        //                                                                                  --> pfn的参数
                        //                                                                                  --> 释放超时bTimeout
struct stTimeout_t;
struct stTimeoutItem_t ;

stTimeout_t *AllocTimeout( int iSize );
void 	FreeTimeout( stTimeout_t *apTimeout );
// 将协程添加到时间轮盘中，依据是apItem设置的超时时间与轮盘启动时间的差值
// 根据差值diff将协程插入到对应轮盘链表（stTimeoutItemLink_t）的尾部
int  	AddTimeout( stTimeout_t *apTimeout,stTimeoutItem_t *apItem ,uint64_t allNow );

struct          stCoEpoll_t;
stCoEpoll_t *   AllocEpoll();
void            FreeEpoll( stCoEpoll_t *ctx );

stCoRoutine_t *		GetCurrThreadCo();
void 			SetEpoll( stCoRoutineEnv_t *env,stCoEpoll_t *ev );

typedef void (*pfnCoRoutineFunc_t)();

#endif

#define __CO_ROUTINE_INNER_H__
