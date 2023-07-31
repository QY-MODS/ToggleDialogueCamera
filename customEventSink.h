#pragma once

template <typename T>
using EventCallback = void (*)(const T *);

template <typename T>
class EventSink : public RE::BSTEventSink<T> {
public:
    EventSink() = default;
    EventSink(const EventSink &) = delete;
    EventSink(EventSink &&) = delete;
    EventSink &operator=(const EventSink &) = delete;
    EventSink &operator=(EventSink &&) = delete;

    static EventSink *GetSingleton() {
        static EventSink singleton;
        return &singleton;
    }

    void AddCallback(EventCallback<T> callback) {
        if (!callback) return;
        std::scoped_lock lock(addCallbackLock);
        callbacks.push_back(callback);
    }

    void RemoveCallback(EventCallback<T> callback) {
        if (!callback) return;
        std::scoped_lock lock(addCallbackLock);
        auto it = std::find(callbacks.begin(), callbacks.end(), callback);
        if (it != callbacks.end()) {
            callbacks.erase(it);
        }
    }

private:
    std::mutex addCallbackLock;
    std::vector<EventCallback<T>> callbacks;

    RE::BSEventNotifyControl ProcessEvent(const T *event, RE::BSTEventSource<T> *) {
        for (auto callback : callbacks) callback(event);
        return RE::BSEventNotifyControl::kContinue;
    }
};