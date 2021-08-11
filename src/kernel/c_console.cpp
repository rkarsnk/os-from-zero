/**
 * @file console.cpp
 *
 * コンソール描画プログラム
 */
#include <console.hpp>

Console::Console(const PixelColor& fg_color, const PixelColor& bg_color)
    : writer_{ nullptr },
      fg_color_{ fg_color },
      bg_color_{ bg_color },
      buffer_{},
      cursor_row_{ 0 },
      cursor_column_{ 0 } {}

void Console::PutString(const char* string) {
  while (*string) {
    if (*string == '\n') {
      NewLine();
    } else if (cursor_column_ < ttyColumns - 1) {
      WriteAscii(*writer_, 8 * cursor_column_, 16 * cursor_row_, *string, fg_color_);
      buffer_[cursor_row_][cursor_column_] = *string;
      ++cursor_column_;
    }
    ++string;
  }
  if (layer_manager) {
    layer_manager->Draw();
  }
}

void Console::SetWriter(PixelWriter* writer) {
  if (writer == writer_) {
    return;
  }
  writer_ = writer;
  Refresh();
}

void Console::NewLine() {
  cursor_column_ = 0;
  if (cursor_row_ < ttyRows - 1) {
    ++cursor_row_;
  } else {
    for (int y = 0; y < 16 * ttyRows; ++y) {
      for (int x = 0; x < 8 * ttyColumns; ++x) {
        writer_->Write(x, y, bg_color_);
      }
    }
    for (int row = 0; row < ttyRows - 1; ++row) {
      memcpy(buffer_[row], buffer_[row + 1], ttyColumns + 1);
      WriteString(*writer_, 0, 16 * row, buffer_[row], fg_color_);
    }
    memset(buffer_[ttyRows - 1], 0, ttyColumns + 1);
  }
}

void Console::Refresh() {
  for (int row = 0; row < ttyRows; ++row) {
    WriteString(*writer_, 0, 16 * row, buffer_[row], fg_color_);
  }
}
