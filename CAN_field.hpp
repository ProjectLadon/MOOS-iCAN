/************************************************************/
/*    NAME:                                               */
/*    ORGN: Project Ladon                                     */
/*    FILE: CAN_field.hpp                       */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include <string>
#include <vector>
#include <memory>
#include <linux/can.h>
#include "rapidjson/rapidjson.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "rapidjson/schema.h"
#include "rapidjson/writer.h"
#include "rapidjson/error/en.h"
#include "MOOS/libMOOS/MOOSLib.h"

#ifndef CAN_FIELD_H
#define CAN_FIELD_H

using namespace std;

class CAN;

class CANtxField {
    public:
        static unique_ptr<CANtxField> CANtxFieldFactory (rapidjson::Value& v, CAN *iface);
        CANtxField() {};
        virtual bool procMail(CMOOSMsg &msg) = 0;
        vector<uint8_t>& getField() {
            return field;
            fresh = false;
        };
        uint8_t getLength() {return length;};
        uint8_t getStart() {return start_byte;};
        bool isFresh() {return fresh;};
        virtual bool subscribe();
    protected:
        string varName;
        bool fresh = false;
        uint8_t start_byte;
        uint8_t length;
        vector<uint8_t> field;
        CAN *interface;
};

class CANrxField {
    public:
        static unique_ptr<CANrxField> CANrxFieldFactory (rapidjson::Value& v, CAN *iface);
        CANrxField() {};
        virtual bool process(vector<uint8_t>& msg) = 0;
    protected:
        string varName;
        uint8_t start_byte;
        uint8_t length;
        CAN *interface;
};

class CANmsgTX {
    public:
        CANmsgTX(rapidjson::Value& v, CAN *iface);
        bool procMail(CMOOSMsg &msg);
        unique_ptr<struct can_frame> compose();
        void subscribe() {for (auto &f: fields) f->subscribe();};
    private:
        uint32_t id;
        vector<unique_ptr<CANtxField>> fields;
};

class CANmsgRX {
    public:
        CANmsgRX(rapidjson::Value& v, CAN *iface);
        bool process(uint32_t incoming_id, vector<uint8_t>& msg);
    private:
        uint32_t id;
        vector<unique_ptr<CANrxField>> fields;
        CAN *interface;
};

class CANconstTXfield : public CANtxField {
    public:
        CANconstTXfield(rapidjson::Value& v, CAN *iface);
        bool procMail(CMOOSMsg &msg) {return false;};
        bool subscribe() {return true;};
};

class CANuint8TXfield : public CANtxField {
    public:
        CANuint8TXfield(rapidjson::Value& v, CAN *iface);
        bool procMail(CMOOSMsg &msg);
};

class CANuint16TXfield : public CANtxField {
    public:
        CANuint16TXfield(rapidjson::Value& v, CAN *iface);
        bool procMail(CMOOSMsg &msg);
};

class CANuint32TXfield : public CANtxField {
    public:
        CANuint32TXfield(rapidjson::Value& v, CAN *iface);
        bool procMail(CMOOSMsg &msg);
};

class CANuint64TXfield : public CANtxField {
    public:
        CANuint64TXfield(rapidjson::Value& v, CAN *iface);
        bool procMail(CMOOSMsg &msg);
};

class CANint8TXfield : public CANtxField {
    public:
        CANint8TXfield(rapidjson::Value& v, CAN *iface);
        bool procMail(CMOOSMsg &msg);
};

class CANint16TXfield : public CANtxField {
    public:
        CANint16TXfield(rapidjson::Value& v, CAN *iface);
        bool procMail(CMOOSMsg &msg);
};

class CANint32TXfield : public CANtxField {
    public:
        CANint32TXfield(rapidjson::Value& v, CAN *iface);
        bool procMail(CMOOSMsg &msg);
};

class CANint64TXfield : public CANtxField {
    public:
        CANint64TXfield(rapidjson::Value& v, CAN *iface);
        bool procMail(CMOOSMsg &msg);
};

class CANfloatTXfield : public CANtxField {
    public:
        CANfloatTXfield(rapidjson::Value& v, CAN *iface);
        bool procMail(CMOOSMsg &msg);
};

class CANdoubleTXfield : public CANtxField {
    public:
        CANdoubleTXfield(rapidjson::Value& v, CAN *iface);
        bool procMail(CMOOSMsg &msg);
};

class CANstringTXfield : public CANtxField {
    public:
        CANstringTXfield(rapidjson::Value& v, CAN *iface);
        bool procMail(CMOOSMsg &msg);
};

class CANbinaryTXfield : public CANtxField {
    public:
        CANbinaryTXfield(rapidjson::Value& v, CAN *iface);
        bool procMail(CMOOSMsg &msg);
};

class CANuint8RXfield : public CANrxField {
    public:
        CANuint8RXfield(rapidjson::Value& v, CAN *iface);
        bool process(vector<uint8_t>& msg);
};

class CANuint16RXfield : public CANrxField {
    public:
        CANuint16RXfield(rapidjson::Value& v, CAN *iface);
        bool process(vector<uint8_t>& msg);
};

class CANuint32RXfield : public CANrxField {
    public:
        CANuint32RXfield(rapidjson::Value& v, CAN *iface);
        bool process(vector<uint8_t>& msg);
};

class CANuint64RXfield : public CANrxField {
    public:
        CANuint64RXfield(rapidjson::Value& v, CAN *iface);
        bool process(vector<uint8_t>& msg);
};

class CANint8RXfield : public CANrxField {
    public:
        CANint8RXfield(rapidjson::Value& v, CAN *iface);
        bool process(vector<uint8_t>& msg);
};

class CANint16RXfield : public CANrxField {
    public:
        CANint16RXfield(rapidjson::Value& v, CAN *iface);
        bool process(vector<uint8_t>& msg);
};

class CANint32RXfield : public CANrxField {
    public:
        CANint32RXfield(rapidjson::Value& v, CAN *iface);
        bool process(vector<uint8_t>& msg);
};

class CANint64RXfield : public CANrxField {
    public:
        CANint64RXfield(rapidjson::Value& v, CAN *iface);
        bool process(vector<uint8_t>& msg);
};

class CANfloatRXfield : public CANrxField {
    public:
        CANfloatRXfield(rapidjson::Value& v, CAN *iface);
        bool process(vector<uint8_t>& msg);
};

class CANdoubleRXfield : public CANrxField {
    public:
        CANdoubleRXfield(rapidjson::Value& v, CAN *iface);
        bool process(vector<uint8_t>& msg);
};

class CANstringRXfield : public CANrxField {
    public:
        CANstringRXfield(rapidjson::Value& v, CAN *iface);
        bool process(vector<uint8_t>& msg);
};

class CANbinaryRXfield : public CANrxField {
    public:
        CANbinaryRXfield(rapidjson::Value& v, CAN *iface);
        bool process(vector<uint8_t>& msg);
};

#endif
