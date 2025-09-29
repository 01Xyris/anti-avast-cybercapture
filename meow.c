#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    uint8_t shutdown_found;
    uint8_t undock_found;
} token_info;

static int get_token(token_info* info) {
    HANDLE token;
    DWORD size = 0;
    TOKEN_PRIVILEGES* privs;
    
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token))
        return 0;
    
    GetTokenInformation(token, TokenPrivileges, NULL, 0, &size);
    if (!size) {
        CloseHandle(token);
        return 0;
    }
    
    privs = (TOKEN_PRIVILEGES*)malloc(size);
    if (!privs) {
        CloseHandle(token);
        return 0;
    }
    
    if (!GetTokenInformation(token, TokenPrivileges, privs, size, &size)) {
        free(privs);
        CloseHandle(token);
        return 0;
    }
    
    info->shutdown_found = 0;
    info->undock_found = 0;
    
    for (DWORD i = 0; i < privs->PrivilegeCount; i++) {
        wchar_t name[256];
        DWORD len = 256;
        
        if (LookupPrivilegeNameW(NULL, &privs->Privileges[i].Luid, name, &len)) {
            if (!wcscmp(name, L"SeShutdownPrivilege"))
                info->shutdown_found = 1;
            else if (!wcscmp(name, L"SeUndockPrivilege"))
                info->undock_found = 1;
        }
    }
    
    free(privs);
    CloseHandle(token);
    return 1;
}

static uint64_t factor_sum(uint64_t n) {
    uint64_t sum = 0;
    uint64_t temp = n;
    
    for (uint64_t i = 2; i * i <= temp; i++) {
        while (temp % i == 0) {
            sum += i;
            temp /= i;
        }
    }
    
    if (temp > 1)
        sum += temp;
        
    return sum;
}

static void compute_task(void) {
    const uint64_t limit = 10000000;
    double result = 0;
    LARGE_INTEGER freq, start, end;
    
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);
    
    printf("Computing prime factorization sums\n");
    printf("Range: 2 to %llu\n", limit);
    
    for (uint64_t i = 2; i < limit; i++) {
        uint64_t fs = factor_sum(i);
        result += (double)fs / i;
        
        if (i % 1000000 == 0) {
            printf("Progress: %llu/%llu (%.1f%%)\n", 
                   i, limit, (double)i * 100.0 / limit);
            printf("  Current number: %llu\n", i);
            printf("  Factor sum: %llu\n", fs);
            printf("  Running total: %.6f\n", result);
        }
    }
    
    QueryPerformanceCounter(&end);
    double elapsed = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
    
    printf("\nCompleted\n");
    printf("Time: %.2f seconds\n", elapsed);
    printf("Final result: %.6f\n", result);
    printf("Numbers processed: %llu\n", limit - 2);
}

int main(void) {
    token_info info;
    
    if (!get_token(&info)) {
        printf("Token query failed\n");
        return 1;
    }
    
    printf("SeShutdownPrivilege: %s\n", info.shutdown_found ? "Present" : "Missing");
    printf("SeUndockPrivilege: %s\n", info.undock_found ? "Present" : "Missing");
    
    if (!info.shutdown_found && !info.undock_found) {
        printf("\nSandbox detected\n\n");
        compute_task();
    } else {
        printf("\nNormal execution\n");
        getchar();
    }
    
    return 0;
}
