#include <string.h>
#include "../engine/db.h"
#include "../engine/variant.h"
#include "bench.h"

#define DATAS ("testdb")

int found = 0; /*global*/

/*-------------------------------write---------------------------------*/

void* _write_test(void *x) /*change of _write_test function*/
{
	int i; /*same*/
	Variant sk, sv; /*same*/
	paramet param = *(paramet *)x; /* cast pointer x to (paramet *) */

	char key[KSIZE + 1]; /*same*/
	char val[VSIZE + 1]; /*same*/
	char sbuf[1024]; /*same*/

	memset(key, 0, KSIZE + 1); /*same*/
	memset(val, 0, VSIZE + 1); /*same*/
	memset(sbuf, 0, 1024); /*same*/

	
	for (i = 0; i < param.count; i++) { /*same* + count changed to param.count*/
		if (param.r)  /* r changed to param.r  (struct in bench.h)*/
			_random_key(key, KSIZE); /*same*/
		else
			snprintf(key, KSIZE, "key-%d", i); /*same*/
		fprintf(stderr, "%d adding %s\n", i, key); /*same*/
		snprintf(val, VSIZE, "val-%d", i); /*same*/

		sk.length = KSIZE; /*same*/
		sk.mem = key; /*same*/
		sv.length = VSIZE; /*same*/
		sv.mem = val; /*same*/

		db_add(param.db, &sk, &sv); /*same + db changed to param.db (struct in bench.c)*/
		if ((i % 10000) == 0) { /*same*/
			fprintf(stderr,"random write finished %d ops%30s\r",  
					i, 
					""); /*same*/

			fflush(stderr); /*same*/
		}
	}
	return 0; /**/
}

void thread_write(long int count, int r, int num_threads){ /*prototype for thread_write function*/
	long long start,end; /*same*/
	double cost; /*same*/
	int remain, quotient; /**/
	DB *db; /*same*/
	paramet par, par2; /*declare struct*/
	pthread_t th[num_threads]; /*initialize threads*/
	
	remain = count % num_threads; /*the remain of count / threads*/
	quotient = count / num_threads; /*the quotient of count / threads*/

	par.count = quotient; /**/
	par2.count = quotient + 1; /**/
	par.r = r; /**/
	par2.r = r; /**/
	
	db = db_open(DATAS); /**/
	par.db = db; /**/
	par2.db = db; /**/

	start = get_ustime_sec(); /*same*/

/*
creating threads in a loop
*/
	for (int i=0; i<num_threads; i++){ 
/*
if remain > 0 . distribute the remainder into as many
threads as needed so that remain = 0
example : 10 reads to 6 threads --> remain = 4 , so thread1=1 +1, thread2=1 +1, 
thread3=1 +1, thread4=1 +1, thread5=1, thread6=1,
*/
		if (remain > 0){ /**/
			remain--; /**/
			if (pthread_create(&th[i], NULL, &_write_test, &par) != 0){ /*creating the thread*/
				fprintf(stderr,"error creating thread\n");
				exit(1);
			}
		}
		else{
			if (pthread_create(&th[i], NULL, &_write_test, &par) != 0){ /**/
				fprintf(stderr,"error creating thread\n"); /**/
				exit(1); /**/ 
			}	
		}
	}
/*
joinning created threads in a loop
*/
	for (int i=0; i<num_threads; i++){
		if (pthread_join(th[i], NULL) != 0){ /*joinning the thread*/
			fprintf(stderr,"error join thread\n");
			exit(1);
		}
	}

	db_close(par.db); /*changed db to par db (struct bench.h)*/

	end = get_ustime_sec(); /*same*/
	cost = end -start; /*same*/

	printf(LINE); /*same*/
	printf("|Random-Write	(done:%ld): %.6f sec/op; %.1f writes/sec(estimated); cost:%.3f(sec);\n"
		,par.count, (double)(cost / par.count)
		,(double)(par.count / cost)
		,cost);	/*same + changed count to par.count (struct in bench.h)*/
}

/*-------------------------------read---------------------------------*/

void* _read_test(void *x) /*change of _read_test function*/
{
	int i; /*same*/
	int ret; /*same*/
	
	Variant sk; /*same*/
	Variant sv; /*same*/
	paramet param = *(paramet *)x; /* cast pointer x to (paramet *) */
	
	char key[KSIZE + 1];

	
	for (i = 0; i < param.count; i++) { /*same + changed count to param.count (struct in bench.h)*/
		memset(key, 0, KSIZE + 1);/*same*/

		/* if you want to test random write, use the following */
		if (param.r) /*changed r to param.r (struct in bench.h)*/				
			_random_key(key, KSIZE); /*same*/
		else
			snprintf(key, KSIZE, "key-%d", i); /*same*/
	
		fprintf(stderr, "%d searching %s\n", i, key); 
		sk.length = KSIZE; /*same*/
		sk.mem = key; /*same*/
		ret = db_get(param.db, &sk, &sv); /*changed db to param.db (struct in bench.h)*/
		if (ret) { /*same*/
			//db_free_data(sv.mem);
			found++; /*same*/
		} else {
			INFO("not found key#%s", 
					sk.mem); /*same*/
    	}

		if ((i % 10000) == 0) { /*same*/
			fprintf(stderr,"random read finished %d ops%30s\r", 
					i, 
					""); /*same*/

			fflush(stderr); /*same*/
		}
	}
	return 0; /**/
}


void thread_read(long int count, int r, int num_threads){ /*prototype for thread_read function*/
	
	long long start,end; /*same*/
	double cost; /*same*/
	int remain, quotient; /*declare variables*/
	DB *db; /*same*/

	paramet par, par2; /*declare struct*/
	pthread_t th[num_threads];	/*initialize threads*/
	
	remain = count % num_threads; /*the remain of count / threads*/
	quotient = count / num_threads; /*the quotient of count / threads*/

	par.count = quotient; /**/
	par2.count = quotient + 1; /**/
	par.r = r; /**/
	par2.r = r; /**/
	
				
	db = db_open(DATAS); /*same*/
	par.db = db; /**/
	par2.db = db; /**/

	start = get_ustime_sec(); /*same*/

/*
creating threads in a loop
*/
	for (int i=0; i<num_threads; i++){ 
/*
if remain > 0 . distribute the remainder into as many
threads as needed so that remain = 0
example : 10 reads to 6 threads --> remain = 4 , so thread1=1 +1, thread2=1 +1, 
thread3=1 +1, thread4=1 +1, thread5=1, thread6=1,*/
		if (remain > 0){ /**/
			remain--; /**/
			if (pthread_create(&th[i], NULL, &_read_test, &par2) != 0){ /*creating the thread*/
				fprintf(stderr,"error creating thread\n");
				exit(1);
			}
		}
		else{
			if (pthread_create(&th[i], NULL, &_read_test, &par) != 0){ /*creating the thread*/
				fprintf(stderr,"error creating thread\n");
				exit(1);
			}
		}
	}
/*
joinning created threads in a loop
*/	
	for (int i=0; i<num_threads; i++){
		if (pthread_join(th[i], NULL) != 0){
			fprintf(stderr,"error join thread\n"); /*joinning the thread*/
			exit(1);
		}
	}

	db_close(db); /*same*/ 

	end = get_ustime_sec();	/*same*/
	cost = end - start;	/*same*/
	printf(LINE); /*same*/
	printf("|Random-Read	(done:%ld, found:%d): %.6f sec/op; %.1f reads /sec(estimated); cost:%.3f(sec)\n",
		par.count, found,
		(double)(cost / par.count),
		(double)(par.count / cost),
		cost); /*same + changed count to par.count (struct in bench.h)*/

}




