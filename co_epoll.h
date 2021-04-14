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

#ifndef __CO_EPOLL_H__
#define __CO_EPOLL_H__
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <time.h>

#if !defined( __APPLE__ ) && !defined( __FreeBSD__ )
// for Unix
#include <sys/epoll.h>

struct co_epoll_res
{
	int size;                       // 激活的事件数量
	struct epoll_event *events;     // 事件数组，包含1.事件类型；2.数据(事件描述符，数据指针，4字节和8字节的两个字段)
	struct kevent *eventlist;       // 这个可能是用来适配不同操作系统的，是不是可以用union结构体减少内存占用
};
// 封装epoll_wait，events实际传入的参数是events->events
int 	co_epoll_wait( int epfd,struct co_epoll_res *events,int maxevents,int timeout );
// 注册epoll事件，epoll_event结构体包含1.事件类型2.和epoll_data
int 	co_epoll_ctl( int epfd,int op,int fd,struct epoll_event * );
int 	co_epoll_create( int size );
// epoll_result -->
//                 size: n
//                 event[0]
//                 event[1]
//                 ...
struct 	co_epoll_res *co_epoll_res_alloc( int n );
void 	co_epoll_res_free( struct co_epoll_res * );

#else
// for Mac
#include <sys/event.h>
// 适配iOS的epoll
enum EPOLL_EVENTS
{
	EPOLLIN = 0X001,
	EPOLLPRI = 0X002,
	EPOLLOUT = 0X004,

	EPOLLERR = 0X008,
	EPOLLHUP = 0X010,

    EPOLLRDNORM = 0x40,
    EPOLLWRNORM = 0x004,
};
#define EPOLL_CTL_ADD 1
#define EPOLL_CTL_DEL 2
#define EPOLL_CTL_MOD 3
typedef union epoll_data
{
	void *ptr;
	int fd;
	uint32_t u32;
	uint64_t u64;

} epoll_data_t;

struct epoll_event
{
	uint32_t events;
	epoll_data_t data;
};

struct co_epoll_res
{
	int size;
	struct epoll_event *events;
	struct kevent *eventlist;
};
int 	co_epoll_wait( int epfd,struct co_epoll_res *events,int maxevents,int timeout );
int 	co_epoll_ctl( int epfd,int op,int fd,struct epoll_event * );
int 	co_epoll_create( int size );
struct 	co_epoll_res *co_epoll_res_alloc( int n );
void 	co_epoll_res_free( struct co_epoll_res * );

#endif
#endif


