#ifndef _HM
#define _HM

typedef enum {
  E_FREE,
  E_FREE_AGAIN,
  E_DATA,
} hash_map_entry_type_e;

typedef struct {
  char *name;
  char *content;
} hash_map_entry_t;

typedef struct {
  hash_map_entry_type_e type;
  hash_map_entry_t *data;
} hash_map_entry_internal_t;

int hm_entry_init(char *name, char *content, hash_map_entry_t *item);

void hm_entry_free(hash_map_entry_t *item);

#define HASH_MAP_CAPACITY 13

typedef struct {
  int count;
  hash_map_entry_internal_t items[HASH_MAP_CAPACITY];
} hash_map_t;

int hm_map_init(hash_map_t *map);
void hm_map_free(hash_map_t *map);

int hm_map_add(hash_map_t *map, hash_map_entry_t item);
int hm_map_get(hash_map_t *map, const char *key, hash_map_entry_t *item);
int hm_map_remove(hash_map_t *map, const char *key);

#endif
