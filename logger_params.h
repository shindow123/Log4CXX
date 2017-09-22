/*
 * logger_params.h
 *
 *  Created on: 2015年11月11日
 *      Author: sangechen
 */

#ifndef SHARE_FW_LOG_LOGGER_PARAMS_H_
#define SHARE_FW_LOG_LOGGER_PARAMS_H_

#include <string>

struct _logger_init_params
{
	bool use_log_server;
	std::string logger;
	std::string loglevel;
	bool additivity;
};

#endif /* SHARE_FW_LOG_LOGGER_PARAMS_H_ */
