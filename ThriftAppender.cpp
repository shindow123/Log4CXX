/*
 * ThriftAppender.cpp
 *
 *  Created on: 2015年11月3日
 *      Author: sangechen
 */

#include "ThriftAppender.h"

#include <log4cplus/spi/loggingevent.h>

#include "../../../share/api/ConfigManager.h"

ThriftAppender::ThriftAppender(const std::string& address)
{
    selfAddress = address;
    _reConnect();
}

ThriftAppender::~ThriftAppender()
{
    close();
}

void ThriftAppender::close()
{
    logServerPtr.reset();
}

void ThriftAppender::append(const log4cplus::spi::InternalLoggingEvent& event)
{
    try
    {
        ::service::log::LogRequest req;
        req.logger  = event.getLoggerName();
        req.level   = (::service::log::LogLevel::type)event.getLogLevel();
        req.ndc     = event.getNDC();
        req.mdc     = event.getMDCCopy();
        req.message = event.getMessage();
        //#ifdef __linux //_WIN32(_WIN64), __APPLE__, __linux, __unix, __posix
        //	req.thread = boost::lexical_cast<std::string>(syscall(SYS_gettid)); //使用ps -eLf
        //#else
        //	req.thread = event.getThread();
        //#endif
        req.thread = event.getThread2()   // SYS_gettid on linux; cygwin::get_current_win32_thread_id; else event.getThread();
                     + "@" + selfAddress; //在线程id后追加本机IP  {server端需要做兼容,保证不会重复追加IP}
        req.sec  = event.getTimestamp().sec();
        req.usec = event.getTimestamp().usec();
        req.file = event.getFile();
        req.line = event.getLine();
        req.func = event.getFunction();

        if (!logServerPtr->isConnectionValid())
        {
            _reConnect();
        }

        //假设经过连接检测 或 重连后, client对象应该是可以直接发起调用的
        logServerPtr->client.writeLog(req);
    }
    catch (std::exception& e)
    {
        printf("ThriftAppender::append() throw %s : %s\n", typeid(e).name(), e.what());
        // ignore this exception...
    }
}

void ThriftAppender::_reConnect()
{
    const config::SocketServer& conf = config::ConfigManager::Instance().getServerConfig("log_server");
    logServerPtr.reset(new ThriftServiceClient<::service::log::LogServerClient>(conf.host, conf.port, conf.timeout * 1000));

    if (selfAddress.empty())
    {
        //根据已经连接的socket fd, 获取本机相对于log_server的IP地址
        int fd = logServerPtr->socket->getSocketFD();
        struct sockaddr_storage addr;
        socklen_t addrLen = sizeof(addr);
        char clienthost[NI_MAXHOST], clientservice[NI_MAXSERV];
        clienthost[0] = '\0';

        getsockname(fd, (sockaddr*)&addr, &addrLen);
        getnameinfo((sockaddr*)&addr, addrLen, clienthost, sizeof(clienthost), clientservice, sizeof(clientservice), NI_NUMERICHOST | NI_NUMERICSERV);

        selfAddress = clienthost;
        if (selfAddress.empty())
        {
            throw std::runtime_error("cann't use getsockname() to get selfIP!");
        }
    }
}
