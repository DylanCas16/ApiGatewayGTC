#pragma once 

#include <map>
#include <mutex>
#include <string>
#include <utility>
#include <vector>
#include <stdint.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/impl/codegen/sync_stream.h>


template<typename T>

class SubscriptionRegistry {
    public:
        SubscriptionRegistry() : next_id_(0) {}

        static std::string dataBlockKey(const std::string& component, 
                                        const std::string& magnitude)
        { return "D:" + component + ":" + magnitude; }

        static std::string stateChangeKey(const std::string& component)
        { return "S:" + component; }

        static std::string magnitudeChangeKey(const std::string& component,
                                        const std::string& magnitude)
        { return "M:" + component + ":" + magnitude; }
    
        uint64_t addEntry(const std::string& key, grpc::ServerWriter<T>* writer)
        {
            std::lock_guard<std::mutex> lock(mutex_);            
            uint64_t id = next_id_++;            
            Entry entry;

            entry.key = key;
            entry.writer = writer;
            entry.active = true;
            entries_[id] = entry;

            return id;
        }

        bool removeEntry(uint64_t id)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            
            typename EntryMap::iterator iter = entries_.find(id);
            if (iter == entries_.end()) return false;

            std::string key = iter->second.key;
            entries_.erase(iter);

            for (typename EntryMap::const_iterator i = entries_.begin(); i != entries_.end(); ++i) {
                if (i->second.active && i->second.key == key) {
                    return false;
                }
            }

            return true;
        }

        bool dispatch(const std::string& key, const T& response)
        {
            typedef std::pair<uint64_t, grpc::ServerWriter<T>*> Target;
            std::vector<Target> targets;

            {
                std::lock_guard<std::mutex> lock(mutex_);
                for (typename EntryMap::iterator iter = entries_.begin(); iter != entries_.end(); ++iter) {
                    Entry& entry = iter->second;
                    if (entry.active && entry.key == key) {
                        targets.push_back(std::make_pair(iter->first, entry.writer));
                    }
                }
            }

            bool all_ok = true;
            for (std::size_t i = 0; i<targets.size(); ++i) {
                if (!targets[i].second->Write(response)) {
                    std::lock_guard<std::mutex> lock(mutex_);
                    typename EntryMap::iterator iter = entries_.find(targets[i].first);
                    
                    if (iter != entries_.end()) {
                        iter->second.active = false;
                    }

                    all_ok = false;
                }
            }
            return all_ok;
        }

        bool isActive(uint64_t id) const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            typename EntryMap::const_iterator iter = entries_.find(id);
            return iter != entries_.end() && iter->second.active;
        }

        bool hasActiveWriters(const std::string& key) const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            for (typename EntryMap::const_iterator iter = entries_.begin(); iter != entries_.end(); ++iter) {
                if (iter->second.active && iter->second.key == key) return true;
            }
            return false;
        }
    
    private:
        struct Entry {
            std::string key;
            grpc::ServerWriter<T>* writer;
            bool active;
        };
        
        typedef std::map<uint64_t, Entry> EntryMap;

        mutable std::mutex mutex_;
        EntryMap entries_;
        uint64_t next_id_;
};
