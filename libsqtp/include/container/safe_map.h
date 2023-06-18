#pragma once

namespace sq
{
    /**
     * 线程安全的map
    */
    template <typename Key, typename Value>
    class safe_map
    {
    public:
        safe_map() {}
        using call_back_fun = void(const Key &key, const Value &value);
        void insert(const Key &key, const Value &value)
        {
            std::unique_lock<std::mutex> lock(mutex_);
            map_[key] = value;
        }

        bool contains(const Key &key) const
        {
            std::unique_lock<std::mutex> lock(mutex_);
            return map_.find(key) != map_.end();
        }

        Value &operator[](const Key &key)
        {
            std::unique_lock<std::mutex> lock(mutex_);
            return map_[key];
        }

        void erase(const Key &key)
        {
            std::unique_lock<std::mutex> lock(mutex_);
            map_.erase(key);
        }

        void clear()
        {
            std::unique_lock<std::mutex> lock(mutex_);
            map_.clear();
        }

        size_t size() const
        {
            std::unique_lock<std::mutex> lock(mutex_);
            return map_.size();
        }

        void for_each(call_back_fun fun)
        {
            std::unique_lock<std::mutex> lock(mutex_);
            auto it = map_.begin();
            for (; it != map_.end(); ++it)
            {
                // it->first;
                fun(it->first, it->second);
            }
        }

    private:
        std::map<Key, Value> map_;
        mutable std::mutex mutex_;
    };
}