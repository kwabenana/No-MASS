// Copyright 2015 Jacob Chapman

#include <limits.h>
#include "Log.h"
#include "gtest/gtest.h"

TEST(Log, error) {
  LOG.reset();
  LOG << "hello";
  EXPECT_EQ(LOG.getError(), false);
  LOG.error();
  EXPECT_EQ(LOG.getError(), true);
  LOG.reset();
}