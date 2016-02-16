#pragma once

#ifndef CPM_SETTINGS_CATCH

#include "CPMCore/CPMSettingsCatch.evc.h"
#include "CPMCore/CPMUtils.h"

#define CPM_SETTINGS_CATCH(settings_file)\
    catch(const FileIOException & fioex)\
    {\
        std::stringstream ss;\
        ss << "I/O error while reading file: '";\
        ss << settings_file;\
        ss << "' (err: ";\
        ss << fioex.what();\
        ss << ").";\
        CPMUtils_ns::CPMUtils::report_settings_failure(ss.str());\
        return false;\
    }\
    catch(const ParseException &pex)\
    {\
        std::stringstream ss;\
        ss << "Parse error in file: '";\
        ss << settings_file;\
        ss << "' at line ";\
        ss << pex.getLine();\
        ss << ".";\
        CPMUtils_ns::CPMUtils::report_settings_failure(ss.str());\
        return false;\
    }\
    catch(SettingTypeException & ex)\
    {\
        std::stringstream ss;\
        ss << "Setting type exception in file: '";\
        ss << settings_file;\
        ss << "' (path: ";\
        ss << ex.getPath();\
        ss << ").";\
        CPMUtils_ns::CPMUtils::report_settings_failure(ss.str());\
        return false;\
    }\
    catch(SettingNotFoundException & ex)\
    {\
        std::stringstream ss;\
        ss << "Setting not found in file: '";\
        ss << settings_file;\
        ss << "' (";\
        ss << ex.getPath();\
        ss << ").";\
        CPMUtils_ns::CPMUtils::report_settings_failure(ss.str());\
        return false;\
    }\
    catch(SettingNameException & ex)\
    {\
        std::stringstream ss;\
        ss << "Setting name exception in file: '";\
        ss << settings_file;\
        ss << "' (";\
        ss << ex.getPath();\
        ss << ").";\
        CPMUtils_ns::CPMUtils::report_settings_failure(ss.str());\
        return false;\
    }\
    catch(SettingException & ex)\
    {\
        std::stringstream ss;\
        ss << "Setting exception in file: '";\
        ss << settings_file;\
        ss << "' (";\
        ss << ex.getPath();\
        ss << ").";\
        CPMUtils_ns::CPMUtils::report_settings_failure(ss.str());\
        return false;\
    }\
    catch(...)\
    {\
        std::stringstream ss;\
        ss << "Unknown exception in file: '";\
        ss << settings_file;\
        ss << "'.";\
        CPMUtils_ns::CPMUtils::report_settings_failure(ss.str());\
        return false;\
    }

#endif // CPM_SETTINGS_CATCH
