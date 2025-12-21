#pragma once
#include <queue>
#include <mutex>
#include <optional>

template<typename T>
class ThreadQueue {
public:
    virtual bool push(T&& item) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(item));
        return true;
    }
    
    std::optional<T> tryPop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) return std::nullopt;
        
        T item = std::move(queue_.front());
        queue_.pop();
        return item;
    }
    
    // Get latest, discard old (useful for config updates)
    std::optional<T> tryPopLatest() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) return std::nullopt;
        
        T item = std::move(queue_.back());
        while (!queue_.empty()) queue_.pop();
        return item;
    }
    
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
    
protected: 
    std::queue<T> queue_;
    mutable std::mutex mutex_;
};
