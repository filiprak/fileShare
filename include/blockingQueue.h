/*
 * blockingQueue.h
 *
 *  Created on: 19.05.2017
 *      Author: raqu
 */

#ifndef UTILS_BLOCKINGQUEUE_H_
#define UTILS_BLOCKINGQUEUE_H_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>



template <class T> class BlockingQueue: public std::queue<T> {

private:
    unsigned maxSize;
    bool blockPush;
    std::mutex readerMutex;
    std::mutex writerMutex;
    std::condition_variable isFull;
    std::condition_variable isEmpty;

public:
	BlockingQueue(int size, bool blockPush) {
		this->maxSize = size;
		this->blockPush = blockPush;
	}

	bool push(T item) {
		std::unique_lock<std::mutex> wlck(writerMutex);
		if(! this->blockPush && Full()) return false;
		while(Full())
			isFull.wait(wlck);
		std::queue<T>::push(item);
		isEmpty.notify_all();
		return true;
	}

	bool notEmpty() {
		return !std::queue<T>::empty();
	}

	bool Full(){
		return std::queue<T>::size() >= maxSize;
	}

    T pop() {
    	std::unique_lock<std::mutex> lck(readerMutex);
        while(std::queue<T>::empty()) {
            isEmpty.wait(lck);
        }
        T value = std::queue<T>::front();
        std::queue<T>::pop();
        if(!Full())
            isFull.notify_all();
        return value;
    }
};


#endif /* UTILS_BLOCKINGQUEUE_H_ */
