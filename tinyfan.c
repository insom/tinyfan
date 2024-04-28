/*
 * (c) 2021, 2024 Aaron Brady
 * Do with this code as you wish, at your own risk.
 *
 * Constrain the temperature of a Thinkpad X131e or T440p between an upper and
 * lower bound. I would rather silence punctuated by the blast of fans than a
 * constant low level whine, so I've written the code to use the fan as a blunt
 * instrument.
 *
 */

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#define ON 1
#define OFF 0
#define MIN 60
#define MID 70
#define MAX 80

void set_fan(char *level) {
  char buf[80] = {0};
  int fd = open("/proc/acpi/ibm/fan", O_WRONLY);
  if (fd < 0) {
    perror("open");
    exit(1);
  }
  snprintf(buf, 80, "level %s", level);
  printf("f: %s\n", level);
  write(fd, buf, strlen(buf));
  close(fd);
}

void failsafe(void) { set_fan("auto"); }

void handler(int number) { exit(1); }

int get_temperature() {
  char buf[80] = {0};
  int fd = open("/proc/acpi/ibm/thermal", O_RDONLY);
  if (fd < 0) {
    perror("open");
    exit(1);
  }
  int bytes = read(fd, buf, 80);
  close(fd);
  if (bytes < 1)
    return -1;
  char *temp = index(buf, '\t');
  return atoi(temp);
}

int main(int c, char **v) {
  char fan = OFF;
  int current_temperature;

  dup2(2, 1); // redirect stdout to stderr. this is bad. oh, well.

  /* no melt thinkpad pls */
  atexit(failsafe);
  signal(SIGINT, handler);
  signal(SIGTERM, handler);

  set_fan("0");
  while (1) {
    current_temperature = get_temperature();
    printf("t: %d\n", current_temperature);
    if (current_temperature == -1) {
      printf("invalid temperature reading");
      exit(1);
    }
    if (current_temperature >= MAX) {
      set_fan("7");
      fan = ON;
    } else if (current_temperature >= MID && fan == OFF) {
      set_fan("1");
      fan = ON;
    }
    if (current_temperature < MIN && fan == ON) {
      set_fan("0");
      fan = OFF;
    }
    sleep(5);
  }
}
