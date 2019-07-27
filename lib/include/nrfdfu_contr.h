
#ifndef _NRFDFU_CONTR_H_
#define _NRFDFU_CONTR_H_

#include <stdint.h>

#include "nrfdfu_common.h"

struct nrfdfu_contr;
struct nrfdfu_transport;

/*
 * A data is a binary buffer sent to the device. There are two data for
 * a firmware: the "firwmare" itself, and the "packet init" which
 * contains firmware's metadata.
 * On NRF devices, there are three firwmares:
 *	- Bootloader (BL)
 *	- Soft device (SD)
 *	- Application (APP)
 *
 * The enumeration is sorted in the order data must be updated. Note
 * the BLSD is a buffer that contains both bootloader and soft device.
 */
enum nrfdfu_data_type {
	NRFDFU_BL_PACKET_INIT = 0,
	NRFDFU_BL_FW,
	NRFDFU_SD_PACKET_INIT,
	NRFDFU_SD_FW,
	NRFDFU_BLSD_PACKET_INIT,
	NRFDFU_BLSD_FW,
	NRFDFU_APP_PACKET_INIT,
	NRFDFU_APP_FW,
        NRFDFU_NR_DATA_TYPE
};

/*
 * Data buffer are provided by the controller users through the event
 * callback.
 */
struct nrfdfu_data {
	enum nrfdfu_data_type type;
	uint8_t *buf;
	uint32_t len;
};

enum nrfdfu_contr_event_type {
	NRFDFU_CONTR_EVENT_GET_DATA = 0,
	NRFDFU_CONTR_EVENT_FREE_DATA,
	NRFDFU_CONTR_EVENT_ERROR
};

struct nrfdfu_contr_event {
	enum nrfdfu_contr_event_type type;

	union {
		struct {
			struct nrfdfu_data data;
		} get_data;
		struct {
			struct nrfdfu_data data;
		} free_data;
		struct {
			nrfdfu_status status;
		} error;
	} data;
};

typedef void (*nrfdfu_contr_events_cb)(struct nrfdfu_contr *contr,
                                       struct nrfdfu_contr_event *event,
                                       void *opaque);

struct nrfdfu_contr_config {
	nrfdfu_contr_events_cb events_cb;
	void *events_opaque;
};

struct nrfdfu_contr {
	struct nrfdfu_contr_config *conf;
	struct nrfdfu_transport *tr;
};

/*
 * Initializes a DFU controller that will drive update. It relies
 * on a generic transport layer to be provided.
 * The controller is configurable with the public nrfdfu_contr_config
 * structure.
 */
nrfdfu_status nrfdfu_contr_init(struct nrfdfu_contr *contr,
                                struct nrfdfu_contr_config *conf,
                                struct nrfdfu_transport *tr);

/*
 * Runs until the update ends on success or error. All states are
 * returns by the event callback.
 */
void nrfdfu_contr_make_update(struct nrfdfu_contr *contr);

#endif
