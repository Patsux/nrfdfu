
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "nrfdfu_contr.h"

static int compare_event(const LargestIntegralType value,
                         const LargestIntegralType check_value_data)
{
	struct nrfdfu_contr_event *event = (struct nrfdfu_contr_event *)value;
	struct nrfdfu_contr_event *check_event = (struct nrfdfu_contr_event *)check_value_data;

	if (event->type != check_event->type) {
		return 0;
	} else {
		return 1;
	}
}

static void dummy_contr_event_cb(struct nrfdfu_contr *contr,
                                 struct nrfdfu_contr_event *event,
                                 void *opaque)
{
	uint8_t dummy_buf;

	check_expected(contr);
	check_expected(event);

	// Must return a data, otherwise it stops.
	switch (event->type) {
	case NRFDFU_CONTR_EVENT_GET_DATA:
		event->data.get_data.data.buf = &dummy_buf;
		event->data.get_data.data.len = sizeof(dummy_buf);
		break;
	default:
		break;
	}
}

static nrfdfu_transport_status_type dummy_transport_open(void *concrete)
{
	return NRFDFU_TRANSPORT_STATUS_OK;
}

static nrfdfu_transport_status_type dummy_transport_close(void *concrete)
{
	return NRFDFU_TRANSPORT_STATUS_OK;
}

static nrfdfu_transport_status_type dummy_transport_send_packet_init(
                                                void *concrete,
                                                struct nrfdfu_data *init_packet)
{
	return NRFDFU_TRANSPORT_STATUS_OK;
}

static nrfdfu_transport_status_type dummy_transport_send_fw(void *concrete,
                                                         struct nrfdfu_data *fw)
{
	return NRFDFU_TRANSPORT_STATUS_OK;
}

static void test_contr_init(void **state)
{
	struct nrfdfu_contr contr;
	struct nrfdfu_contr_config conf;
	struct nrfdfu_transport tr;
	uint8_t dummy_opaque;
	uint8_t dummy_tr_concrete;
	nrfdfu_contr_status res;

	// Set dummy transport layer
	tr.concrete = &dummy_tr_concrete;
	tr.open = dummy_transport_open;
	tr.close = dummy_transport_close;
	tr.send_init_packet = dummy_transport_send_packet_init;
	tr.send_fw = dummy_transport_send_fw;

	// Set controller configuration
	conf.events_cb = dummy_contr_event_cb;
	conf.events_opaque = &dummy_opaque;

	// Run test
	res = nrfdfu_contr_init(&contr, &conf, &tr);
	assert_int_equal(res, NRFDFU_CONTR_STATUS_OK);
	assert_ptr_equal(contr.conf, &conf);
	assert_ptr_equal(contr.conf->events_cb, dummy_contr_event_cb);
	assert_ptr_equal(contr.conf->events_opaque, &dummy_opaque);
	assert_ptr_equal(contr.tr, &tr);
	assert_ptr_equal(contr.tr->concrete, &dummy_tr_concrete);
	assert_ptr_equal(contr.tr->open, &dummy_transport_open);
	assert_ptr_equal(contr.tr->close, &dummy_transport_close);
	assert_ptr_equal(contr.tr->send_init_packet, &dummy_transport_send_packet_init);
	assert_ptr_equal(contr.tr->send_fw, &dummy_transport_send_fw);
}

static void test_contr_make_update_success(void **state)
{
	struct nrfdfu_contr contr;
	struct nrfdfu_contr_config conf;
	struct nrfdfu_transport tr;
	uint8_t dummy_opaque;
	uint8_t dummy_tr_concrete;
	nrfdfu_contr_status res;

	struct nrfdfu_contr_event expected_events[] = {
		// BLSD
		{.type = NRFDFU_CONTR_EVENT_GET_DATA},
		{.type = NRFDFU_CONTR_EVENT_FREE_DATA},
		{.type = NRFDFU_CONTR_EVENT_GET_DATA},
		{.type = NRFDFU_CONTR_EVENT_FREE_DATA},
		{.type = NRFDFU_CONTR_EVENT_SLEEP},
		// SD
		{.type = NRFDFU_CONTR_EVENT_GET_DATA},
		{.type = NRFDFU_CONTR_EVENT_FREE_DATA},
		{.type = NRFDFU_CONTR_EVENT_GET_DATA},
		{.type = NRFDFU_CONTR_EVENT_FREE_DATA},
		{.type = NRFDFU_CONTR_EVENT_SLEEP},
		// BL
		{.type = NRFDFU_CONTR_EVENT_GET_DATA},
		{.type = NRFDFU_CONTR_EVENT_FREE_DATA},
		{.type = NRFDFU_CONTR_EVENT_GET_DATA},
		{.type = NRFDFU_CONTR_EVENT_FREE_DATA},
		{.type = NRFDFU_CONTR_EVENT_SLEEP},
		// APP
		{.type = NRFDFU_CONTR_EVENT_GET_DATA},
		{.type = NRFDFU_CONTR_EVENT_FREE_DATA},
		{.type = NRFDFU_CONTR_EVENT_GET_DATA},
		{.type = NRFDFU_CONTR_EVENT_FREE_DATA},
		{.type = NRFDFU_CONTR_EVENT_SLEEP}
	};
	const uint8_t nr_events = sizeof(expected_events) / sizeof(expected_events[0]);

	// Set dummy transport layer
	tr.concrete = &dummy_tr_concrete;
	tr.open = dummy_transport_open;
	tr.close = dummy_transport_close;
	tr.send_init_packet = dummy_transport_send_packet_init;
	tr.send_fw = dummy_transport_send_fw;

	// Set controller configuration
	conf.events_cb = dummy_contr_event_cb;
	conf.events_opaque = &dummy_opaque;

	// Init controller
	res = nrfdfu_contr_init(&contr, &conf, &tr);
	assert_int_equal(res, NRFDFU_CONTR_STATUS_OK);

	// Set expected of the events callback.
	for (uint8_t i = 0; i < nr_events; i++) {
		expect_value(dummy_contr_event_cb, contr, &contr);
		expect_check(dummy_contr_event_cb, event, compare_event,
		             &expected_events[i]);
	}

	// Run
	nrfdfu_contr_make_update(&contr);
}

void run_controller_tests(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup(test_contr_init, NULL),
		cmocka_unit_test_setup(test_contr_make_update_success, NULL),
	};

	cmocka_run_group_tests(tests, NULL, NULL);
}
