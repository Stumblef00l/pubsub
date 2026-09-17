// Provides template implementation for algorithms/linked_list.hpp. DO NOT IMPORT DIRECTLY
#ifndef _ALGORITHMS_LINKED_LIST_TPP_
#define _ALGORITHMS_LINKED_LIST_TPP_

#include "algorithms/linked_list.hpp"

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

namespace algorithms {

template<class ValueType>
Node<ValueType>::Node(
    ValueType value)
    : value(std::move(value)),
      next(nullptr) {}

template<class ValueType>
std::unique_ptr<Node<ValueType>>
ReverseIteratively(
    std::unique_ptr<Node<ValueType>> head) {
    auto reversed = std::unique_ptr<Node<ValueType>>{};

    while (head) {
        // Detach the rest of the list, hang what we have reversed so far
        // off the current node, and make the current node the new head
        auto remaining = std::move(head->next);
        head->next = std::move(reversed);
        reversed = std::move(head);
        head = std::move(remaining);
    }

    return reversed;
}

template<class ValueType>
std::unique_ptr<Node<ValueType>>
ReverseRecursively(
    std::unique_ptr<Node<ValueType>> head) {
    // An empty list and a single node list are their own reversal
    if (!head || !(head->next))
        return head;

    // The second node becomes the tail of the reversed list, so we keep a
    // borrowed pointer to it before handing ownership to the recursive call
    auto* second = (head->next).get();
    auto reversed = ReverseRecursively(std::move(head->next));

    // head->next was emptied by the move above, so head is a valid tail
    second->next = std::move(head);

    return reversed;
}

template<class ValueType>
SinglyLinkedList<ValueType>::SinglyLinkedList()
    : head_(nullptr),
      size_(0) {}

template<class ValueType>
SinglyLinkedList<ValueType>::SinglyLinkedList(
    const std::vector<ValueType>& values)
    : head_(nullptr),
      size_(0) {
    for (auto value = values.rbegin(); value != values.rend(); value++)
        PushFront(*value);
}

template<class ValueType>
SinglyLinkedList<ValueType>::SinglyLinkedList(
    SinglyLinkedList<ValueType>&& other) noexcept
    : head_(std::move(other.head_)),
      size_(other.size_) {
    other.size_ = 0;
}

template<class ValueType>
SinglyLinkedList<ValueType>&
SinglyLinkedList<ValueType>::operator=(
    SinglyLinkedList<ValueType>&& other) noexcept {
    if (this == &other)
        return *this;

    Clear();

    head_ = std::move(other.head_);
    size_ = other.size_;
    other.size_ = 0;

    return *this;
}

template<class ValueType>
SinglyLinkedList<ValueType>::~SinglyLinkedList() {
    Clear();
}

template<class ValueType>
void
SinglyLinkedList<ValueType>::PushFront(
    ValueType value) {
    auto node = std::make_unique<Node<ValueType>>(std::move(value));
    node->next = std::move(head_);
    head_ = std::move(node);
    size_++;
}

template<class ValueType>
void
SinglyLinkedList<ValueType>::Reverse() {
    head_ = ReverseIteratively(std::move(head_));
}

template<class ValueType>
void
SinglyLinkedList<ValueType>::ReverseUsingRecursion() {
    head_ = ReverseRecursively(std::move(head_));
}

template<class ValueType>
bool
SinglyLinkedList<ValueType>::IsEmpty() const {
    return (size_ == 0);
}

template<class ValueType>
size_t
SinglyLinkedList<ValueType>::GetSize() const {
    return size_;
}

template<class ValueType>
std::vector<ValueType>
SinglyLinkedList<ValueType>::ToVector() const {
    auto values = std::vector<ValueType>{};
    values.reserve(size_);

    for (auto* node = head_.get(); node != nullptr; node = (node->next).get())
        values.push_back(node->value);

    return values;
}

template<class ValueType>
void
SinglyLinkedList<ValueType>::Clear() {
    // Destroying head_ directly would recurse once per node, so we unlink
    // the nodes one at a time and let each of them go out of scope alone
    while (head_) {
        auto node = std::move(head_);
        head_ = std::move(node->next);
    }

    size_ = 0;
}

} // namespace algorithms

#endif
