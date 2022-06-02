//
//  logging.hpp
//  vmwaregfx
//
//  Created by John Othwolo on 5/30/22.
//

#ifndef logging_hpp
#define logging_hpp

#include <sys/systm.h>
#include <libkern/libkern.h>

enum {
    VLOG_INFO,
    VLOG_WARN,
    VLOG_PANIC
};

void __vmware_log(int,char*,int,char*, ...) __printflike(4, 5);

#define _vmware_log(type, fmt, ...) \
        __vmware_log((type), (char*)__FILE__, __LINE__, ((char*)fmt), __VA_ARGS__)

#define vmware_log               vmware_info
#define vmware_debug             vmware_info
#define vmware_info(fmt, ...)    _vmware_log(VLOG_INFO,  fmt, __VA_ARGS__)
#define vmware_warn(fmt, ...)    _vmware_log(VLOG_WARN,  fmt, __VA_ARGS__)
#define vmware_panic(fmt, ...)   _vmware_log(VLOG_PANIC, fmt, __VA_ARGS__)

#endif /* logging_hpp */
