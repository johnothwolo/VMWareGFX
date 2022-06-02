//
//  logging.cpp
//  vmwaregfx
//
//  Created by John Othwolo on 5/30/22.
//

#include "logging.h"

static lck_spin_t gLogLock;
char gLogBuffer[8192];

__attribute__((constructor)) void init_log_func(){
    lck_spin_init(&gLogLock, nullptr, nullptr);
}

__attribute__((destructor)) void destroy_log_func(){
    lck_spin_destroy(&gLogLock, nullptr);
}

void __vmware_log(int type, char* file, int line, char *fmt, ...)
{
    va_list ap;
    auto fname = file;
    
    lck_spin_lock(&gLogLock); // lock
    
    assert(file != nullptr);
    memset(gLogBuffer, 0x0, sizeof(gLogBuffer));
    
    for (size_t len = strlen(file); len > 0; len--) {
        if (fname[len - 1] == '/') {
            fname += len;
            break;
        }
    }
    
    va_start(ap, fmt);
    vsnprintf(gLogBuffer, sizeof(gLogBuffer), fmt, ap);
    va_end(ap);
    
    printf("(%s:%d): %s", fname, line, gLogBuffer);
    
    lck_spin_unlock(&gLogLock); // unlock
    
    if(type == VLOG_PANIC)
        panic(buf);
}
