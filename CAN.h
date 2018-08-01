/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: CAN.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef CAN_HEADER
#define CAN_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

class CAN : public CMOOSApp
{
 public:
   CAN();
   ~CAN();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected:
   void RegisterVariables();

 private: // Configuration variables

 private: // State variables
};

#endif 
