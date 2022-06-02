//
//  vmware_fbclient.hpp
//  vmwaregfx
//
//  Created by John Othwolo on 5/30/22.
//

#ifndef vmwaregfx_fbclient_hpp
#define vmwaregfx_fbclient_hpp

#include <IOKit/graphics/IOFramebuffer.h>
#include <IOKit/IORegistryEntry.h>
#include <IOKit/IOUserClient.h>
#include <IOKit/IOService.h>

#include "vmware_userclient.h"

class VMWareFbUserClient : public VMWareUserClient {
    
    OSDeclareDefaultStructors(VMWareFbUserClient)
    
    IOReturn clientClose() override;
    IOExternalMethod* getTargetAndMethodForIndex(IOService **targetP, UInt32 index) override;
    
};

#endif /* vmwaregfx_fbclient_hpp */
