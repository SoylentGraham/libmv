// Copyright (c) 2007, 2008 libmv authors.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#ifndef TESTING_TESTING_H_
#define TESTING_TESTING_H_

#include "libmv/numeric/numeric.h"
#include "third_party/gtest/include/gtest/gtest.h"
#include "third_party/gflags/gflags/gflags.h"

DECLARE_string(test_tmpdir);

#define EXPECT_MATRIX_NEAR(a, b, tolerance) \
do { \
  bool dims_match = (a.rows() == b.rows()) && (a.cols() == b.cols()); \
  EXPECT_EQ(a.rows(), b.rows()) << "Matrix rows don't match."; \
  EXPECT_EQ(a.cols(), b.cols()) << "Matrix cols don't match."; \
  if (dims_match) { \
    for (int r = 0; r < a.rows(); ++r) { \
      for (int c = 0; c < a.cols(); ++c) { \
        EXPECT_NEAR(a(r, c), b(r, c), tolerance) \
          << "r=" << r << ", c=" << c << "."; \
      } \
    } \
  } \
} while(false);

#define EXPECT_MATRIX_NEAR_ZERO(a, tolerance) \
do { \
  for (int r = 0; r < a.rows(); ++r) { \
    for (int c = 0; c < a.cols(); ++c) { \
      EXPECT_NEAR(0.0, a(r, c), tolerance) \
        << "r=" << r << ", c=" << c << "."; \
    } \
  } \
} while(false);

#define EXPECT_MATRIX_EQ(a, b) \
do { \
  bool dims_match = (a.rows() == b.rows()) && (a.cols() == b.cols()); \
  EXPECT_EQ(a.rows(), b.rows()) << "Matrix rows don't match."; \
  EXPECT_EQ(a.cols(), b.cols()) << "Matrix cols don't match."; \
  if (dims_match) { \
    for (int r = 0; r < a.rows(); ++r) { \
      for (int c = 0; c < a.cols(); ++c) { \
        EXPECT_EQ(a(r, c), b(r, c)) \
          << "r=" << r << ", c=" << c << "."; \
      } \
    } \
  } \
} while(false);

// Check that sin(angle(a, b)) < tolerance.
#define EXPECT_MATRIX_PROP(a, b, tolerance) \
do { \
  bool dims_match = (a.rows() == b.rows()) && (a.cols() == b.cols()); \
  EXPECT_EQ(a.rows(), b.rows()) << "Matrix rows don't match."; \
  EXPECT_EQ(a.cols(), b.cols()) << "Matrix cols don't match."; \
  if (dims_match) { \
    double c = CosinusBetweenMatrices(a, b); \
    if (c * c < 1) { \
      double s = sqrt(1 - c * c); \
      EXPECT_NEAR(0, s, tolerance); \
    } \
  } \
} while(false);

template<class TMat>
double CosinusBetweenMatrices(const TMat &a, const TMat &b) {
  return (a.array() * b.array()).sum() / 
      libmv::FrobeniusNorm(a) / libmv::FrobeniusNorm(b);
}

#endif  // TESTING_TESTING_H_
