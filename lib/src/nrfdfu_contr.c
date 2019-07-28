
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "nrfdfu_contr.h"

static void send_event_get_data(struct nrfdfu_contr *contr,
                                struct nrfdfu_data *data);
static void send_event_free_data(struct nrfdfu_contr *contr,
                                 struct nrfdfu_data *data);
static void send_event_sleep(struct nrfdfu_contr *contr, uint32_t ms);
static void send_event_contr_error(struct nrfdfu_contr *contr,
                                   nrfdfu_contr_status status);
static void send_event_transport_error(struct nrfdfu_contr *contr,
                                       struct nrfdfu_transport_error *error);

nrfdfu_contr_status nrfdfu_contr_init(struct nrfdfu_contr *contr,
                                      struct nrfdfu_contr_config *conf,
                                      struct nrfdfu_transport *tr)
{
	nrfdfu_contr_status res = NRFDFU_CONTR_STATUS_OK;

	assert(conf != NULL);
	assert(conf->events_cb != NULL);
	assert(tr->open != NULL);
	assert(tr->close != NULL);
	assert(tr->send_init_packet != NULL);
	assert(tr->send_fw != NULL);

	contr->conf = conf;
	contr->tr = tr;

	return res;
}

static int8_t send_data(struct nrfdfu_contr *contr, enum nrfdfu_data_type type,
                        uint8_t is_packet_init)
{
	int res = 0;
	struct nrfdfu_data data;
	nrfdfu_transport_status_type tr_status;

	data.type = type;
	data.buf = NULL;
	data.len = 0;
	send_event_get_data(contr, &data);
	if ((data.buf != NULL) && (data.len > 0)) {
		if (is_packet_init) {
			tr_status = contr->tr->send_init_packet(contr->tr->concrete,
			                                        &data);
		} else {
			tr_status = contr->tr->send_fw(contr->tr->concrete, &data);
		}
		send_event_free_data(contr, &data);

		if (tr_status != NRFDFU_TRANSPORT_STATUS_OK) {
			send_event_transport_error(contr, &contr->tr->last_error);
		}
	} else {
		send_event_contr_error(contr, NRFDFU_CONTR_STATUS_ERR_GET_DATA);
		res = -1;
	}

	return res;
}

static int8_t send_packet_init_and_fw(struct nrfdfu_contr *contr,
                                      enum nrfdfu_data_type packet_init_type,
                                      enum nrfdfu_data_type fw_type)
{
	int8_t res;

	res = send_data(contr, packet_init_type, 1);
	if (res < 0) {
		return res;
	}
	res = send_data(contr, fw_type, 0);
	if (res < 0) {
		return res;
	}

	// Wait the device reboots.
	send_event_sleep(contr, 3000);

	return 0;
}

void nrfdfu_contr_make_update(struct nrfdfu_contr *contr)
{
	int8_t res;

	// Sorted in the order firmwares must be updated.
	enum nrfdfu_data_type data_to_sent[][2] = {
		{NRFDFU_BLSD_PACKET_INIT, NRFDFU_BLSD_FW},
		{NRFDFU_SD_PACKET_INIT, NRFDFU_SD_FW},
		{NRFDFU_BL_PACKET_INIT, NRFDFU_BL_FW},
		{NRFDFU_APP_PACKET_INIT, NRFDFU_APP_FW}
	};
	const int8_t nr_send = sizeof(data_to_sent) / sizeof(data_to_sent[0]);

	res = 0;
	for (uint8_t i = 0; (i < nr_send) && (res == 0); i++) {
		res = send_packet_init_and_fw(contr, data_to_sent[i][0],
		                              data_to_sent[i][1]);
	}
}

static void send_event_get_data(struct nrfdfu_contr *contr,
                                struct nrfdfu_data *data)
{
	struct nrfdfu_contr_event event;

	event.type = NRFDFU_CONTR_EVENT_GET_DATA;
	memcpy(&event.data.get_data.data, data, sizeof(struct nrfdfu_data));
	contr->conf->events_cb(contr, &event, contr->conf->events_opaque);
	memcpy(data, &event.data.get_data.data, sizeof(struct nrfdfu_data));
}

static void send_event_free_data(struct nrfdfu_contr *contr,
                                 struct nrfdfu_data *data)
{
	struct nrfdfu_contr_event event;

	event.type = NRFDFU_CONTR_EVENT_FREE_DATA;
	memcpy(&event.data.get_data.data, data, sizeof(struct nrfdfu_data));
	contr->conf->events_cb(contr, &event, contr->conf->events_opaque);
}

static void send_event_sleep(struct nrfdfu_contr *contr, uint32_t ms)
{
	struct nrfdfu_contr_event event;

	event.type = NRFDFU_CONTR_EVENT_SLEEP;
	event.data.sleep.ms = ms;
	contr->conf->events_cb(contr, &event, contr->conf->events_opaque);
}

static void send_event_contr_error(struct nrfdfu_contr *contr,
                                   nrfdfu_contr_status status)
{
	struct nrfdfu_contr_event event;

	event.type = NRFDFU_CONTR_EVENT_CONTR_ERROR;
	event.data.contr_error.status = status;
	contr->conf->events_cb(contr, &event, contr->conf->events_opaque);
}

static void send_event_transport_error(struct nrfdfu_contr *contr,
                                       struct nrfdfu_transport_error *error)
{
	struct nrfdfu_contr_event event;

	event.type = NRFDFU_CONTR_EVENT_TRANSPORT_ERROR;
	memcpy(&event.data.transport_error, error, sizeof(struct nrfdfu_transport_error));
	contr->conf->events_cb(contr, &event, contr->conf->events_opaque);
}
