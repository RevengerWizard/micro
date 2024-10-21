#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #include <sys/time.h>
#endif

#include <GLFW/glfw3.h>

#include <tea.h>

static void timer_getNow(tea_State* T)
{
    double t;
#ifdef _WIN32
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    t = (ft.dwHighDateTime * 4294967296.0 / 1e7) + ft.dwLowDateTime / 1e7;
    t -= 11644473600.0;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    t = tv.tv_sec + tv.tv_usec / 1e6;
#endif
    tea_push_number(T, t);
}

static void timer_getTime(tea_State* T)
{
    tea_push_number(T, glfwGetTime());
}

static const tea_Reg reg[] = {
    { "getNow", timer_getNow, 0, 0 },
    { "getTime", timer_getTime, 0, 0 },
    { NULL, NULL }
};

void micro_open_timer(tea_State* T)
{
    tea_create_submodule(T, "timer", reg);
}