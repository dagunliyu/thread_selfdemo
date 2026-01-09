 
//17项目案例线程通信使用互斥锁和list实现线程通信////////////////////////////////////// COMMENT ///////////////////////////////////////////////

#include "xmsg_server.h"
#include <sstream>
using namespace std;
int main(int argc, char* argv[])
{
    XMsgServer server;
    server.Start();
    for (int i = 0; i < 10; i++)
    {
        stringstream ss;
        ss << " msg : " << i + 1;
        server.SendMsg(ss.str());
        this_thread::sleep_for(500ms);
    }
    server.Stop();
    return 0;
}