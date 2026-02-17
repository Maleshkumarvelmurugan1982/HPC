#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX 100

int arr[MAX];
int n;
int sum1 = 0, sum2 = 0;

void* partial_sum1(void* arg) {
    for (int i = 0; i < n/2; i++) {
        sum1 += arr[i];
    }
    pthread_exit(NULL);
}

void* partial_sum2(void* arg) {
    for (int i = n/2; i < n; i++) {
        sum2 += arr[i];
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t t1, t2;

    printf("Enter number of elements: ");
    scanf("%d", &n);

    printf("Enter %d numbers:\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }

    // Create threads
    pthread_create(&t1, NULL, partial_sum1, NULL);
    pthread_create(&t2, NULL, partial_sum2, NULL);

    // Wait for threads
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    int total = sum1 + sum2;

    printf("Total Sum = %d\n", total);

    return 0;
}
