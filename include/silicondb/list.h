#ifndef SILICONDB_H
#define SILICONDB_H

#include <memory>
#include <mutex>

#include <shared_mutex>

namespace silicondb
{
    // list: threadsafe implementation of a singly linked list. This is meant to be
    // used as the underlying datastructure for silicondb::map::bucket::data as a
    // standin replacement for std::list. As a result, we don't provide exhaustive
    // API for this list implementation.
    template <typename T>
    class list
    {
    private:
        // list::node: node in a linked list. Every node contains a unique pointer
        // to the next node, thereby having sole ownership of the next node.
        // Furthermore, every node contains a mutex to enable unique (read/write)
        // lock on the sub-list starting from that node.
        struct node
        {
            std::unique_ptr<node> next; // unique ownership of the next pointer
            std::shared_mutex subl_mtx; // mutex to lock on the sublist starting
                                        // from this node

            std::shared_ptr<T> data; // shared ownership of data with pointer

            node() : next{} {}
            node(T const &data_) : data{std::make_shared<T>(data_)} {}
        };

        node head; // sentinel head node

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

        void push_front(T const &value)
        {
            std::unique_ptr<node> node_created{new node(value)};
            std::lock_guard<std::shared_mutex> lk(head.subl_mtx);

            node_created->next = std::move(head.next);
            head.next = std::move(node_created);
        }

        template <typename Predicate>
        void remove_if(Predicate p)
        {
            node *current = &head;
            std::unique_lock<std::shared_mutex> lk(head.subl_mtx);

            while (node *const next = current->next.get())
            {
                std::unique_lock<std::shared_mutex> next_lk(next->subl_mtx);
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

        template <typename Function>
        void for_each(Function f)
        {
            node *current = &head;
            std::shared_lock<std::shared_mutex> lk(head.subl_mtx);

            while (node *const next = current->next.get())
            {
                std::shared_lock<std::shared_mutex> next_lk(next->subl_mtx);
                lk.unlock();

                f(*next->data);

                current = next;
                lk = std::move(next_lk);
            }
        }

        template <typename Predicate>
        std::shared_ptr<T> find_first_if(Predicate p)
        {
            node *current = &head;
            std::shared_lock<std::shared_mutex> lk(head.subl_mtx);

            while (node *const next = current->next.get())
            {
                std::shared_lock<std::shared_mutex> next_lk(next->subl_mtx);
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