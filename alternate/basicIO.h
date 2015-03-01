#ifndef BASICIO_H_
#define BASICIO_H_

namespace basicIO {
	float digitalGet(byte port);
	void digitalSet(byte port, float value);
	float analogGet(byte port);
	void analogSet(byte port, float value);
}



#endif /* BASICIO_H_ */
