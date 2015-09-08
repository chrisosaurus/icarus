/*  gcc linear_set.c test_linear_set.c -Wall -Wextra -Werror -o test_ls
 * ./test_ls
 */
#include <assert.h> /* assert */
#include <stdio.h> /* puts */
#include <stdlib.h> /* calloc */

#include "linear_set.h"

/* headers for internal functions within linear_set.c
 * that are not exposed via the header
 * these would be static but we want to be able to test them
 */
unsigned int ls_entry_eq(struct ls_entry *cur, unsigned long int hash, unsigned long int key_len, char *key);
char * ls_strdupn(char *str, size_t len);
unsigned int ls_entry_init(struct ls_entry *entry, unsigned long int hash, char *key, size_t key_len);
unsigned int ls_entry_destroy(struct ls_entry *entry);
struct ls_entry * ls_find_entry(struct ls_set *set, char *key);


void new_insert_exists_destroy(void){
    /* our simple hash set */
    struct ls_set *set = 0;

    /* some keys */
    char *key_1 = "bbbbb";
    char *key_2 = "aaaaa";
    char *key_3 = "ccccc";

    puts("\ntesting basic functionality");

    puts("testing new");
    set = ls_new();
    assert(set);
    assert( 32 == set->size );
    assert( 0 == ls_nelems(set) );
    assert( 0 == ls_load(set) );


    puts("testing insert and get");
    puts("one");
    assert( ls_insert(set, key_1) );
    assert( 1 == ls_nelems(set) );
    assert( 0 == ls_exists(set, key_2) );
    assert( 0 == ls_exists(set, key_3) );
    assert(ls_exists(set, key_1) );


    puts("two");
    assert( ls_insert(set, key_2) );
    assert( 2 == ls_nelems(set) );
    assert( 0 == ls_exists(set, key_3) );
    assert( ls_exists(set, key_2) );


    puts("three");
    assert( ls_insert(set, key_3) );
    assert( 3 == ls_nelems(set) );
    assert( ls_exists(set, key_3) );


    assert( ls_destroy(set, 1) );
    puts("success!");
}

void delete(void){
    /* our simple hash set */
    struct ls_set *set = 0;

    /* some keys */
    char *key_1 = "bacon";
    char *key_2 = "chicken";
    char *key_3 = "pork";

    puts("\ntesting delete functionality ");

    puts("creating a set");
    set = ls_new();
    assert(set);
    assert( 32 == set->size );
    assert( 0 == ls_nelems(set) );


    puts("inserting some data");
    assert( ls_insert(set, key_1) );
    assert( 1 == ls_nelems(set) );
    assert( 0 == ls_exists(set, key_2) );
    assert( 0 == ls_exists(set, key_3) );
    assert( ls_exists(set, key_1) );


    assert( ls_insert(set, key_2) );
    assert( 2 == ls_nelems(set) );
    assert( 0 == ls_exists(set, key_3) );
    assert( ls_exists(set, key_2) );


    assert( ls_insert(set, key_3) );
    assert( 3 == ls_nelems(set) );
    assert( ls_exists(set, key_3) );


    puts("testing delete");
    puts("one delete");
    assert( ls_delete(set, key_1) );
    assert( 2 == ls_nelems(set) );

    /* should not be able to re-delete */
    assert( 0 == ls_delete(set, key_1) );


    puts("two delete");
    assert( ls_delete(set, key_3) );
    assert( 1 == ls_nelems(set) );

    /* should not be able to re-delete */
    assert( 0 == ls_delete(set, key_3) );


    puts("three delete");
    assert( ls_delete(set, key_2) );
    assert( 0 == ls_nelems(set) );

    /* should not be able to re-delete */
    assert( 0 == ls_delete(set, key_2) );


    assert( ls_destroy(set, 1) );
    puts("success!");
}

void collision(void){
    /* our simple hash set */
    struct ls_set *set = 0;

    /* some keys */
    char *key_1 = "bacon";
    char *key_2 = "chicken";
    char *key_3 = "pork";
    char *key_4 = "pig";
    char *key_5 = "lettuce";
    char *key_6 = "beetroot";
    char *key_7 = "chocolate";
    /* long string to try cause some havoc */
    char *key_8 = "frying pan porcupine";
    char *key_9 = "a4 paper";

    /* temporary data pointer used for testing get */
    int *data = 0;

    puts("\ntesting collision behavior ");

    puts("creating a set");
    set = ls_new();
    assert(set);
    assert( 32 == set->size );
    assert( 0 == ls_nelems(set) );
    assert( 0 == ls_load(set) );

    /* artificially shrink down set */
    assert( ls_resize(set, 9) );
    assert( 9 == set->size );
    assert( 0 == ls_nelems(set) );
    assert( 0 == ls_load(set) );


    puts("inserting some data");
    assert( ls_insert(set, key_1) );
    assert( 1 == ls_nelems(set) );
    assert( ls_exists(set, key_1) );

    assert( ls_insert(set, key_2) );
    assert( 2 == ls_nelems(set) );
    assert( ls_exists(set, key_2) );

    assert( ls_insert(set, key_3) );
    assert( 3 == ls_nelems(set) );
    assert( ls_exists(set, key_3) );

    assert( ls_insert(set, key_4) );
    assert( 4 == ls_nelems(set) );
    assert( ls_exists(set, key_4) );

    assert( ls_insert(set, key_5) );
    assert( 5 == ls_nelems(set) );
    assert( ls_exists(set, key_5) );

    assert( ls_insert(set, key_6) );
    assert( 6 == ls_nelems(set) );
    assert( ls_exists(set, key_6) );

    assert( ls_insert(set, key_7) );
    assert( 7 == ls_nelems(set) );
    assert( ls_exists(set, key_7) );

    assert( ls_insert(set, key_8) );
    assert( 8 == ls_nelems(set) );
    assert( ls_exists(set, key_8) );

    assert( ls_insert(set, key_9) );
    assert( 9 == ls_nelems(set) );
    assert( ls_exists(set, key_9) );

    assert( 9 == ls_nelems(set) );
    /* assert that our resize succeeded and we
     * are not full */
    assert( 10 > ls_load(set) );
    assert( 9 < set->size );


    puts("testing delete collision handling");

    assert( ls_delete(set, key_1) );
    assert( 8 == ls_nelems(set) );
    /* should not be able to re-delete */
    assert( 0 == ls_delete(set, key_1) );

    assert( ls_delete(set, key_3) );
    assert( 7 == ls_nelems(set) );
    /* should not be able to re-delete */
    assert( 0 == ls_delete(set, key_3) );


    assert( ls_delete(set, key_2) );
    /* should not be able to re-delete */
    assert( 0 == ls_delete(set, key_2) );

    puts("checking post-delete that all items that should be reachable are");

    assert( ls_exists(set, key_5) );

    assert( ls_exists(set, key_8) );

    assert( ls_exists(set, key_4) );

    assert( ls_exists(set, key_6) );

    assert( ls_exists(set, key_9) );

    assert( ls_exists(set, key_7) );

    assert( ls_destroy(set, 1) );
    puts("success!");
}

void resize(void){
    /* our simple hash set */
    struct ls_set *set = 0;

    /* some keys */
    char *key_1 = "bbbbb";
    char *key_2 = "aaaaa";
    char *key_3 = "ccccc";


    puts("\ntesting resize functionality");

    puts("creating set");
    set = ls_new();
    assert(set);
    assert( 32 == set->size );
    assert( 0 == ls_nelems(set) );

    /* artificially shrink down */
    assert( ls_resize(set, 3) );
    assert( 3 == set->size );
    assert( 0 == ls_nelems(set) );

    puts("inserting some data");
    assert( ls_insert(set, key_1) );
    assert( 1 == ls_nelems(set) );
    assert( 0 == ls_exists(set, key_2) );
    assert( 0 == ls_exists(set, key_3) );
    assert( ls_exists(set, key_1) );


    assert( ls_insert(set, key_2) );
    assert( 2 == ls_nelems(set) );
    assert( 0 == ls_exists(set, key_3) );
    assert( ls_exists(set, key_2) );


    assert( ls_insert(set, key_3) );
    assert( 3 == ls_nelems(set) );
    assert( ls_exists(set, key_3) );


    puts("testing resize");
    assert( ls_resize(set, 10) );
    assert( 3  == ls_nelems(set) );
    assert( 10 == set->size );

    puts("testing we can still fetch all the old values");
    assert( ls_exists(set, key_3) );

    assert( ls_exists(set, key_2) );

    assert( ls_exists(set, key_1) );

    assert( ls_destroy(set, 1) );
    puts("success!");
}

void error_handling(void){
    /* our simple hash set */
    struct ls_set *set = 0;
    struct ls_set *not_set = 0;
    struct ls_set static_set;

    /* some keys */
    char *key_1 = "bbbbb";
    char *key_2 = "aaaaa";
    char *key_3 = "ccccc";

    puts("\ntesting handling of error cases");

    puts("testing ls_nelems");
    assert( 0 == ls_nelems(0) );

    puts("setting up...");

    puts("creating set");
    set = ls_new();
    assert(set);
    assert( 32 == set->size );
    assert( 0 == ls_nelems(set) );

    /* artificially shrink */
    assert( ls_resize(set, 3) );
    assert( 3 == set->size );
    assert( 0 == ls_nelems(set) );

    puts("inserting some data");
    assert( ls_insert(set, key_1) );
    assert( 1 == ls_nelems(set) );
    assert( 0 == ls_exists(set, key_2) );
    assert( 0 == ls_exists(set, key_3) );
    assert( ls_exists(set, key_1) );


    assert( ls_insert(set, key_2) );
    assert( 2 == ls_nelems(set) );
    assert( 0 == ls_exists(set, key_3) );
    assert( ls_exists(set, key_2) );


    puts("beginning actual testing, cue wall of errors");
    /* ls_hash */
    puts("testing ls_hash");
    assert( 0 == ls_hash(0, 0) );
    assert( ls_hash(key_1, 0) );

    /* ls_init */
    puts("testing ls_init");
    assert( 0 == ls_init(0, 100) );
    assert( 0 == ls_init(&static_set, 0) );

    /* ls_resize */
    puts("testing ls_resize");
    assert( 0 == ls_resize(0, 100) );
    assert( 0 == ls_resize(set, 0) );

    /* ls_exists */
    puts("testing ls_exists");
    assert( 0 == ls_exists(0, key_1) );
    assert( 0 == ls_exists(set, 0) );
    assert( 0 == ls_exists(set, key_3) );

    /* ls_insert */
    puts("testing ls_insert");
    assert( 0 == ls_insert(0, key_1) );
    assert( 0 == ls_insert(set, 0) );
    /* cannot insert if already exists */
    assert( 0 == ls_insert(set, key_1) );

    /* ls_exists */
    puts("testing ls_exists");
    assert( 0 == ls_exists(0, key_1) );
    assert( 0 == ls_exists(set, 0) );
    assert( 0 == ls_exists(set, key_3) );

    /* ls_delete */
    puts("testing ls_delete");
    assert( 0 == ls_delete(0, key_1) );
    assert( 0 == ls_delete(set, 0) );
    /* cannot delete a non-existent key */
    assert( 0 == ls_delete(set, key_3) );

    /* ls_tune_threshold */
    puts("testing ls_tune_threshold");
    assert( 0 == ls_tune_threshold(0, 0) );
    assert( 0 == ls_tune_threshold(set, 0) );
    assert( 0 == ls_tune_threshold(set, 11) );

    /* ls_destroy */
    assert( 0 == ls_destroy(0, 1) );

    /* tidy up */
    assert( ls_destroy(set, 1) );
    puts("success!");
}

void internal(void){
    struct ls_set set;
    struct ls_entry she;
    struct ls_entry static_she;
    char * c = 0;

    puts("\ntesting internal functions");

    /* ls_load */
    puts("testing ls_load error handling");
    assert( 0 == ls_load(0) );

    /* ls_strdupn */
    puts("testing ls_strdupn");
    assert( 0 == ls_strdupn(0, 6) );
    c = ls_strdupn("hello", 0);
    assert(c);
    free(c);

    /* ls_entry_new and ls_entry_init */
    puts("testing ls_entry_new and ls_entry_init");
    assert( 0 == ls_entry_init(0, 0, 0, 0) );
    assert( 0 == ls_entry_init(&she, 0, 0, 0) );
    assert( ls_entry_init(&static_she, 0, "hello", 0) );

    /* ls_entry_destroy */
    puts("testing ls_entry_destroy");
    assert( 0 == ls_entry_destroy(0) );
    assert( ls_entry_destroy(&static_she) );

    /* ls_find_entry */
    puts("testing ls_find_entry");
    assert( 0 == ls_find_entry(0, "hello") );
    assert( 0 == ls_find_entry(&set, 0) );

    /* ls_entry_eq */
    puts("testing ls_entry_eq");
    assert( 0 == ls_entry_eq(0, 0, 0, 0) );
    assert( 0 == ls_entry_eq(&she, 0, 0, 0) );

    puts("success!");
}

void load_resize(void){
    /* our simple hash set */
    struct ls_set *set = 0;

    /* some keys */
    char *key_1 = "bbbbb";
    char *key_2 = "aaaaa";
    char *key_3 = "ccccc";
    char *key_4 = "ddddd";


    puts("\ntesting auto resizing at load");

    puts("testing new");
    set = ls_new();
    assert(set);
    assert( 32 == set->size );
    assert( 0 == ls_nelems(set) );
    assert( 0 == ls_load(set) );

    /* artificially shrink */
    assert( ls_resize(set, 4) );
    assert( 4 == set->size );
    assert( 0 == ls_nelems(set) );
    assert( 0 == ls_load(set) );

    puts("adding some data to force a resize");
    assert( ls_insert(set, key_1) );
    assert( 1 == ls_nelems(set) );
    assert( 0 == ls_exists(set, key_2) );
    assert( 0 == ls_exists(set, key_3) );
    assert( ls_exists(set, key_1) );

    /* insert tests resize before inserting
     * so from it's view at that time:
     * 0 / 4 = 0 % loading
     * no resize
     */
    assert( 4 == set->size );
    assert( 1 == ls_nelems(set) );
    /* however the load now will be
     * 1 / 4 = 25 %
     */
    assert( 2 == ls_load(set) );


    assert( ls_insert(set, key_2) );
    assert( 2 == ls_nelems(set) );
    assert( 0 == ls_exists(set, key_3) );
    assert( ls_exists(set, key_2) );

    /* insert tests resize before inserting
     * so from it's view at that time:
     * 1 / 4 = 25 % loading
     * no resize
     */
    assert( 4 == set->size );
    assert( 2 == ls_nelems(set) );
    /* however the load now will be
     * 2 / 4 = 50 %
     */
    assert( 5 == ls_load(set) );


    assert( ls_insert(set, key_3) );
    assert( 3 == ls_nelems(set) );
    assert( ls_exists(set, key_3) );

    /* insert tests resize before inserting
     * so from it's view at that time:
     * 2 / 4 = 50 % loading
     * no resize
     */
    assert( 4 == set->size );
    assert( 3 == ls_nelems(set) );
    /* however the load now will be
     * 3 / 4 = 70 %
     * so this will trigger a resize (as 70 >= 60)
     */
    assert( 7 == ls_load(set) );


    assert( ls_insert(set, key_4) );
    assert( 4 == ls_nelems(set) );
    assert( ls_exists(set, key_4) );

    /* insert tests resize before inserting
     * so from it's view at that time:
     * 3 / 4 = 75 % loading
     * so resize called to double
     */
    assert( 8 == set->size );
    assert( 4 == ls_nelems(set) );
    /* and after resizing the load now will be
     * 4 / 8 = 50 %
     */
    assert( 5 == ls_load(set) );


    assert( ls_destroy(set, 1) );
    puts("success!");

}

void rollover(void){
    struct ls_set *set = 0;

    /* with length 4 and key 'c' we can hit the 3rd spot */
    char *key = "c";

    /* with length 4 we know that both 'c' and 'g' will hit the 3rd spot */
    char *key_2 = "g";

    puts("\ntesting rollover");

    puts("making set");
    set = ls_new();
    assert(set);

    /* force shrink */
    assert( ls_resize(set, 4) );
    assert( 4 == set->size );

    /* mark 3rd position as occupied as we know
     * that our key would end up there
     * the intention here is to force the second for loop
     * for insert and delete to activate
     */
    set->entries[3].state = ls_ENTRY_OCCUPIED;

    /* force insert to roll over */
    assert( ls_insert(set, key) );

    /* force delete to roll over */
    assert( ls_delete(set, key) );

    /* make sure to remark as empty */
    set->entries[3].state = ls_ENTRY_EMPTY;

    /* force resize to 2 */
    assert( ls_resize(set, 2) );
    assert( 2 == set->size );
    assert( 0 == ls_nelems(set) );

    /* insert 2 elements that we know will collide */
    assert( ls_insert(set, key) );
    assert( ls_insert(set, key_2) );
    /* no resize yet */
    assert( 2 == set->size );

    /* force a resize, this will move to 4
     * and this will trigger the second for
     * loop in ls_resize
     */
    assert( ls_resize(set, 4) );


    /* cleanup */
    assert( ls_destroy(set, 1) );
}

void threshold(void){
    struct ls_set *set = 0;

    puts("\ntesting ls load threshold setting");

    puts("building set");
    set = ls_new();
    assert(set);

    /* shrink artificially */
    assert( ls_resize(set, 4) );

    /* tune to only resize at 100% */
    assert( ls_tune_threshold(set, 10) );
    assert( 10 == set->threshold );

    /* insert 4 times checking there has been no resizing */
    assert( ls_insert(set, "a") );
    assert( 1 == ls_nelems(set) );
    assert( 4 == set->size );

    assert( ls_insert(set, "b") );
    assert( 2 == ls_nelems(set) );
    assert( 4 == set->size );

    assert( ls_insert(set, "c") );
    assert( 3 == ls_nelems(set) );
    assert( 4 == set->size );

    assert( ls_insert(set, "d") );
    assert( 4 == ls_nelems(set) );
    assert( 4 == set->size );

    /* insert 1 more and check resize */
    assert( ls_insert(set, "e") );
    assert( 5 == ls_nelems(set) );
    assert( 8 == set->size );
    assert( 10 == set->threshold );

    assert( ls_destroy(set, 1) );
    puts("success!");
}

void artificial(void){
    struct ls_set *set = 0;

    puts("\ntesting artificial linear search filure");

    puts("building set");
    set = ls_new();
    assert(set);

    /* shrink artificially */
    assert( ls_resize(set, 2) );

    puts("manipulating and testing of find and insert");
    /* fill only positions */
    set->entries[0].state = ls_ENTRY_OCCUPIED;
    set->entries[1].state = ls_ENTRY_OCCUPIED;

    /* this should trigger the final return 0 of ls_find_entry */
    assert( 0 == ls_find_entry(set, "hello") );

    /* likewise an insert should fail */
    assert( 0 == ls_insert(set, "c") );

    puts("further manipulation for ls_find_entry");
    set->entries[0].state = ls_ENTRY_DUMMY;
    set->entries[1].state = ls_ENTRY_DUMMY;
    assert( 0 == ls_find_entry(set, "c") );

    set->entries[0].state   = ls_ENTRY_OCCUPIED;
    set->entries[0].hash    = 98; /* collide with b */
    set->entries[0].key_len = 2;
    set->entries[1].state   = ls_ENTRY_OCCUPIED;
    set->entries[0].hash    = 98;
    set->entries[1].key_len = 2;
    assert( 0 == ls_find_entry(set, "b") );

    set->entries[0].state   = ls_ENTRY_OCCUPIED;
    set->entries[0].hash    = 99; /* force hash collision with c */
    set->entries[0].key_len = 4; /* but with different len */
    set->entries[0].key     = "a";
    set->entries[1].state   = ls_ENTRY_OCCUPIED;
    set->entries[1].hash    = 99; /* force hash collision with c */
    set->entries[1].key_len = 1;  /* with same len */
    set->entries[1].key     = "b"; /* but different key */
    assert( 0 == ls_find_entry(set, "c") );

    assert( ls_resize(set, 3) );
    set->entries[0].state   = ls_ENTRY_OCCUPIED;
    set->entries[0].hash    = 98; /* force hash collision with b */
    set->entries[0].key_len = 4;  /* but with different len */
    set->entries[0].key     = "a";

    set->entries[1].state   = ls_ENTRY_OCCUPIED;
    set->entries[1].hash    = 98; /* force hash collision with b */
    set->entries[1].key_len = 1;  /* with same len */
    set->entries[1].key     = "a"; /* but different key */

    set->entries[2].state   = ls_ENTRY_DUMMY;
    assert( 0 == ls_find_entry(set, "b") );

    puts("manipulation to provoke ls_resize");
    assert( ls_resize(set, 5) );
    /* provoke 3 hash collision on a resize
     * where the preferred pos is the final element
     *
     * we also secretly fill up the elements without
     * increment n_elems
     *
     * we then resize so that there is not enough room
     * this is only possible with manipulation as insert
     * will increase n_elems and resize will refuse to
     * shrink below n_elems
     */
    set->entries[0].state   = ls_ENTRY_OCCUPIED;
    set->entries[0].hash    = 101;
    set->entries[1].state   = ls_ENTRY_OCCUPIED;
    set->entries[1].hash    = 101;
    set->entries[2].state   = ls_ENTRY_OCCUPIED;
    set->entries[2].hash    = 101;
    set->entries[3].state   = ls_ENTRY_OCCUPIED;
    set->entries[3].hash    = 101;
    set->entries[4].state   = ls_ENTRY_OCCUPIED;
    set->entries[4].hash    = 101;
    /* we won't be able to fit everything in! */
    assert( 0 ==  ls_resize(set, 4) );

    puts("manipulating to torture ls_delete");

    /*
     * examples of some useful keys in our length
     * 4 set
     *
     * ls_pos(ls_hash(X, 1), 4) == Y
     *
     * for key  X  we get hash  Y
     *
     *  for key 'a' we get hash '1'
     *  for key 'b' we get hash '2'
     *  for key 'c' we get hash '3'
     *  for key 'd' we get hash '0'
     *  for key 'e' we get hash '1'
     *  for key 'f' we get hash '2'
     *  for key 'g' we get hash '3'
     */

    set->entries[4].state   = ls_ENTRY_EMPTY;
    assert( ls_resize(set, 4) );

    set->entries[0].state   = ls_ENTRY_OCCUPIED;
    set->entries[0].hash    = 99; /* hash collide with c */
    set->entries[0].key_len = 2; /* different key len*/

    set->entries[1].state   = ls_ENTRY_OCCUPIED;
    set->entries[1].hash    = 99;
    set->entries[1].key_len = 1;
    set->entries[1].key     = "z"; /* different key */

    set->entries[2].state   = ls_ENTRY_DUMMY;

    set->entries[3].state   = ls_ENTRY_OCCUPIED;
    set->entries[3].hash    = 99;

    assert( 0 == ls_delete(set, "c") );

    set->entries[0].state   = ls_ENTRY_OCCUPIED;
    set->entries[0].hash    = 1; /* hash differ */

    set->entries[1].state   = ls_ENTRY_OCCUPIED;
    set->entries[1].hash    = 100; /* hash collide with d */
    set->entries[1].key_len = 2; /* different key len*/

    set->entries[2].state   = ls_ENTRY_OCCUPIED;
    set->entries[2].hash    = 100;
    set->entries[2].key_len = 1;
    set->entries[2].key     = "z"; /* different key */

    set->entries[3].state   = ls_ENTRY_DUMMY;

    assert( 0 == ls_delete(set, "d") );

    /* we want to force wrap around
     * and then encounter an empty
     */
    set->entries[0].state   = ls_ENTRY_EMPTY;

    set->entries[1].state   = ls_ENTRY_OCCUPIED;
    set->entries[1].hash    = 99; /* hash collide with c */
    set->entries[1].key_len = 2; /* different key len*/
    set->entries[1].key     = "z"; /* different key */

    set->entries[2].state   = ls_ENTRY_OCCUPIED;
    set->entries[2].hash    = 99; /* hash collide with c */
    set->entries[2].key_len = 2; /* different key len*/
    set->entries[2].key     = "z"; /* different key */

    set->entries[3].state   = ls_ENTRY_OCCUPIED;
    set->entries[3].hash    = 99; /* hash collide with c */
    set->entries[3].key_len = 2; /* different key len*/
    set->entries[3].key     = "z"; /* different key */

    assert( 0 == ls_delete(set, "c") );


    puts("success!");
}

int main(void){
    new_insert_exists_destroy();

    delete();

    collision();

    resize();

    error_handling();

    internal();

    load_resize();

    rollover();

    threshold();

    artificial();

    puts("\noverall testing success!");

    return 0;
}
