/**
 * A thread safe Queue for a single producer/consumer pair
 *
 * count_ must be only variable accessed by both producer and consummer.
 *
 */
#ifndef Queue_h
#define Queue_h
#include <util/atomic.h>
#include <stdint.h>

template<typename Item, uint16_t Size>
class QueueTemplate {
 public:
  /** constructor */
  QueueTemplate() : count_(0), head_(0), tail_(0) {}
  /** destructor */
  ~QueueTemplate() {}
  /** \return count of items in queue */
  uint16_t count() {
    uint16_t rtn;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      rtn = count_;
    }
    return rtn;
  }
  /** \return head index */
  uint16_t head() {return head_;}
  /** \return Queue size in items */
  uint16_t size() {return Size;}
  /** \return tail index */
  uint16_t tail() {return tail_;}
  /** \return pointer to next empty item at head of queue */
  Item* headItem() {return count() < Size ? buff_ + head_ : 0;}
  /** \return pointer to next filled item at end of queue */
  Item* tailItem() {return count() != 0 ? buff_ + tail_ : 0;}
  /** Mark item at head of queue filled
   *
   * \return true for success else false
   */
  bool headNext() {
    if (count() >= Size) return false;
    head_ = head_ < (Size - 1) ? head_ + 1 : 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      count_++;
    }
    return true;
  }
  /** Free item at tail of queue
   *
   * \return true for success else false
   */
  bool tailNext() {
    if (count() == 0) return false;
    tail_ = tail_ < (Size - 1) ? tail_ + 1 : 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      count_--;
    }
    return true;
  }
 private:
  Item buff_[Size];
  volatile uint16_t count_;
  volatile uint16_t head_;
  volatile uint16_t tail_;
};
#endif  // Queue_h