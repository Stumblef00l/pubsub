
#include "gmock/gmock.h"
#include "gtest/gtest.h"

unsigned int ConvertRGBAToBGRA(unsigned int v) {
  unsigned char r = (v)&0xFF;
  unsigned char g = (v >> 8) & 0xFF;
  unsigned char b = (v >> 16) & 0xFF;
  unsigned char a = (v >> 24) & 0xFF;
  return (a << 24) | (r << 16) | (g << 8) | b;
};

MATCHER_P(IsBgraOf, n, "") {
  return ((n & 0xFF000000) == (arg & 0xFF000000)) &&
         ((n & 0x00FF0000) == ((arg << 16) & 0x00FF0000)) &&
         ((n & 0x0000FF00) == (arg & 0x0000FF00));
}

TEST(ConvertRGBAToBGRATest, SimpleTests) {
  EXPECT_EQ(ConvertRGBAToBGRA(0x12'34'56'78), 0x12'78'56'34);
  EXPECT_EQ(ConvertRGBAToBGRA(0x12'78'56'34), 0x12'34'56'78);
  EXPECT_EQ(ConvertRGBAToBGRA(0x11'11'11'11), 0x11'11'11'11);
  EXPECT_EQ(ConvertRGBAToBGRA(0x00'00'00'00), 0x00'00'00'00);
  EXPECT_EQ(ConvertRGBAToBGRA(0xAa'Bb'Cc'Dd), 0xAa'Dd'Cc'Bb);

  EXPECT_EQ(ConvertRGBAToBGRA(ConvertRGBAToBGRA(0x12'34'56'78)), 0x12'34'56'78);
  EXPECT_EQ(ConvertRGBAToBGRA(ConvertRGBAToBGRA(0x12'78'56'34)), 0x12'78'56'34);
  EXPECT_EQ(ConvertRGBAToBGRA(ConvertRGBAToBGRA(0x11'11'11'11)), 0x11'11'11'11);
  EXPECT_EQ(ConvertRGBAToBGRA(ConvertRGBAToBGRA(0x00'00'00'00)), 0x00'00'00'00);
  EXPECT_EQ(ConvertRGBAToBGRA(ConvertRGBAToBGRA(0xAa'Bb'Cc'Dd)), 0xAa'Bb'Cc'Dd);
}

TEST(ConvertRGBAToBGRATest, WithExpectThat) {
  EXPECT_THAT(ConvertRGBAToBGRA(0x12'34'56'78), IsBgraOf(0x12'34'56'78));
  EXPECT_THAT(ConvertRGBAToBGRA(0x12'78'56'34), IsBgraOf(0x12'78'56'34));
  EXPECT_THAT(ConvertRGBAToBGRA(0xAa'Bb'Cc'Dd), IsBgraOf(0xAa'Bb'Cc'Dd));
  EXPECT_THAT(ConvertRGBAToBGRA(0x00'00'00'00), IsBgraOf(0x00'00'00'00));
  EXPECT_THAT(ConvertRGBAToBGRA(0x11'11'11'11), IsBgraOf(0x11'11'11'11));
}