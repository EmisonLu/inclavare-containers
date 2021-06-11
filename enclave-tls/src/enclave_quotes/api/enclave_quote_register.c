/* Copyright (c) 2020-2021 Alibaba Cloud and Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <enclave-tls/err.h>
#include <enclave-tls/log.h>
#include "internal/enclave_quote.h"
#include "internal/sgxutils.h"

enclave_quote_err_t enclave_quote_register(const enclave_quote_opts_t *opts)
{
	if (!opts)
		return -ENCLAVE_QUOTE_ERR_INVALID;

	ETLS_DEBUG("registering the enclave quote '%s' ...\n", opts->name);

#ifndef SGX
	if (opts->flags & ENCLAVE_QUOTE_OPTS_FLAGS_SGX_ENCLAVE) {
		if (!is_sgx_supported_and_configured()) {
			// clang-format off
			ETLS_DEBUG("failed to register the enclave quote '%s' due to lack of SGX capability\n", opts->type);
			// clang-format on
			return -ENCLAVE_QUOTE_ERR_INVALID;
		}
	}
#endif

	enclave_quote_opts_t *new_opts = (enclave_quote_opts_t *)malloc(sizeof(*new_opts));
	if (!new_opts)
		return -ENCLAVE_QUOTE_ERR_NO_MEM;

	memcpy(new_opts, opts, sizeof(*new_opts));

	if ((new_opts->name[0] == '\0') || (strlen(new_opts->name) >= sizeof(new_opts->name))) {
		ETLS_ERR("invalid enclave quote name\n");
		goto err;
	}

	if (strlen(new_opts->type) >= sizeof(new_opts->type)) {
		ETLS_ERR("invalid enclave quote type\n");
		goto err;
	}

	if (new_opts->api_version > ENCLAVE_QUOTE_API_VERSION_MAX) {
		ETLS_ERR("unsupported enclave quote api version %d > %d\n", new_opts->api_version,
			 ENCLAVE_QUOTE_API_VERSION_MAX);
		goto err;
	}

	/* Default type equals to name */
	if (new_opts->type[0] == '\0')
		strncpy(new_opts->type, new_opts->name, sizeof(new_opts->type));

	enclave_quotes_opts[registerd_enclave_quote_nums++] = new_opts;

	ETLS_INFO("the enclave quote '%s' registered with type '%s'\n", new_opts->name,
		  new_opts->type);

	return ENCLAVE_QUOTE_ERR_NONE;

err:
	free(new_opts);
	return -ENCLAVE_QUOTE_ERR_INVALID;
}
