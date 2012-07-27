/*
 * Ring.h
 *
 *  Created on: 2012/01/19
 *      Author: sin
 */

#ifndef TEXTRING_H_
#define TEXTRING_H_

class TextRing {
private:
	char * buffer;
	int bufferSize;
	int head, tail;

public:
	TextRing(char buf[], const int sz) :
			buffer(buf), bufferSize(sz) {
		reset();
	}

	void reset() {
		tail = head;
		buffer[tail] = 0;
	}

	void append(const char c) {
		buffer[tail++] = c;
		tail %= bufferSize;
		buffer[tail] = 0;
	}

	int length() {
		return (tail + bufferSize - head) % bufferSize;
	}

	char & operator[](int i) {
		return buffer[(head + i) % bufferSize];
	}

	int copyInto(char * dst) {
		return copyIntoFrom(dst, 0, 0);
	}

	int copyIntoFrom(char * dst, int frm, char dlm) {
		int cnt = 0;
		int i = (head + frm) % bufferSize;
		while (i != tail && buffer[i] != dlm) {
			*dst++ = buffer[i++];
			i %= bufferSize;
			cnt++;
		}
		*dst = 0;
		return cnt;
	}

};

#endif /* TEXTRING_H_ */
