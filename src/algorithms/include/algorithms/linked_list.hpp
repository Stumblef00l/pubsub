#ifndef _ALGORITHMS_LINKED_LIST_HPP_
#define _ALGORITHMS_LINKED_LIST_HPP_

#include <cstddef>
#include <memory>
#include <vector>

namespace algorithms {

// Declarations

// A node of a singly linked list. Every node owns the remainder
// of the list that follows it
template<class ValueType>
struct Node {
    explicit Node(
        ValueType value);

    ValueType value;
    std::unique_ptr<Node<ValueType>> next;
};

// Reverses the list starting at head and returns the head of the
// reversed list. The caller gives up ownership of head
//
// Runs in O(n) time and O(1) additional space
template<class ValueType>
std::unique_ptr<Node<ValueType>>
ReverseIteratively(
    std::unique_ptr<Node<ValueType>> head);

// Same contract as ReverseIteratively, expressed recursively
//
// Runs in O(n) time and O(n) additional space (one stack frame per node),
// so it is only meant for short lists
template<class ValueType>
std::unique_ptr<Node<ValueType>>
ReverseRecursively(
    std::unique_ptr<Node<ValueType>> head);

// An owning singly linked list, kept deliberately small: it exists to
// demonstrate the reversal above
template<class ValueType>
class SinglyLinkedList {
    public:
        SinglyLinkedList();

        // Builds a list whose elements appear in the same order as values
        explicit SinglyLinkedList(
            const std::vector<ValueType>& values);

        ~SinglyLinkedList();

        SinglyLinkedList(const SinglyLinkedList&) = delete;
        SinglyLinkedList& operator=(const SinglyLinkedList&) = delete;

        SinglyLinkedList(
            SinglyLinkedList&& other) noexcept;

        SinglyLinkedList&
        operator=(
            SinglyLinkedList&& other) noexcept;

        void
        PushFront(
            ValueType value);

        // Reverses the list in place, iteratively
        void
        Reverse();

        // Reverses the list in place, recursively
        void
        ReverseUsingRecursion();

        bool
        IsEmpty() const;

        size_t
        GetSize() const;

        std::vector<ValueType>
        ToVector() const;

    private:
        // Releases every node without recursing, so that destroying a long
        // list cannot overflow the stack
        void
        Clear();

        std::unique_ptr<Node<ValueType>> head_;
        size_t size_;
};

} // namespace algorithms

// Implementation
#include "algorithms/linked_list.tpp"

#endif
