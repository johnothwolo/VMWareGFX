//
//  vmwaregfx_framebuffer.cpp
//  vmwaregfx
//
//  Created by John Othwolo on 5/30/22.
//

#include <IOKit/IODeviceTreeSupport.h>
#include <libkern/version.h>
#include <kern/thread_call.h>
#include <kern/sched_prim.h>

#include "vmware_framebuffer.h"
#include "VMwareGFX.h"
#include "logging.h"

typedef IOService super;

// x86 is little Endian...
// BB60: 00 00 00 00 00 00 00 00  03 00 00 00 80 02 00 00
// BB70: E0 01 00 00 03 00 00 00  20 03 00 00 58 02 00 00
// BB80: 03 00 00 00 00 04 00 00  00 03 00 00 07 00 00 00
// BB90: 80 04 00 00 D0 02 00 00  03 00 00 00 80 04 00 00
// BBA0: 60 03 00 00 03 00 00 00  80 04 00 00 84 03 00 00
// BBB0: 03 00 00 00 00 05 00 00  20 03 00 00 03 00 00 00
// BBC0: 00 05 00 00 00 04 00 00  03 00 00 00 60 05 00 00
// BBD0: 08 04 00 00 03 00 00 00  78 05 00 00 84 03 00 00
// .....

// decompiler returns (&qword_BB60 + 12 * (a3 - 1)).
// BB60 + 12 is BB6C.
// 0xBB6C has 80 02, which is actually 0x280 (little endian)
// 0x280 is 640. is this 640x480 ?
// the modes are uint32_t values, so 2 bytes later we have E0 01 (0x1e0) which is 480!
// this must be some sort of array of modes!!!
// ....
//
//    flags    |      x      |     y
// ---------------------------------------
// 03 00 00 00 | 80 02 00 00 | E0 01 00 00 640x480
// 03 00 00 00 | 20 03 00 00 | 58 02 00 00 800x600
// 03 00 00 00 | 00 04 00 00 | 00 03 00 00 1024x768
// 07 00 00 00 | 80 04 00 00 | D0 02 00 00 1152x720
// 03 00 00 00 | 80 04 00 00 | 60 03 00 00 1152x864
// 03 00 00 00 | 80 04 00 00 | 84 03 00 00 1152x900
// 03 00 00 00 | 00 05 00 00 | 20 03 00 00 1280x800
// 03 00 00 00 | 00 05 00 00 | 00 04 00 00 1280x1024
// 03 00 00 00 | 60 05 00 00 | 08 04 00 00 1376x1152
// 03 00 00 00 | 78 05 00 00 | 84 03 00 00 1400x900

// the flag field might correspond to DRM_MODE_FLAG_* in vmwgfx in mesa
// i think vmware's kext merges the first 2 fields into a 64bit integer, but I won't do that.
dmode_t supportedModes[] = {
    { 0,    0,      3 },
    { 640,  480,    3 },
    { 800,  600,    3 },
    { 1024, 768,    7 },
    { 1152, 720,    3 },
    { 1152, 864,    3 },
    { 1152, 900,    3 },
    { 1280, 800,    3 },
    { 1280, 1024,   3 },
    { 1376, 1032,   3 },
    { 1400, 900,    3 },
    { 1400, 1050,   3 },
    { 1600, 1024,   3 },
    { 1600, 1200,   3 },
    { 1680, 1050,   3 },
    { 1920, 1200,   3 },
    { 2560, 1440,   3 },
    { 2560, 1600,   3 },
    { 2880, 1800,   3 },
};


// set ioregistry resolution property
OSData* setIoRegistryProperty(const char* propertyName, unsigned* value)
{
    OSData *resProp = OSData::withBytes(value, 4);
    IORegistryEntry *options = IORegistryEntry::fromPath("/options", gIODTPlane, 0, 0, 0);
    if (options){
        options->setProperty(propertyName, resProp);
    }
    return resProp;
}

// get ioregistry resolution property
OSData* getIoRegistryProperty(const char* propertyName)
{
    OSData *prop = nullptr;
    IORegistryEntry *options = IORegistryEntry::fromPath("/options", gIODTPlane, 0, 0, 0);
    if (options){
        prop = OSDynamicCast(OSData, options->getProperty(propertyName));
    }
    return prop;
}

// get resolution
bool getResolution(uint32_t *x, uint32_t *y)
{
    OSData *xData = getIoRegistryProperty("AC20C489-DD86-4E99-992C-B7C742C1DDA9:width");
    OSData *yData = getIoRegistryProperty("AC20C489-DD86-4E99-992C-B7C742C1DDA9:height");
    int status = 0;
    if (!xData || !yData){
        vmware_log("%s: %s: Last custom resolution: missing\n",
                   "VMwareFramebufferGetLastCustomResolution", "fb");
        return false;
    }
    
    if(xData->getLength() == 4 && yData->getLength() == 4){
        *x = *(uint32_t*)xData->getBytesNoCopy();
        *y = *(uint32_t*)yData->getBytesNoCopy();
        status = 1;
        vmware_log("%s: %s: Last custom resolution: %ux%u\n",
                   "VMwareFramebufferGetLastCustomResolution",
                   "fb", *x, *y);
    }
    
    xData->release();
    yData->release();
    
    return true;
}

bool
VMWareFrameBuffer::init(OSDictionary *properties)
{
    
    vmware_info("%s: %s: \n", "init", "fb");
    
    if (super::init(properties) != 0){
        vmware_debug("%s: %s: super::init failed.\n", "init","fb");
        return false;
    }
    
//    *(this + 61) = 0;
//    *(this + 67) = 0;
//    *(this + 83) = 0;

    vramDescriptor = OSDynamicCast(OSNumber, properties->getObject("vramDescriptor"));
    gfx = OSDynamicCast(OSNumber, properties->getObject("gfx"));
    
    vmware_info("vramDescriptor: %llu: \n", vramDescriptor->unsigned64BitValue());
    vmware_info("gfx: %llu: \n", gfx->unsigned64BitValue());
    
    darwinGreaterThan12 = version_major > 12;
    
    return true;
}

IOReturn
VMWareFrameBuffer::InitFrameBuffer()
{
    unsigned x, y;
    int result = 0;
//    if (*(((char*)this)+464))
    if(0){
        return -1;
    }
    
    if(getResolution(&x, &y))
        return this->CustomModeInternal(x, y);
    
    return result;
}

IOReturn
VMWareFrameBuffer::CustomModeInternal(int in_x, int in_y)
{
    unsigned int v28 = in_x;
    unsigned int v29 = in_y;
    int v11; // anon member variable
    int v6, v7;
    int v12; // ???
    dmode_t *display_mode = nullptr;
    dmode_t *v23 = nullptr;
    int result;
    AbsoluteTime deadline;
    
    vmware_log("%s: %s: Requested custom resolution %ux%u.\n", "CustomModeInternal", "fb", in_x, in_y);
    
    if (v28 >= 1024){
        int v9 = *(((char*)this) + 129); // current x resolution
        if (v9 < v28){
            v28 = *(((char*)this) + 129);
            v7 = v9;
        }
    } else {
        v28 = 1024;
        v7 = 1024;
    }
    
    if (v29 >= 768){
      int v10 = *((char*)this + 130); // current y resolution
      if ( v10 < v29 ){
        v29 = *((char*)this + 130);
        v6 = v10;
      }
    } else {
      v29 = 768;
      v6 = 768;
    }
    
    v11 = *((char*)this + 131); // vram size (according to VMWareFrameBuffer::start())
    
    if (v6 * ((4 * (v7 & 0x7FFFFFF) + 31) & 0x3FFFFFE0) > v11){
        v12 = 0;
        int v13 = v7 * (v11 >> 2) / v6;
        int v14 = 0x80000000;
        
        do{
            if ((v12 | v14) * (v12 | v14) <= v13)
                v12 |= v14;
            v14 >>= 1;
        } while(v14);
        
        int v15 = v12 & 0x3FFFFFF8;
        int v16 = v15 * v6;
        in_y = v16 % v7; // what is this doing?
        v29 = v16 / v7 & 0xFFFFFFFE;
        v28 = v15;
    }
    
    // ...
    setIoRegistryProperty("AC20C489-DD86-4E99-992C-B7C742C1DDA9:width", &v28);
    setIoRegistryProperty("AC20C489-DD86-4E99-992C-B7C742C1DDA9:height", &v29);
    display_mode = this->GetDisplayMode(*((uint32_t *)this + 132));
    
    if (display_mode != nullptr){
        if (display_mode->x != v28 || display_mode->y != v29){
            if ( *((char*)this + 128) )
                v23 = &supportedModes[0];
            else
                vmware_log("%s: %s: Bad mode ID=%d\n", "GetDisplayMode", "fb", display_mode->flags);
            
        } else {
            vmware_log("%s: %s: Set resolution to %ux%u (already set)\n",
                       "CustomModeInternal", "fb", v28, v29);
        }
        
        v23->x = v28;
        v23->y = v29;
        
        vmware_log("%s: %s: Set resolution to %ux%u\n", "CustomModeInternal", "fb", v28, v29);
        
        this->CustomSwitchStepSet(1);
        this->EmitConnectChangedEvent(1);
        if ( *((char*)this + 464) ){
          result = this->CustomSwitchStepWait(0);
        } else {
          this->CustomSwitchStepWait(2);
          clock_interval_to_deadline(2000, 1000000, &deadline);
          result = thread_call_enter_delayed(*((thread_call_t*)this + 67), deadline);
        }
    }
    
    return 0;
}

dmode_t*
VMWareFrameBuffer::GetDisplayMode(uint32_t mode)
{
  if (mode - 1 < *((char*)this + 128)) // this + 128 could be the number of supported modes
      return &supportedModes[1];      // (int*)(0x0BB60 + 12 * (mode - 1));

  vmware_log("%s: %s: Bad mode ID=%d\n", "GetDisplayMode", "fb", mode);
  return nullptr;
}

IOReturn
VMWareFrameBuffer::CustomSwitchStepWait(unsigned value)
{
    return 0;
}

IOReturn
VMWareFrameBuffer::CustomSwitchStepSet(unsigned value)
{
    vmware_log("%s: %s: value=%u\n", "CustomSwitchStepSet", "fb", value);
    *((uint32_t*)this + 155) = value;
    return thread_wakeup_prim(this + 620, 0, 0);
}

IOReturn VMWareFrameBuffer::EmitConnectChangedEvent(int64_t event)
{
    return 0;
}

//VMwareFramebuffer::start(IOService *)
//VMwareFramebuffer::Cleanup(void)
//VMwareFramebuffer::IsValidMode(uint,uint)
//VMwareFramebuffer::_RestoreAllModes(void *,void *)
//VMwareFramebuffer::stop(IOService *)
//VMwareFramebuffer::setupForCurrentConfig(void)
//VMwareFramebuffer::RestoreAllModes(void)
//VMwareFramebuffer::GetDisplayMode(int)
//VMwareFramebuffer::CustomSwitchStepSet(uint)
//VMwareFramebuffer::EmitConnectChangedEvent(void)
//VMwareFramebuffer::CustomSwitchStepWait(uint)
//VMwareFramebuffer::CustomModeInternal(uint,uint)
//VMwareFramebuffer::FindDepthMode(VMDepthMode)
//VMwareFramebuffer::CustomMode(VMwareGfx_CustomModeData *,VMwareGfx_CustomModeData *,ulong long,ulong long *)
//VMwareFramebuffer::isConsoleDevice(void)
//VMwareFramebuffer::getApertureRange(int)
//VMwareFramebuffer::GetApertureSize(int,int)
//VMwareFramebuffer::getVRAMRange(void)
//VMwareFramebuffer::getPixelFormats(void)
//VMwareFramebuffer::getDisplayModeCount(void)
//VMwareFramebuffer::getDisplayModes(int *)
//VMwareFramebuffer::getInformationForDisplayMode(int,IODisplayModeInformation *)
//VMwareFramebuffer::getPixelFormatsForDisplayMode(int,int)
//VMwareFramebuffer::getPixelInformation(int,int,int,IOPixelInformation *)
//VMwareFramebuffer::getCurrentDisplayMode(int *,int *)
//VMwareFramebuffer::setDisplayMode(int,int)
//VMwareFramebuffer::IOSelectToString(uint,char *)
//VMwareFramebuffer::getConnectionCount(void)
//VMwareFramebuffer::getAttribute(uint,ulong *)
//VMwareFramebuffer::getAttributeForConnection(int,uint,ulong *)
//VMwareFramebuffer::setAttribute(uint,ulong)
//VMwareFramebuffer::setAttributeForConnection(int,uint,ulong)
//VMwareFramebuffer::registerForInterruptType(uint,void (*)(OSObject *,void *),OSObject *,void *,void **)
//VMwareFramebuffer::unregisterInterrupt(void *)
//VMwareFramebuffer::setInterruptState(void *,uint)
//VMwareFramebuffer::ConvertAlphaCursor(uint *,uint,uint)
//VMwareFramebuffer::setCursorImage(void *)
//VMwareFramebuffer::setCursorState(int,int,bool)
//VMwareFramebuffer::hasDDCConnect(int)
//VMwareFramebuffer::getDDCBlock(int,uint,uint,uint,uchar *,ulong long *)
