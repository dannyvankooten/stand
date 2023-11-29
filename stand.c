#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

int parse_int(const char *s) {
    int n = 0;
    int i = 0;
    while (*s >= '0' && *s <= '9' && i < 10) {
        n = n * 10 + (*s - '0');
        i++;
        s++;
    }

    return n;
}

int notify() {
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

void print_startup_message(time_t tnext, int interval) {
    struct tm *t = localtime(&tnext);
    char s[32] = "";
    strftime(s, 32, "%l:%M %p", t);

    // print some info to stdout
    printf("Stand is now running!\n");
    printf("-  Interval: %d seconds\n", interval);
    printf("-  Next reminder: %s\n", s);
    fflush(stdout);
}

int main(int argc, char **argv) {
    time_t tstart;
    time_t tnow;
    time_t tnext;
    int action;
    int interval;

    if (argc > 1) {
        interval = parse_int(argv[1]);
    } else {
        interval = 3600; // default interval: 1 hour
    }

    time(&tstart);
    tnext = tstart + interval;

    print_startup_message(tnext, interval);

    while (1) {
        sleep(1);
        time(&tnow);

        if (tnow > tnext) {
            action = notify();

            // determine next notification time
            time(&tnow);
            tnext = tnow + (action * interval);
        }
    }

    return 0;
}