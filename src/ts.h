#ifndef _TS_H_
#define _TS_H_
#include <sys/time.h>
#include <time.h>

struct timespec ts;

struct timespec recorder_wtime(void)
{
	clock_gettime(CLOCK_MONOTONIC, &ts);

    	return ts;
}

const long nano = 1000000000;

struct timespec operator+(const struct timespec& a, const struct timespec& b)
{
	struct timespec result;

	result.tv_sec = a.tv_sec+b.tv_sec;
	result.tv_nsec = a.tv_nsec+b.tv_nsec;

	if (result.tv_nsec >= nano) {
		result.tv_nsec -= nano;
		result.tv_sec++;
	}

	return result;
}

struct timespec& operator+=(struct timespec& a, const timespec& b)
{
	a.tv_sec += b.tv_sec;
	a.tv_nsec += b.tv_nsec;

	if (a.tv_nsec >= nano) {
		a.tv_nsec -= nano;
		a.tv_sec++;
	}

	return a;
}

struct timespec operator-(const struct timespec& a, const struct timespec& b)
{
	struct timespec result;

	result.tv_sec = a.tv_sec-b.tv_sec;
	result.tv_nsec = a.tv_nsec-b.tv_nsec;

	if (result.tv_nsec < 0) {
		result.tv_nsec += nano;
		result.tv_sec--;
	}

	return result;
}

#endif
