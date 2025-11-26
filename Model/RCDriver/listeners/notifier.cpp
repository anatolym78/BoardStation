/**
 * @file notifier.cpp
 * @brief Определение класса ListenerNotifier
 */
#include "notifier.h"

#include <algorithm>

void ListenerNotifier::attach(Listener_Ptr listener) {
    std::lock_guard lock(m_listeners_mtx);
    auto it = std::find_if(m_listeners.begin(), m_listeners.end(),
                           [=](Listener_WeakPtr weakOther) {
        if (auto other = weakOther.lock()) {
            return other == listener;
        }

        return false;
    });

    if (it != m_listeners.end()) {
        return;
    }

    m_listeners.push_back(listener);
}

void ListenerNotifier::detach(Listener_Ptr listener) {
    std::lock_guard lock(m_listeners_mtx);
    auto it = std::find_if(m_listeners.begin(), m_listeners.end(),
                           [=](Listener_WeakPtr weakOther) {
        if (auto other = weakOther.lock()) {
            return other == listener;
        }

        return false;
    });

    m_listeners.erase(it);
}

void ListenerNotifier::notify(const Subject &subject) {
    std::list<Listener_WeakPtr> listenersSnapshot;
    {
        std::lock_guard lock(m_listeners_mtx);

        // Удалить невалидных подписчиков
        m_listeners.remove_if([](const Listener_WeakPtr &wp) {
            return wp.expired();
        });

        listenersSnapshot = m_listeners;
    }

    for (Listener_WeakPtr listener : listenersSnapshot) {
        if (auto locked = listener.lock()) {
            locked->update(subject);
        }
    }
}
