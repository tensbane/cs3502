#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define MAX 10240
#define NUM_THREADS 10

int total = 0;
int n1, n2;
char *s1, *s2;
FILE *fp;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int thread_id; 
    int start;
    int end;
} ThreadArg;

int readf(FILE *fp)
{
    if ((fp = fopen("strings.txt", "r")) == NULL) {
        printf("can't open strings.txt!\n");
        return 0;
    }

    s1 = (char *)malloc(sizeof(char) * MAX);
    if (s1 == NULL) { printf("Out of memory!\n"); return -1; }

    s2 = (char *)malloc(sizeof(char) * MAX);
    if (s2 == NULL) { printf("Out of memory!\n"); return -1; }

    s1 = fgets(s1, MAX, fp); 
    s2 = fgets(s2, MAX, fp);

    n1 = strlen(s1);
    n2 = strlen(s2) - 1;

    if (s1 == NULL || s2 == NULL || n1 < n2)
        return -1;

    return 0;
}

void *thread_func(void *arg)
{
    ThreadArg *targ = (ThreadArg *)arg;
    int thread_id = targ->thread_id;
    int start = targ->start;
    int end = targ->end;
    int local_count = 0;

    int i, j, k, count;

    int search_end = end;
    if (search_end > n1 - n2 + 1)
        search_end = n1 - n2 + 1;

    for (i = start; i < search_end; i++) {
        count = 0;
        for (j = i, k = 0; k < n2; j++, k++) {
            if (*(s1 + j) != *(s2 + k))
                break;
            else
                count++;

            if (count == n2)
                local_count++;
        }
    }

    pthread_mutex_lock(&mutex);
    total += local_count;
    printf("The number of substrings thread %d finds is %d\n",
           thread_id, local_count);
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_t tid[NUM_THREADS];
    ThreadArg args[NUM_THREADS];
    int chunk, i;

    if (readf(fp) != 0) {
        printf("ERROR: Failed to read input file.\n");
        return -1;
    }

    chunk = n1 / NUM_THREADS;

    for (i = 0; i < NUM_THREADS; i++) {
        args[i].thread_id = i + 1;
        args[i].start     = i * chunk;
        args[i].end       = (i + 1) * chunk;

        if (pthread_create(&tid[i], NULL, thread_func, (void *)&args[i]) != 0) {
            printf("ERROR: Failed to create thread %d\n", i + 1);
            return -1;
        }
    }

    for (i = 0; i < NUM_THREADS; i++)
        pthread_join(tid[i], NULL);

    printf("Total substrings is %d\n", total);

    pthread_mutex_destroy(&mutex);
    free(s1);
    free(s2);
    return 0;
}
