/*
 * util/edns.h - handle base EDNS options.
 *
 * Copyright (c) 2018, NLnet Labs. All rights reserved.
 *
 * This software is open source.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of the NLNET LABS nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 *
 * This file contains functions for base EDNS options.
 */

#ifndef UTIL_EDNS_H
#define UTIL_EDNS_H

#include "util/storage/dnstree.h"

struct edns_data;
struct config_file;
struct comm_point;
struct regional;
struct comm_reply;

/**
 * Validate the EDNS Server cookie.
 *
 * @param cfg: configuration.
 * @param repinfo: struct containing the address of the querier.
 * @param cookie_opt: comm channel.
 * @param now: current time.
 * @return 1 on a valid server cookie, otherwise 0.
 */
int edns_cookie_validate(struct config_file* cfg, struct comm_reply* repinfo,
                struct edns_option* cookie_opt, time_t now) ;
/**
 * Structure containing all EDNS tags.
 */
struct edns_tags {
	/** Tree of EDNS client tags to use in upstream queries, per address
	 * prefix. Contains nodes of type edns_tag_addr. */
	rbtree_type client_tags;
	/** EDNS opcode to use for client tags */
	uint16_t client_tag_opcode;
	/** region to allocate tree nodes in */
	struct regional* region;
};

/**
 * EDNS tag. Node of rbtree, containing tag and prefix.
 */
struct edns_tag_addr {
	/** node in address tree, used for tree lookups. Need to be the first
	 * member of this struct. */
	struct addr_tree_node node;
	/** tag data, in host byte ordering */
	uint16_t tag_data;
};

/**
 * Create structure to hold EDNS tags
 * @return: newly created edns_tags, NULL on alloc failure.
 */
struct edns_tags* edns_tags_create(void);

/** Delete EDNS tags structure
 * @param edns_tags: struct to delete
 */
void edns_tags_delete(struct edns_tags* edns_tags);

/**
 * Add configured EDNS tags
 * @param edns_tags: edns tags to apply config to
 * @param config: struct containing EDNS tags configuration
 * @return 0 on error
 */
int edns_tags_apply_cfg(struct edns_tags* edns_tags,
	struct config_file* config);

/**
 * Find tag for address.
 * @param tree: tree containing EDNS tags per address prefix.
 * @param addr: address to use for tree lookup
 * @param addrlen: length of address
 * @return: matching tree node, NULL otherwise
 */
struct edns_tag_addr*
edns_tag_addr_lookup(rbtree_type* tree, struct sockaddr_storage* addr,
	socklen_t addrlen);

/**
 * Apply common EDNS options.
 *
 * @param edns_out: initialised edns information with outbound edns.
 * @param edns_in: initialised edns information with received edns.
 * @param cfg: configuration.
 * @param c: comm channel.
 * @param now: the current time.
 * @param region: the region to allocate the edns options in.
 * @return 1 on success, otherwise 0.
 */
int apply_edns_options(struct edns_data* edns_out, struct edns_data* edns_in,
	struct config_file* cfg, struct comm_point* c,
	struct comm_reply *repinfo, time_t now, struct regional* region);

#endif
