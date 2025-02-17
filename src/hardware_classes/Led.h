#ifndef LED_H_
#define LED_H_

class Led {
  public:
    Led(const uint pin);
    void operator()(bool state);
    void operator()();
    uint state();

  private:
    const uint pin;
};

#endif
