sivajothi@sivajothi-HP-Laptop-15s-fq5xxx:~$ cat 4.c
#include <mpi.h>
#include <stdio.h>

#define MAX 50

struct Student {
    char name[20];
    int roll;
    int total;
    char grade;
};

char findGrade(int marks) {
    if (marks >= 90) return 'A';
    else if (marks >= 75) return 'B';
    else if (marks >= 60) return 'C';
    else if (marks >= 50) return 'D';
    else return 'F';
}

int main() {
    int rank, size, n, chunk;
    struct Student students[MAX];

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* Master process reads input file */
    if (rank == 0) {
        FILE *fp = fopen("students.txt", "r");
        if (fp == NULL) {
            printf("Error: students.txt not found\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fscanf(fp, "%d", &n);

        for (int i = 0; i < n; i++) {
            fscanf(fp, "%s %d %d",
                   students[i].name,
                   &students[i].roll,
                   &students[i].total);
        }
        fclose(fp);
    }

    /* Broadcast number of students */
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    chunk = n / size;

    if (chunk == 0) {
        if (rank == 0)
            printf("Error: Number of processes > number of students\n");
        MPI_Finalize();
        return 0;
    }

    struct Student local[chunk];

    /* Scatter student data */
    MPI_Scatter(students,
                chunk * sizeof(struct Student), MPI_BYTE,
                local,
                chunk * sizeof(struct Student), MPI_BYTE,
                0, MPI_COMM_WORLD);

    /* Each process calculates grade */
    for (int i = 0; i < chunk; i++) {
        local[i].grade = findGrade(local[i].total);
    }

    /* Gather results */
    MPI_Gather(local,
               chunk * sizeof(struct Student), MPI_BYTE,
               students,
               chunk * sizeof(struct Student), MPI_BYTE,
               0, MPI_COMM_WORLD);

    /* Master writes output file */
    if (rank == 0) {
        FILE *fp = fopen("grades.txt", "w");
        fprintf(fp, "Name Roll Marks Grade\n");

        for (int i = 0; i < n; i++) {
            fprintf(fp, "%s %d %d %c\n",
                    students[i].name,
                    students[i].roll,
                    students[i].total,
                    students[i].grade);
        }
        fclose(fp);
        printf("Output written to grades.txt\n");
    }

    MPI_Finalize();
    return 0;
}
sivajothi@sivajothi-HP-Laptop-15s-fq5xxx:~$ cat students.txt
5
Ravi 101 85
Anu 102 92
Kumar 103 67
Divya 104 74
Siva 105 45
sivajothi@sivajothi-HP-Laptop-15s-fq5xxx:~$ cat grades.txt
Name Roll Marks Grade
Ravi 101 85 B
Anu 102 92 A
Kumar 103 67 C
Divya 104 74 C
Siva 105 45 F
sivajothi@sivajothi-HP-Laptop-15s-fq5xxx:~$ 
