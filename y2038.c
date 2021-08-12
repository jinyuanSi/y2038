#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define IS_LEAP(n)  (!(((n) + 1900) % 400) || (!(((n) + 1900) % 4) && (((n) + 1900) % 100)))

// The representable range is
// from: Jan  1 00:00:00 1970
// to: Feb  7 06:28:15 2106
static struct tm * gmtime_u(uint32_t time)
{
    const char days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const uint32_t days_in_pass4years= 1461L;

    static struct tm t;
    memset(&t, 0, sizeof t);

    t.tm_sec = (int)(time % 60);
    time /= 60;

    t.tm_min = (int)(time % 60);
    time /= 60;

    t.tm_wday = (time / 24 + 4) % 7;

    t.tm_hour= (int)(time % 24);
    time /= 24;

    uint32_t pass4year = time / days_in_pass4years;
    t.tm_year= (pass4year << 2) + 70;

    time %= days_in_pass4years;

    if (t.tm_year > 200) {
        time++;
    }

    int days_in_year;
    while(1) {
        days_in_year = 365;
        if (IS_LEAP(t.tm_year)) days_in_year += 1;
        if (time < days_in_year) break;
        t.tm_year++;
        time -= days_in_year;
    }

    t.tm_yday= time % 365;

    time++;
    if(IS_LEAP(t.tm_year)) {
        if (time > 60) {
            time--;
        } else {
            if (time == 60) {
                t.tm_mon = 1;
                t.tm_mday = 29;
                return &t;
            }
        }
    }

    for (t.tm_mon = 0; days_in_month[t.tm_mon] < time; t.tm_mon++) {
        time -= days_in_month[t.tm_mon];
    }

    t.tm_mday = (int)(time);

    return &t;
}

int main(int argc, char *argv[])
{
    time_t x;
    const int days_in_seconds = 24 * 60 * 60;
    char * timStr;
    char buf1[64] = {0};
    char buf2[64] = {0};
    uint32_t start = 0;
    const uint32_t end = (~0);
    const uint32_t end_day = 4294923648;

    if (argc > 1) {
        start = strtoll(argv[1], NULL, 0);
    }

    for (uint32_t i = start; i < end_day; i += days_in_seconds) {
        x = i;
        struct tm *t = gmtime(&x);
        timStr = asctime(t);
        snprintf(buf1, 64, "%s", timStr);

        int32_t xx = (int32_t) x;
        t = gmtime_u(xx);
        timStr = asctime(t);
        snprintf(buf2, 64, "%s", timStr);
        if (strcmp(buf1, buf2)) {
            printf("=====================not matched=========================\n");
            printf("%d: %ld\n", xx, x);
            printf("o: %s", buf1);
            printf("c: %s\n", buf2);
            break;
        }
    }

    struct tm *t = gmtime_u(0);
    timStr = asctime(t);
    printf("from: %s", timStr);

    int32_t xx = (int32_t) end;
    t = gmtime_u(xx);
    timStr = asctime(t);
    printf("to: %s\n", timStr);

    return 0;
}
