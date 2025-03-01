#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define NUM_THREADS 8

char letters[] = "abcdefghijklmnopqrstuvwxyz";
char digits[] = "0123456789";
int found = 0;
pthread_mutex_t lock;
char name_rar[100];

void *brute_force(void *arg) {
    int thread_id = *(int *)arg;
    free(arg);

    char password[8];
    char command[128];

    for (int l1 = thread_id; l1 < 26; l1 += NUM_THREADS) { 
        for (int l2 = 0; l2 < 26; l2++) {
            for (int l3 = 0; l3 < 26; l3++) {
                for (int l4 = 0; l4 < 26; l4++) {
                    for (int d1 = 0; d1 < 10; d1++) {
                        for (int d2 = 0; d2 < 10; d2++) {
                            if (found) return NULL;

                            snprintf(password, sizeof(password), "%c%c%c%c%c%c0",
                                     letters[l1], letters[l2], letters[l3], letters[l4], digits[d1], digits[d2]);
                            printf("password tested: %s\n", password);

                            snprintf(command, sizeof(command), "unrar x -p%s %s > /dev/null 2>&1", password, name_rar);

                            if (system(command) == 0) {
                                pthread_mutex_lock(&lock);
                                if (!found) {
                                    printf("\password found: %s\n", password);
                                    FILE *output = fopen("password_found.txt", "w");
                                    fprintf(output, "password found: %s\n", password);
                                    fclose(output);
                                    found = 1;
                                }
                                pthread_mutex_unlock(&lock);
                                return NULL;
                            }
                        }
                    }
                }
            }
        }
    }
    return NULL;
}

int main() {
    printf("Enter the name of the RAR file: ");
    scanf("%99s", name_rar);

    printf("Start brute-force attack on file %s\n", name_rar);

    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&lock, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        int *thread_id = malloc(sizeof(int));
        *thread_id = i;
        pthread_create(&threads[i], NULL, brute_force, thread_id);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    return 0;
}
