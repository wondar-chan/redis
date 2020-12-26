/* Hash Tables Implementation.
 *
 * This file implements in-memory hash tables with insert/del/replace/find/
 * get-random-element operations. Hash tables will auto-resize if needed
 * tables of power of two in size are used, collisions are handled by
 * chaining. See the source code for more information... :)
 *
 * Copyright (c) 2006-2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __DICT_H
#define __DICT_H

#include "mt19937-64.h"
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

#define DICT_OK 0
#define DICT_ERR 1

/* Unused arguments generate annoying warnings... */
#define DICT_NOTUSED(V) ((void) V)

/*
 * hash表中的实体，保存KV信息  
 */ 
typedef struct dictEntry {
    void *key;
    union {   // dictEntry在不同用途时存储不同的数据 
        void *val;
        uint64_t u64;
        int64_t s64;
        double d;
    } v;
    struct dictEntry *next; // hash冲突时开链，单链表的next指针 
} dictEntry;

typedef struct dictType {
    uint64_t (*hashFunction)(const void *key);  // 对key生成hash值 
    void *(*keyDup)(void *privdata, const void *key); // 对key进行拷贝 
    void *(*valDup)(void *privdata, const void *obj);  // 对val进行拷贝
    int (*keyCompare)(void *privdata, const void *key1, const void *key2); // 两个key的对比函数
    void (*keyDestructor)(void *privdata, void *key); // key的销毁
    void (*valDestructor)(void *privdata, void *obj); // val的销毁
    int (*expandAllowed)(size_t moreMem, double usedRatio); 
} dictType;

/*  保存每个hashtable的数据信息，当前大小 hash掩码 使用量 */
typedef struct dictht {
    dictEntry **table;  // hashtable中的连续空间 
    unsigned long size; // table的大小 
    unsigned long sizemask;  // hashcode的掩码  
    unsigned long used; // 已存储的数据个数
} dictht;

typedef struct dict {
    dictType *type;  // dictType结构的指针，封装了很多数据操作的函数指针，使得dict能处理任意数据类型（类似面向对象语言的interface，可以重载其方法）
    void *privdata;  // 一个私有数据指针(privdata),由调用者在创建dict的时候传进来。
    dictht ht[2];  // 两个hashtable，ht[0]为主，ht[1]在渐进式hash的过程中才会用到。  
    long rehashidx; /* 增量hash过程过程中记录rehash执行到第几个bucket了，当rehashidx == -1表示没有在做rehash */
    unsigned long iterators; /* 正在运行的迭代器数量 */
} dict;

/*  如果safe为1，说明他是一个安全的迭代器，可以调用dictAdd、dictFind或者其他dict函数。
 * 否则，说明当前迭代器是非安全的，只能调用dictNext()方法 */
typedef struct dictIterator { 
    dict *d;
    long index;
    int table, safe;
    dictEntry *entry, *nextEntry;
    /* 不安全迭代器的指纹，用于误用检测 */
    long long fingerprint;
} dictIterator;

typedef void (dictScanFunction)(void *privdata, const dictEntry *de);
typedef void (dictScanBucketFunction)(void *privdata, dictEntry **bucketref);

/* This is the initial size of every hash table */
#define DICT_HT_INITIAL_SIZE     4

/* ------------------------------- Macros ------------------------------------*/
#define dictFreeVal(d, entry) \
    if ((d)->type->valDestructor) \
        (d)->type->valDestructor((d)->privdata, (entry)->v.val)

#define dictSetVal(d, entry, _val_) do { \
    if ((d)->type->valDup) \
        (entry)->v.val = (d)->type->valDup((d)->privdata, _val_); \
    else \
        (entry)->v.val = (_val_); \
} while(0)

#define dictSetSignedIntegerVal(entry, _val_) \
    do { (entry)->v.s64 = _val_; } while(0)

#define dictSetUnsignedIntegerVal(entry, _val_) \
    do { (entry)->v.u64 = _val_; } while(0)

#define dictSetDoubleVal(entry, _val_) \
    do { (entry)->v.d = _val_; } while(0)

#define dictFreeKey(d, entry) \
    if ((d)->type->keyDestructor) \
        (d)->type->keyDestructor((d)->privdata, (entry)->key)

#define dictSetKey(d, entry, _key_) do { \
    if ((d)->type->keyDup) \
        (entry)->key = (d)->type->keyDup((d)->privdata, _key_); \
    else \
        (entry)->key = (_key_); \
} while(0)

#define dictCompareKeys(d, key1, key2) \
    (((d)->type->keyCompare) ? \
        (d)->type->keyCompare((d)->privdata, key1, key2) : \
        (key1) == (key2))

#define dictHashKey(d, key) (d)->type->hashFunction(key)
#define dictGetKey(he) ((he)->key)
#define dictGetVal(he) ((he)->v.val)
#define dictGetSignedIntegerVal(he) ((he)->v.s64)
#define dictGetUnsignedIntegerVal(he) ((he)->v.u64)
#define dictGetDoubleVal(he) ((he)->v.d)
#define dictSlots(d) ((d)->ht[0].size+(d)->ht[1].size)
#define dictSize(d) ((d)->ht[0].used+(d)->ht[1].used)
#define dictIsRehashing(d) ((d)->rehashidx != -1)

/* If our unsigned long type can store a 64 bit number, use a 64 bit PRNG. */
#if ULONG_MAX >= 0xffffffffffffffff
#define randomULong() ((unsigned long) genrand64_int64())
#else
#define randomULong() random()
#endif
/* dict所有的API */
dict *dictCreate(dictType *type, void *privDataPtr);  // 创建dict 
int dictExpand(dict *d, unsigned long size);  // 扩缩容
int dictTryExpand(dict *d, unsigned long size);
int dictAdd(dict *d, void *key, void *val);  // 添加k-v
dictEntry *dictAddRaw(dict *d, void *key, dictEntry **existing); // 添加的key对应的dictEntry 
dictEntry *dictAddOrFind(dict *d, void *key); // 添加或者查找 
int dictReplace(dict *d, void *key, void *val); // 替换key对应的value，如果没有就添加新的k-v
int dictDelete(dict *d, const void *key);  // 删除某个key对应的数据 
dictEntry *dictUnlink(dict *ht, const void *key); // 卸载某个key对应的entry 
void dictFreeUnlinkedEntry(dict *d, dictEntry *he); // 卸载并清除key对应的entry
void dictRelease(dict *d);  // 释放整个dict 
dictEntry * dictFind(dict *d, const void *key);  // 数据查找
void *dictFetchValue(dict *d, const void *key);  // 获取key对应的value
int dictResize(dict *d);  // 重设dict的大小，主要是缩容用的
/************    迭代器相关     *********** */
dictIterator *dictGetIterator(dict *d);  
dictIterator *dictGetSafeIterator(dict *d);
dictEntry *dictNext(dictIterator *iter);
void dictReleaseIterator(dictIterator *iter);
/************    迭代器相关     *********** */
dictEntry *dictGetRandomKey(dict *d);  // 随机返回一个entry 
dictEntry *dictGetFairRandomKey(dict *d);   // 随机返回一个entry，但返回每个entry的概率会更均匀 
unsigned int dictGetSomeKeys(dict *d, dictEntry **des, unsigned int count); // 获取dict中的部分数据 
void dictGetStats(char *buf, size_t bufsize, dict *d);  
uint64_t dictGenHashFunction(const void *key, int len);
uint64_t dictGenCaseHashFunction(const unsigned char *buf, int len);
void dictEmpty(dict *d, void(callback)(void*));
void dictEnableResize(void);
void dictDisableResize(void);
int dictRehash(dict *d, int n);
int dictRehashMilliseconds(dict *d, int ms);
void dictSetHashFunctionSeed(uint8_t *seed);
uint8_t *dictGetHashFunctionSeed(void);
unsigned long dictScan(dict *d, unsigned long v, dictScanFunction *fn, dictScanBucketFunction *bucketfn, void *privdata);
uint64_t dictGetHash(dict *d, const void *key);
dictEntry **dictFindEntryRefByPtrAndHash(dict *d, const void *oldptr, uint64_t hash);

/* Hash table types */
extern dictType dictTypeHeapStringCopyKey;
extern dictType dictTypeHeapStrings;
extern dictType dictTypeHeapStringCopyKeyValue;

#endif /* __DICT_H */
