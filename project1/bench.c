#include "bench.h"

void _random_key(char *key,int length) {
	int i;
	char salt[36]= "abcdefghijklmnopqrstuvwxyz0123456789";

	for (i = 0; i < length; i++)
		key[i] = salt[rand() % 36];
}

void _print_header(int count)
{
	double index_size = (double)((double)(KSIZE + 8 + 1) * count) / 1048576.0;
	double data_size = (double)((double)(VSIZE + 4) * count) / 1048576.0;

	printf("Keys:\t\t%d bytes each\n", 
			KSIZE);
	printf("Values: \t%d bytes each\n", 
			VSIZE);
	printf("Entries:\t%d\n", 
			count);
	printf("IndexSize:\t%.1f MB (estimated)\n",
			index_size);
	printf("DataSize:\t%.1f MB (estimated)\n",
			data_size);

	printf(LINE1);
}

void _print_environment()
{
	time_t now = time(NULL);

	printf("Date:\t\t%s", 
			(char*)ctime(&now));

	int num_cpus = 0;
	char cpu_type[256] = {0};
	char cache_size[256] = {0};

	FILE* cpuinfo = fopen("/proc/cpuinfo", "r");
	if (cpuinfo) {
		char line[1024] = {0};
		while (fgets(line, sizeof(line), cpuinfo) != NULL) {
			const char* sep = strchr(line, ':');
			if (sep == NULL || strlen(sep) < 10)
				continue;

			char key[1024] = {0};
			char val[1024] = {0};
			strncpy(key, line, sep-1-line);
			strncpy(val, sep+1, strlen(sep)-1);
			if (strcmp("model name", key) == 0) {
				num_cpus++;
				strcpy(cpu_type, val);
			}
			else if (strcmp("cache size", key) == 0)
				strncpy(cache_size, val + 1, strlen(val) - 1);	
		}

		fclose(cpuinfo);
		printf("CPU:\t\t%d * %s", 
				num_cpus, 
				cpu_type);

		printf("CPUCache:\t%s\n", 
				cache_size);
	}
}

int main(int argc,char** argv)
{
	long int count;
	int num_threads; /*input number of threads from the keyboard*/
	pthread_mutex_init(&glob_lock, NULL); /*lock initialization  --> extern in db.h*/
	pthread_cond_init(&reader_wait, NULL); //*dynamic initialization --> extern in db.h*/
	pthread_cond_init(&writer_wait, NULL); //*dynamic initialization --> extern in db.h*/

	srand(time(NULL));

/*
changing argc < 4 because the number of inputs increased.
input0: ./kiwi-bench, input1: write | read, input2: count, input3:
num threads
*/
	if (argc < 4) {  /**/
		fprintf(stderr,"Usage: db-bench <write | read> <count> <num threads>\n");   /**/
		exit(1);
	} 

		if (strcmp(argv[1], "write") == 0) {
		int r = 0;

		count = atoi(argv[2]);
		num_threads = atoi(argv[3]); /*how many threads it will get from the keyboard*/
		_print_header(count);
		_print_environment();
		if (argc == 5) /**/
			r = 1;

		thread_write(count, r, num_threads); /*add the num_threads*/
	} else if (strcmp(argv[1], "read") == 0) {
		int r = 0;
	
		count = atoi(argv[2]);
		num_threads = atoi(argv[3]); /*threads from the keyboard*/
		_print_header(count);
		_print_environment();
		
/*
changing argc == 5 because the number of inputs increased.
input0: ./kiwi-bench, input1: write|read, input2: count, input3: num
threads, input4: random
*/		
		if (argc == 5)	/*4 -> 5*/
			r = 1;
	
		thread_read(count, r, num_threads);

	/*
	creating a readwrite option for read and write
	requests from the same thread-s
	*/
	} else if (strcmp(argv[1], "readwrite") == 0){
		int r = 0;
		double perce, count_r, count_w;

		count = atoi(argv[2]); /*total number of read and writes*/
		num_threads = atoi(argv[3]);/* number of threads*/
		perce = atoi(argv[4]); /* from 1 to 100 */
		_print_header(count);
		_print_environment();
		/*
argc == 6 because the number of inputs increased.
input0: ./kiwi-bench, input1: write|read|readwrite, input2: count, input3: num
threads, input4: random, input5: perce -only for readwrite-.
		*/
		if (argc == 6){ 
			r = 1;
		}
		count_r = count * (perce / 100); /* the percentage of reads we want */
		count_w = count - (int)count_r;	 /*remaining amount - rounding with the "int", 
		example: 32% for 7 = 2.24 -> round to 2 & the remain = int */	
		thread_read((int)count_r, r, num_threads); /**/
		thread_write((int)count_w, r, num_threads); /**/
		
	} else {
		fprintf(stderr,"Usage: db-bench <write | read> <count> <num threads> <random>\n"
		"db-bench <readwrite> <count> <num threads> <percentage> <random>\n");
		exit(1);
	}
	pthread_mutex_destroy(&glob_lock); /*destroy due to dynamic initialization*/
	pthread_cond_destroy(&reader_wait); /*destroy due to dynamic initialization*/
	pthread_cond_destroy(&writer_wait); /*destroy due to dynamic initialization*/
	return 1;
}

