/*

// Task 0: Calculate prime numbers using Sieve of Eratosthenes
void task0() {
    int n = 1000000; // Adjust the limit for different computation intensity
    bool prime[n+1]; //if this doesnt break ill eat my hat
    memset(prime, 1, sizeof(prime));

    for (int p = 2; p * p <= n; p++) {
        if (prime[p] == True) {
            for (int i = p * p; i <= n; i += p)
                prime[i] = 0;
        }
    }

    printf("Task 0: Prime numbers up to %d:\n", n);
    int highest = 0;
    for (int p = 2; p <= n; p++)
        if (prime[p])
            highest = p; 

    printf("%d ", highest);
    printf("\n");
}

// Task 1: Perform matrix multiplication of two 100x100 matrices
void task1() {
    int n = 100; // Adjust the matrix size for different computation intensity
    int a[100][100], b[100][100], c[100][100];

    // Initialize matrices a and b
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            a[i][j] = i + j;
            b[i][j] = i - j;
        }
    }

    // Perform matrix multiplication
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            c[i][j] = 0;
            for (int k = 0; k < n; k++)
                c[i][j] += a[i][k] * b[k][j];
        }
    }

    printf("Task 1: Matrix multiplication of %dx%d matrices completed\n", n, n);
}

// Task 2: Perform a series of calculations (CPU-intensive)
void task2() {
    unsigned long long result = 0;
    for (unsigned long long i = 0; i < 1000000000; i++) {
        result += i * i;
    }
    printf("Task 2: Calculation completed. Result: %llu\n", result);
}

// Task 3: Perform a series of calculations (CPU-intensive)
void task3() {
    unsigned long long result = 1;
    for (unsigned long long i = 1; i <= 20; i++) {
        result *= i;
    }
    printf("Task 3: Calculation completed. Result: %llu\n", result);
}

// Task 4: Calculate Fibonacci sequence iteratively
void task4() {
    unsigned long long n = 40; // Adjust this number for different computation intensity
    unsigned long long a = 0, b = 1, c;

    printf("Task 4: Calculating Fibonacci sequence for n = %llu...\n", n);
    for (unsigned long long i = 2; i <= n; i++) {
        c = a + b;
        a = b;
        b = c;
    }
    printf("Task 4: Result: %llu\n", b);
}

// Task 5: Perform a series of calculations (CPU-intensive)
void task5() {
    unsigned long long result = 0;
    for (unsigned long long i = 0; i < 1000000000; i++) {
        result += i * i * i;
    }
    printf("Task 5: Calculation completed. Result: %llu\n", result);
}

// Task 6: Perform a series of calculations (CPU-intensive)
void task6() {
    unsigned long long result = 0;
    for (unsigned long long i = 0; i < 1000000000; i++) {
        result += i * i * i * i;
    }
    printf("Task 6: Calculation completed. Result: %llu\n", result);
}

// Task 7: Perform a series of calculations (CPU-intensive)
void task7() {
    unsigned long long result = 0;
    for (unsigned long long i = 0; i < 1000000000; i++) {
        result += i * i * i * i * i;
    }
    printf("Task 7: Calculation completed. Result: %llu\n", result);
}

*/