#pragma once

#ifdef __x86_64__
typedef unsigned long size_t;
#else
typedef unsigned int size_t;
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

