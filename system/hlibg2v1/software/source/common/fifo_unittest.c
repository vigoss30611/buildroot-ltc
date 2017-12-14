/* Copyright 2013 Google Inc. All Rights Reserved. */

#include "fifo.h"

#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define DEFAULT_COUNT_READ 10
#define DEFAULT_COUNT_WRITE 10
#define DEFAULT_FORCED_EXITTIME 0
#define DEFAULT_IDLETIME_READ 0
#define DEFAULT_IDLETIME_WRITE 0
#define DEFAULT_QUEUE_SIZE 3

struct TestParams {
  FifoInst fifo;
  u32 queue_size;
  u32 read_count;
  u32 write_count;
  struct timespec idle_time_between_pops;
  struct timespec idle_time_between_pushes;
  struct timespec time_to_forced_exit;
};

/* Ugly globals for easy maintenance of testing. */
static i32 g_global_object_counter = 0;
static u8 g_running = 1;

void PrintUsage(char* executable) {
  printf("Usage: %s [options]\n", executable);
  printf("\t-Cn have at maximum n items in queue. [%i]\n", DEFAULT_QUEUE_SIZE);
  printf("\t-Nn read n times from the queue. [%i]\n", DEFAULT_COUNT_READ);
  printf("\t-Mn write n times from the queue. [%i]\n", DEFAULT_COUNT_WRITE);
  printf("\t-Rn delay n milliseconds between reads. [%i]\n",
         DEFAULT_IDLETIME_READ);
  printf("\t-Wn delay n milliseconds between writes. [%i]\n",
         DEFAULT_IDLETIME_WRITE);
  printf("\t-Xn forced exit after n milliseconds. 0 to disable. [%i]\n",
         DEFAULT_IDLETIME_WRITE);
}

i32 GetParams(int argc, char* argv[], struct TestParams* params) {
  i32 i;
  memset(params, 0, sizeof(struct TestParams));
  params->queue_size = DEFAULT_QUEUE_SIZE;
  params->read_count = DEFAULT_COUNT_READ;
  params->write_count = DEFAULT_COUNT_WRITE;
  params->idle_time_between_pops.tv_nsec = DEFAULT_IDLETIME_READ * 1000000;
  params->idle_time_between_pushes.tv_nsec = DEFAULT_IDLETIME_WRITE * 1000000;
  params->time_to_forced_exit.tv_nsec = DEFAULT_FORCED_EXITTIME;
  /* read command line arguments */
  for (i = 1; i < argc; i++) {
    if (strncmp(argv[i], "-C", 2) == 0)
      params->queue_size = (u32)atoi(argv[i] + 2);
    else if (strncmp(argv[i], "-N", 2) == 0)
      params->read_count = (u32)atoi(argv[i] + 2);
    else if (strncmp(argv[i], "-M", 2) == 0)
      params->write_count = (u32)atoi(argv[i] + 2);
    else if (strncmp(argv[i], "-R", 2) == 0)
      params->idle_time_between_pops.tv_nsec = atoi(argv[i] + 2) * 1000000;
    else if (strncmp(argv[i], "-W", 2) == 0)
      params->idle_time_between_pushes.tv_nsec = atoi(argv[i] + 2) * 1000000;
    else if (strncmp(argv[i], "-X", 2) == 0)
      params->time_to_forced_exit.tv_nsec = atoi(argv[i] + 2) * 1000000;
    else {
      PrintUsage(argv[0]);
      return 1;
    }
  }
  return 0;
}

static void timer_handler(int sig, siginfo_t* si, void* uc) {
  printf("Exit timer expired, forced exit.\n");
  g_running = 0;
  signal(sig, SIG_IGN);
}

static i32 CreateTimer(u32 time_to_forced_exit, timer_t* timerid) {
  struct sigevent sev;
  struct itimerspec its;
  struct sigaction sa;
  memset(&sev, 0, sizeof(sev));
  memset(&its, 0, sizeof(its));
  memset(&sa, 0, sizeof(sa));

  /* Establish handler for timer signal */
  sa.sa_flags = SA_SIGINFO | SA_ONESHOT;
  sa.sa_sigaction = timer_handler;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGRTMIN, &sa, NULL) == -1) return -1;

  /* Create the timer */
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIGRTMIN;
  sev.sigev_value.sival_ptr = timerid;
  if (timer_create(CLOCK_REALTIME, &sev, timerid) == -1) return -1;

  /* Start the timer */
  its.it_value.tv_sec = 0;
  its.it_value.tv_nsec = time_to_forced_exit;
  its.it_interval.tv_sec = its.it_value.tv_sec;
  its.it_interval.tv_nsec = its.it_value.tv_nsec;
  if (timer_settime(*timerid, 0, &its, NULL) == -1) return -1;

  return 0;
}

static void* PopThread(void* arg) {
  u32 i;
  struct TestParams* params = (struct TestParams*)arg;
  for (i = 0; i < params->read_count; i++) {
    long* object;
    FifoPop(params->fifo, (void**)&object, FIFO_EXCEPTION_DISABLE);
    assert(*object == i);
    printf("Popped object with value %i\n", *object);
    free(object);
    g_global_object_counter--;
    printf("%i existing objects\n", g_global_object_counter);
    nanosleep(&params->idle_time_between_pops, NULL);
    if (!g_running && g_global_object_counter == 0) break;
  }
  return 0;
}

int main(int argc, char* argv[]) {
  i32 i;
  pthread_t reader_thread;
  struct TestParams params;
  timer_t timerid;
  memset(&timerid, 0, sizeof(timerid));

  /* Parse the command line params. */
  if (GetParams(argc, argv, &params) != 0) return 1;

  /* Initialize the fifo queue. */
  FifoInit(params.queue_size, &params.fifo);

  /* Create the exit timer if one is wanted. */
  if (CreateTimer(params.time_to_forced_exit.tv_nsec, &timerid)) {
    printf("Failed to create timer\n");
    FifoRelease(params.fifo);
    return -1;
  }

  /* Start the reader thread. */
  pthread_create(&reader_thread, NULL, PopThread, &params);
  for (i = 0; i < params.write_count && g_running; i++) {
    int* j = malloc(sizeof(int));
    g_global_object_counter++;
    printf("%i existing objects\n", g_global_object_counter);
    *j = i;
    printf("Pushing object with value %i\n", *j);
    FifoPush(params.fifo, j, FIFO_EXCEPTION_DISABLE);
    nanosleep(&params.idle_time_between_pushes, NULL);
  }
  pthread_join(reader_thread, NULL);
  timer_delete(timerid);
  FifoRelease(params.fifo);

  assert(g_global_object_counter == 0);
  return 0;
}
