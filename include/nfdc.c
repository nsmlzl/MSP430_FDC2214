#include "nfdc.h"


uint8_t nc_init(){
	return ni_slave_present(0x2A);
}
