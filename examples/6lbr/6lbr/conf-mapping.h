#ifndef CONF_MAPPING_H_
#define CONF_MAPPING_H_

#include "nvm-config.h"


//RA Configuration

#undef UIP_CONF_ROUTER_LIFETIME
#define UIP_CONF_ROUTER_LIFETIME (nvm_data.ra_router_lifetime)

#undef UIP_CONF_ND6_MAX_RA_INTERVAL
#define UIP_CONF_ND6_MAX_RA_INTERVAL (nvm_data.ra_max_interval)

#undef UIP_CONF_ND6_MIN_RA_INTERVAL
#define UIP_CONF_ND6_MIN_RA_INTERVAL  (nvm_data.ra_min_interval)

#undef UIP_CONF_ND6_MIN_DELAY_BETWEEN_RAS
#define UIP_CONF_ND6_MIN_DELAY_BETWEEN_RAS  (nvm_data.ra_min_delay)

//RPL Configuration

#undef RPL_CONF_DEFAULT_INSTANCE
#define RPL_CONF_DEFAULT_INSTANCE    (nvm_data.rpl_instance_id)

#undef RPL_CONF_DIO_INTERVAL_DOUBLINGS
#define RPL_CONF_DIO_INTERVAL_DOUBLINGS    (nvm_data.rpl_dio_intdoubl)

#undef RPL_CONF_DIO_INTERVAL_MIN
#define RPL_CONF_DIO_INTERVAL_MIN    (nvm_data.rpl_dio_intmin)

#undef RPL_CONF_DIO_REDUNDANCY
#define RPL_CONF_DIO_REDUNDANCY    (nvm_data.rpl_dio_redundancy)

#undef RPL_CONF_DEFAULT_LIFETIME
#define RPL_CONF_DEFAULT_LIFETIME    (nvm_data.rpl_default_lifetime)

#undef RPL_CONF_MIN_HOPRANKINC
#define RPL_CONF_MIN_HOPRANKINC    (nvm_data.rpl_min_hoprankinc)

#undef RPL_CONF_DEFAULT_LIFETIME_UNIT
#define RPL_CONF_DEFAULT_LIFETIME_UNIT    (nvm_data.rpl_lifetime_unit)

#endif
