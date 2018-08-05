/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: CAN.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "CAN.h"
#include <linux/can.h>
#include <linux/can/raw.h>
#include "schema/channel_schema.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "rapidjson/schema.h"
#include "rapidjson/writer.h"
#include "rapidjson/error/en.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <net/if.h>

using namespace std;

//---------------------------------------------------------
// Procedure: OnNewMail

bool CAN::OnNewMail(MOOSMSG_LIST &NewMail) {
    for(auto &p: NewMail) {
        for (auto &m: TXmsgs) {
            m.procMail(p);
            sendCAN(m.compose());
        }
    }
    return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool CAN::OnConnectToServer() {
    RegisterVariables();
    return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool CAN::Iterate() {
    struct can_frame *msgframe;
    while (msgframe = getCAN()) {
        auto id = msgframe->can_id;
        vector<uint8_t> msgdata;
        for (uint8_t i; i < msgframe->can_dlc; i++) {
            msgdata.push_back(msgframe->data[i]);
        }
        for (auto &m: RXmsgs) {
            m.process(id, msgdata);
        }
    }
    return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool CAN::OnStartUp() {
    list<string> sParams;
    m_MissionReader.EnableVerbatimQuoting(false);
    unique_ptr<rapidjson::Document> conf = nullptr;
    if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
        for(auto &p: sParams) {
            string original_line = p;
            string param = stripBlankEnds(toupper(biteString(p, '=')));
            string value = stripBlankEnds(p);
            if (param == "CONF") {
                conf = parseConf(value);
            } else if (param == "CONFFILE") {
                conf = loadConfFile(value);
            }
        }
    } else {
        abort();
    }
    if (conf) {
        interfaceName = (*conf)["interface"].GetString();
        if (conf->HasMember("tx_messages") && (*conf)["tx_messages"].IsArray()) {
            for (auto &v: (*conf)["tx_messages"].GetArray()) {
                TXmsgs.emplace_back(v, this);
            }
        }
        if (conf->HasMember("rx_messages") && (*conf)["rx_messages"].IsArray()) {
            for (auto &v: (*conf)["rx_messages"].GetArray()) {
                RXmsgs.emplace_back(v, this);
            }
        }
        initCAN();
    } else return(false);

    RegisterVariables();
    return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void CAN::RegisterVariables() {
    for (auto &m: TXmsgs) m.subscribe();
}

void CAN::sendCAN(unique_ptr<struct can_frame> msg) {
    write(CANhnd, msg.get(), CAN_MTU);
}

struct can_frame* CAN::getCAN() {
    int nbytes = recvmsg(CANhnd, &msg, 0);
    if (nbytes < 0) return nullptr;
    if (nbytes < CAN_MTU) return nullptr;
    return &frame;
}

void CAN::initCAN() {
    CANhnd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ - 1);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);
    addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
    iov.iov_base = &frame;
	msg.msg_name = &addr;
	msg.msg_iov = &iov;
	msg.msg_control = &ctrlmsg;
    iov.iov_len = sizeof(frame);
    msg.msg_namelen = sizeof(addr);
    msg.msg_controllen = sizeof(ctrlmsg);
    msg.msg_flags = 0;
    ioctl(CANhnd, SIOCGIFMTU, &ifr);
	setsockopt(CANhnd, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
    bind(CANhnd, (struct sockaddr *)&addr, sizeof(addr));
}

unique_ptr<rapidjson::Document> CAN::parseConf(std::string conf) {
        rapidjson::Document confSchema;
        auto configuration = new rapidjson::Document;
        if (confSchema.Parse(reinterpret_cast<char*>(channel_schema_json, channel_schema_json_len)).HasParseError()) {
            cerr << "JSON parse error " << GetParseError_En(confSchema.GetParseError());
            cerr << " in configuration schema at offset " << confSchema.GetErrorOffset() << endl;
            std::abort();
        }
        rapidjson::SchemaDocument confSchemaDoc(confSchema);
        rapidjson::SchemaValidator validator(confSchemaDoc);
        if (configuration->Parse(conf.c_str()).HasParseError()) {
            cerr << "JSON parse error " << GetParseError_En(configuration->GetParseError());
            cerr << " in configuration JSON at offset " << configuration->GetErrorOffset() << endl;
            std::abort();
        }
        if (!configuration->Accept(validator)) {
            rapidjson::StringBuffer sb;
            validator.GetInvalidSchemaPointer().StringifyUriFragment(sb);
            cerr << "Invalid configuration schema: " << sb.GetString() << endl;
            cerr << "Invalid keyword: " << validator.GetInvalidSchemaKeyword() << endl;
            sb.Clear();
            validator.GetInvalidDocumentPointer().StringifyUriFragment(sb);
            cerr << "Invalid document: " << sb.GetString() << endl;
            std::abort();
        }
        return unique_ptr<rapidjson::Document>(configuration);
}

unique_ptr<rapidjson::Document> CAN::loadConfFile(std::string confFile) {
    ifstream infile;
    infile.open(confFile);
    // Make sure the file opened correctly
    if (!infile.is_open()) {
        cerr << "Failed to open configuration file " << confFile << endl;
        return unique_ptr<rapidjson::Document>(nullptr);
    }
    // Vacuum up the conf file
    string json;
    infile.seekg(0, ios::end);
    json.reserve(infile.tellg());
    infile.seekg(0, ios::beg);
    json.assign((istreambuf_iterator<char>(infile)), istreambuf_iterator<char>());
    return parseConf(json);
}
