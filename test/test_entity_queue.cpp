#include "entities/lockqueue.hpp"
#include "gtest/gtest.h"

TEST(EntityQueue, LockQueueBasicTest) {
  int i = 0;
  ::entities::LockQueue<int> queue;
  EXPECT_EQ(queue.empty(), true);

  queue.push(1);
  EXPECT_EQ(queue.empty(), false);
  i = queue.front();
  EXPECT_EQ(i, 1);
  EXPECT_EQ(queue.empty(), false);

  i = queue.pop();
  EXPECT_EQ(queue.empty(), true);
  EXPECT_EQ(i, 1);
}