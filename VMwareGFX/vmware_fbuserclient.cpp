//
//  vmware_fbclient.cpp
//  vmwaregfx
//
//  Created by John Othwolo on 5/30/22.
//

#include "vmware_fbuserclient.h"
#include "vmwaregfx.h"
#include "logging.h"

IOReturn
VMWareFbUserClient::clientClose()
{
    int ret = this->terminate();;
    vmware_debug("%s: %s: \n", "gfxuc", "clientClose");
    if (!ret) vmware_debug("%s: %s: terminate failed.\n", "gfxuc", "clientClose");
    
    return ret;
}

IOExternalMethod*
VMWareFbUserClient::getTargetAndMethodForIndex(IOService **targetP,
                                               UInt32 index)
{
    // every kernel cpp func has first argument set to this.
    // rdi == this
    // rsi == targetP
    // edx == index (32bit value)
    
    *targetP = this->getProvider(); // set [rsi] to our provider
    
    static const IOExternalMethod methods[] = {
        /* 0 */  { NULL, (IOMethod) &VmwareGfx::CustomMode, kIOUCScalarIScalarO, 3, 0 },
        /* 1 */  { NULL, (IOMethod) &VmwareGfx::gfxInit,    kIOUCScalarIScalarO, 3, 0 },
    };
    
    // index (edx) moves to r15d, then copied rax
    // lea rax, [rax+rax*2] ; is lea arithmetic.
    // index is 1, so this basically means mov rax, 1*1+2 ; or ; mov rax, 3 << 4
    // the statically alloced data must be the method.
    // I copied the bytes into methods[] above, and that's what it returns.
    
    if (index == 0)
        vmware_info("%s: %s: index=CustomMode\n","getTargetAndMethodForIndex", "gfxfuc");
    if (index == 1)
        vmware_info("%s: %s: index=Init\n","getTargetAndMethodForIndex", "gfxfuc");
    
    return const_cast<IOExternalMethod *>(&methods[index]);
}
