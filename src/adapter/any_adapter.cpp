#include "any_adapter.hpp"
#include <tao/AnyTypeCode/Any_Unknown_IDL_Type.h>
#include <tao/AnyTypeCode/TypeCode_Constants.h>
#include <DGTC.h>
#include <tao/CDR.h>
#include <iostream>
#include <cstring>
 
 
namespace AnyAdapter {
    gateway::TypeKind toTypeKind(CORBA::TypeCode_ptr tc) {
        if (!tc) return gateway::TYPE_UNKNOWN;
        switch (tc->kind()) {
            case CORBA::tk_double:  return gateway::TYPE_DOUBLE;
            case CORBA::tk_float:   return gateway::TYPE_FLOAT;
            case CORBA::tk_long:    return gateway::TYPE_LONG;
            case CORBA::tk_short:   return gateway::TYPE_SHORT;
            case CORBA::tk_boolean: return gateway::TYPE_BOOLEAN;
            case CORBA::tk_octet:   return gateway::TYPE_OCTET;
            case CORBA::tk_string:  return gateway::TYPE_STRING;
 
            case CORBA::tk_struct: {
                const char* name = "";
                try { name = tc->name(); }
                catch (...) {}
 
                if (std::strcmp(name, "TimeValue") == 0) {
                    return gateway::TYPE_TIMEVALUE;
                }
                return gateway::TYPE_UNKNOWN;
            }
 
            default: return gateway::TYPE_UNKNOWN;
        }
    }
 
    gateway::AnyValue fromCorba(const CORBA::Any& any, CORBA::TypeCode_ptr tc) {
        gateway::AnyValue dto;
        dto.set_type_kind(toTypeKind(tc));
 
        if (!tc || tc->kind() == CORBA::tk_void) {
            return dto;
        }
 
        switch (tc->kind()) {
            case CORBA::tk_double: {
                CORBA::Double value = 0.0;
                any >>= value;
                dto.set_double_val(value);
                break;
            }
 
            case CORBA::tk_float: {
                CORBA::Float value = 0.0f;
                any >>= value;
                dto.set_float_val(value);
                break;
            }
 
            case CORBA::tk_long: {
                CORBA::Long value = 0;
                any >>= value;
                dto.set_long_val(value);
                break;
            }
 
            case CORBA::tk_short: {
                CORBA::Short value = 0;
                any >>= value;
                dto.set_short_val(value);
                break;
            }
 
            case CORBA::tk_boolean: {
                CORBA::Boolean value = false;
                any >>= CORBA::Any::to_boolean(value);
                dto.set_bool_val(value);
                break;
            }
 
            case CORBA::tk_octet: {
                CORBA::Octet value = 0;
                any >>= CORBA::Any::to_octet(value);
                dto.set_octet_val(value);
                break;
            }
 
            case CORBA::tk_string: {
                const char* value = nullptr;
                any >>= value;
                if (value) dto.set_string_val(value);
                break;
            }
 
            case CORBA::tk_struct: {
                const char* type_name = "";
                try { type_name = tc->name(); } catch (...) {}
 
                if (std::strcmp(type_name, "TimeValue") == 0) {
                    const DGT::TimeValue* tv = nullptr;
                    if (any >>= tv) {
                        dto.set_time_val(tv->usec);
                    }
                } else {
                    std::cerr << "[AnyAdapter] WARNING: struct not recognized: '"
                              << type_name << "'" << std::endl;
                    TAO_OutputCDR cdr;
                    cdr << any;
                    dto.set_raw_val(cdr.buffer(), cdr.length());
                }
                break;
            }
 
            default:
                std::cerr << "[AnyAdapter] WARNING: TCKind=" << tc->kind()
                          << " not supported raw_val" << std::endl;
                TAO_OutputCDR cdr;
                cdr << any;
                dto.set_raw_val(cdr.buffer(), cdr.length());
                break;
        }
 
        return dto;
    }
 
    CORBA::Any toCorba(const gateway::AnyValue& dto) {
        CORBA::Any any;
 
        switch (dto.type_kind()) {
            case gateway::TYPE_DOUBLE: {
                any <<= dto.double_val();
                break;
            }
 
            case gateway::TYPE_FLOAT: {
                any <<= dto.float_val();
                break;
            }
 
            case gateway::TYPE_LONG: {
                any <<= static_cast<CORBA::Long>(dto.long_val());
                break;
            }
 
            case gateway::TYPE_SHORT: {
                any <<= static_cast<CORBA::Short>(dto.short_val());
                break;
            }
 
            case gateway::TYPE_BOOLEAN: {
                any <<= CORBA::Any::from_boolean(static_cast<CORBA::Boolean>(dto.bool_val()));
                break;
            }
 
            case gateway::TYPE_OCTET: {
                any <<= CORBA::Any::from_octet(static_cast<CORBA::Octet>(dto.octet_val()));
                break;
            }
 
            case gateway::TYPE_STRING: {
                const char* s = dto.string_val().c_str();
                any <<= s;
                break;
            }
 
            case gateway::TYPE_TIMEVALUE: {
                DGT::TimeValue time_value;
                time_value.usec = static_cast<CORBA::LongLong>(dto.time_val());
                any <<= time_value;
                break;
            }
 
            default:
                break;
        }
 
        return any;
    }
 
}
