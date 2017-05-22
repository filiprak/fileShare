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
#include <utility>


template <class T> class BlockingQueue {

private:
	std::queue<T> queue;

    unsigned maxSize;
    bool blockPush;
    std::mutex readerMutex;
    std::mutex writerMutex;
    std::condition_variable isFull;
    std::condition_variable isEmpty;

public:
	BlockingQueue(int size, bool blockPush=false) {
		this->maxSize = size;
		this->blockPush = blockPush;
	}

	bool insert(T item) {
		std::unique_lock<std::mutex> wlck(writerMutex);
		if(! this->blockPush && Full()) return false;
		while(Full())
			isFull.wait(wlck);
		queue.push(item);
		isEmpty.notify_all();
		return true;
	}

	bool notEmpty() {
		return !queue.empty();
	}

	bool Full(){
		return queue.size() >= maxSize;
	}

    T take() {
    	std::unique_lock<std::mutex> lck(readerMutex);
        while(queue.empty()) {
            isEmpty.wait(lck);
        }
        T value = queue.front();
        queue.pop();
        if(!Full())
            isFull.notify_all();
        return value;
    }

    void clear() {
    	std::queue<T>().swap(queue);
    }
};


#endif /* UTILS_BLOCKINGQUEUE_H_ */
