/*
 * ThriftAppender.h
 *
 *  Created on: 2015年11月3日
 *	  Author: sangechen
 */

#ifndef SHARE_FW_LOG_THRIFTAPPENDER_H_
#define SHARE_FW_LOG_THRIFTAPPENDER_H_

#include <boost/shared_ptr.hpp>
#include <log4cplus/appender.h>

#include "../../../share/fw/thrift/ThriftServiceClient.hpp"
#include "../../../rpc-thrift/src_gen/gen-cpp/LogServer.h"

class ThriftAppender : public log4cplus::Appender
{
public:
	ThriftAppender(const std::string& address = "");
	virtual ~ThriftAppender();

	/**
	 * 关闭到log_server的连接
	 */
	virtual void close();

protected:
	/**
	 * 向log_server发送log数据包 {one-way请求包}
	 * [每次writeLog调用会检测连接是否正常, 并自动重连(会获取一次最新配置)]
	 * [append()保证一定是线程安全的, 因为log4cplus的Appender框架在调用append()前会锁mutex]
	 * [另外 这里可以考虑前面套一个AsyncAppender]
	 * [这个函数不能抛异常...]
	 */
	virtual void append(const log4cplus::spi::InternalLoggingEvent& event);

	/**
	 * 获取一次最新配置, 重连
	 */
	void _reConnect();

	/**
	 * log_server的Client对象
	 */
	boost::shared_ptr<ThriftServiceClient<::service::log::LogServerClient>> logServerPtr; //9070~log_server
	/**
	 * 保存本机相对于log_server的IP地址, 方便追踪日志.
	 */
	std::string selfAddress;
};

#endif /* SHARE_FW_LOG_THRIFTAPPENDER_H_ */
