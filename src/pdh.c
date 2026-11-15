typedef unsigned long DWORD;
typedef long LONG;
typedef int BOOL;
typedef unsigned long long SIZE_T, DWORD_PTR;
typedef void *HANDLE, *HMODULE, *FARPROC;
typedef unsigned short WCHAR;

__declspec(dllimport) HMODULE __stdcall GetModuleHandleW(const WCHAR *);
__declspec(dllimport) HMODULE __stdcall LoadLibraryW(const WCHAR *);
__declspec(dllimport) FARPROC __stdcall GetProcAddress(HMODULE, const char *);
__declspec(dllimport) BOOL __stdcall VirtualProtect(void *, SIZE_T, DWORD, DWORD *);
__declspec(dllimport) BOOL __stdcall FlushInstructionCache(HANDLE, const void *, SIZE_T);
__declspec(dllimport) HANDLE __stdcall GetCurrentProcess(void);
__declspec(dllimport) void __stdcall OutputDebugStringA(const char *);
__declspec(dllimport) unsigned int __stdcall GetSystemDirectoryW(WCHAR *, unsigned int);

static const unsigned char gore_ctx[52] = {
    0x40,0x84,0xff,0x75,0x45, 0x40,0x84,0xf6,0x74,0x28, 0xb9,0x01,0x00,0x00,0x00, 0xeb,0x3e,
    0x48,0x39,0x1d,0x28,0xb3,0xd7,0x05,
    0x74,0x2a, 0xe8,0xa9,0xd5,0xe4,0x01, 0x84,0xc0, 0x74,0x21, 0xe8,0x30,0xd7,0xe4,0x01, 0x84,0xc0,
    0x75,0x18, 0x8b,0x05,0x8e,0xdb,0xd0,0x03, 0x38,0x1d};
static const unsigned char locale_ctx[32] = {
    0xeb,0x0c,0xb9,0x87,0x00,0x00,0x00,0xeb,0x05,0xb9,0x88,0x00,0x00,0x00,
    0xe8,0x6d,0x01,0x01,0x00,
    0x4c,0x8b,0x05,0xfe,0xcf,0xa7,0x03,0x8b,0xcb,0x0f,0xb6,0xd0,0x40};

static int same(const unsigned char *a, const unsigned char *b, int n) {
    for (int i = 0; i < n; i++)
        if (a[i] != b[i]) return 0;
    return 1;
}

static void put(unsigned char *p, int n, const unsigned char *pos[], const unsigned char *val[], const int len[], int k) {
    DWORD old, tmp;
    if (!VirtualProtect(p, n, 0x40, &old)) return;
    for (int i = 0; i < k; i++)
        for (int j = 0; j < len[i]; j++) ((unsigned char *)pos[i])[j] = val[i][j];
    VirtualProtect(p, n, old, &tmp);
    FlushInstructionCache(GetCurrentProcess(), p, n);
}

static void patch(void) {
    unsigned char *base = (unsigned char *)GetModuleHandleW(0);
    if (!base || *(unsigned short *)base != 0x5A4D) return;
    static const unsigned char nop2[2] = {0x90, 0x90};
    static const unsigned char true5[5] = {0xb0, 0x01, 0x90, 0x90, 0x90};
    unsigned char *g = base + 0x11DA00UL - 24;
    if (same(g, gore_ctx, 52)) {
        const unsigned char *pos[3] = {g + 24, g + 33, g + 42};
        const unsigned char *val[3] = {nop2, nop2, nop2};
        const int len[3] = {2, 2, 2};
        put(g, 52, pos, val, len, 3);
        OutputDebugStringA("sc2uncensor: gore patched\n");
    }
    unsigned char *l = base + 0x118110UL;
    if (same(l, locale_ctx, 32)) {
        const unsigned char *pos[1] = {l + 14};
        const unsigned char *val[1] = {true5};
        const int len[1] = {5};
        put(l, 32, pos, val, len, 1);
        OutputDebugStringA("sc2uncensor: locale patched\n");
    }
}

static HMODULE real;
static FARPROC get(const char *name) {
    if (!real) {
        WCHAR path[300];
        static const WCHAR tail[] = L"\\pdh.dll";
        unsigned int n = GetSystemDirectoryW(path, 280);
        if (n == 0 || n >= 280) return 0;
        for (int i = 0; i < 9; i++) path[n + i] = tail[i];
        real = LoadLibraryW(path);
    }
    return real ? GetProcAddress(real, name) : 0;
}

__declspec(dllexport) LONG __stdcall PdhOpenQueryW(const WCHAR *src, DWORD_PTR user, HANDLE *q) {
    static LONG (__stdcall *f)(const WCHAR *, DWORD_PTR, HANDLE *);
    if (!f) f = (void *)get("PdhOpenQueryW");
    return f ? f(src, user, q) : (LONG)0xC0000BBBL;
}
__declspec(dllexport) LONG __stdcall PdhAddCounterA(HANDLE q, const char *path, DWORD_PTR user, HANDLE *c) {
    static LONG (__stdcall *f)(HANDLE, const char *, DWORD_PTR, HANDLE *);
    if (!f) f = (void *)get("PdhAddCounterA");
    return f ? f(q, path, user, c) : (LONG)0xC0000BBBL;
}
__declspec(dllexport) LONG __stdcall PdhCloseQuery(HANDLE q) {
    static LONG (__stdcall *f)(HANDLE);
    if (!f) f = (void *)get("PdhCloseQuery");
    return f ? f(q) : (LONG)0xC0000BBBL;
}

BOOL __stdcall DllMain(HANDLE h, DWORD reason, void *r) {
    if (reason == 1) patch();
    return 1;
}
