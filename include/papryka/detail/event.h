/**
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file        event.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 */
#pragma once
#include "logger.h"
#include <vector>
#include <functional>
#include <memory>
#include <tuple>
#include <type_traits>
#include <atomic>
/**
 * @caveat using volatile on plain boolean flag is causing segmentation fault.
 * replacing with std::atomic<bool> fixes this error
 */
#define STL_ATOMIC

namespace papryka {
namespace detail {
    template<int> struct placeholder {};
}}

namespace std {
    template<int N>
    struct is_placeholder<papryka::detail::placeholder<N> >
            : integral_constant<int, N + 1 > {};
}

namespace papryka {
namespace detail {
    template <int...I> struct index{};
    template <int N, int...I>
    struct sequence : sequence<N-1, N-1, I...> {};

    template <int... I>
    struct sequence<0, I...> : index<I...> {};


    template <typename _T> struct static_id { static uint32_t value; };
    template <typename _T> uint32_t static_id<_T>::value = 0;
    typedef static_id<void> static_id_t;

    struct Action_ : private static_id_t
    {
        Action_() : id_(++static_id_t::value) {}
        virtual ~Action_() {}
        uint32_t id_;
    };
}

template <typename...Args>
class Action : public detail::Action_
{
private:
    std::function<void(Args...) > fn;

    template<int...Is>
    void func_(std::tuple<Args...>& tup, detail::index<Is...>)
    {
        fn(std::get<Is>(tup)...);
    }

    template <typename T, int...Is>
    void bind_(void(T::*fp)(Args...), T* t, detail::index<Is...>)
    {
        fn = std::bind(fp, t, detail::placeholder<Is>{}...);
    }

public:

    template <typename T>
    Action(void(T::*fp)(Args...), T* t)
    {
        bind_(fp, t, detail::sequence<sizeof...(Args)>{});
    }

    template <typename F>
    Action(F&& func) : fn(std::forward<F>(func))
    {
    }

    void operator()(Args&&...args)
    {
        std::tuple < Args...> tup(std::forward<Args>(args)...);
        func_(tup, detail::sequence<sizeof...(Args)>{});
    }
};

class Event
{
private:
    typedef std::shared_ptr<detail::Action_> action_t;

    typedef std::vector<action_t, std::allocator<action_t>> subscribers_t;

    typedef std::vector<subscribers_t::iterator, std::allocator<subscribers_t::iterator>> unsubscribers_t;

    subscribers_t subscribers_;
    subscribers_t to_subscribe_;
    unsubscribers_t to_unsubscribe_;

    // works but not guaranteed
#ifndef STL_ATOMIC
    volatile bool is_emitting_;
#else
    std::atomic<bool> is_emitting_;
#endif
public:
    Event() : is_emitting_(false)  { log_trace("Event::{}", __func__); }

    ~Event() { log_trace("Event::{}", __func__); }
    /**
     *
     * @param
     * @param
     * @return
     */
    template<typename T, typename...Args>
    uint32_t subscribe(void(T::*fp)(Args...), T* t)
    {
        action_t action(new Action < Args...>(fp, t));
#ifndef STL_ATOMIC
        if (!is_emitting_)
#else
        if (!is_emitting_.load())
#endif
            subscribers_.push_back(action);
        else
            to_subscribe_.push_back(action);
        return action->id_;
    }

    void unsubscribe(uint32_t id)
    {
        // suboptimal
        // cant use map which sorts the input
        // input sequence must be ordered as they arrive.
        subscribers_t::iterator iter = subscribers_.begin();
        for (; iter != subscribers_.end(); ++iter)
        {
            if ((*iter)->id_ == id)
                break;
        }

        if ((*iter)->id_ == id)
        {

#ifndef STL_ATOMIC
        if (!is_emitting_)
#else
        if (!is_emitting_.load())
#endif
                subscribers_.erase(iter);
            else
                to_unsubscribe_.push_back(iter);
        }
    }

    template <typename...Args>
    void emit(Args&&...args)
    {
#ifndef STL_ATOMIC
        is_emitting_ = true;
#else
        is_emitting_.store(true);
#endif
        for (action_t& subscriber : subscribers_)
        {
            Action < Args...>* action = static_cast<Action < Args...>*> (subscriber.get());
            if (action != nullptr)
                (*action)(std::forward<Args>(args)...);
        }
#ifndef STL_ATOMIC
        is_emitting_ = false;
#else
        is_emitting_.store(false);
#endif
        apply_changes_();
    }

private:
    void apply_changes_()
    {
        unsubscribers_t::iterator iter = to_unsubscribe_.begin();
        for (; iter != to_unsubscribe_.end(); ++iter)
            subscribers_.erase(*iter);
        subscribers_t::iterator iter_ = to_subscribe_.begin();
        for (; iter_ != to_subscribe_.end(); ++iter_)
            subscribers_.push_back(*iter_);
    }
};

} // namespace papryka

