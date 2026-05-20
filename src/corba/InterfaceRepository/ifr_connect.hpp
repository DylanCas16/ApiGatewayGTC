#pragma once

#include "../corba_types.h"
#include <tao/ORB.h>
#include <tao/IFR_Client/IFR_BasicC.h>
#include <string>
#include <unordered_map>
#include <mutex>


class IfrClient {
    public:
        explicit IfrClient(CORBA::ORB_ptr orb);
        
        void connect();
        
        const InterfaceInfo& describeInterface(const std::string& repid);
        
        CORBA::TypeCode_ptr returnTypeCode(const std::string& repid, 
                                            const std::string& method) const;
        
        void preloadAll();
        
        bool isConnected() const { return !CORBA::is_nil(repository_.in()); }
    
    private:
        CORBA::ORB_var         orb_;
        CORBA::Repository_var  repository_;
    
        // Caché de interfaces: repid -> InterfaceInfo
        mutable std::mutex cache_mutex_;
        std::unordered_map<std::string, InterfaceInfo> interface_cache_;
    
        // Caché plana: "repid#method" -> TypeCode
        std::unordered_map<std::string, CORBA::TypeCode_var> tc_cache_;
        static std::string cacheKey(const std::string& repid, 
                                    const std::string& method);
    
        InterfaceInfo fetchFromIfr(const std::string& repid);
        
        void populateCache(const InterfaceInfo& info);
};