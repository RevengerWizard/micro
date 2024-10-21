#if _WIN32
#include <windows.h>
#elif __linux__
#include <unistd.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#endif

#include <GLFW/glfw3.h>

#include <tea.h>

static char* dirname(char* str)
{
    char* p = str + strlen(str);
    while(p != str)
    {
        if(*p == '/' || *p == '\\')
        {
            *p = '\0';
            break;
        }
        p--;
    }
    return str;
}

static void system_info(tea_State* T)
{
    const char* const opts[] = { "exedir", "appdata", NULL };
    
    int id = tea_check_option(T, 0, NULL, opts);
    switch(id)
    {
        case 0:
#if _WIN32
            char buf[1024];
            int len = GetModuleFileName(NULL, buf, sizeof(buf) - 1);
            buf[len] = '\0';
            dirname(buf);
            tea_push_string(T, buf);
#elif __linux__
            char path[128];
            char buf[1024];
            sprintf(path, "/proc/%d/exe", getpid());
            int len = readlink(path, buf, sizeof(buf) - 1);
            buf[len] = '\0';
            dirname(buf);
            tea_push_string(T, buf);
#elif __FreeBSD__
            /* TODO : Implement this */
            tea_push_string(T, ".");
#elif __APPLE__
            char buf[1024];
            uint32_t size = sizeof(buf);
            dirname(buf);
            tea_push_string(T, buf);
#else
            tea_push_string(T, ".");
#endif
            break;
        case 1:
#if _WIN32
            tea_push_string(T, getenv("APPDATA"));
#elif __APPLE__
            tea_push_fstring(T, "%s/Library/Application Support", getenv("HOME"));
#else
            tea_push_fstring(T, "%s/.local/share", getenv("HOME"));
#endif
            break;
    }
}

static void system_getClipboard(tea_State* T)
{

}

static void system_setClipboard(tea_State* T)
{
    const char* text = tea_check_string(T, 0);
}

static const tea_Reg reg[] = {
    { "info", system_info, 1, 0 },
    { "getClipboard", system_getClipboard, 0, 0 },
    { "setClipboard", system_setClipboard, 1, 0 },
    { NULL, NULL }
};

void micro_open_system(tea_State* T)
{
    tea_create_submodule(T, "system", reg);
}