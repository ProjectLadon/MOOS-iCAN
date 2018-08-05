/************************************************************/
/*    NAME:                                               */
/*    ORGN: Project Ladon                                     */
/*    FILE: CAN_field.hpp                       */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include <string>
#include <vector>
#include <linux/can.h>
#include <memory>
#include "rapidjson/rapidjson.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "rapidjson/schema.h"
#include "rapidjson/writer.h"
#include "rapidjson/error/en.h"
#include "MOOS/libMOOS/MOOSLib.h"
#include "CAN_field.hpp"
#include "CAN.h"

using namespace std;
using namespace rapidjson;

unique_ptr<CANtxField> CANtxField::CANtxFieldFactory (rapidjson::Value& v, CAN *iface) {
    if (v.HasMember("type") && v["type"].IsString()) {
        if (v["type"].GetString() == "const") {
            return unique_ptr<CANtxField>(new CANconstTXfield(v, iface));
        } else if (v["type"].GetString() == "uint8") {
            return unique_ptr<CANtxField>(new CANuint8TXfield(v, iface));
        } else if (v["type"].GetString() == "uint16") {
            return unique_ptr<CANtxField>(new CANuint16TXfield(v, iface));
        } else if (v["type"].GetString() == "uint32") {
            return unique_ptr<CANtxField>(new CANuint32TXfield(v, iface));
        } else if (v["type"].GetString() == "uint64") {
            return unique_ptr<CANtxField>(new CANuint64TXfield(v, iface));
        } else if (v["type"].GetString() == "int8") {
            return unique_ptr<CANtxField>(new CANint8TXfield(v, iface));
        } else if (v["type"].GetString() == "int16") {
            return unique_ptr<CANtxField>(new CANint16TXfield(v, iface));
        } else if (v["type"].GetString() == "int32") {
            return unique_ptr<CANtxField>(new CANint32TXfield(v, iface));
        } else if (v["type"].GetString() == "int64") {
            return unique_ptr<CANtxField>(new CANint64TXfield(v, iface));
        } else if (v["type"].GetString() == "float") {
            return unique_ptr<CANtxField>(new CANfloatTXfield(v, iface));
        } else if (v["type"].GetString() == "double") {
            return unique_ptr<CANtxField>(new CANdoubleTXfield(v, iface));
        } else if (v["type"].GetString() == "string") {
            return unique_ptr<CANtxField>(new CANstringTXfield(v, iface));
        } else if (v["type"].GetString() == "binary") {
            return unique_ptr<CANtxField>(new CANbinaryTXfield(v, iface));
        }
    }
    return unique_ptr<CANtxField>(nullptr);
}

bool CANtxField::subscribe() {return interface->registerVar(varName);}

unique_ptr<CANrxField> CANrxField::CANrxFieldFactory (rapidjson::Value& v, CAN *iface) {
    if (v.HasMember("type") && v["type"].IsString()) {
        if (v["type"].GetString() == "uint8") {
            return unique_ptr<CANrxField>(new CANuint8RXfield(v, iface));
        } else if (v["type"].GetString() == "uint16") {
            return unique_ptr<CANrxField>(new CANuint16RXfield(v, iface));
        } else if (v["type"].GetString() == "uint32") {
            return unique_ptr<CANrxField>(new CANuint32RXfield(v, iface));
        } else if (v["type"].GetString() == "uint64") {
            return unique_ptr<CANrxField>(new CANuint64RXfield(v, iface));
        } else if (v["type"].GetString() == "int8") {
            return unique_ptr<CANrxField>(new CANint8RXfield(v, iface));
        } else if (v["type"].GetString() == "int16") {
            return unique_ptr<CANrxField>(new CANint16RXfield(v, iface));
        } else if (v["type"].GetString() == "int32") {
            return unique_ptr<CANrxField>(new CANint32RXfield(v, iface));
        } else if (v["type"].GetString() == "int64") {
            return unique_ptr<CANrxField>(new CANint64RXfield(v, iface));
        } else if (v["type"].GetString() == "float") {
            return unique_ptr<CANrxField>(new CANfloatRXfield(v, iface));
        } else if (v["type"].GetString() == "double") {
            return unique_ptr<CANrxField>(new CANdoubleRXfield(v, iface));
        } else if (v["type"].GetString() == "string") {
            return unique_ptr<CANrxField>(new CANstringRXfield(v, iface));
        } else if (v["type"].GetString() == "binary") {
            return unique_ptr<CANrxField>(new CANbinaryRXfield(v, iface));
        }
    }
    return unique_ptr<CANrxField>(nullptr);
}

CANmsgTX::CANmsgTX(rapidjson::Value& v, CAN *iface) {
    id = v["id"].GetInt();
    for (auto &f: v["fields"].GetArray()) {
        fields.emplace_back(CANtxField::CANtxFieldFactory(f, iface));
    }
}

bool CANmsgTX::procMail(CMOOSMsg &msg) {
    bool result = false;
    for (auto &m: fields) {
        result |= m->procMail(msg);
    }
    return result;
}

unique_ptr<struct can_frame> CANmsgTX::compose() {
    struct can_frame* result = new struct can_frame;
    result->can_id = id;
    for (auto &f: fields) {
        if (!f->isFresh()) {
            delete result;
            return unique_ptr<struct can_frame>(nullptr);
        }
        for (int i = f->getStart(); ((i < (f->getStart() + f->getLength())) && (i < 8)); i++) {
            result->data[i] = f->getField()[i - f->getStart()];
            result->can_dlc = (i + 1);
        }
    }
    return unique_ptr<struct can_frame>(result);
}

CANmsgRX::CANmsgRX(rapidjson::Value& v, CAN *iface) {
    id = v["id"].GetInt();
    for (auto &f: v["fields"].GetArray()) {
        fields.emplace_back(CANrxField::CANrxFieldFactory(f, iface));
    }
    interface = iface;
}

bool CANmsgRX::process(uint32_t incoming_id, vector<uint8_t>& msg) {
    if (incoming_id == id) {
        bool result = true;
        for (auto &f: fields) {
            result &= process(incoming_id, msg);
        }
        return result;
    }
    return false;
}

CANconstTXfield::CANconstTXfield(rapidjson::Value& v, CAN *iface) {
    varName = "";
    fresh = true;
    start_byte = v["start_byte"].GetInt();
    for (auto &b: v["value"].GetArray()) {
        field.push_back(b.GetInt());
    }
    length = field.size();
}

CANuint8TXfield::CANuint8TXfield(rapidjson::Value& v, CAN *iface) {
    varName = v["var"].GetString();
    start_byte = v["start_byte"].GetInt();
    length = 1;
    field.resize(1, 0);
}

bool CANuint8TXfield::procMail(CMOOSMsg &msg) {
    if (msg.GetKey() == varName) {
        field[0] = (uint8_t)(msg.GetDouble());
        fresh = true;
        return true;
    }
    return false;
}

CANuint16TXfield::CANuint16TXfield(rapidjson::Value& v, CAN *iface) {
    varName = v["var"].GetString();
    start_byte = v["start_byte"].GetInt();
    length = 2;
    field.resize(2, 0);
}

bool CANuint16TXfield::procMail(CMOOSMsg &msg) {
    if (msg.GetKey() == varName) {
        uint16_t value = (uint16_t)(msg.GetDouble());
        auto uv = reinterpret_cast<uint8_t *>(value);
        for (int i = 0; i < field.size(); i++) {
            field[i] = uv[i];
        }
        fresh = true;
        return true;
    }
    return false;
}

CANuint32TXfield::CANuint32TXfield(rapidjson::Value& v, CAN *iface) {
    varName = v["var"].GetString();
    start_byte = v["start_byte"].GetInt();
    length = 4;
    field.resize(4, 0);
}

bool CANuint32TXfield::procMail(CMOOSMsg &msg) {
    if (msg.GetKey() == varName) {
        uint32_t value = (uint32_t)(msg.GetDouble());
        auto uv = reinterpret_cast<uint8_t *>(value);
        for (int i = 0; i < field.size(); i++) {
            field[i] = uv[i];
        }
        fresh = true;
        return true;
    }
    return false;
}

CANuint64TXfield::CANuint64TXfield(rapidjson::Value& v, CAN *iface) {
    varName = v["var"].GetString();
    start_byte = 0;
    length = 8;
    field.resize(8, 0);
}

bool CANuint64TXfield::procMail(CMOOSMsg &msg) {
    if (msg.GetKey() == varName) {
        uint64_t value = (uint64_t)(msg.GetDouble());
        auto uv = reinterpret_cast<uint8_t *>(value);
        for (int i = 0; i < field.size(); i++) {
            field[i] = uv[i];
        }
        fresh = true;
        return true;
    }
    return false;
}

CANint8TXfield::CANint8TXfield(rapidjson::Value& v, CAN *iface) {
    varName = v["var"].GetString();
    start_byte = v["start_byte"].GetInt();
    length = 1;
    field.resize(1, 0);
}

bool CANint8TXfield::procMail(CMOOSMsg &msg) {
    if (msg.GetKey() == varName) {
        int8_t value = static_cast<int8_t>(msg.GetDouble());
        field[0] = static_cast<uint8_t>(value);
        fresh = true;
        return true;
    }
    return false;
}

CANint16TXfield::CANint16TXfield(rapidjson::Value& v, CAN *iface) {
    varName = v["var"].GetString();
    start_byte = v["start_byte"].GetInt();
    length = 2;
    field.resize(2, 0);
}

bool CANint16TXfield::procMail(CMOOSMsg &msg) {
    if (msg.GetKey() == varName) {
        int16_t value = static_cast<int16_t>(msg.GetDouble());
        auto uv = reinterpret_cast<uint8_t *>(value);
        for (int i = 0; i < field.size(); i++) {
            field[i] = uv[i];
        }
        fresh = true;
        return true;
    }
    return false;
}

CANint32TXfield::CANint32TXfield(rapidjson::Value& v, CAN *iface) {
    varName = v["var"].GetString();
    start_byte = v["start_byte"].GetInt();
    length = 4;
    field.resize(4, 0);
}

bool CANint32TXfield::procMail(CMOOSMsg &msg) {
    if (msg.GetKey() == varName) {
        int32_t value = static_cast<int32_t>(msg.GetDouble());
        auto uv = reinterpret_cast<uint8_t *>(value);
        for (int i = 0; i < field.size(); i++) {
            field[i] = uv[i];
        }
        fresh = true;
        return true;
    }
    return false;
}

CANint64TXfield::CANint64TXfield(rapidjson::Value& v, CAN *iface) {
    varName = v["var"].GetString();
    start_byte = 0;
    length = 8;
    field.resize(8, 0);
}

bool CANint64TXfield::procMail(CMOOSMsg &msg) {
    if (msg.GetKey() == varName) {
        int64_t value = static_cast<int64_t>(msg.GetDouble());
        auto uv = reinterpret_cast<uint8_t *>(value);
        for (int i = 0; i < field.size(); i++) {
            field[i] = uv[i];
        }
        fresh = true;
        return true;
    }
    return false;
}

CANfloatTXfield::CANfloatTXfield(rapidjson::Value& v, CAN *iface) {
    static_assert(sizeof(float) == 4);
    varName = v["var"].GetString();
    start_byte = v["start_byte"].GetInt();
    length = 4;
    field.resize(4, 0);
}

bool CANfloatTXfield::procMail(CMOOSMsg &msg) {
    if (msg.GetKey() == varName) {
        float value = msg.GetDouble();
        auto uv = reinterpret_cast<uint8_t *>(&value);
        for (int i = 0; i < field.size(); i++) {
            field[i] = uv[i];
        }
        fresh = true;
        return true;
    }
    return false;
}

CANdoubleTXfield::CANdoubleTXfield(rapidjson::Value& v, CAN *iface) {
    static_assert(sizeof(double) == 8);
    varName = v["var"].GetString();
    start_byte = 0;
    length = 8;
    field.resize(8, 0);
}

bool CANdoubleTXfield::procMail(CMOOSMsg &msg) {
    if (msg.GetKey() == varName) {
        double value = msg.GetDouble();
        auto uv = reinterpret_cast<uint8_t *>(&value);
        for (int i = 0; i < field.size(); i++) {
            field[i] = uv[i];
        }
        fresh = true;
        return true;
    }
    return false;
}

CANstringTXfield::CANstringTXfield(rapidjson::Value& v, CAN *iface) {
    varName = v["var"].GetString();
    start_byte = v["start_byte"].GetInt();
    length = v["length"].GetInt();
    field.resize(length, 0);
}

bool CANstringTXfield::procMail(CMOOSMsg &msg) {
    if (msg.GetKey() == varName) {
        string value = msg.GetString();
        uint8_t len = field.size();
        if (value.length() < len) len = value.length();
        for (int i = 0; i < field.size(); i++) {
            field[i] = (uint8_t)value[i];
        }
        fresh = true;
        return true;
    }
    return false;
}

CANbinaryTXfield::CANbinaryTXfield(rapidjson::Value& v, CAN *iface) {
    varName = v["var"].GetString();
    start_byte = v["start_byte"].GetInt();
    length = v["length"].GetInt();
    field.resize(length, 0);
}

bool CANbinaryTXfield::procMail(CMOOSMsg &msg) {
    if (msg.GetKey() == varName) {
        auto value = msg.GetBinaryDataAsVector();
        uint8_t len = field.size();
        if (value.size() < len) len = value.size();
        for (int i = 0; i < field.size(); i++) {
            field[i] = value[i];
        }
        fresh = true;
        return true;
    }
    return false;
}

CANuint8RXfield::CANuint8RXfield(rapidjson::Value& v, CAN *iface) {
    varName = v["var"].GetString();
    start_byte = v["start_byte"].GetInt();
    length = 1;
    interface = iface;
}

bool CANuint8RXfield::process(vector<uint8_t>& msg) {
    if (msg.size() < (start_byte + length)) return false;
    return interface->notify(varName, (double)msg[start_byte]);
}

CANuint16RXfield::CANuint16RXfield(rapidjson::Value& v, CAN *iface) {
    varName = v["var"].GetString();
    start_byte = v["start_byte"].GetInt();
    length = 2;
    interface = iface;
}

bool CANuint16RXfield::process(vector<uint8_t>& msg) {
    if (msg.size() < (start_byte + length)) return false;
    uint16_t *value = reinterpret_cast<uint16_t *>(msg.data() + start_byte);
    return interface->notify(varName, (double)(*value));
}

CANuint32RXfield::CANuint32RXfield(rapidjson::Value& v, CAN *iface) {
    varName = v["var"].GetString();
    start_byte = v["start_byte"].GetInt();
    length = 4;
    interface = iface;
}

bool CANuint32RXfield::process(vector<uint8_t>& msg) {
    if (msg.size() < (start_byte + length)) return false;
    uint32_t *value = reinterpret_cast<uint32_t *>(msg.data() + start_byte);
    return interface->notify(varName, (double)(*value));
}

CANuint64RXfield::CANuint64RXfield(rapidjson::Value& v, CAN *iface) {
    varName = v["var"].GetString();
    start_byte = v["start_byte"].GetInt();
    length = 8;
    interface = iface;
}

bool CANuint64RXfield::process(vector<uint8_t>& msg) {
    if (msg.size() < 8) return false;
    uint64_t *value = reinterpret_cast<uint64_t *>(msg.data());
    return interface->notify(varName, (double)(*value));
}

CANint8RXfield::CANint8RXfield(rapidjson::Value& v, CAN *iface) {
    varName = v["var"].GetString();
    start_byte = v["start_byte"].GetInt();
    length = 1;
    interface = iface;
}

bool CANint8RXfield::process(vector<uint8_t>& msg) {
    if (msg.size() < (start_byte + length)) return false;
    int8_t *value = reinterpret_cast<int8_t *>(msg.data() + start_byte);
    return interface->notify(varName, (double)(*value));
}

CANint16RXfield::CANint16RXfield(rapidjson::Value& v, CAN *iface) {
    varName = v["var"].GetString();
    start_byte = v["start_byte"].GetInt();
    length = 2;
    interface = iface;
}

bool CANint16RXfield::process(vector<uint8_t>& msg) {
    if (msg.size() < (start_byte + length)) return false;
    int16_t *value = reinterpret_cast<int16_t *>(msg.data() + start_byte);
    return interface->notify(varName, (double)(*value));
}

CANint32RXfield::CANint32RXfield(rapidjson::Value& v, CAN *iface) {
    varName = v["var"].GetString();
    start_byte = v["start_byte"].GetInt();
    length = 4;
    interface = iface;
}

bool CANint32RXfield::process(vector<uint8_t>& msg) {
    if (msg.size() < (start_byte + length)) return false;
    int32_t *value = reinterpret_cast<int32_t *>(msg.data() + start_byte);
    return interface->notify(varName, (double)(*value));
}

CANint64RXfield::CANint64RXfield(rapidjson::Value& v, CAN *iface) {
    varName = v["var"].GetString();
    start_byte = v["start_byte"].GetInt();
    length = 8;
    interface = iface;
}

bool CANint64RXfield::process(vector<uint8_t>& msg) {
    if (msg.size() < 8) return false;
    int64_t *value = reinterpret_cast<int64_t *>(msg.data() + start_byte);
    return interface->notify(varName, (double)(*value));
}

CANfloatRXfield::CANfloatRXfield(rapidjson::Value& v, CAN *iface) {
    static_assert(sizeof(float) == 4);
    varName = v["var"].GetString();
    start_byte = v["start_byte"].GetInt();
    length = 4;
    interface = iface;
}

bool CANfloatRXfield::process(vector<uint8_t>& msg) {
    if (msg.size() < (start_byte + length)) return false;
    float *value = reinterpret_cast<float *>(msg.data() + start_byte);
    return interface->notify(varName, (double)(*value));
}

CANdoubleRXfield::CANdoubleRXfield(rapidjson::Value& v, CAN *iface) {
    static_assert(sizeof(double) == 8);
    varName = v["var"].GetString();
    start_byte = v["start_byte"].GetInt();
    length = 8;
    interface = iface;
}

bool CANdoubleRXfield::process(vector<uint8_t>& msg) {
    if (msg.size() < (start_byte + length)) return false;
    double *value = reinterpret_cast<double *>(msg.data());
    return interface->notify(varName, *value);
}

CANstringRXfield::CANstringRXfield(rapidjson::Value& v, CAN *iface) {
    varName = v["var"].GetString();
    start_byte = v["start_byte"].GetInt();
    length = v["length"].GetInt();
    if (start_byte + length > 8) length = (7 - start_byte);
    interface = iface;
}

bool CANstringRXfield::process(vector<uint8_t>& msg) {
    if (msg.size() < (start_byte + length)) return false;
    string value;
    value.append((char *)(msg.data() + start_byte), length);
    return interface->notify(varName, value);
}

CANbinaryRXfield::CANbinaryRXfield(rapidjson::Value& v, CAN *iface) {
    varName = v["var"].GetString();
    start_byte = v["start_byte"].GetInt();
    length = v["length"].GetInt();
    if (start_byte + length > 8) length = (7 - start_byte);
    interface = iface;
}

bool CANbinaryRXfield::process(vector<uint8_t>& msg) {
    if (msg.size() < (start_byte + length)) return false;
    vector<uint8_t> value;
    value.reserve(length);
    for (int i = start_byte; i < (start_byte + length); i++) {
        value.push_back(msg[i]);
    }
    return interface->notify(varName, value);
}
