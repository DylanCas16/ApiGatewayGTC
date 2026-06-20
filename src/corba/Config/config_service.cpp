#include "config_service.hpp"
#include <iostream>
#include <sstream>


ConfigService::ConfigService(NsResolver& ns) : ns_(ns) {}

void ConfigService::connect() {
    std::cout << "[ConfigService] connecting to " << gcs_env::GCS_CONFIG_NAME << std::endl;
 
    CORBA::Object_var obj = ns_.resolve(gcs_env::GCS_CONFIG_NAME);
    if (CORBA::is_nil(obj.in())) {
        throw std::runtime_error("[ConfigService] string_to_object returned nil");
    }
 
    manager_ = CONFIG::Manager_ifce::_narrow(obj.in());
    if (CORBA::is_nil(manager_.in())) {
        throw std::runtime_error("[ConfigService] reference is not a CONFIG::Manager_ifce");
    }
 
    repository_ = CONFIG::PredefConfigRepo_ifce::_duplicate(manager_->predefConfigRepo());
    if (CORBA::is_nil(repository_.in())) {
        throw std::runtime_error("[ConfigService] predefConfigRepo() returned nil");
    }
 
    std::cout << "[ConfigService] connected" << std::endl;
}

CONFIG::Property ConfigService::getProperty(const std::string& property_name) {
    if (CORBA::is_nil(repository_.in())) {
        throw std::runtime_error("[ConfigService] Not connected - call connect() first");
    }

    std::cout << "[ConfigService] getProperty: " << property_name << std::endl;
 
    try {
        CONFIG::Property_var property = repository_->getProperty(property_name.c_str());
 
        std::cout << "[ConfigService] getProperty executed correctly" << std::endl;
 
        return *property;
    }
    catch (const DGT::GCSException& exception) {
        throw std::runtime_error(
            std::string("[ConfigService] getProperty GCSException: ")
            + exception._info().c_str());
    }
    catch (const CORBA::Exception& exception) {
        throw std::runtime_error(
            std::string("[ConfigService] getProperty CORBA exception: ")
            + exception._info().c_str());
    }
}

bool ConfigService::setProperty(const CONFIG::Property& property) {
    if (CORBA::is_nil(repository_.in())) {
        throw std::runtime_error("[ConfigService] Not connected - call connect() first");
    }

    std::cout << "[ConfigService] setProperty: " << property.name << std::endl;
 
    try {
        repository_->setProperty(property);

        notifyPropertyChange(property.name.in());
 
        std::cout << "[ConfigService] setProperty executed correctly" << std::endl;
 
        return true;
    }
    catch (const DGT::GCSException& exception) {
        std::cout << "[ConfigService] setProperty GCSException: "
                  << exception._info().c_str() << std::endl;
        return false;
    }
    catch (const CORBA::Exception& exception) {
        throw std::runtime_error(
            std::string("[ConfigService] setProperty CORBA exception: ")
            + exception._info().c_str());
    }
}

void ConfigService::notifyPropertyChange(const std::string& full_name) {
    std::size_t dot_pos = full_name.find_last_of('.');

    std::string component = (dot_pos == std::string::npos)
        ? "" : full_name.substr(0, dot_pos);

    std::string property_name = (dot_pos == std::string::npos)
        ? full_name : full_name.substr(dot_pos + 1);

    CONFIG::PropertyChange change;
    change.gcs_component = CORBA::string_dup(component.c_str());
    change.property_name = CORBA::string_dup(property_name.c_str());

    const int64_t now_usec = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
    change.time_stamp.usec = static_cast<decltype(change.time_stamp.usec)>(now_usec);

    CONFIG::PropertyChangeList changes;
    changes.length(1);
    changes[0] = change;

    try {
        manager_->receivePropertyChanges(changes);
        std::cout << "[ConfigService] receivePropertyChanges called: "
                  << full_name << std::endl;
    }
    catch (const CORBA::Exception& exception) {
        std::cerr << "[ConfigService] receivePropertyChanges failed: "
                  << exception._info().c_str() << std::endl;
    }
}
