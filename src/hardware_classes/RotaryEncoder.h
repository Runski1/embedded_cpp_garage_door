#ifndef ROTARY_ENCODER_H_
#define ROTARY_ENCODER_H_

#define ROT_TEST

class RotaryEncoder {
  public:
    RotaryEncoder();
    void print();
#ifdef ROT_TEST
    void test();
#endif
  
  private:
    unsigned int rot_a;
    unsigned int rot_b;
};

#endif
