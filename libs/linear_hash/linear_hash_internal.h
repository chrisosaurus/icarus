
#ifndef LINEAR_HASH_INTERNAL_H
#define LINEAR_HASH_INTERNAL_H


unsigned int lh_entry_eq(struct lh_entry *cur, unsigned long int hash, unsigned long int key_len, const char *key);

char * lh_strdupn(const char *str, size_t len);

unsigned int lh_insert_internal(struct lh_table *table, struct lh_entry *entry, unsigned long int hash, const char *key, size_t key_len, void *data);

unsigned int lh_entry_init(struct lh_entry *entry, unsigned long int hash, const char *key, size_t key_len, void *data );

unsigned int lh_entry_destroy(struct lh_entry *entry, unsigned int free_data);

enum lh_find_entry_state {
    /* error:
     *  arguments were bad
     *  call to internal function failed
     *  failed to find dummy OR empty
     */
    LH_FIND_ENTRY_STATE_ERROR  = 0,

    /* key did exist
     * returned entry that is occupied
     */
    LH_FIND_ENTRY_STATE_EXISTS = 1,

    /* key didn't already exist
     * returned entry that is either dummy or empty
     * returned entry is place that key *should* go
     */
    LH_FIND_ENTRY_STATE_SLOT   = 2,
};

enum lh_find_entry_state lh_find_entry(const struct lh_table *table, unsigned long int hash, const char *key, size_t key_len, struct lh_entry **entry);

#endif

