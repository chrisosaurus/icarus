#ifndef LINEAR_SET_INTERNAL_H
#define LINEAR_SET_INTERNAL_H

unsigned int ls_entry_eq(const struct ls_entry *cur, unsigned long int hash, unsigned long int key_len, const char *key);
char * ls_strdupn(const char *str, size_t len);
unsigned int ls_entry_init(struct ls_entry *entry, unsigned long int hash, const char *key, size_t key_len);
unsigned int ls_entry_destroy(struct ls_entry *entry);
struct ls_entry * ls_find_entry(const struct ls_set *set, const char *key);

#endif

