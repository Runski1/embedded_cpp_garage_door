
#include "../../src/ctl_unit/state_machine.h"
#include <memory>

int main()
{
    std::shared_ptr<Button> bt0=std::make_shared<Button>(10);
    StateMachine st_mach(bt0, 1);
    
    int ST=st_mach();
    long unsigned it=0;

    for (;;)    // let it burn
    {
        st_mach.operate();
        ++it;
    }
}
