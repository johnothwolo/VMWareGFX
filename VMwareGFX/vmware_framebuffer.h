//
//  vmwaregfx_framebuffer.hpp
//  vmwaregfx
//
//  Created by John Othwolo on 5/30/22.
//

#ifndef vmwaregfx_framebuffer_hpp
#define vmwaregfx_framebuffer_hpp

#include <IOKit/graphics/IOFramebuffer.h>
#include <IOKit/IORegistryEntry.h>
#include <IOKit/IONotifier.h>
#include <IOKit/IOService.h>

typedef struct dmode dmode_t;

class VMWareFrameBuffer : public IOFramebuffer {
     OSDeclareDefaultStructors(VMWareFrameBuffer)
  
protected:
    
private:
    
private:
    OSMetaClassDeclareReservedUsed(VMWareFrameBuffer, 0);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 1);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 2);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 3);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 4);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 5);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 6);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 7);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 8);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 9);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 10);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 11);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 12);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 13);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 14);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 15);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 16);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 17);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 18);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 19);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 20);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 21);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 22);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 23);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 24);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 25);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 26);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 27);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 28);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 29);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 30);
    OSMetaClassDeclareReservedUnused(VMWareFrameBuffer, 31);

private:
    bool darwinGreaterThan12 = false;
    OSNumber* vramDescriptor = nullptr; // offset 60
    OSNumber* gfx = nullptr; // offset 59

public:
    IOService *probe(IOService*     provider,
                     SInt32*        score) override;

    bool init(OSDictionary *dictionary = NULL) override;
    bool start(IOService* provider) override;
    void stop(IOService* provider) override;
    void free(void) override;
    
    IOReturn InitFrameBuffer(void);
    IOReturn CustomModeInternal(int x, int y);
    dmode_t* GetDisplayMode(uint32_t mode);
    IOReturn CustomSwitchStepWait(uint32_t);
    IOReturn CustomSwitchStepSet(unsigned value);
    IOReturn EmitConnectChangedEvent(int64_t event);
};

#endif /* vmwaregfx_framebuffer_hpp */
