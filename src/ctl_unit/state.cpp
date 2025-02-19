
#include "state.h"


State::State(int st_in)
{
    state=st_in;
}

int State::operator()()
{
    return state;
}

void State::operator()(int st_update)
{
    state=st_update;
}

