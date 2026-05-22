#pragma once

#include "../corba_types.h"
#include <tao/ORB.h>
#include <tao/DynamicInterface/Request.h>
#include <string>
#include <vector>


struct InvokeResult {
    CORBA::Any return_value;
    std::vector<CORBA::Any> out_args;
};

class DiiEngine {
    public:
        explicit DiiEngine(CORBA::ORB_ptr orb);

        InvokeResult invoke(CORBA::Object_ptr target,
                            const std::string& method,
                            CORBA::TypeCode_ptr ret_tc,
                            const std::vector<ParamInfo>& params,
                            const std::vector<CORBA::Any>& args = std::vector<CORBA::Any>());
    
    private:
        CORBA::ORB_var orb_;

        DiiEngine(const DiiEngine&);
        DiiEngine& operator=(const DiiEngine&);

        void addInArgs(CORBA::Request_ptr request,
                        const std::vector<ParamInfo>& params,
                        const std::vector<CORBA::Any>& args);

        std::vector<CORBA::Any> extractOutArgs(CORBA::Request_ptr request,
                                                const std::vector<ParamInfo>& params);

};