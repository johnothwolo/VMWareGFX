//
//  vmware_userclient.cpp
//  vmwaregfx
//
//  Created by John Othwolo on 5/30/22.
//

#include "vmware_userclient.h"
#include "logging.h"


IOReturn
VMWareUserClient::clientMemoryForType(UInt32 type, IOOptionBits *options,
                                      IOMemoryDescriptor **memory)
{
    IOReturn ret;
    
    vmware_log("%s %lx: %s: Unexpected call. type %u options %p memory %p.\n",
               "clientMemoryForType", 0L, "base", type, options, memory);
    
    vmware_log("%s %lx: %s: before *options 0x%x.\n",
               "clientMemoryForType", 0L, "base", options ? *options : 0);
    
    vmware_log("%s %lx: %s: before *memory %p.\n",
               "clientMemoryForType", 0L, "base", memory ? *memory : 0);
    
    ret = IOUserClient::clientMemoryForType(type, options, memory);
    
    vmware_log("%s %lx: %s: after *options 0x%x.\n",
               "clientMemoryForType", 0L, "base", options ? *options : 0);
    vmware_log("%s %lx: %s: after *memory %p.\n",
               "clientMemoryForType", 0L, "base", memory ? *memory : 0);
    
    return ret;
}
