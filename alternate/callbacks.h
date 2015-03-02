#ifndef CALLBACKS_H_
#define CALLBACKS_H_

namespace Callbacks {
	typedef struct _queue *Entry;
	typedef void (*callback)();

	Entry add(callback cb);
	void call();
}

#endif // CALLBACKS_H_
