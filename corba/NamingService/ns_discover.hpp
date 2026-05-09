#pragma once

#include <tao/ORB.h>
#include <orbsvcs/CosNamingC.h>
#include <string>
#include <vector>


class NsResolver {
    public:
        explicit NsResolver(CORBA::ORB_ptr orb);
        
        void connect();

        CORBA::Object_var resolve(const std::string& path,
                                    const std::string& leaf_kind = "") const;

        void listRoot(CORBA::ULong max_entries = 32) const;

        CosNaming::NamingContext_ptr root() const { return root_.in(); }
    
    private:
        CORBA::ORB_var orb_;
        CosNaming::NamingContext_var root_;

        static CosNaming::Name buildName(const std::string& path,
                                            const std::string& leaf_kind);
}