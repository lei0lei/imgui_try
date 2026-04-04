/**
 * @file event_bus.cpp
 * @brief Simple in-process event bus with queued dispatch
 */

#include "event_bus.h"

#include <utility>

void EventBus::Publish(const std::string& topic, std::string payload)
{
    if (topic.empty()) {
        return;
    }

    Event ev{ topic, std::move(payload) };
    if (dispatching_) {
        next_queue_.push_back(std::move(ev));
    } else {
        queue_.push_back(std::move(ev));
    }
}

IEventBus::SubscriptionId EventBus::Subscribe(const std::string& topic, Handler handler)
{
    if (topic.empty() || !handler) {
        return 0;
    }

    const SubscriptionId id = next_id_++;
    Subscription sub{};
    sub.id = id;
    sub.topic = topic;
    sub.handler = std::move(handler);

    subs_by_id_.emplace(id, std::move(sub));
    subs_by_topic_.emplace(topic, id);
    return id;
}

void EventBus::Unsubscribe(SubscriptionId id)
{
    if (id == 0) {
        return;
    }

    auto it = subs_by_id_.find(id);
    if (it == subs_by_id_.end()) {
        return;
    }

    const std::string topic = it->second.topic;
    subs_by_id_.erase(it);

    auto range = subs_by_topic_.equal_range(topic);
    for (auto t = range.first; t != range.second;) {
        if (t->second == id) {
            t = subs_by_topic_.erase(t);
        } else {
            ++t;
        }
    }
}

void EventBus::Dispatch()
{
    if (dispatching_) {
        return;
    }

    dispatching_ = true;
    for (size_t i = 0; i < queue_.size(); ++i) {
        const Event& ev = queue_[i];
        auto range = subs_by_topic_.equal_range(ev.topic);
        for (auto it = range.first; it != range.second; ++it) {
            const SubscriptionId id = it->second;
            auto sub_it = subs_by_id_.find(id);
            if (sub_it == subs_by_id_.end()) {
                continue;
            }
            sub_it->second.handler(ev.topic, ev.payload);
        }
    }
    queue_.clear();
    dispatching_ = false;

    if (!next_queue_.empty()) {
        queue_.swap(next_queue_);
        next_queue_.clear();
    }
}

