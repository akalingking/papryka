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
 * @date        July 2, 2016 10:38 PM
 * @copyright   (c) 2016-2027 <www.sequenceresearch.com>
 */
#pragma once
#include <vector>
#include <functional>
#include <memory>
#include <tuple>
#include <type_traits>

namespace papryka {
    namespace detail {
        template<int> struct placeholder {};
    }
}

namespace std {
    template<int N>
    struct is_placeholder<papryka::detail::placeholder<N> > 
            : integral_constant<int, N + 1 > {};
}

namespace papryka {
    
    namespace detail {
        //@{ integer sequence
        template <int...I>
        struct index{};
        
        template <int N, int...I>
        struct sequence : sequence<N-1, N-1, I...> {};

        template <int... I>
        struct sequence<0, I...> : index<I...> {};
        //@}
        
        //@{ order id
        template <typename _T> struct static_id { static uint32_t value; };
        template <typename _T> uint32_t static_id<_T>::value = 0;
        typedef static_id<void> static_id_t;
        //@}
        
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

        typedef std::vector<action_t, 
            std::allocator<action_t>> subscribers_t;

        typedef std::vector<subscribers_t::iterator,
            std::allocator<subscribers_t::iterator>> unsubscribers_t;

        subscribers_t subscribers_;
        subscribers_t to_subscribe_;
        unsubscribers_t to_unsubscribe_;

        bool is_emitting_;

    public:
        Event() : is_emitting_(false) {}

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
            if (!is_emitting_)
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
                if (!is_emitting_)
                    subscribers_.erase(iter);
                else
                    to_unsubscribe_.push_back(iter);
            }
        }

        template <typename...Args>
        void emit(Args&&...args)
        {
            is_emitting_ = true;
            for (action_t& subscriber : subscribers_)
            {
                Action < Args...>* action = static_cast<Action < Args...>*> (subscriber.get());
                if (action != nullptr)
                    (*action)(std::forward<Args>(args)...);
            }
            is_emitting_ = false;
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

