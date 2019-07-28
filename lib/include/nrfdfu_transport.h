
#ifndef _NRFDFU_TRANSPORT_H_
#define _NRFDFU_TRANSPORT_H_

struct nrfdfu_data;

/*
 * Protocol's response code. The last one is for extended codes represented by
 * the enumeration nrfdfu_ext_err.
 */
typedef enum {
	NRFDFU_RESPCODE_INVALID_CODE = 0,
	NRFDFU_RESPCODE_SUCCESS,
	NRFDFU_RESPCODE_NOT_SUPPORTED,
	NRFDFU_RESPCODE_INVALID_PARAMETER,
	NRFDFU_RESPCODE_NOT_INSUFFICIENT_RESSOURCES,
	NRFDFU_RESPCODE_INVALID_OBJECT,
	NRFDFU_RESPCODE_INVALID_SIGNATURE,
	NRFDFU_RESPCODE_UNSUPPORTED_TYPE,
	NRFDFU_RESPCODE_OPERATION_NOT_PERMITTED,
	NRFDFU_RESPCODE_OPERATION_FAILED,
	NRFDFU_RESPCODE_EXTENDED_ERROR
} nrfdfu_resp_code;

/* Protocol's extended error codes when the responde code is
 * NRFDFU_RESPCODE_EXTENDED_ERROR. */
typedef enum {
	NRFDFU_EXTERR_NO_ERR = 0,
	NRFDFU_EXTERR_INVALID_ERROR_CODE,
	NRFDFU_EXTERR_BAD_CMD_FORMAT,
	NRFDFU_EXTERR_UNSUPORTED_CMD,
	NRFDFU_EXTERR_INVALID_INIT_CMD,
	NRFDFU_EXTERR_FW_VERSION_TOO_LOW,
	NRFDFU_EXTERR_HW_VERSION_MISMATCHED,
	NRFDFU_EXTERR_UNSUPORTED_SD,
	NRFDFU_EXTERR_SIGNATURE_REQUIRED,
	NRFDFU_EXTERR_UNSUPORTED_HASH,
	NRFDFU_EXTERR_CANNOT_COMPUTE_HASH,
	NRFDFU_EXTERR_UNSUPORTED_SIGNATURE,
	NRFDFU_EXTERR_HASH_INVALID,
	NRFDFU_EXTERR_INSUFFICIENT_MEMORY,
	NRFDFU_EXTERR_REQUESTED_FW_PRESENT
} nrfdfu_ext_err;

/*
 * Defines status type for the transport layer. An error can occur on the protocol
 * layer or on the link. The protocol's errors are shared between implementations,
 *  whereas link errors depends on concrete transport layer.
 */
typedef enum {
	NRFDFU_TRANSPORT_STATUS_OK = 0,
	NRFDFU_TRANSPORT_STATUS_ERR_PROTOCOL,
	NRFDFU_TRANSPORT_STATUS_ERR_LINK
} nrfdfu_transport_status_type;

/*
 * Represents an error of the transport layer. The type defines if there
 * is an error. On error, the union can be inspected to known the error, either
 * for the protocol when type is NRFDFU_TRANSPORT_STATUS_ERR_PROTOCOL, or for
 * the link when type is NRFDFU_TRANSPORT_STATUS_ERR_LINK. The value of this
 * last differs depending of underlaying transport layer.
 */
struct nrfdfu_transport_error {
	nrfdfu_transport_status_type type;

	union {
		struct {
			nrfdfu_resp_code resp_code;
			nrfdfu_ext_err ext_err;
		} protocol;
		struct {
			uint32_t error;
		} link;
	} data;
};
typedef nrfdfu_transport_status_type (*nrfdfu_transport_open)(void *concrete);
typedef nrfdfu_transport_status_type (*nrfdfu_transport_close)(void *concrete);
typedef nrfdfu_transport_status_type (*nrfdfu_transport_send_init_packet)(
                                               void *concrete,
                                               struct nrfdfu_data *init_packet);
typedef nrfdfu_transport_status_type (*nrfdfu_transport_send_fw)(void *concrete,
                                                        struct nrfdfu_data *fw);

/* Generic transport layer used by the controller to interact with a device. */
struct nrfdfu_transport {
	void *concrete; /* Concrete instance of the transport object. */
	struct nrfdfu_transport_error last_error;
	nrfdfu_transport_open open;
	nrfdfu_transport_close close;
	nrfdfu_transport_send_init_packet send_init_packet;
	nrfdfu_transport_send_fw send_fw;
};

#endif
