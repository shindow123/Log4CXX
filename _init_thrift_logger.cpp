/*
 * _init_thrift_logger.cpp
 *
 *  Created on: 2015年11月6日
 *      Author: sangechen
 */

#include "_init_thrift_logger.h"
#include "ThriftAppender.h"

#include <log4cplus/logger.h>
#include <log4cplus/asyncappender.h>

//为了更方便理解_x_log_xxx()和thrift_logger的关系, 可以考虑用include:
//#include "../../../share/srv/_x_log_wrapper__ref_X_LOGGER_NAME.h"
/** 引用外部定义好的logger_name变量 */
extern const char* ref_X_LOGGER_NAME;

void _init_thrift_logger(const _logger_init_params& params)
{
	if (params.logger.empty()) return; //logger_name空则不做任何操作

	//对ref_X_LOGGER_NAME进行重新赋值,
	//保证_x_log_xxx()用的值和thrift_logger初始化的值 都与命令行选项一致
	ref_X_LOGGER_NAME = params.logger.c_str(); //这里假定params.logger的生命周期足够长


	if (!params.use_log_server) return; //无需初始化thrift_logger

	//初始化logger基本属性
	log4cplus::Logger _logger = log4cplus::Logger::getInstance(params.logger);
	_logger.setLogLevel(log4cplus::getLogLevelManager().fromString(params.loglevel)); //设置日志级别
	_logger.setAdditivity(params.additivity); //不将log_event传递到上层logger

	//初始化Appender
	log4cplus::SharedAppenderPtr t_app(new ThriftAppender());
	t_app->setName("thrift_appender");
	log4cplus::SharedAppenderPtr _appender(new log4cplus::AsyncAppender(t_app, 65536)); //容量64K
	_appender->setName("aysnc_thrift_appender");

	_logger.removeAllAppenders();
	_logger.addAppender(_appender);
}

