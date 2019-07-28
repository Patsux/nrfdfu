
#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include "nrfdfu_contr.h"
/* TODO To be removed when concrete transport includes will do the job. */
#include "nrfdfu_transport.h"

void dfu_contr_event_cb(struct nrfdfu_contr *contr,
                        struct nrfdfu_contr_event *event,
                        void *opaque)
{
	uint8_t dummy_buf;

	switch (event->type) {
	case NRFDFU_CONTR_EVENT_GET_DATA:
		printf("NRFDFU_CONTR_EVENT_GET_DATA: data_type: %d\n",
		       event->data.get_data.data.type);
		event->data.get_data.data.buf = &dummy_buf;
		event->data.get_data.data.len = sizeof(dummy_buf);
		break;
	case NRFDFU_CONTR_EVENT_FREE_DATA:
		printf("NRFDFU_CONTR_EVENT_FREE_DATA: data_type: %d\n",
		       event->data.free_data.data.type);
		break;
	case NRFDFU_CONTR_EVENT_SLEEP:
		printf("NRFDFU_CONTR_EVENT_SLEEP: ms: %d\n",
		       event->data.sleep.ms);
		usleep(event->data.sleep.ms * 1000);
		break;
	case NRFDFU_CONTR_EVENT_TRANSPORT_ERROR:
		if (event->data.transport_error.error.type == NRFDFU_TRANSPORT_STATUS_ERR_PROTOCOL) {
			printf("NRFDFU_CONTR_EVENT_TRANSPORT_ERROR: type: %d"
			       "resp_code: %d, ext_err: %d\n",
			       event->data.transport_error.error.type,
			       event->data.transport_error.error.data.protocol.resp_code,
			       event->data.transport_error.error.data.protocol.ext_err);
		} else {
			printf("NRFDFU_CONTR_EVENT_TRANSPORT_ERROR: type: %d\n",
			       event->data.transport_error.error.type,
			       event->data.transport_error.error.data.link.error);
		}
		break;
	case NRFDFU_CONTR_EVENT_CONTR_ERROR:
		printf("NRFDFU_CONTR_EVENT_CONTR_ERROR: error: %d\n",
		       event->data.contr_error.status);
		break;
	default:
		assert(0);
		break;
	}
}

nrfdfu_transport_status_type dummy_transport_open(void *concrete)
{
	return NRFDFU_TRANSPORT_STATUS_OK;
}

nrfdfu_transport_status_type dummy_transport_close(void *concrete)
{
	return NRFDFU_TRANSPORT_STATUS_OK;
}

nrfdfu_transport_status_type dummy_transport_send_packet_init(
                                               void *concrete,
                                               struct nrfdfu_data *init_packet)
{
	return NRFDFU_TRANSPORT_STATUS_OK;
}

nrfdfu_transport_status_type dummy_transport_send_fw(void *concrete,
                                                     struct nrfdfu_data *fw)
{
	return NRFDFU_TRANSPORT_STATUS_OK;
}

int main(int argc, char *argv[])
{
	nrfdfu_contr_status dfu_status;
	struct nrfdfu_contr dfu_contr;
	struct nrfdfu_contr_config dfu_contr_config;
	struct nrfdfu_transport dfu_transport;

	// Set a dummy transport
	dfu_transport.open = dummy_transport_open;
	dfu_transport.close = dummy_transport_close;
	dfu_transport.send_init_packet = dummy_transport_send_packet_init;
	dfu_transport.send_fw = dummy_transport_send_fw;

	// Set the DFU controller
	dfu_contr_config.events_cb = dfu_contr_event_cb;
	dfu_contr_config.events_opaque = NULL;

	dfu_status = nrfdfu_contr_init(&dfu_contr, &dfu_contr_config,
	                               &dfu_transport);
	if (dfu_status != NRFDFU_CONTR_STATUS_OK) {
		printf("Error when initializing the dfu controller: %d\n",
		       dfu_status);
		return -1;
	}
	nrfdfu_contr_make_update(&dfu_contr);

	return 0;
}
