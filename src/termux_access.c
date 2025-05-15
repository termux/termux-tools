#define _GNU_SOURCE
#include <dlfcn.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

// This library is needed to bypass W^X restriction in Dalvik code.
// https://cs.android.com/android/platform/superproject/+/dd6fde63193fda968440fc0a4cd9ccee5d282ab0:art/runtime/native/dalvik_system_DexFile.cc;l=381

static int stub_access(const char* __unused path, int __unused mode) {
    return -1;
}

// Function pointer for the original access function
static int (*orig_access)(const char *path, int mode) = stub_access;

// Constructor to load the original access function
__attribute__((constructor, visibility("hidden")))
static void load_orig_access(void) {
    orig_access = (int (*)(const char *, int))dlsym(RTLD_NEXT, "access");
    if (orig_access == NULL) {
        dprintf(2, "Error in `dlsym`: %s\n", dlerror());
        orig_access = stub_access;
    }
}

int access(const char *path, int mode) {
    // Checking if path ends with ".apk", if ".apk" is an extension and if mode is W_OK
    const char *apk = path == NULL ? NULL : strstr(path, ".apk");
    if (mode == W_OK && apk != NULL && !strcmp(apk, ".apk"))
        return 1;

    // Call the original access function for other cases
    return orig_access(path, mode);
}

