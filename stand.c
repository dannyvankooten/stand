#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

uint8_t parse_int(const char *s) {
    uint8_t n = 0;
    uint8_t i = 0;
    while (*s >= '0' && *s <= '9' && i < 3) {
        n = n * 10 + (*s - '0');
        i++;
        s++;
    }

    return n;
}

uint8_t notify() {
    // send notification
    FILE *pout = popen("notify-send -u critical --app-name=\"Stand!\" --action=1=\"I'm up!\" \"Stand\" --action=2=\"Ignore for 2 hours\" --action=24=\"Ignore rest of day\" \"It's time to stand up!\"", "r");
    if (pout == NULL) {
       return 1;
    }

    // read stdout
    char s[32] = "";
    fread(s, 1, 30, pout);
    int status = pclose(pout);
    if (status != EXIT_SUCCESS) {
        return 1;
    }

    return parse_int(s);
}

void print_startup_message(time_t tnext, uint16_t interval) {
    struct tm *t = localtime(&tnext);
    char s[32] = "";
    strftime(s, 32, "%I:%M %p", t);

    // print some info to stdout
    printf("Stand is now running\n");
    printf("Interval: %d seconds\n", interval);
    printf("Next reminder: %s\n", s);
    fflush(stdout);
}

int main(int argc, char **argv) {
    time_t tstart = time(NULL);
    time_t tnow = time(NULL);
    time_t tnext;
    uint8_t action;
    uint16_t interval;

    if (argc > 1) {
        interval = parse_int(argv[1]);
    } else {
        interval = 3600; // default interval: 1 hour
    }

    tnext = tstart + interval;
    print_startup_message(tnext, interval);

    while (1) {
        sleep(1);
        tnow = time(NULL);

        if (tnow > tnext) {
            action = notify();

            // determine next notification time
            tnow = time(NULL);

            if (action != 24) {
                tnext = tnow + (action * interval);
            } else {
                // 24 is a special case were we postpone until next midnight, then resume normal interval again
                time_t seconds_to_midnight = 86400 - (time(NULL) % 86400);
                tnext = tnow + seconds_to_midnight;
            }
        }
    }

    return 0;
}