#pragma once

#include <mutex>
#include <stdexcept>
#include <string>
#include <orbsvcs/CosNamingC.h>
#include <tao/corba.h>
#include "../NamingService/ns_discover.hpp"
#include "../gcs_endpoints.h"
#include "CONFIGC.h"
#include "CONFIGManagerC.h"
#include "DGTC.h"


class ConfigService {
    public:
        explicit ConfigService(NsResolver& ns);
        void connect();
        
        CONFIG::Property getProperty(const std::string& propertyName);
        bool setProperty(const CONFIG::Property& prop);
    
    private:
        NsResolver& ns_;
        CONFIG::PredefConfigRepo_ifce_var repository_;
    
        ConfigService(const ConfigService&);
        ConfigService& operator=(const ConfigService&);
};