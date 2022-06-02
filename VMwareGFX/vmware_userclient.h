//
//  vmware_userclient.hpp
//  vmwaregfx
//
//  Created by John Othwolo on 5/30/22.
//

#ifndef vmware_userclient_hpp
#define vmware_userclient_hpp

#include <IOKit/graphics/IOFramebuffer.h>
#include <IOKit/IORegistryEntry.h>
#include <IOKit/IOUserClient.h>
#include <IOKit/IOService.h>

class VMWareUserClient : public IOUserClient {

    IOFramebuffer *fOwner;
    
    OSDeclareDefaultStructors(VMWareUserClient)
    
    IOReturn clientMemoryForType(UInt32 type, IOOptionBits *options, IOMemoryDescriptor **memory) override;
    
    virtual IOReturn clientClose() override = 0;
    virtual IOExternalMethod* getTargetAndMethodForIndex(IOService **targetP, UInt32 index) override;
    
};

#endif /* vmware_userclient_hpp */
