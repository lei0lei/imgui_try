#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class IEventBus {
public:
    using SubscriptionId = uint64_t;
    using Handler = std::function<void(const std::string& topic, const std::string& payload)>;

    virtual ~IEventBus() = default;

    // Enqueue an event for delivery on the next Dispatch().
    virtual void Publish(const std::string& topic, std::string payload = {}) = 0;

    // Subscribe to events for a topic. Returns a token for Unsubscribe().
    virtual SubscriptionId Subscribe(const std::string& topic, Handler handler) = 0;

    virtual void Unsubscribe(SubscriptionId id) = 0;

    // Deliver all currently queued events. Events published during Dispatch() are delivered on the next call.
    virtual void Dispatch() = 0;
};

class EventBus final : public IEventBus {
public:
    void Publish(const std::string& topic, std::string payload = {}) override;
    SubscriptionId Subscribe(const std::string& topic, Handler handler) override;
    void Unsubscribe(SubscriptionId id) override;
    void Dispatch() override;

private:
    struct Subscription {
        SubscriptionId id = 0;
        std::string topic;
        Handler handler{};
    };

    struct Event {
        std::string topic;
        std::string payload;
    };

    SubscriptionId next_id_ = 1;
    bool dispatching_ = false;

    // Active subscriptions.
    std::unordered_map<SubscriptionId, Subscription> subs_by_id_{};
    std::unordered_multimap<std::string, SubscriptionId> subs_by_topic_{};

    // Two-queue scheme to avoid re-entrancy issues.
    std::vector<Event> queue_{};
    std::vector<Event> next_queue_{};
};

