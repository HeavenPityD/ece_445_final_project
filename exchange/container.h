#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <iostream>
#define MAPSIZE 20000
#pragma once

using namespace std;

template <class T>
class SafeQueue {
public:
	void push(T t) {
		lock_guard<mutex> lock(m);
		q.push(t);
		c.notify_one();
	}

    T front() {
        unique_lock<mutex> lock(m);
		while(!done and q.empty()) c.wait(lock);
        return q.front();
    }

  	T pop(void) {
		unique_lock<mutex> lock(m);
		while(!done and q.empty()) c.wait(lock);
        if (done and q.empty()) return T();
		T val = q.front();
		q.pop();
		return val;
  	}

  	bool empty() {
		return q.empty();
	}
	void markDone() {
		done = true;
        c.notify_one();
	}

private:
	queue<T> q;
  	mutable mutex m;
	condition_variable c;
	volatile bool done = false;
};

template <class Key, class Data>
class SafeMap {
    private:
        struct V {
            Key key;
            Data data;
            mutex entryLock;
            bool occupied;
            V(): occupied(false) {}
        };
        V mem[MAPSIZE];
        hash<Key> hashFunc = hash<Key>{};

        inline V& get(Key key) {
            size_t idx = hashFunc(key) % MAPSIZE;
            while (mem[idx].occupied && mem[idx].key != key) idx = (idx+1)%MAPSIZE;
            return mem[idx];
        }
    public:
        Data& operator[](Key key) {
            V& v = get(key);
            if (!v.occupied) {
                v.occupied = true;
                v.key = key;
            }
            return v.data;
        }

        bool contains(Key key) {
            return get(key).occupied;
        }

        void lock(Key key) {
            get(key).entryLock.lock();
        }

        void unlock(Key key) {
            get(key).entryLock.unlock();
        }
};
