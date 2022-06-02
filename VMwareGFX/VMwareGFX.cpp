/* add your code here */


#include "vmware_svgadevice.h"
#include "vmware_framebuffer.h"
#include "vmwaregfx.h"
#include "logging.h"

bool
VmwareGfx::init(OSDictionary *dictionary)
{
    vmware_log("%s: %s: \n", "init", "gfx");
    
    if (IOService::init(dictionary) != true){
        vmware_log("%s: %s: super::init failed.\n", "init", "gfx");
        return false;
    }
    
//    (this + 136)  is the svgaDevice
    if (svgaDevice->init() != true){
        vmware_log("%s: %s: svga.Init failed.\n", "init", "gfx");
        return false;
    }
    
//    *(this + 27) = 0LL; // IOLock
//    *(this + 28) = 0LL; // IOLock
//    *(this + 29) = 0LL; // frameBufferArray
//    *(this + 30) = 0LL;
//    *(this + 31) = 0LL;
    return true;
}

void
VmwareGfx::free()
{
}

bool
VmwareGfx::start(IOService *provider)
{
    bool success = false;
    OSString *logstring = nullptr;
    
    vmware_log("%s: %s: \n", "start", "gfx");
    
    do {
        if(svgaDevice->start(provider) == false){
            logstring = OSString::withCString("svgaDev.Start failed.");
            break;
        }
        
        if ((svgaMutex_27 = IOLockAlloc()) == nullptr){
            logstring = OSString::withCString("failed to alloc svgaMutex_27");
            break;
        }
        
        if ((userClientMutex_28 = IOLockAlloc()) == nullptr){
            logstring = OSString::withCString("failed to alloc userClientMutex_28");
            break;
        }
        
        if ((frameBufferArray_29 = OSArray::withCapacity(1)) == nullptr){
            logstring = OSString::withCString("Failed to allocate fbs.");
            break;
        }

    } while(0);
    
    if (!success){
        vmware_log("%s", logstring->getCStringNoCopy());
        logstring->free();
        return false;
    } else return true;
}

void
VmwareGfx::stop(IOService *provider)
{
}

IOReturn
VmwareGfx::gfxInit(void *arg1, u_long, u_long)
{
    VMWareFrameBuffer *fb = nullptr;
    vmware_log("%s: %s: \n", "gfxInit", "gfx");
    
    if (frameBufferArray_29->getCount() == 1) {
        IOLockLock(svgaMutex_27);
        fb = OSDynamicCast(VMWareFrameBuffer, frameBufferArray_29->getObject(0));
        fb->InitFrameBuffer();
        IOLockUnlock(svgaMutex_27);
    }
    return 0;
}

IOReturn
VmwareGfx::CustomMode(VmwareGfxCustomModeData*, VmwareGfxCustomModeData*, uint64_t, uint64_t*)
{
    return 0;
}


IOReturn
VmwareGfx::_DisplayWranglerPublished(void* arg1, void* arg2, IOService* service, IONotifier* notifier)
{
    return DisplayWranglerPublished(arg2, service, notifier);
}

IOReturn
VmwareGfx::DisplayWranglerPublished(void*, IOService* service, IONotifier* notifier)
{
    vmware_log("%s: %s: \n", "DisplayWranglerPublished", "gfx");
    IOService::deRegisterInterestedDriver(service);
    return 1;
}

IOReturn
VmwareGfx::powerStateWillChangeTo(u_long, u_long, IOService*)
{
    return 0;
}

IOReturn
VmwareGfx::powerStateDidChangeTo(u_long, u_long, IOService*)
{
    return 0;
}


IOReturn
VmwareGfx::Cleanup()
{
    return 0;
}

IOReturn
VmwareGfx::CleanupFb(VMwareFramebuffer *a2)
{
    return 0;
}

