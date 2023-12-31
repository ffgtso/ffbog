/* SPDX-FileCopyrightText: 2016, Matthias Schiffer <mschiffer@universe-factory.net> */
/* SPDX-License-Identifier: BSD-2-Clause */


#include <respondd.h>

#include <json-c/json.h>
#include <libgluonutil.h>

#include <uci.h>

#include <stdlib.h>
#include <string.h>


static struct uci_section * get_first_section(struct uci_package *p, const char *type) {
	struct uci_element *e;
	uci_foreach_element(&p->sections, e) {
		struct uci_section *s = uci_to_section(e);
		if (!strcmp(s->type, type))
			return s;
	}

	return NULL;
}

static const char * get_first_option(struct uci_context *ctx, struct uci_package *p, const char *type, const char *option) {
	struct uci_section *s = get_first_section(p, type);
	if (s)
		return uci_lookup_option_string(ctx, s, option);
	else
		return NULL;
}

static struct json_object * get_number(struct uci_context *ctx, struct uci_section *s, const char *name) {
	const char *val = uci_lookup_option_string(ctx, s, name);
	if (!val || !*val)
		return NULL;

	char *end;
	double d = strtod(val, &end);
	if (*end)
		return NULL;

	struct json_object *jso = json_object_new_double(d);
	json_object_set_serializer(jso, json_object_double_to_json_string, "%.8f", NULL);
	return jso;
}

static void maybe_add_number(struct uci_context *ctx, struct uci_section *s, const char *name, struct json_object *parent) {
	struct json_object *jso = get_number(ctx, s, name);
	if (jso)
		json_object_object_add(parent, name, jso);
}

static struct json_object * get_location(struct uci_context *ctx, struct uci_package *p) {
	struct uci_section *s = get_first_section(p, "location");
	if (!s)
		return NULL;

	const char *share = uci_lookup_option_string(ctx, s, "share_location");
	if (!share || strcmp(share, "1"))
		return NULL;

	struct json_object *ret = json_object_new_object();

	maybe_add_number(ctx, s, "latitude", ret);
	maybe_add_number(ctx, s, "longitude", ret);
	maybe_add_number(ctx, s, "altitude", ret);

	return ret;
}

static struct json_object * get_owner(struct uci_context *ctx, struct uci_package *p) {
	const char *contact = get_first_option(ctx, p, "owner", "contact");
	if (!contact || !*contact)
		return NULL;

	struct json_object *ret = json_object_new_object();
	json_object_object_add(ret, "contact", gluonutil_wrap_string(contact));
	return ret;
}

static struct json_object * get_system(struct uci_context *ctx, struct uci_package *p) {
	struct json_object *ret = json_object_new_object();

	const char *role = get_first_option(ctx, p, "system", "role");
	if (role && *role)
		json_object_object_add(ret, "role", gluonutil_wrap_string(role));

	return ret;
}

static struct json_object * respondd_provider_nodeinfo(void) {
	struct json_object *ret = json_object_new_object();

	struct uci_context *ctx = uci_alloc_context();
	if (!ctx)
		return ret;
	ctx->flags &= ~UCI_FLAG_STRICT;

	struct uci_package *p;
	if (!uci_load(ctx, "gluon-node-info", &p)) {
		struct json_object *location = get_location(ctx, p);
		if (location)
			json_object_object_add(ret, "location", location);

		struct json_object *owner = get_owner(ctx, p);
		if (owner)
			json_object_object_add(ret, "owner", owner);

		json_object_object_add(ret, "system", get_system(ctx, p));
	}

	uci_free_context(ctx);

	return ret;
}


const struct respondd_provider_info respondd_providers[] = {
	{"nodeinfo", respondd_provider_nodeinfo},
	{}
};
