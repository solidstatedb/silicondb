#ifndef SILICONDB_H
#define SILICONDB_H

#include <memory>
#include <mutex>

#include <shared_mutex>

namespace silicondb
{
    // list: threadsafe implementation of a singly linked list. This is meant to be
    // used as the underlying datastructure for silicondb::map::bucket::data as a
    // standin replacement for std::list. As a result, we don't provide an
    // exhaustive API for this list implementation.
    template <typename T>
    class list
    {
    private:
        /// list::node: node in a linked list. Every node contains a unique pointer
        /// to the next node, thereby having sole ownership of the next node.
        /// Furthermore, every node contains a mutex. This mutex acts as a locked
        /// gate, which lets threads pass through it when unlocked.
        struct node
        {
            std::unique_ptr<node> next; ///< unique ownership of the next pointer
            std::shared_mutex mtx;      ///< mutex to act as a locked gate.

            std::shared_ptr<T> data;    ///< shared ownership of data

            node() : next{} {}
            node(T const &data_) : data{std::make_shared<T>(data_)} {}
        };

        node head; ///< sentinel head node

    public:
        list() {}

        ~list()
        {
            remove_if([](T const &_)
                      { return true; });
        }

        // remove all copy operations by default
        list(list const &other) = delete;
        list &operator=(list const &other) = delete;

        /// Pushes a new value at the front of the list. This operation creates a new
        /// node from the given value, and places it next to the sentinel node. To
        /// ensure thread safety, this operation accquires a unique lock on the head
        /// node.
        void push_front(T const &value)
        {
            std::unique_ptr<node> node_created{new node(value)};
            std::unique_lock<std::shared_mutex> lk(head.mtx);

            node_created->next = std::move(head.next);
            head.next = std::move(node_created);
        }

        /// Removes all elements for which the given predicate function returns true.
        /// This operation requires unique locking of a sequential pair of nodes
        /// during the traversal of the list.
        template <typename Predicate>
        void remove_if(Predicate p)
        {
            node *current = &head;
            std::unique_lock<std::shared_mutex> lk(head.mtx);

            while (node *const next = current->next.get())
            {
                std::unique_lock<std::shared_mutex> next_lk(next->mtx);
                if (p(*next->data))
                {
                    std::unique_ptr<node> old_next = std::move(current->next);
                    current->next = std::move(next->next);
                    next_lk.unlock();
                }
                else
                {
                    lk.unlock();
                    current = next;
                    lk = std::move(next_lk);
                }
            }
        }

        /// Operates on every data value stored in this list with the given function.
        /// It acts a replacement for the iterator API since the iterator API is
        /// difficult to implement while taking thread safety into account.
        template <typename Function>
        void for_each(Function f)
        {
            node *current = &head;
            std::shared_lock<std::shared_mutex> lk(head.mtx);

            while (node *const next = current->next.get())
            {
                std::shared_lock<std::shared_mutex> next_lk(next->mtx);
                lk.unlock();

                f(*next->data);

                current = next;
                lk = std::move(next_lk);
            }
        }

        /// Returns a std::shared_ptr to the first data value for which the given the
        /// given predicate function returns true. The ordering of the nodes is
        /// determined from head to tail.
        template <typename Predicate>
        std::shared_ptr<T> find_first_if(Predicate p)
        {
            node *current = &head;
            std::shared_lock<std::shared_mutex> lk(head.mtx);

            while (node *const next = current->next.get())
            {
                std::shared_lock<std::shared_mutex> next_lk(next->mtx);
                lk.unlock();

                if (p(*next->data))
                    return next->data;

                current = next;
                lk = std::move(next_lk);
            }

            return std::shared_ptr<T>{};
        }
    };

};

#endif