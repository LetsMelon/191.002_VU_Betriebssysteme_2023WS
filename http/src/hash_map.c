#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_map.h"

int hm_entry_init(char *name, char *content, hash_map_entry_t *item) {
  item->name = strdup(name);
  if (item->name == NULL) {
    return -1;
  }

  item->content = strdup(content);
  if (item->content == NULL) {
    free(item->name);

    return -1;
  }

  return 0;
}

void hm_entry_free(hash_map_entry_t *item) {
  free(item->name);
  free(item->content);
}

int hm_map_init(hash_map_t *map) {
  map->count = 0;

  for (int i = 0; i < HASH_MAP_CAPACITY; i += 1) {
    map->items[i].type = E_FREE;
    map->items[i].data = NULL;
  }

  return 0;
}

void hm_map_free(hash_map_t *map) {
  for (int i = 0; i < HASH_MAP_CAPACITY; i += 1) {
    if (map->items[i].type == E_DATA) {
      hm_entry_free(map->items[i].data);
    }

    map->items[i].type = E_FREE;
    map->items[i].data = NULL;
  }
  map->count = 0;
}

static bool hm_map_is_full(hash_map_t *map) {
  return map->count >= HASH_MAP_CAPACITY;
}

static bool hm_map_is_empty(hash_map_t *map) { return map->count == 0; }

static int hm_hash_function(const char *key, int iterations) {
  return (strlen(key) + 7 * iterations) % HASH_MAP_CAPACITY;
}

int hm_map_add(hash_map_t *map, hash_map_entry_t item) {
  if (hm_map_is_full(map) == true) {
    return -1;
  }

  hash_map_entry_t *new_entry = malloc(sizeof(hash_map_entry_t));
  if (new_entry == NULL) {
    return -1;
  }

  if (hm_entry_init(item.name, item.content, new_entry) != 0) {
    free(new_entry);

    return -1;
  }

  int iterations = 0;
  while (1) {
    int index = hm_hash_function(item.name, iterations);

    printf("index = %d, iterations = %d\n", index, iterations);

    if (map->items[index].type != E_DATA) {
      printf("Add\n");

      map->items[index].data = new_entry;
      map->items[index].type = E_DATA;
      map->count += 1;

      break;
    } else {

      if (strcmp(map->items[index].data->name, new_entry->name) == 0) {
        printf("Update\n");

        map->items[index].data->content = new_entry->content;
        free(new_entry->name);

        break;
      }
    }

    iterations += 1;
  }

  hm_entry_free(&item);

  return 0;
}

static int hm_map_get_data_internal(hash_map_t *map, const char *key,
                                    hash_map_entry_internal_t *item) {
  if (hm_map_is_empty(map) == true) {
    return -1;
  }

  int iterations = 0;
  while (1) {
    if (iterations > HASH_MAP_CAPACITY) {
      return -1;
    }

    int index = hm_hash_function(key, iterations);

    if (map->items[index].type == E_FREE) {
      return -1;
    } else if (map->items[index].type == E_DATA) {
      if (strcmp(map->items[index].data->name, key) == 0) {

        item = &map->items[index];

        break;
      }
    }

    iterations += 1;
  }

  return 0;
}

int hm_map_get(hash_map_t *map, const char *key, hash_map_entry_t *item) {
  hash_map_entry_internal_t internal_item;
  if (hm_map_get_data_internal(map, key, &internal_item) < 0) {
    return -1;
  }

  item->name = NULL;
  item->content = NULL;

  item->name = strdup(internal_item.data->name);
  if (item->name == NULL) {
    return -1;
  }

  item->content = strdup(internal_item.data->content);
  if (item->content == NULL) {
    free(item->name);
    item->name = NULL;

    return -1;
  }

  return 0;
}

int hm_map_remove(hash_map_t *map, const char *key) {
  hash_map_entry_internal_t internal_item;
  if (hm_map_get_data_internal(map, key, &internal_item) < 0) {
    return -1;
  }

  internal_item.data = NULL;
  internal_item.type = E_FREE_AGAIN;

  map->count -= 1;

  return 0;
}
