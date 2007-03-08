#ifndef TIMEVAL_H
#define TIMEVAL_H

#include <sys/time.h>

/* GFDL Code from http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html */
static inline int timeval_subtract(struct timeval *x, struct timeval *y) {
	if (x->tv_usec < y->tv_usec) {
		int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 * nsec;
		y->tv_sec += nsec;
	}

	if (x->tv_usec - y->tv_usec > 1000000) {
		int nsec = (x->tv_usec - y->tv_usec) / 1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec -= nsec;
	}

	return x->tv_sec * 1000 - y->tv_sec * 1000 + x->tv_usec / 1000 - y->tv_usec / 1000;
}

#endif
