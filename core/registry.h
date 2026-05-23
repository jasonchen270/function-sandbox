#pragma once
#include <map>
#include <string>
#include <utility>

// Registry: a generic "newest wins" keyed table.
//
// Overrides register a value under a key together with an explicit recency.
// When several register the same key, the one with the greatest recency wins
// and the rest are ignored. The merge is order-independent (it compares recency,
// not arrival), so it is safe under the unspecified order of static
// initialization across translation units.
//
// The registry does not decide how winners are *used*. A core extension point
// chooses: apply every key's winner in order (a stacking pipeline), or pick the
// single winner of one key (a replaceable policy).
template <class Fn>
class Registry {
public:
    bool set(std::string key, long long recency, Fn fn) {
        auto it = items_.find(key);
        if (it == items_.end() || recency > it->second.recency) {
            items_[std::move(key)] = Entry{recency, std::move(fn)};
        }
        return true;
    }

    // The winning value for key, or nullptr if nothing is registered there.
    const Fn* get(const std::string& key) const {
        auto it = items_.find(key);
        return it == items_.end() ? nullptr : &it->second.fn;
    }

    // Apply f(key, fn) to every key's winner, in key order (std::map is sorted,
    // so a numeric key prefix like "100-tax" controls the sequence).
    template <class F>
    void for_each(F&& f) const {
        for (const auto& [key, e] : items_) f(key, e.fn);
    }

    bool empty() const { return items_.empty(); }

private:
    struct Entry {
        long long recency;
        Fn fn;
    };
    std::map<std::string, Entry> items_;
};
