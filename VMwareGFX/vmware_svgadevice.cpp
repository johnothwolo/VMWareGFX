//
//  vmware_svgadevice.cpp
//  vmwaregfx
//
//  Created by John Othwolo on 5/31/22.
//

#include "vmware_svgadevice.h"
#include "vmware_framebuffer.h"
#include "vmwaregfx.h"
#include "logging.h"


bool
SVGADevice::init()
{
      vmware_log("%s: %s: \n", "Init", "svga");
//    *(this + 1) = 0LL;
//    *(this + 2) = 0LL;
//    *(this + 3) = 0LL;
//    *(this + 4) = 0LL;
//    *(this + 5) = 0LL;
//    *(this + 18) = 1;
    return true;
}
