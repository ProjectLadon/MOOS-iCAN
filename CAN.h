/************************************************************/
/*    NAME:                                               */
/*    ORGN: Project Ladon                                             */
/*    FILE: CAN.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef CAN_HEADER
#define CAN_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "CAN_field.hpp"
#include <vector>
#include <memory>
#include <linux/can.h>
#include <linux/can/raw.h>
#include "rapidjson/rapidjson.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "rapidjson/schema.h"
#include "rapidjson/writer.h"
#include "rapidjson/error/en.h"
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <net/if.h>

using namespace std;

class CAN : public CMOOSApp {
    public:
        CAN() {};
        ~CAN() {close(CANhnd);};
        bool notify(const std::string &var, const std::string &val) {
            return Notify(var, val);
        };
        bool notify(const std::string &var, const double &val) {
            return Notify(var, val);
        };
        bool notify(const std::string &var, const std::vector<uint8_t> &val) {
            return Notify(var, val);
        };
        bool registerVar(const std::string &var) {return Register(var);};

    protected: // Standard MOOSApp functions to overload
        bool OnNewMail(MOOSMSG_LIST &NewMail);
        bool Iterate();
        bool OnConnectToServer();
        bool OnStartUp();

    protected:
        void RegisterVariables();
        void sendCAN(unique_ptr<struct can_frame> msg);
        void initCAN();
        struct can_frame* getCAN();
        unique_ptr<rapidjson::Document> parseConf(std::string conf);
        unique_ptr<rapidjson::Document> loadConfFile(std::string confFile);

    private: // Configuration variables
        string              interfaceName;
        vector<CANmsgTX>    TXmsgs;
        vector<CANmsgRX>    RXmsgs;
        int                 CANhnd;
    private: // State variables
        struct sockaddr_can addr;
    	char ctrlmsg[(sizeof(struct timeval) + 3*sizeof(struct timespec) + sizeof(__u32))];
    	struct iovec iov;
    	struct msghdr msg;
    	struct cmsghdr *cmsg;
    	can_err_mask_t err_mask;
    	struct can_frame frame;
    	struct ifreq ifr;
};

#endif
