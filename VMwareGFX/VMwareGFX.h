//
//  vmwaregfx.h
//  vmwaregfx
//
//  Created by John Othwolo on 5/30/22.
//

#ifndef vmwaregfx_hpp
#define vmwaregfx_hpp

#include <IOKit/IORegistryEntry.h>
#include <IOKit/IONotifier.h>
#include <IOKit/IOService.h>
#include <IOKit/IOLocks.h>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#include <IOKit/IOInterruptEventSource.h>
#pragma clang diagnostic pop

class VMwareFramebuffer;
struct VmwareGfxCustomModeData;
class SVGADevice;

typedef struct dmode {
    int x;
    int y;
    int flags;
} dmode_t;

class VmwareGfx : public IOService
{
    OSDeclareAbstractStructors(VmwareGfx)
private:
    IOLock *svgaMutex_27 = nullptr;
    IOLock *userClientMutex_28 = nullptr;
    OSArray *frameBufferArray_29 = nullptr;
    SVGADevice *svgaDevice;

public:
    virtual IOReturn getSymbol( const char * symbolName,
                                IOLogicalAddress * address ) = 0;
    
    bool init(OSDictionary *dictionary = NULL) override;
    void free() override;
    bool start(IOService *provider) override;
    void stop(IOService *provider) override;
    
    IOReturn gfxInit(void *arg1, u_long, u_long);
    IOReturn CustomMode(VmwareGfxCustomModeData*, VmwareGfxCustomModeData*, uint64_t, uint64_t*);
    
    IOReturn _DisplayWranglerPublished(void*, void*, IOService*, IONotifier*);
    IOReturn DisplayWranglerPublished(void*, IOService*, IONotifier*);
    IOReturn powerStateWillChangeTo(u_long, u_long, IOService*) override;
    IOReturn powerStateDidChangeTo(u_long, u_long, IOService*) override;
    
    IOReturn Cleanup();
    IOReturn CleanupFb(VMwareFramebuffer *a2);
    
};

#endif /* vmwaregfx_hpp */
