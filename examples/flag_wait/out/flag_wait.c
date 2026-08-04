#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct { uint8_t *ptr; size_t len; } klin_slice_u8;

typedef void (*klin_fn_mut_ptr_u8__void)(uint8_t *);
typedef int32_t (*klin_fn_mut_ptr_u8__i32)(uint8_t *);

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int64_t klin_time_wall_ns(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) return 0;
    return (int64_t)ts.tv_sec * 1000000000LL + (int64_t)ts.tv_nsec;
}

int64_t klin_time_mono_ns(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) return 0;
    return (int64_t)ts.tv_sec * 1000000000LL + (int64_t)ts.tv_nsec;
}

int32_t klin_time_format(uint8_t *buf, int32_t buflen, const char *fmt, int64_t unix_ns) {
    if (buf == NULL || buflen <= 0 || fmt == NULL) return -1;
    time_t sec = (time_t)(unix_ns / 1000000000LL);
    struct tm tm;
    if (gmtime_r(&sec, &tm) == NULL) return -1;
    size_t n = strftime((char *)buf, (size_t)buflen, fmt, &tm);
    if (n == 0) return -1;
    return (int32_t)n;
}

static int32_t klin_time_from_tm(int64_t *out_ns, struct tm *tm) {
    errno = 0;
    time_t sec = timegm(tm);
    if (sec == (time_t)-1 && errno != 0) return 2;
    *out_ns = (int64_t)sec * 1000000000LL;
    return 0;
}

int32_t klin_time_parse_iso(int64_t *out_ns, const char *s) {
    if (out_ns == NULL || s == NULL) return 1;
    int y = 0, mo = 0, d = 0, h = 0, mi = 0, sec = 0;
    int consumed = 0;
    if (sscanf(s, "%d-%d-%dT%d:%d:%dZ%n", &y, &mo, &d, &h, &mi, &sec, &consumed) == 6) {
        if (s[consumed] != '\0') return 1;
    } else if (sscanf(s, "%d-%d-%d%n", &y, &mo, &d, &consumed) == 3) {
        if (s[consumed] != '\0') return 1;
        h = 0; mi = 0; sec = 0;
    } else {
        return 1;
    }
    if (mo < 1 || mo > 12 || d < 1 || d > 31) return 1;
    struct tm tm;
    memset(&tm, 0, sizeof(tm));
    tm.tm_year = y - 1900;
    tm.tm_mon = mo - 1;
    tm.tm_mday = d;
    tm.tm_hour = h;
    tm.tm_min = mi;
    tm.tm_sec = sec;
    return klin_time_from_tm(out_ns, &tm);
}

int32_t klin_time_parse(int64_t *out_ns, const char *fmt, const char *s) {
    if (out_ns == NULL || fmt == NULL || s == NULL) return 1;
    struct tm tm;
    memset(&tm, 0, sizeof(tm));
    const char *end = strptime(s, fmt, &tm);
    if (end == NULL || *end != '\0') return 1;
    return klin_time_from_tm(out_ns, &tm);
}

int32_t klin_time_add_date(int64_t *out_ns, int64_t unix_ns, int32_t years, int32_t months, int32_t days) {
    if (out_ns == NULL) return 1;
    int64_t sec = unix_ns / 1000000000LL;
    int64_t frac = unix_ns % 1000000000LL;
    if (frac < 0) {
        frac += 1000000000LL;
        sec -= 1;
    }
    time_t tsec = (time_t)sec;
    struct tm tm;
    if (gmtime_r(&tsec, &tm) == NULL) return 1;
    tm.tm_year += (int)years;
    tm.tm_mon += (int)months;
    tm.tm_mday += (int)days;
    tm.tm_isdst = 0;
    int32_t rc = klin_time_from_tm(out_ns, &tm);
    if (rc != 0) return rc;
    *out_ns += frac;
    return 0;
}

#line 10 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
typedef struct {
    int32_t id;
    bool active;
    bool repeating;
    int64_t interval_ms;
    int64_t deadline_ns;
    void (*cb)(uint8_t *);
    uint8_t * ctx;
} eventloop_Slot;

#line 23 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
typedef struct {
    bool active;
    int32_t (*poll)(uint8_t *);
    uint8_t state[256];
} eventloop_TaskSlot;

#line 29 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
typedef struct {
    eventloop_Slot slots[16];
    eventloop_TaskSlot tasks[8];
    bool running;
    int32_t next_id;
} eventloop_Executor;

#line 37 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
typedef struct {
    int64_t deadline_ns;
} eventloop_SleepFuture;

#line 44 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
typedef struct {
    int32_t set;
} eventloop_Flag;

#line 49 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
typedef struct {
    eventloop_Flag * flag;
} eventloop_FlagFuture;

#line 6 "/workspace/stdlib/time.kl"
typedef struct {
    int64_t ns;
} time_Duration;

#line 11 "/workspace/stdlib/time.kl"
typedef struct {
    int64_t unix_ns;
} time_Instant;

#line 16 "/workspace/stdlib/time.kl"
typedef struct {
    int64_t ns;
} time_MonoInstant;

typedef struct {
    bool is_err;
    union { int32_t ok; int32_t err; } u;
} klin_res_i32;

typedef struct {
    bool is_err;
    union { time_Instant ok; int32_t err; } u;
} klin_res_time_Instant;

eventloop_Flag * flag_cell(void);
static void flag_wait_on_fake_irq(uint8_t * ctx);
int main(void);
static void eventloop__clear_slots(eventloop_Executor * ex);
static void eventloop__clear_tasks(eventloop_Executor * ex);
static klin_res_i32 eventloop__alloc_slot(eventloop_Executor * ex);
static bool eventloop__has_timers(eventloop_Executor * ex);
static bool eventloop__has_tasks(eventloop_Executor * ex);
static bool eventloop__has_work(eventloop_Executor * ex);
static int64_t eventloop__next_deadline_ns(eventloop_Executor * ex);
static klin_res_i32 eventloop__arm(eventloop_Executor * ex, int64_t ms, bool repeating, void (*cb)(uint8_t *), uint8_t * ctx);
static void eventloop__poll_tasks(eventloop_Executor * ex);
klin_res_i32 eventloop_init(eventloop_Executor * ex);
klin_res_i32 eventloop_every_ms(eventloop_Executor * ex, int64_t ms, void (*cb)(uint8_t *), uint8_t * ctx);
klin_res_i32 eventloop_once_ms(eventloop_Executor * ex, int64_t ms, void (*cb)(uint8_t *), uint8_t * ctx);
void eventloop_cancel(eventloop_Executor * ex, int32_t id);
void eventloop_stop(eventloop_Executor * ex);
eventloop_SleepFuture eventloop_sleep_ms(int64_t ms);
static int32_t eventloop_SleepFuture_poll(eventloop_SleepFuture *f);
static volatile int32_t * eventloop__flag_ptr(eventloop_Flag * f);
void eventloop_flag_init(eventloop_Flag * f);
void eventloop_flag_set(eventloop_Flag * f);
void eventloop_flag_clear(eventloop_Flag * f);
eventloop_FlagFuture eventloop_flag_wait(eventloop_Flag * f);
static int32_t eventloop_FlagFuture_poll(eventloop_FlagFuture *fut);
klin_res_i32 eventloop_spawn(eventloop_Executor * ex, int32_t (*poll)(uint8_t *), void (*init)(uint8_t *));
void eventloop_run(eventloop_Executor * ex);
int32_t eventloop_version(void);
int64_t klin_time_wall_ns(void);
int64_t klin_time_mono_ns(void);
int32_t klin_time_format(uint8_t * buf, int32_t buflen, const char* fmt, int64_t unix_ns);
int32_t klin_time_parse_iso(int64_t * out_ns, const char* s);
int32_t klin_time_parse(int64_t * out_ns, const char* fmt, const char* s);
int32_t klin_time_add_date(int64_t * out_ns, int64_t unix_ns, int32_t years, int32_t months, int32_t days);
time_Duration time_nanosecond(void);
time_Duration time_microsecond(void);
time_Duration time_millisecond(void);
time_Duration time_second(void);
time_Duration time_minute(void);
time_Duration time_hour(void);
time_Duration time_nanoseconds(int64_t n);
time_Duration time_microseconds(int64_t n);
time_Duration time_milliseconds(int64_t n);
time_Duration time_seconds(int64_t n);
time_Duration time_minutes(int64_t n);
time_Duration time_hours(int64_t n);
int64_t time_Duration_as_ns(time_Duration d);
int64_t time_Duration_as_us(time_Duration d);
int64_t time_Duration_as_ms(time_Duration d);
int64_t time_Duration_as_s(time_Duration d);
time_Duration time_Duration_add(time_Duration a, time_Duration b);
time_Duration time_Duration_sub(time_Duration a, time_Duration b);
time_Duration time_Duration_abs(time_Duration d);
time_Duration time_Duration_mul(time_Duration d, int64_t n);
time_Instant time_now(void);
time_MonoInstant time_mono(void);
time_Instant time_unix(int64_t sec);
time_Instant time_unix_ns(int64_t ns);
time_Duration time_between(time_Instant a, time_Instant b);
time_Duration time_since(time_Instant a);
time_Duration time_mono_between(time_MonoInstant a, time_MonoInstant b);
time_Duration time_mono_since(time_MonoInstant a);
time_Instant time_Instant_add(time_Instant t, time_Duration d);
time_Instant time_Instant_sub(time_Instant t, time_Duration d);
klin_res_time_Instant time_Instant_add_date(time_Instant t, int32_t years, int32_t months, int32_t days);
klin_res_time_Instant time_Instant_add_years(time_Instant t, int32_t n);
klin_res_time_Instant time_Instant_add_months(time_Instant t, int32_t n);
klin_res_time_Instant time_Instant_add_days(time_Instant t, int32_t n);
time_Duration time_Instant_until(time_Instant t, time_Instant u);
time_MonoInstant time_MonoInstant_add(time_MonoInstant m, time_Duration d);
time_MonoInstant time_MonoInstant_sub(time_MonoInstant m, time_Duration d);
time_Duration time_MonoInstant_until(time_MonoInstant a, time_MonoInstant b);
bool time_Instant_before(time_Instant t, time_Instant u);
bool time_Instant_after(time_Instant t, time_Instant u);
bool time_Instant_equal(time_Instant t, time_Instant u);
int32_t time_format(klin_slice_u8 buf, const char* fmt, time_Instant t);
int32_t time_Instant_format(time_Instant t, klin_slice_u8 buf, const char* fmt);
klin_res_time_Instant time_parse_iso(const char* s);
klin_res_time_Instant time_parse(const char* fmt, const char* s);
int32_t puts(const char* msg);
void io_print(const char* msg);

typedef struct {
    int32_t __stage;
    eventloop_FlagFuture __aw0;
} flag_wait_reader_State;

static void flag_wait_reader_init(flag_wait_reader_State *st) {
    st->__stage = 0;
}

static int32_t flag_wait_reader_poll(flag_wait_reader_State *st) {
    switch (st->__stage) {
    case 0:
#line 18 "/tmp/eventloop-flag/examples/flag_wait/flag_wait.kl"
        st->__aw0 = eventloop_flag_wait(flag_cell());
        st->__stage = 1;
        /* fallthrough */
        case 1:
        if (eventloop_FlagFuture_poll(&st->__aw0) == 0) return 0;
#line 19 "/tmp/eventloop-flag/examples/flag_wait/flag_wait.kl"
        puts("flag ready — continued after await");
        st->__stage = -1;
        return 1;
    default:
        return 1;
    }
}

static void flag_wait_reader_init_erased(uint8_t *p) {
    flag_wait_reader_init((flag_wait_reader_State *)p);
}

static int32_t flag_wait_reader_poll_erased(uint8_t *p) {
    return flag_wait_reader_poll((flag_wait_reader_State *)p);
}

#line 12 "/tmp/eventloop-flag/examples/flag_wait/flag_wait.kl"
static void flag_wait_on_fake_irq(uint8_t * ctx) {
#line 13 "/tmp/eventloop-flag/examples/flag_wait/flag_wait.kl"
    uint8_t * _ = ctx;
#line 14 "/tmp/eventloop-flag/examples/flag_wait/flag_wait.kl"
    eventloop_flag_set(flag_cell());
}

#line 22 "/tmp/eventloop-flag/examples/flag_wait/flag_wait.kl"
int main(void) {
#line 23 "/tmp/eventloop-flag/examples/flag_wait/flag_wait.kl"
    eventloop_flag_init(flag_cell());
#line 25 "/tmp/eventloop-flag/examples/flag_wait/flag_wait.kl"
    eventloop_Executor ex = {0};
#line 26 "/tmp/eventloop-flag/examples/flag_wait/flag_wait.kl"
    int32_t rc_ex;
    klin_res_i32 klin_val_0 = eventloop_init(&(ex));
    if (klin_val_0.is_err) {
        int32_t err = klin_val_0.u.err;
        rc_ex = 1;
    } else {
        rc_ex = klin_val_0.u.ok;
    }
#line 27 "/tmp/eventloop-flag/examples/flag_wait/flag_wait.kl"
    if ((rc_ex != 0)) {
#line 28 "/tmp/eventloop-flag/examples/flag_wait/flag_wait.kl"
        return 0;
    }
#line 31 "/tmp/eventloop-flag/examples/flag_wait/flag_wait.kl"
    int32_t arm_rc;
    klin_res_i32 klin_val_1 = eventloop_once_ms(&(ex), 30, flag_wait_on_fake_irq, (uint8_t *)(uintptr_t)(0));
    if (klin_val_1.is_err) {
        int32_t err = klin_val_1.u.err;
        arm_rc = 0;
    } else {
        arm_rc = klin_val_1.u.ok;
    }
#line 34 "/tmp/eventloop-flag/examples/flag_wait/flag_wait.kl"
    if ((arm_rc == 0)) {
#line 35 "/tmp/eventloop-flag/examples/flag_wait/flag_wait.kl"
        return 0;
    }
#line 37 "/tmp/eventloop-flag/examples/flag_wait/flag_wait.kl"
    int32_t spawn_rc;
    klin_res_i32 klin_val_2 = eventloop_spawn(&(ex), flag_wait_reader_poll_erased, flag_wait_reader_init_erased);
    if (klin_val_2.is_err) {
        int32_t err = klin_val_2.u.err;
        spawn_rc = (0 - 1);
    } else {
        spawn_rc = klin_val_2.u.ok;
    }
#line 38 "/tmp/eventloop-flag/examples/flag_wait/flag_wait.kl"
    if ((spawn_rc < 0)) {
#line 39 "/tmp/eventloop-flag/examples/flag_wait/flag_wait.kl"
        return 0;
    }
#line 42 "/tmp/eventloop-flag/examples/flag_wait/flag_wait.kl"
    eventloop_run(&(ex));
#line 44 "/tmp/eventloop-flag/examples/flag_wait/flag_wait.kl"
    printf("flag_wait demo done version=%d\n", eventloop_version());
    return 0;
}

#line 53 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
static void eventloop__clear_slots(eventloop_Executor * ex) {
#line 54 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    int32_t i = 0;
#line 55 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    while ((i < 16)) {
#line 56 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        (*(ex)).slots[i].id = 0;
#line 57 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        (*(ex)).slots[i].active = false;
#line 58 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        (*(ex)).slots[i].repeating = false;
#line 59 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        (*(ex)).slots[i].interval_ms = 0;
#line 60 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        (*(ex)).slots[i].deadline_ns = 0;
#line 61 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        (*(ex)).slots[i].ctx = (uint8_t *)(uintptr_t)(0);
#line 62 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        i = (i + 1);
    }
}

#line 66 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
static void eventloop__clear_tasks(eventloop_Executor * ex) {
#line 67 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    int32_t i = 0;
#line 68 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    while ((i < 8)) {
#line 69 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        (*(ex)).tasks[i].active = false;
#line 70 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        i = (i + 1);
    }
}

#line 74 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
static klin_res_i32 eventloop__alloc_slot(eventloop_Executor * ex) {
#line 75 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    int32_t i = 0;
#line 76 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    while ((i < 16)) {
#line 77 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        if (!(((*(ex)).slots[i].active))) {
#line 78 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
            klin_res_i32 klin_ret_0 = (klin_res_i32){ .is_err = false, .u.ok = i };
            return klin_ret_0;
        }
#line 80 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        i = (i + 1);
    }
#line 82 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    klin_res_i32 klin_ret_1 = (klin_res_i32){ .is_err = true, .u.err = 1 };
    return klin_ret_1;
}

#line 85 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
static bool eventloop__has_timers(eventloop_Executor * ex) {
#line 86 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    int32_t i = 0;
#line 87 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    while ((i < 16)) {
#line 88 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        if ((*(ex)).slots[i].active) {
#line 89 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
            bool klin_ret_0 = true;
            return klin_ret_0;
        }
#line 91 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        i = (i + 1);
    }
#line 93 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    bool klin_ret_1 = false;
    return klin_ret_1;
}

#line 96 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
static bool eventloop__has_tasks(eventloop_Executor * ex) {
#line 97 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    int32_t i = 0;
#line 98 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    while ((i < 8)) {
#line 99 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        if ((*(ex)).tasks[i].active) {
#line 100 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
            bool klin_ret_0 = true;
            return klin_ret_0;
        }
#line 102 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        i = (i + 1);
    }
#line 104 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    bool klin_ret_1 = false;
    return klin_ret_1;
}

#line 107 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
static bool eventloop__has_work(eventloop_Executor * ex) {
#line 108 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    if (eventloop__has_timers(ex)) {
#line 109 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        bool klin_ret_0 = true;
        return klin_ret_0;
    }
#line 111 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    bool klin_ret_1 = eventloop__has_tasks(ex);
    return klin_ret_1;
}

#line 114 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
static int64_t eventloop__next_deadline_ns(eventloop_Executor * ex) {
#line 115 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    int64_t best = 0;
#line 116 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    bool have = false;
#line 117 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    int32_t i = 0;
#line 118 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    while ((i < 16)) {
#line 119 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        if ((*(ex)).slots[i].active) {
#line 120 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
            int64_t d = (*(ex)).slots[i].deadline_ns;
#line 121 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
            if (!((have))) {
#line 122 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                best = d;
#line 123 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                have = true;
#line 124 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
            } else if ((d < best)) {
#line 125 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                best = d;
            }
        }
#line 128 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        i = (i + 1);
    }
#line 130 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    int64_t klin_ret_0 = best;
    return klin_ret_0;
}

#line 133 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
static klin_res_i32 eventloop__arm(eventloop_Executor * ex, int64_t ms, bool repeating, void (*cb)(uint8_t *), uint8_t * ctx) {
#line 134 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    if ((ms < 1)) {
#line 135 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        klin_res_i32 klin_ret_0 = (klin_res_i32){ .is_err = true, .u.err = 2 };
        return klin_ret_0;
    }
#line 137 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    int32_t idx;
    klin_res_i32 klin_val_0 = eventloop__alloc_slot(ex);
    if (klin_val_0.is_err) {
        return klin_val_0;
    }
    idx = klin_val_0.u.ok;
#line 138 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    int32_t id = (*(ex)).next_id;
#line 139 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    (*(ex)).next_id = ((*(ex)).next_id + 1);
#line 140 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    time_MonoInstant now = time_mono();
#line 141 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    time_MonoInstant deadline = time_MonoInstant_add(now, time_milliseconds(ms));
#line 142 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    (*(ex)).slots[idx].id = id;
#line 143 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    (*(ex)).slots[idx].active = true;
#line 144 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    (*(ex)).slots[idx].repeating = repeating;
#line 145 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    (*(ex)).slots[idx].interval_ms = ms;
#line 146 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    (*(ex)).slots[idx].deadline_ns = deadline.ns;
#line 147 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    (*(ex)).slots[idx].cb = cb;
#line 148 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    (*(ex)).slots[idx].ctx = ctx;
#line 149 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    klin_res_i32 klin_ret_1 = (klin_res_i32){ .is_err = false, .u.ok = id };
    return klin_ret_1;
}

#line 152 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
static void eventloop__poll_tasks(eventloop_Executor * ex) {
#line 153 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    int32_t i = 0;
#line 154 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    while ((i < 8)) {
#line 155 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        if ((*(ex)).tasks[i].active) {
#line 156 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
            int32_t (*poll)(uint8_t *) = (*(ex)).tasks[i].poll;
#line 157 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
            int32_t rc = poll((uint8_t *)(uintptr_t)(&((*(ex)).tasks[i].state[0])));
#line 158 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
            if ((rc != 0)) {
#line 159 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                (*(ex)).tasks[i].active = false;
            }
        }
#line 162 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        i = (i + 1);
    }
}

#line 167 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
klin_res_i32 eventloop_init(eventloop_Executor * ex) {
#line 168 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    (*(ex)).running = false;
#line 169 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    (*(ex)).next_id = 1;
#line 170 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    eventloop__clear_slots(ex);
#line 171 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    eventloop__clear_tasks(ex);
#line 172 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    klin_res_i32 klin_ret_0 = (klin_res_i32){ .is_err = false, .u.ok = 0 };
    return klin_ret_0;
}

#line 176 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
klin_res_i32 eventloop_every_ms(eventloop_Executor * ex, int64_t ms, void (*cb)(uint8_t *), uint8_t * ctx) {
#line 177 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    klin_res_i32 klin_ret_0 = eventloop__arm(ex, ms, true, cb, ctx);
    return klin_ret_0;
}

#line 181 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
klin_res_i32 eventloop_once_ms(eventloop_Executor * ex, int64_t ms, void (*cb)(uint8_t *), uint8_t * ctx) {
#line 182 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    klin_res_i32 klin_ret_0 = eventloop__arm(ex, ms, false, cb, ctx);
    return klin_ret_0;
}

#line 185 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
void eventloop_cancel(eventloop_Executor * ex, int32_t id) {
#line 186 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    int32_t i = 0;
#line 187 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    while ((i < 16)) {
#line 188 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        if ((*(ex)).slots[i].active) {
#line 189 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
            if (((*(ex)).slots[i].id == id)) {
#line 190 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                (*(ex)).slots[i].active = false;
#line 191 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                (*(ex)).slots[i].id = 0;
#line 192 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                (*(ex)).slots[i].ctx = (uint8_t *)(uintptr_t)(0);
#line 193 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                return;
            }
        }
#line 196 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        i = (i + 1);
    }
}

#line 201 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
void eventloop_stop(eventloop_Executor * ex) {
#line 202 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    (*(ex)).running = false;
}

#line 206 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
eventloop_SleepFuture eventloop_sleep_ms(int64_t ms) {
#line 207 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    time_MonoInstant now = time_mono();
#line 208 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    if ((ms < 1)) {
#line 209 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        eventloop_SleepFuture klin_ret_0 = (eventloop_SleepFuture){ .deadline_ns = now.ns };
        return klin_ret_0;
    }
#line 211 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    time_MonoInstant deadline = time_MonoInstant_add(now, time_milliseconds(ms));
#line 212 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    eventloop_SleepFuture klin_ret_1 = (eventloop_SleepFuture){ .deadline_ns = deadline.ns };
    return klin_ret_1;
}

#line 215 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
static int32_t eventloop_SleepFuture_poll(eventloop_SleepFuture *f) {
#line 216 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    time_MonoInstant now = time_mono();
#line 217 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    if ((now.ns >= f->deadline_ns)) {
#line 218 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        int32_t klin_ret_0 = 1;
        return klin_ret_0;
    }
#line 220 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    int32_t klin_ret_1 = 0;
    return klin_ret_1;
}

#line 223 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
static volatile int32_t * eventloop__flag_ptr(eventloop_Flag * f) {
#line 224 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    volatile int32_t * klin_ret_0 = (volatile int32_t *)(uintptr_t)(&((*(f)).set));
    return klin_ret_0;
}

#line 228 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
void eventloop_flag_init(eventloop_Flag * f) {
#line 229 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    volatile int32_t * p = eventloop__flag_ptr(f);
#line 230 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    *(p) = 0;
}

#line 234 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
void eventloop_flag_set(eventloop_Flag * f) {
#line 235 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    volatile int32_t * p = eventloop__flag_ptr(f);
#line 236 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    *(p) = 1;
}

#line 240 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
void eventloop_flag_clear(eventloop_Flag * f) {
#line 241 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    volatile int32_t * p = eventloop__flag_ptr(f);
#line 242 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    *(p) = 0;
}

#line 247 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
eventloop_FlagFuture eventloop_flag_wait(eventloop_Flag * f) {
#line 248 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    eventloop_FlagFuture klin_ret_0 = (eventloop_FlagFuture){ .flag = f };
    return klin_ret_0;
}

#line 251 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
static int32_t eventloop_FlagFuture_poll(eventloop_FlagFuture *fut) {
#line 252 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    volatile int32_t * p = eventloop__flag_ptr(fut->flag);
#line 253 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    if ((*(p) != 0)) {
#line 254 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        *(p) = 0;
#line 255 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        int32_t klin_ret_0 = 1;
        return klin_ret_0;
    }
#line 257 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    int32_t klin_ret_1 = 0;
    return klin_ret_1;
}

#line 262 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
klin_res_i32 eventloop_spawn(eventloop_Executor * ex, int32_t (*poll)(uint8_t *), void (*init)(uint8_t *)) {
#line 263 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    int32_t i = 0;
#line 264 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    while ((i < 8)) {
#line 265 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        if (!(((*(ex)).tasks[i].active))) {
#line 266 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
            (*(ex)).tasks[i].active = true;
#line 267 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
            (*(ex)).tasks[i].poll = poll;
#line 268 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
            init((uint8_t *)(uintptr_t)(&((*(ex)).tasks[i].state[0])));
#line 269 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
            klin_res_i32 klin_ret_0 = (klin_res_i32){ .is_err = false, .u.ok = i };
            return klin_ret_0;
        }
#line 271 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        i = (i + 1);
    }
#line 273 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    klin_res_i32 klin_ret_1 = (klin_res_i32){ .is_err = true, .u.err = 1 };
    return klin_ret_1;
}

#line 277 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
void eventloop_run(eventloop_Executor * ex) {
#line 278 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    (*(ex)).running = true;
#line 279 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
    while ((*(ex)).running) {
#line 280 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        if (!((eventloop__has_work((eventloop_Executor *)(uintptr_t)(ex))))) {
#line 281 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
            (*(ex)).running = false;
#line 282 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
            return;
        }
#line 284 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        time_MonoInstant now = time_mono();
#line 285 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        int32_t i = 0;
#line 286 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        while ((i < 16)) {
#line 287 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
            if ((*(ex)).slots[i].active) {
#line 288 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                if (((*(ex)).slots[i].deadline_ns <= now.ns)) {
#line 289 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                    void (*cb)(uint8_t *) = (*(ex)).slots[i].cb;
#line 290 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                    uint8_t * ctx = (*(ex)).slots[i].ctx;
#line 291 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                    if ((*(ex)).slots[i].repeating) {
#line 292 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                        time_MonoInstant next = time_MonoInstant_add(now, time_milliseconds((*(ex)).slots[i].interval_ms));
#line 293 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                        (*(ex)).slots[i].deadline_ns = next.ns;
                    } else {
#line 295 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                        (*(ex)).slots[i].active = false;
#line 296 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                        (*(ex)).slots[i].id = 0;
#line 297 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                        (*(ex)).slots[i].ctx = (uint8_t *)(uintptr_t)(0);
                    }
#line 299 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                    cb(ctx);
#line 300 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                    if (!(((*(ex)).running))) {
#line 301 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                        return;
                    }
                }
            }
#line 305 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
            i = (i + 1);
        }
#line 307 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        eventloop__poll_tasks(ex);
#line 308 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        if (!(((*(ex)).running))) {
#line 309 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
            return;
        }
#line 311 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        if (!((eventloop__has_work((eventloop_Executor *)(uintptr_t)(ex))))) {
#line 312 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
            (*(ex)).running = false;
#line 313 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
            return;
        }
#line 315 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
        if (eventloop__has_timers((eventloop_Executor *)(uintptr_t)(ex))) {
#line 316 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
            int64_t target = eventloop__next_deadline_ns((eventloop_Executor *)(uintptr_t)(ex));
#line 318 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
            while ((*(ex)).running) {
#line 319 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                eventloop__poll_tasks(ex);
#line 320 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                if (!(((*(ex)).running))) {
#line 321 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                    return;
                }
#line 323 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                if (!((eventloop__has_timers((eventloop_Executor *)(uintptr_t)(ex))))) {
#line 324 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                    break;
                }
#line 326 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                time_MonoInstant t = time_mono();
#line 327 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                if ((t.ns >= target)) {
#line 328 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/executor.kl"
                    break;
                }
            }
        }
    }
}

#line 3 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/version.kl"
int32_t eventloop_version(void) {
#line 4 "/tmp/eventloop-flag/examples/flag_wait/../../eventloop/version.kl"
    int32_t klin_ret_0 = 4;
    return klin_ret_0;
}

#line 38 "/workspace/stdlib/time.kl"
time_Duration time_nanosecond(void) {
#line 39 "/workspace/stdlib/time.kl"
    time_Duration klin_ret_0 = (time_Duration){ .ns = 1 };
    return klin_ret_0;
}

#line 42 "/workspace/stdlib/time.kl"
time_Duration time_microsecond(void) {
#line 43 "/workspace/stdlib/time.kl"
    time_Duration klin_ret_0 = (time_Duration){ .ns = 1000 };
    return klin_ret_0;
}

#line 46 "/workspace/stdlib/time.kl"
time_Duration time_millisecond(void) {
#line 47 "/workspace/stdlib/time.kl"
    time_Duration klin_ret_0 = (time_Duration){ .ns = 1000000 };
    return klin_ret_0;
}

#line 50 "/workspace/stdlib/time.kl"
time_Duration time_second(void) {
#line 51 "/workspace/stdlib/time.kl"
    time_Duration klin_ret_0 = (time_Duration){ .ns = 1000000000 };
    return klin_ret_0;
}

#line 54 "/workspace/stdlib/time.kl"
time_Duration time_minute(void) {
#line 55 "/workspace/stdlib/time.kl"
    time_Duration klin_ret_0 = (time_Duration){ .ns = 60000000000 };
    return klin_ret_0;
}

#line 58 "/workspace/stdlib/time.kl"
time_Duration time_hour(void) {
#line 59 "/workspace/stdlib/time.kl"
    time_Duration klin_ret_0 = (time_Duration){ .ns = 3600000000000 };
    return klin_ret_0;
}

#line 62 "/workspace/stdlib/time.kl"
time_Duration time_nanoseconds(int64_t n) {
#line 63 "/workspace/stdlib/time.kl"
    time_Duration klin_ret_0 = (time_Duration){ .ns = n };
    return klin_ret_0;
}

#line 66 "/workspace/stdlib/time.kl"
time_Duration time_microseconds(int64_t n) {
#line 67 "/workspace/stdlib/time.kl"
    time_Duration klin_ret_0 = (time_Duration){ .ns = (n * 1000) };
    return klin_ret_0;
}

#line 70 "/workspace/stdlib/time.kl"
time_Duration time_milliseconds(int64_t n) {
#line 71 "/workspace/stdlib/time.kl"
    time_Duration klin_ret_0 = (time_Duration){ .ns = (n * 1000000) };
    return klin_ret_0;
}

#line 74 "/workspace/stdlib/time.kl"
time_Duration time_seconds(int64_t n) {
#line 75 "/workspace/stdlib/time.kl"
    time_Duration klin_ret_0 = (time_Duration){ .ns = (n * 1000000000) };
    return klin_ret_0;
}

#line 78 "/workspace/stdlib/time.kl"
time_Duration time_minutes(int64_t n) {
#line 79 "/workspace/stdlib/time.kl"
    time_Duration klin_ret_0 = (time_Duration){ .ns = (n * 60000000000) };
    return klin_ret_0;
}

#line 82 "/workspace/stdlib/time.kl"
time_Duration time_hours(int64_t n) {
#line 83 "/workspace/stdlib/time.kl"
    time_Duration klin_ret_0 = (time_Duration){ .ns = (n * 3600000000000) };
    return klin_ret_0;
}

#line 86 "/workspace/stdlib/time.kl"
int64_t time_Duration_as_ns(time_Duration d) {
#line 87 "/workspace/stdlib/time.kl"
    int64_t klin_ret_0 = d.ns;
    return klin_ret_0;
}

#line 90 "/workspace/stdlib/time.kl"
int64_t time_Duration_as_us(time_Duration d) {
#line 91 "/workspace/stdlib/time.kl"
    int64_t klin_ret_0 = (d.ns / 1000);
    return klin_ret_0;
}

#line 94 "/workspace/stdlib/time.kl"
int64_t time_Duration_as_ms(time_Duration d) {
#line 95 "/workspace/stdlib/time.kl"
    int64_t klin_ret_0 = (d.ns / 1000000);
    return klin_ret_0;
}

#line 98 "/workspace/stdlib/time.kl"
int64_t time_Duration_as_s(time_Duration d) {
#line 99 "/workspace/stdlib/time.kl"
    int64_t klin_ret_0 = (d.ns / 1000000000);
    return klin_ret_0;
}

#line 102 "/workspace/stdlib/time.kl"
time_Duration time_Duration_add(time_Duration a, time_Duration b) {
#line 103 "/workspace/stdlib/time.kl"
    time_Duration klin_ret_0 = (time_Duration){ .ns = (a.ns + b.ns) };
    return klin_ret_0;
}

#line 106 "/workspace/stdlib/time.kl"
time_Duration time_Duration_sub(time_Duration a, time_Duration b) {
#line 107 "/workspace/stdlib/time.kl"
    time_Duration klin_ret_0 = (time_Duration){ .ns = (a.ns - b.ns) };
    return klin_ret_0;
}

#line 110 "/workspace/stdlib/time.kl"
time_Duration time_Duration_abs(time_Duration d) {
#line 111 "/workspace/stdlib/time.kl"
    if ((d.ns < 0)) {
#line 112 "/workspace/stdlib/time.kl"
        time_Duration klin_ret_0 = (time_Duration){ .ns = (0 - d.ns) };
        return klin_ret_0;
    }
#line 114 "/workspace/stdlib/time.kl"
    time_Duration klin_ret_1 = d;
    return klin_ret_1;
}

#line 117 "/workspace/stdlib/time.kl"
time_Duration time_Duration_mul(time_Duration d, int64_t n) {
#line 118 "/workspace/stdlib/time.kl"
    time_Duration klin_ret_0 = (time_Duration){ .ns = (d.ns * n) };
    return klin_ret_0;
}

#line 121 "/workspace/stdlib/time.kl"
time_Instant time_now(void) {
#line 122 "/workspace/stdlib/time.kl"
    time_Instant klin_ret_0 = (time_Instant){ .unix_ns = klin_time_wall_ns() };
    return klin_ret_0;
}

#line 125 "/workspace/stdlib/time.kl"
time_MonoInstant time_mono(void) {
#line 126 "/workspace/stdlib/time.kl"
    time_MonoInstant klin_ret_0 = (time_MonoInstant){ .ns = klin_time_mono_ns() };
    return klin_ret_0;
}

#line 129 "/workspace/stdlib/time.kl"
time_Instant time_unix(int64_t sec) {
#line 130 "/workspace/stdlib/time.kl"
    time_Instant klin_ret_0 = (time_Instant){ .unix_ns = (sec * 1000000000) };
    return klin_ret_0;
}

#line 133 "/workspace/stdlib/time.kl"
time_Instant time_unix_ns(int64_t ns) {
#line 134 "/workspace/stdlib/time.kl"
    time_Instant klin_ret_0 = (time_Instant){ .unix_ns = ns };
    return klin_ret_0;
}

#line 137 "/workspace/stdlib/time.kl"
time_Duration time_between(time_Instant a, time_Instant b) {
#line 138 "/workspace/stdlib/time.kl"
    time_Duration klin_ret_0 = (time_Duration){ .ns = (b.unix_ns - a.unix_ns) };
    return klin_ret_0;
}

#line 141 "/workspace/stdlib/time.kl"
time_Duration time_since(time_Instant a) {
#line 142 "/workspace/stdlib/time.kl"
    time_Duration klin_ret_0 = time_between(a, time_now());
    return klin_ret_0;
}

#line 145 "/workspace/stdlib/time.kl"
time_Duration time_mono_between(time_MonoInstant a, time_MonoInstant b) {
#line 146 "/workspace/stdlib/time.kl"
    time_Duration klin_ret_0 = (time_Duration){ .ns = (b.ns - a.ns) };
    return klin_ret_0;
}

#line 149 "/workspace/stdlib/time.kl"
time_Duration time_mono_since(time_MonoInstant a) {
#line 150 "/workspace/stdlib/time.kl"
    time_Duration klin_ret_0 = time_mono_between(a, time_mono());
    return klin_ret_0;
}

#line 153 "/workspace/stdlib/time.kl"
time_Instant time_Instant_add(time_Instant t, time_Duration d) {
#line 154 "/workspace/stdlib/time.kl"
    time_Instant klin_ret_0 = (time_Instant){ .unix_ns = (t.unix_ns + d.ns) };
    return klin_ret_0;
}

#line 157 "/workspace/stdlib/time.kl"
time_Instant time_Instant_sub(time_Instant t, time_Duration d) {
#line 158 "/workspace/stdlib/time.kl"
    time_Instant klin_ret_0 = (time_Instant){ .unix_ns = (t.unix_ns - d.ns) };
    return klin_ret_0;
}

#line 162 "/workspace/stdlib/time.kl"
klin_res_time_Instant time_Instant_add_date(time_Instant t, int32_t years, int32_t months, int32_t days) {
#line 163 "/workspace/stdlib/time.kl"
    int64_t ns = 0;
#line 164 "/workspace/stdlib/time.kl"
    int32_t rc = klin_time_add_date(&(ns), t.unix_ns, years, months, days);
#line 165 "/workspace/stdlib/time.kl"
    if ((rc != 0)) {
#line 166 "/workspace/stdlib/time.kl"
        klin_res_time_Instant klin_ret_0 = (klin_res_time_Instant){ .is_err = true, .u.err = rc };
        return klin_ret_0;
    }
#line 168 "/workspace/stdlib/time.kl"
    klin_res_time_Instant klin_ret_1 = (klin_res_time_Instant){ .is_err = false, .u.ok = (time_Instant){ .unix_ns = ns } };
    return klin_ret_1;
}

#line 171 "/workspace/stdlib/time.kl"
klin_res_time_Instant time_Instant_add_years(time_Instant t, int32_t n) {
#line 172 "/workspace/stdlib/time.kl"
    klin_res_time_Instant klin_ret_0 = time_Instant_add_date(t, n, 0, 0);
    return klin_ret_0;
}

#line 175 "/workspace/stdlib/time.kl"
klin_res_time_Instant time_Instant_add_months(time_Instant t, int32_t n) {
#line 176 "/workspace/stdlib/time.kl"
    klin_res_time_Instant klin_ret_0 = time_Instant_add_date(t, 0, n, 0);
    return klin_ret_0;
}

#line 179 "/workspace/stdlib/time.kl"
klin_res_time_Instant time_Instant_add_days(time_Instant t, int32_t n) {
#line 180 "/workspace/stdlib/time.kl"
    klin_res_time_Instant klin_ret_0 = time_Instant_add_date(t, 0, 0, n);
    return klin_ret_0;
}

#line 184 "/workspace/stdlib/time.kl"
time_Duration time_Instant_until(time_Instant t, time_Instant u) {
#line 185 "/workspace/stdlib/time.kl"
    time_Duration klin_ret_0 = time_between(t, u);
    return klin_ret_0;
}

#line 188 "/workspace/stdlib/time.kl"
time_MonoInstant time_MonoInstant_add(time_MonoInstant m, time_Duration d) {
#line 189 "/workspace/stdlib/time.kl"
    time_MonoInstant klin_ret_0 = (time_MonoInstant){ .ns = (m.ns + d.ns) };
    return klin_ret_0;
}

#line 192 "/workspace/stdlib/time.kl"
time_MonoInstant time_MonoInstant_sub(time_MonoInstant m, time_Duration d) {
#line 193 "/workspace/stdlib/time.kl"
    time_MonoInstant klin_ret_0 = (time_MonoInstant){ .ns = (m.ns - d.ns) };
    return klin_ret_0;
}

#line 197 "/workspace/stdlib/time.kl"
time_Duration time_MonoInstant_until(time_MonoInstant a, time_MonoInstant b) {
#line 198 "/workspace/stdlib/time.kl"
    time_Duration klin_ret_0 = time_mono_between(a, b);
    return klin_ret_0;
}

#line 201 "/workspace/stdlib/time.kl"
bool time_Instant_before(time_Instant t, time_Instant u) {
#line 202 "/workspace/stdlib/time.kl"
    bool klin_ret_0 = (t.unix_ns < u.unix_ns);
    return klin_ret_0;
}

#line 205 "/workspace/stdlib/time.kl"
bool time_Instant_after(time_Instant t, time_Instant u) {
#line 206 "/workspace/stdlib/time.kl"
    bool klin_ret_0 = (t.unix_ns > u.unix_ns);
    return klin_ret_0;
}

#line 209 "/workspace/stdlib/time.kl"
bool time_Instant_equal(time_Instant t, time_Instant u) {
#line 210 "/workspace/stdlib/time.kl"
    bool klin_ret_0 = (t.unix_ns == u.unix_ns);
    return klin_ret_0;
}

#line 215 "/workspace/stdlib/time.kl"
int32_t time_format(klin_slice_u8 buf, const char* fmt, time_Instant t) {
#line 216 "/workspace/stdlib/time.kl"
    if ((buf.len <= 0)) {
#line 217 "/workspace/stdlib/time.kl"
        int32_t klin_ret_0 = -(1);
        return klin_ret_0;
    }
#line 219 "/workspace/stdlib/time.kl"
    int32_t klin_ret_1 = klin_time_format((uint8_t *)(uintptr_t)(&(buf.ptr[0])), buf.len, fmt, t.unix_ns);
    return klin_ret_1;
}

#line 222 "/workspace/stdlib/time.kl"
int32_t time_Instant_format(time_Instant t, klin_slice_u8 buf, const char* fmt) {
#line 223 "/workspace/stdlib/time.kl"
    int32_t klin_ret_0 = time_format(buf, fmt, t);
    return klin_ret_0;
}

#line 226 "/workspace/stdlib/time.kl"
klin_res_time_Instant time_parse_iso(const char* s) {
#line 227 "/workspace/stdlib/time.kl"
    int64_t ns = 0;
#line 228 "/workspace/stdlib/time.kl"
    int32_t rc = klin_time_parse_iso(&(ns), s);
#line 229 "/workspace/stdlib/time.kl"
    if ((rc != 0)) {
#line 230 "/workspace/stdlib/time.kl"
        klin_res_time_Instant klin_ret_0 = (klin_res_time_Instant){ .is_err = true, .u.err = rc };
        return klin_ret_0;
    }
#line 232 "/workspace/stdlib/time.kl"
    klin_res_time_Instant klin_ret_1 = (klin_res_time_Instant){ .is_err = false, .u.ok = (time_Instant){ .unix_ns = ns } };
    return klin_ret_1;
}

#line 235 "/workspace/stdlib/time.kl"
klin_res_time_Instant time_parse(const char* fmt, const char* s) {
#line 236 "/workspace/stdlib/time.kl"
    int64_t ns = 0;
#line 237 "/workspace/stdlib/time.kl"
    int32_t rc = klin_time_parse(&(ns), fmt, s);
#line 238 "/workspace/stdlib/time.kl"
    if ((rc != 0)) {
#line 239 "/workspace/stdlib/time.kl"
        klin_res_time_Instant klin_ret_0 = (klin_res_time_Instant){ .is_err = true, .u.err = rc };
        return klin_ret_0;
    }
#line 241 "/workspace/stdlib/time.kl"
    klin_res_time_Instant klin_ret_1 = (klin_res_time_Instant){ .is_err = false, .u.ok = (time_Instant){ .unix_ns = ns } };
    return klin_ret_1;
}

#line 8 "/workspace/stdlib/io.kl"
void io_print(const char* msg) {
#line 9 "/workspace/stdlib/io.kl"
    printf("%s", msg);
}

