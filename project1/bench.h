#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

/*
we are making threads in kiwi.c, so we use the same
headerfiles as the already include "pthread.h"
*/
#include "../engine/db.h" /**/
#include "../engine/variant.h"  /**/
 

#define KSIZE (16)
#define VSIZE (1000)

#define LINE "+-----------------------------+----------------+------------------------------+-------------------+\n"
#define LINE1 "---------------------------------------------------------------------------------------------------\n"

long long get_ustime_sec(void); 
void _random_key(char *key,int length); 

/*
a struct we created to pass the needed data to the _write_test, 
thread_write, _read_test functions and thread_read  --> kiwi.c
*/
typedef struct paramet{
    long int count; /*copy form kiwi.c*/
    int r; /*copy form kiwi.c*/
    DB *db; /*copy form kiwi.c*/
    
}paramet;


void thread_write(long int count, int r, int num_threads); /*declare thread_write fucntion --> kiwi.c*/
void thread_read(long int count, int r, int num_threads); /*declare thread_read fucntion --> kiwi.c*/

