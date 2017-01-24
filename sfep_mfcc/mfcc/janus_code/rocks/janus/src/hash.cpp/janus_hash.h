#ifndef JANUS_HASH_H
#define JANUS_HASH_H

#ifdef __cplusplus
extern "C" {
#endif
  
extern void* create_long_double_hash();
extern void set_int_double_hash(void* table, long key, double value);
extern double get_int_double_hash(void* table, long key, int* isfound);
extern void clear_long_double_hash(void* table);

extern void* create_II2I_hash();
extern void set_II2I_hash(void* table, int k1, int k2, int value);
extern int get_II2I_hash(void* table, int k1, int k2, int* isfound);
extern void clear_II2I_hash(void* table);

extern void* create_III2I_hash();
extern void set_III2I_hash(void* table, int k1, int k2, int k3, int value);
extern int get_III2I_hash(void* table, int k1, int k2, int k3, int* isfound);
extern void clear_III2I_hash(void* table);

extern void* create_III2I_vector_hash();
extern void set_III2I_vector_hash(void* table, int k1, int k2, int k3, int value);
extern int get_III2I_vector_hash(void* table, int k1, int k2, int k3, int* isfound);
extern void clear_III2I_vector_hash(void* table);

#ifdef __cplusplus
}
#endif

#endif
