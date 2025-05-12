// app_registry.h
#ifndef APP_REGISTRY_H
#define APP_REGISTRY_H

typedef struct {
    char name[64];
    char icon_path[128];
    void (*exec_fn)(void);
    void* library_handle; // 👈 needed for dlclose
} AppInfo;

extern AppInfo app_registry[];
extern int app_count;

void build_app_registry(const char* apps_base_path);

#endif