#include <stdlib.h>

#include <tea.h>

#include "fs.h"

static void check_error(tea_State* T, int err, const char* str)
{
    if(!err)
        return;
    if(err == FS_ENOWRITEPATH || !str)
    {
        tea_error(T, "%s", fs_errorStr(err));
    }
    tea_error(T, "%s '%s'", fs_errorStr(err), str);
}

static void filesystem_mount(tea_State* T)
{
    const char* path = tea_check_string(T, 0);
    int res = fs_mount(path);
    if(res != FS_ESUCCESS)
        tea_push_false(T);
    else
        tea_push_true(T);
}

static void filesystem_unmount(tea_State* T)
{
    const char* path = tea_check_string(T, 0);
    fs_unmount(path);
}

static void filesystem_setWritePath(tea_State* T)
{
    const char* path = tea_check_string(T, 0);
    int res = fs_setWritePath(path);
    check_error(T, res, path);
}

static void filesystem_exists(tea_State* T)
{
    const char* filename = tea_check_string(T, 0);
    tea_push_bool(T, fs_exists(filename));
}

static void filesystem_read(tea_State* T)
{
    const char* filename = tea_check_string(T, 0);
    size_t len;
    char* data = fs_read(filename, &len);
    if(!data)
    {
        tea_error(T, "could not read file '%s'", filename);
    }
    tea_push_lstring(T, data, len);
    free(data);
}

static void filesystem_write(tea_State* T)
{
    const char* filename = tea_check_string(T, 0);
    size_t len;
    const char* data = tea_check_lstring(T, 1, &len);
    int res = fs_write(filename, data, len);
    check_error(T, res, filename);
}

static void filesystem_delete(tea_State* T)
{
    const char* filename = tea_check_string(T, 0);
    int res = fs_delete(filename);
    if(res != FS_ESUCCESS)
        tea_push_false(T);
    else
        tea_push_true(T);
}

static void filesystem_getSize(tea_State* T)
{
    const char* filename = tea_check_string(T, 0);
    size_t sz;
    int res = fs_size(filename, &sz);
    check_error(T, res, filename);
    tea_push_number(T, sz);
}

static void filesystem_getModified(tea_State* T)
{
    const char* filename = tea_check_string(T, 0);
    unsigned t;
    int res = fs_modified(filename, &t);
    check_error(T, res, filename);
    tea_push_number(T, t);
}

static void filesystem_isDir(tea_State* T)
{
    const char* filename = tea_check_string(T, 0);
    tea_push_bool(T, fs_isDir(filename));
}

static void filesystem_listDir(tea_State* T)
{
    const char* path = tea_check_string(T, 0);
    fs_FileListNode* list = fs_listDir(path);
    tea_new_list(T, 0);
    fs_FileListNode* n = list;
    while(n)
    {
        tea_push_string(T, n->name);
        tea_add_item(T, -2);
        n = n->next;
    }
    fs_freeFileList(list);
}

static void filesystem_append(tea_State* T)
{
    const char* filename = tea_check_string(T, 0);
    size_t len;
    const char* data = tea_check_lstring(T, 1, &len);
    int res = fs_append(filename, data, len);
    check_error(T, res, filename);
}

static void filesystem_makeDirs(tea_State* T)
{
    const char* path = tea_check_string(T, 0);
    int res = fs_makeDirs(path);
    if(res != FS_ESUCCESS)
    {
        tea_error(T, "%s '%s'", fs_errorStr(res), path);
    }
}

static const tea_Reg reg[] = {
    { "mount", filesystem_mount, 1, 0 },
    { "unmount", filesystem_unmount, 1, 0 },
    { "setWritePath", filesystem_setWritePath, 1, 0 },
    { "read", filesystem_read, 1, 0 },
    { "exists", filesystem_exists, 1, 0 },
    { "write", filesystem_write, 2, 0 },
    { "delete", filesystem_delete, 1, 0 },
    { "getSize", filesystem_getSize, 1, 0 },
    { "getModified", filesystem_getModified, 1, 0 },
    { "isDir", filesystem_isDir, 1, 0 },
    { "listDir", filesystem_listDir, 1, 0 },
    { "append", filesystem_append, 2, 0 },
    { "makeDirs", filesystem_makeDirs, 1, 0 },
    { NULL, NULL }
};

void micro_open_filesystem(tea_State* T)
{
    tea_create_submodule(T, "filesystem", reg);
    atexit(fs_deinit);
}