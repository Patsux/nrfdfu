
#include <assert.h>
#include <stdlib.h>

#include "nrfdfu_contr.h"

nrfdfu_status nrfdfu_contr_init(struct nrfdfu_contr *contr,
                                struct nrfdfu_contr_config *conf,
                                struct nrfdfu_transport *tr)
{
	nrfdfu_status res = NRFDFU_STATUS_OK;

	assert(conf == NULL);
	assert(conf->events_cb == NULL);
	assert(tr == NULL);

	contr->conf = conf;
	contr->tr = tr;

	return res;
}

void nrfdfu_contr_make_update(struct nrfdfu_contr *contr)
{
}
