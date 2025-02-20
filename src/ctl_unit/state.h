
#ifndef STATE_H_

/* 
class Controller {
    public:
        int RM; 
    private:
        int RM0;
};

 * the highest class. It would contain methods to do stuff with states 
 * > Do something based on info which it would get from state.
 *      Maybe smart pointer?
 *
 * State would be an object to set and change the state of the system 
 * > contain info about state
 *
 * */

class State {
public:
    int operator()();
    void operator()(int);
    int getState(void);
    void setState(int);

    State(int);
    State(State &) = delete; // do not copy
private:
    int state;
    bool mode_local;
    bool error;
};

#endif
