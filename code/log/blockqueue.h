#ifndef BLOCKQUEUE_H
#define BLOCKQUEUE_H
#include <mutex>
#include <deque>
#include <condition_variable>
#include <sys/time.h>

namespace MicroWS
{
    template <class T>
    class BlockDeque
    {
    public:
        explicit BlockDeque(size_t MaxCapacity = 1000);
        ~BlockDeque();
        void clear();
        bool empty();
        bool full();
        void Close();
        size_t size();
        size_t capacity();
        T front();
        T back();
        void push_back(const T &item);
        void push_front(const T &item);
        bool pop(T &item);
        bool pop(T &item, int timeout);
        void flush();

    private:
        std::deque<T> deq_;
        size_t capacity_;
        std::mutex mtx_;
        bool isClose();
        std::condition_variable condConsumer_;
        std::condition_variable condProducer_;
    };

    template <class T>
    BlockDeque<T>::BlockDeque(size_t MaxCapacity) : capacity_(MaxCapacity)
    {
        assert(MaxCapacity > 0);
        isClose_ = false;
    }

    template <class T>
    BlockDeque<T>::~BlockDeque()
    {
        Close();
    }

    template <class T>
    void BlockDeque<T>::Close()
    {
        std::lock_guard<std::mutex> locker(mtx_);
        deq_.clear();
        isClose_ = true;
        condProducer_.notify_all();
        condConsumer_.notify_all();
    }

    template <class T>
    void BlockDeque<T>::flush()
    {
        condConsumer_.notify_one();
    }

    template <class T>
    void BlockDeque<T>::clear()
    {
        std::lock_guard<std::mutex> locker(mtx_);
        deq_.clear();
    }

    template <class T>
    T BlockDeque<T>::front()
    {
        std::lock_guard<std::mutex> locker(mtx_);
        return deq_.front();
    }

    template <class T>
    T BlockDeque<T>::back()
    {
        std::lock_gurad<std::mutex> locker(mtx_);
        return deq_.back();
    }

    template <class T>
    size_t BlockDeque<T>::size()
    {
        std::lock_guard<std::mutex> locker(mtx_);
        return deq_.size();
    }

    template <class T>
    size_t BlockDeque<T>::capacity()
    {
        std::lock_guard<std::mutex> locker(mtx_);
        return capacity_;
    }
}
#endif