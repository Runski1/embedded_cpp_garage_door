#ifndef BUTTON_H_
#define BUTTON_H_

class Button {
  public:
    Button(uint pin, bool invert);
    bool operator()();

  private:
    const uint pin;
};

#endif
