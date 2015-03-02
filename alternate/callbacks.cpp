#include <Arduino.h>
#include "callbacks.h"

struct Callbacks::_queue {
	Callbacks::callback cb;
	Entry next;
};
Callbacks::Entry queue = 0;

Callbacks::Entry Callbacks::add(Callbacks::callback cb) {
	Entry newEntry = (Entry)malloc(sizeof(struct _queue));
	newEntry->cb = cb;
	newEntry->next = queue;
	queue = newEntry;
	return queue;
}

void Callbacks::call() {
	Entry q = queue;
	while (q != 0) {
		(q->cb)();
		q = q->next;
	}
}
