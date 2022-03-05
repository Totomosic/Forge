#pragma once
#include "ForgePch.h"

namespace Forge
{

    template<typename T>
    class EventEmitter;

    template<typename T>
    class FORGE_API ScopedEventListener
    {
    private:
        EventEmitter<T>* m_Emitter;
        uint32_t m_Id;

    public:
        ScopedEventListener(EventEmitter<T>* emitter, uint32_t listenerId);
        ScopedEventListener(const ScopedEventListener<T>& other) = delete;
        ScopedEventListener<T>& operator=(const ScopedEventListener<T>& other) = delete;
        ScopedEventListener(ScopedEventListener<T>&& other);
        ScopedEventListener<T>& operator=(ScopedEventListener<T>&& other);
        ~ScopedEventListener();
    };

    template<typename T>
    class FORGE_API EventEmitter
    {
    public:
        using Listener = std::function<bool(const T&)>;

    private:
        struct FORGE_API ListenerContainer
        {
        public:
            Listener Callback;
            uint32_t Id;
        };

        uint32_t m_NextId = 0;
        std::vector<ListenerContainer> m_Listeners;

    public:
        EventEmitter() = default;

        uint32_t AddEventListener(const Listener& listener)
        {
            uint32_t id = GetNextId();
            m_Listeners.push_back({listener, id});
            return id;
        }

        ScopedEventListener<T> AddScopedEventListener(const Listener& listener)
        {
            return ScopedEventListener<T>(this, AddEventListener(listener));
        }

        bool RemoveEventListener(uint32_t listener)
        {
            auto it = std::find_if(m_Listeners.begin(),
              m_Listeners.end(),
              [&](const ListenerContainer& container) { return container.Id == listener; });
            if (it != m_Listeners.end())
            {
                m_Listeners.erase(it);
                return true;
            }
            return false;
        }

        bool Trigger(const T& evt) const
        {
            bool handled = false;
            for (size_t i = 0; i < m_Listeners.size() && !handled; i++)
            {
                handled |= m_Listeners[i].Callback(evt);
            }
            return handled;
        }

        bool Trigger() const
        {
            return Trigger(T {});
        }

    private:
        uint32_t GetNextId()
        {
            return m_NextId++;
        }
    };

    template<typename T>
    ScopedEventListener<T>::ScopedEventListener(EventEmitter<T>* emitter, uint32_t listenerId)
        : m_Emitter(emitter), m_Id(listenerId)
    {
    }

    template<typename T>
    ScopedEventListener<T>::ScopedEventListener(ScopedEventListener<T>&& other)
        : m_Emitter(other.m_Emitter), m_Id(other.m_Id)
    {
        other.m_Emitter = nullptr;
    }

    template<typename T>
    ScopedEventListener<T>& ScopedEventListener<T>::operator=(ScopedEventListener<T>&& other)
    {
        std::swap(m_Emitter, other.m_Emitter);
        std::swap(m_Id, other.m_Id);
        return *this;
    }

    template<typename T>
    ScopedEventListener<T>::~ScopedEventListener()
    {
        if (m_Emitter)
            m_Emitter->RemoveEventListener(m_Id);
    }

}
