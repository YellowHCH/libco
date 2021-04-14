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

#ifndef __CO_CTX_H__
#define __CO_CTX_H__
#include <stdlib.h>
// 协程上下文切换
typedef void* (*coctx_pfn_t)( void* s, void* s2 );
// coctx_param_t是栈的顶部两个指针，一个是函数入口，一个应该是返回入口
struct coctx_param_t
{
	const void *s1;
	const void *s2;
};
// 函数调用栈的上下文，即寄存器和栈地址 栈大小
// 协程的上下文：
// regs寄存器
// ss_sp栈地址
/*
 *	------------
 *	|  regs[]  |
 *	|----------|
 *	|  ss_sp   |
 *	|----------|
 * */ 
struct coctx_t
{
#if defined(__i386__)
	void *regs[ 8 ];	// 32位系统的寄存器大小是8
#else
	void *regs[ 14 ];	// 64为系统的寄存器大小是14
#endif
	size_t ss_size;
	char *ss_sp;
	
};

// 初始化协程上下文
int coctx_init( coctx_t *ctx );
// coctx_t --> regs[0]
//         --> regs[1]
//         --> ...
//         --> regs[7] ---------------
//         --> ...                   |
//         --> ss_size               |
//         --> ss_sp --> --------|   |
//                       |_______|   |
//                       |_______|   |
//                       |_______|   |
//                       |_______|   |
//                       .........<--|
//                       |___s1__| 
//                       |___s2__|
// 创建一个协程上下文，参数是：协程上下文地址，执行的函数地址...
int coctx_make( coctx_t *ctx,coctx_pfn_t pfn,const void *s,const void *s1 );
#endif
