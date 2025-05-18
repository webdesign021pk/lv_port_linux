#include "app_registry.h"
#include "../system/app_manager.h"  // To use AppOpenFunc

#include <cjson/cJSON.h>

#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>


#define MAX_APPS 16
AppInfo app_registry[MAX_APPS];
int app_count = 0;

// Define a standard type for the application entry point function
// typedef void (*app_entry_point_t)(void);
typedef void (*app_entry_point_t)(lv_obj_t ** out_screen);  // ✅


void build_app_registry(const char* apps_base_path) {
    app_count = 0;
    DIR* dir = opendir(apps_base_path);
    if (!dir) return;

    struct dirent* entry;
    while ((entry = readdir(dir)) && app_count < MAX_APPS) {
        struct stat entry_stat;
        char entry_path[256];
        snprintf(entry_path, sizeof(entry_path), "%s/%s", apps_base_path, entry->d_name);
        if (stat(entry_path, &entry_stat) == 0 && S_ISDIR(entry_stat.st_mode) && entry->d_name[0] != '.') {
            char json_path[256];
            snprintf(json_path, sizeof(json_path), "%s/%s/app.json", apps_base_path, entry->d_name);

            FILE* file = fopen(json_path, "r");
            if (!file) continue;

            fseek(file, 0, SEEK_END);
            long len = ftell(file); rewind(file);

            char* content = malloc(len + 1);
            fread(content, 1, len, file); content[len] = '\0';
            fclose(file);

            cJSON* json = cJSON_Parse(content);
            free(content);
            if (!json) continue;

            const cJSON *name_json = cJSON_GetObjectItem(json, "name");
            const cJSON *icon_json = cJSON_GetObjectItem(json, "icon");
            const cJSON *library_json = cJSON_GetObjectItem(json, "library"); // Assuming app.json now has "library" field
            const cJSON *exec_name_json = cJSON_GetObjectItem(json, "exec_name"); // Name of the entry function in the library

            if (cJSON_IsString(name_json) && cJSON_IsString(icon_json) && cJSON_IsString(library_json) && cJSON_IsString(exec_name_json)) {
                AppInfo* app = &app_registry[app_count];
                snprintf(app->name, sizeof(app->name), "%s", name_json->valuestring);
                snprintf(app->icon_path, sizeof(app->icon_path), "A:/apps/%s/%s",
                entry->d_name, icon_json->valuestring);

                char library_path[256];
                snprintf(library_path, sizeof(library_path), "%s/%s/%s", apps_base_path, entry->d_name, library_json->valuestring);

                // Dynamically load the shared library
                void* library_handle = dlopen(library_path, RTLD_LAZY);
                if (library_handle) {
                    // Look up the entry point function within the loaded library
                    app_entry_point_t app_exec_fn = (app_entry_point_t)dlsym(library_handle, exec_name_json->valuestring);
                    if (app_exec_fn) {
                        // app->exec_fn = (void (*)(void))app_exec_fn; // Cast to your generic function pointer type
                        app->exec_fn = (AppOpenFunc)app_exec_fn;
                        app->library_handle = library_handle; // Store the handle for later unloading if needed
                        app_count++;
                    } else {
                        fprintf(stderr, "Error: Could not find symbol '%s' in library '%s'\n", exec_name_json->valuestring, library_path);
                        dlclose(library_handle); // Close the library on failure
                    }
                } else {
                    fprintf(stderr, "Error: Could not open library '%s': %s\n", library_path, dlerror());
                }
            }

            cJSON_Delete(json);
        }
    }

    closedir(dir);
}

// You might need a function to unload libraries when apps are closed or the system shuts down
void unload_app_libraries() {
    for (int i = 0; i < app_count; ++i) {
        if (app_registry[i].library_handle) {
            dlclose(app_registry[i].library_handle);
            app_registry[i].library_handle = NULL;
        }
    }
    app_count = 0;
}