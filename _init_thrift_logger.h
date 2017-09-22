/*
 * _init_thrift_logger.h
 *
 *  Created on: 2015年11月6日
 *      Author: sangechen
 */

#ifndef SHARE_FW_LOG__INIT_THRIFT_LOGGER_H_
#define SHARE_FW_LOG__INIT_THRIFT_LOGGER_H_

#include "logger_params.h"

/**
 * 定义一个函数原型, 不同编译单元可以有不同实现. ThriftAppender 本地文件Appender ...
 */
void _init_thrift_logger(const _logger_init_params& params);

#endif /* SHARE_FW_LOG__INIT_THRIFT_LOGGER_H_ */
