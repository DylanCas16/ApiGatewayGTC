#include "dii_invocation.hpp"
#include <stdexcept>
#include <iostream>


DiiEngine::DiiEngine(CORBA::ORB_ptr orb) : orb_(CORBA::ORB::_duplicate(orb)) {}


InvokeResult DiiEngine::invoke(CORBA::Object_ptr target,
                                const std::string& method,
                               CORBA::TypeCode_ptr ret_tc,
                               const std::vector<ParamInfo>& params,
                               const std::vector<CORBA::Any>& args){
    if (CORBA::is_nil(target)) {
        throw std::runtime_error("DiiEngine: target is nil");
    }

    CORBA::Request_var request = target->_request(method.c_str());
    request->set_return_type(ret_tc);
    std::cout << "[DiiEngine] Operation " << method << " currently working" << std::endl;
    addInArgs(request.in(), params, args);

    try {
        request->invoke();
        std::cout << "[DiiEngine] Invocation executed correctly" << std::endl;
    } catch(const CORBA::Exception& exception) {
        throw std::runtime_error(std::string("DiiEngine: CORBA exception: ") + exception._info().c_str());
    }

    InvokeResult result;
    result.return_value = request->return_value();
    result.out_args = extractOutArgs(request.in(), params);
    
    return result;
}

void DiiEngine::addInArgs(CORBA::Request_ptr request,
                          const std::vector<ParamInfo>& params,
                          const std::vector<CORBA::Any>& args) {
    if (args.empty()) return;
    
    size_t index = 0;
    for (size_t p = 0; p < params.size(); p++) {
        if (index >= args.size()) break;
        if (params[p].mode == ParamInfo::OUT) continue;
 
        CORBA::Any& arg = request->add_in_arg(params[p].name.c_str());
        arg = args[index++];
    }
}
 
std::vector<CORBA::Any> DiiEngine::extractOutArgs(CORBA::Request_ptr request,
                                                    const std::vector<ParamInfo>& params){
    std::vector<CORBA::Any> result;
    
    for (size_t p = 0; p < params.size(); ++p) {
        if (params[p].mode == ParamInfo::IN ||
            params[p].mode == ParamInfo::UNSPECIFIED) continue;
 
        const CORBA::Any* val =
            request->arguments()->item(static_cast<CORBA::ULong>(p))->value();
        if (val) result.push_back(*val);
    }
    
    return result;
}
