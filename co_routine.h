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

#ifndef __CO_ROUTINE_H__
#define __CO_ROUTINE_H__

#include <stdint.h>
#include <sys/poll.h>
#include <pthread.h>

//1.struct

struct stCoRoutine_t;
struct stShareStack_t;

// 协程属性，这里是共享栈的信息
struct stCoRoutineAttr_t
{
	int stack_size;
	stShareStack_t*  share_stack;
	stCoRoutineAttr_t()
	{
		stack_size = 128 * 1024;
		share_stack = NULL;
	}
}__attribute__ ((packed));

struct stCoEpoll_t;
typedef int (*pfn_co_eventloop_t)(void *);
typedef void *(*pfn_co_routine_t)( void * );

//2.co_routine
// 封装了协程的创建，执行，切换，释放等操作
/*
 * co_create
 * param:
 *  --> co:     创建的协程对象的指针
 *  --> attr:   创建的协程的属性，主要是共享栈相关
 *  --> pfn:    协程执行的函数
 *  --> arg:    协程执行函数的参数
 * exec:
 *  协程的创建包括栈内存的获取、初始化，绑定执行的函数，关联到当前线程，将协程上下文复制到栈内存，初始化一些协程属性
 * */
int 	co_create( stCoRoutine_t **co,const stCoRoutineAttr_t *attr,void *(*routine)(void*),void *arg );
/*
 * co_resume
 * param:
 *  --> co: 协程对象
 * exec:
 *  在线程栈恢复（执行）协程co，调用co_swap切换当前占用线程的协程 
 * */
void    co_resume( stCoRoutine_t *co );
/*
 * co_yield
 * 切换这个协程所属的线程正在执行的协程
 * co_yield_ct: 直接切换当前线程的协程
 * */
void    co_yield( stCoRoutine_t *co );
void    co_yield_ct(); //ct = current thread
/*
 * 调用co_free释放协程对象，如果是私有栈那么直接释放内存，如果是共享栈则释放暂存的栈内存、断开共享栈的绑定
 * */
void    co_release( stCoRoutine_t *co );
/*
 * co_reset 重置超时的协程
 * */
void    co_reset(stCoRoutine_t * co); 
// 获取当前执行的协程
// cur_env->cur_co, iCallStackSize是当前线程栈待调度协程的数量，最后一个协程是正在执行的协程
stCoRoutine_t *co_self();
// 协程调度相关
int		co_poll( stCoEpoll_t *ctx,struct pollfd fds[], nfds_t nfds, int timeout_ms );
void 	co_eventloop( stCoEpoll_t *ctx,pfn_co_eventloop_t pfn,void *arg );

//3.specific
// 协程私有变量的实现，限制128个
int 	co_setspecific( pthread_key_t key, const void *value );
void *	co_getspecific( pthread_key_t key );

//4.event

stCoEpoll_t * 	co_get_epoll_ct(); //ct = current thread

//5.hook syscall ( poll/read/write/recv/send/recvfrom/sendto )

void 	co_enable_hook_sys();  
void 	co_disable_hook_sys();  
bool 	co_is_enable_sys_hook();

//6.sync
// 协程同步机制的实现
struct stCoCond_t;

stCoCond_t *co_cond_alloc();
int co_cond_free( stCoCond_t * cc );

int co_cond_signal( stCoCond_t * );
int co_cond_broadcast( stCoCond_t * );
int co_cond_timedwait( stCoCond_t *,int timeout_ms );

//7.share stack
stShareStack_t* co_alloc_sharestack(int iCount, int iStackSize);

//8.init envlist for hook get/set env
void co_set_env_list( const char *name[],size_t cnt);

void co_log_err( const char *fmt,... );
#endif

