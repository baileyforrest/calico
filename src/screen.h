#pragma once

#include <set>
#include <string>

class Screen {
 public:
  class Observer {
   public:
    virtual void OnScreenSizeChanged() = 0;

   protected:
    ~Observer() = default;
  };

  struct KeyState {
    bool alt = false;
    bool ctrl = false;
    bool is_key_code = false;
    wint_t code = 0;
  };

  Screen();
  ~Screen();

  void AddObserver(Observer* o);
  void RemoveObserver(Observer* o);

  void Clear();
  void SetChar(int row, int col, wchar_t val);
  void SetChars(int row, int col, const std::string& str);
  void SetCursorPos(int row, int col);
  void Refresh();
  void EnableReverse();
  void DisableReverse();
  void EnableBold();
  void DisableBold();

  KeyState ReadKey();

  int rows() const { return rows_; }
  int cols() const { return cols_; }

 private:
  void RefreshSize();

  std::set<Observer*> observers_;

  int rows_ = 0;
  int cols_ = 0;
};
