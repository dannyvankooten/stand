#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#define ACTION_ACCEPT 1
#define ACTION_IGNORE_1H 2
#define ACTION_IGNORE_MIDNIGHT 24

static uint8_t parse_uint8(const char *s) {
    unsigned int n = 0;
    unsigned int i = 0;
    while (*s >= '0' && *s <= '9' && i < 3) {
        n = n * 10u + (unsigned int)(*s - '0');
        i++;
        s++;
    }

    return (uint8_t) n;
}

static uint8_t notify(void) {
    // send notification
    FILE *pout = popen("notify-send -u critical --app-name=\"Stand!\" --action=1=\"I'm up!\" \"Stand\" --action=2=\"Ignore for 2 hours\" --action=24=\"Ignore rest of day\" \"It's time to stand up!\"", "r");
    if (pout == NULL) {
       return 1;
    }

    // read stdout
    char s[8] = {0};
    unsigned long nread = fread(s, 1, 30, pout);
    s[nread] = '\0';

    int status = pclose(pout);
    if (status != EXIT_SUCCESS) {
        return 1;
    }

    return parse_uint8(s);
}

static void log_message(time_t t, char *fmt, ...) {
  char buffer[BUFSIZ];
  size_t n = strftime(buffer, 32, "[%X] ", localtime(&t));

  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer+n, sizeof(buffer)-n, fmt, args);
  va_end(args);

  printf("%s", buffer);
}

static void print_startup_message(time_t tnow, time_t tnext, uint16_t interval) {
    struct tm *t = localtime(&tnext);
    char s[32] = {0};
    strftime(s, 32, "%I:%M %p", t);

    // print some info to stdout
    log_message(tnow, "Stand is now running\n");
    log_message(tnow, "Interval: %d seconds\n", interval);
    log_message(tnow, "Next reminder: %s\n", s);
    fflush(stdout);
}


int main(int argc, char **argv) {
    time_t tstart = time(NULL);
    time_t tnow = time(NULL);
    time_t tnext;
    uint8_t action;
    uint16_t interval;

    if (argc > 1) {
        interval = parse_uint8(argv[1]);
        interval = interval < 1 ? 1 : interval;
    } else {
        interval = 3600; // default interval: 1 hour
    }

    tnext = tstart + interval;
    print_startup_message(tnow, tnext, interval);

    while (1) {
        sleep(1);
        tnow = time(NULL);

        if (tnow > tnext) {
            log_message(tnow, "Notification sent\n");
            action = notify();

            // determine next notification time
            tnow = time(NULL);

            switch (action) {
              case ACTION_ACCEPT: log_message(tnow, "Notification accepted\n"); break;
              case ACTION_IGNORE_1H: log_message(tnow, "Notifications temporarily ignored\n"); break;
              case ACTION_IGNORE_MIDNIGHT: log_message(tnow, "Notifications ignored for rest of day\n"); break;
            }

            if (action != 24) {
                tnext = tnow + (action * interval);
            } else {
                // 24 is a special case were we postpone until next midnight, then resume normal interval again
                time_t seconds_to_midnight = 86400 - (time(NULL) % 86400);
                tnext = tnow + seconds_to_midnight;
            }

            log_message(tnow, "Next notification in %ld seconds\n", tnext - tnow);
            fflush(stdout);
        }
    }

    return 0;
}
