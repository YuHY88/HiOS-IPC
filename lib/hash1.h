/***********************************************************************
*
* hash.h
*
* Hash table utilities
*
* Copyright (C) 2002 Roaring Penguin Software Inc.
*
* LIC: GPL
*
***********************************************************************/

#ifndef HIOS_HASH_H
#define HIOS_HASH_H


/* Fixed-size hash tables for now */
#define HASHTAB_SIZE 1024


/* 创建一个 bucket 节点，分配内存 */
#define HASH_BUCKET_CREATE(bucket)\
	bucket = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));

/* 释放一个 bucket 节点的内存 */
#define HASH_BUCKET_DESTROY(bucket)\
	XFREE(MTYPE_HASH_BACKET, bucket);


/* hash 表遍历操作, 只用于读操作，不能删除节点 */
#define HASH_BUCKET_LOOP(bucket, cursor, table)\
    for (cursor = 0; cursor  < HASHTAB_SIZE; cursor++)\
        for((bucket) = table.buckets[cursor]; bucket; (bucket) = (bucket)->next)\


/* hash 表遍历操作, 可以删除节点, cursor 是桶的位置, bucket 是节点位置, 调用者自己移动 bucket */
#define HASH_BUCKET_LOOPW(bucket, cursor, table)\
    for (cursor = 0; cursor  < HASHTAB_SIZE; cursor++)\
        for((bucket) = table.buckets[cursor]; bucket; )\


/* A hash bucket */
struct hash_bucket
{
	struct hash_bucket *next;
    struct hash_bucket *prev;
    unsigned int hashval;
	void *hash_key;
	void *data;
};


/* A hash table */
struct hash_table
{
    struct hash_bucket *buckets[HASHTAB_SIZE]; /* bucket size */
    unsigned int hash_size;     /* max_num of hash table */
    unsigned int num_entries;   /* real number */
    unsigned int (*compute_hash)(void *hash_key);
    int (*compare)(void *item, void *hash_key);
};


/* Functions */
void hios_hash_init(struct hash_table *tab, unsigned int hash_size,
	       unsigned int (*compute)(void *hash_key), int (*compare)(void *item1, void *hash_key));

struct hash_bucket *hios_hash_bucket_creat();/* 创建一个 bucket 节点，分配内存 */
int hios_hash_add(struct hash_table *tab, struct hash_bucket *item);
void hios_hash_delete(struct hash_table *tab, struct hash_bucket *item);
struct hash_bucket *hios_hash_find(struct hash_table *tab, void *hash_key);
struct hash_bucket *hios_hash_find_next(struct hash_table *tab, void *hash_key);
unsigned int hios_hash_num_entries(struct hash_table *tab);


/* Iteration functions */
struct hash_bucket *hios_hash_start(struct hash_table *tab, void **cursor);
struct hash_bucket *hios_hash_next(struct hash_table *tab, void **cursor);
void *hios_hash_next_cursor(struct hash_table *tab, struct hash_bucket *b);

/* Utility function: hashpjw for strings */
unsigned int hios_hash_string(char const *str);

#endif
