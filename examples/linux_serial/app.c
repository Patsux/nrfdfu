
#include <assert.h>
#include <stdio.h>

#include "nrfdfu_contr.h"
/* TODO To be removed when concrete transport includes will do the job. */
#include "nrfdfu_transport.h"

void dfu_contr_event_cb(struct nrfdfu_contr *contr,
                        struct nrfdfu_contr_event *event,
                        void *opaque)
{
	switch (event->type) {
	case NRFDFU_CONTR_EVENT_GET_DATA:
		printf("NRFDFU_CONTR_EVENT_GET_DATA: data_type: %d\n",
		       event->data.get_data.data.type);
		break;
	case NRFDFU_CONTR_EVENT_FREE_DATA:
		printf("NRFDFU_CONTR_EVENT_FREE_DATA: data_type: %d\n",
		       event->data.free_data.data.type);
		break;
	case NRFDFU_CONTR_EVENT_ERROR:
		printf("NRFDFU_CONTR_EVENT_ERROR: error: %d\n",
		       event->data.error.status);
		break;
	default:
		assert(0);
		break;
	}
}

int main(int argc, char *argv[])
{
	nrfdfu_status dfu_status;
	struct nrfdfu_contr dfu_contr;
	struct nrfdfu_contr_config dfu_contr_config;
	struct nrfdfu_transport dfu_transport;

	// Set the DFU controller
	dfu_contr_config.events_cb = dfu_contr_event_cb;
	dfu_contr_config.events_opaque = NULL;

	dfu_status = nrfdfu_contr_init(&dfu_contr, &dfu_contr_config,
	                               &dfu_transport);
	if (dfu_status != NRFDFU_STATUS_OK) {
		printf("Error when initializing the dfu controller: %d\n",
		       dfu_status);
		return -1;
	}
	nrfdfu_contr_make_update(&dfu_contr);

	return 0;
}
