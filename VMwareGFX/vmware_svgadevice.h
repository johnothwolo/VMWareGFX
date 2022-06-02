//
//  vmware_svgadevice.hpp
//  vmwaregfx
//
//  Created by John Othwolo on 5/31/22.
//

#ifndef vmware_svgadevice_hpp
#define vmware_svgadevice_hpp

#include <IOKit/graphics/IOFramebuffer.h>
#include <IOKit/IORegistryEntry.h>
#include <IOKit/IONotifier.h>
#include <IOKit/IOService.h>
#include <IOKit/IOLocks.h>

class SVGADevice : public OSObject {
        
public:
    
    bool init() override;
    bool start(IOService *provider);
    void stop(IOService *provider);
    
};

#endif /* vmware_svgadevice_hpp */
