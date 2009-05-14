/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2008, Nokia
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 * Authors:
 *  Philip Van Hoof <philip@codeminded.be>
 */

#include "config.h"

#include <glib-object.h>
#include <dbus/dbus-glib-bindings.h>

#include <libtracker-data/tracker-data-update.h>
#include <libtracker-data/tracker-data-manager.h>
#include <libtracker-data/tracker-data-query.h>

#include <tracker-store/tracker-push-registrar.h>

#define __TRACKER_RSS_REGISTRAR_C__

#include "tracker-rss-registrar.h"
#include "tracker-rss-registrar-glue.h"

#define TRACKER_RSS_REGISTRAR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TRACKER_TYPE_RSS_REGISTRAR, TrackerRssRegistrarPrivate))

#define TRACKER_TYPE_RSS_PUSH_REGISTRAR    (tracker_rss_push_registrar_get_type ())
#define TRACKER_RSS_PUSH_REGISTRAR(module) (G_TYPE_CHECK_INSTANCE_CAST ((module), TRACKER_TYPE_RSS_PUSH_REGISTRAR, TrackerRssPushRegistrar))

/* This is of course TODO (and to move to libtracker-common/tracker-ontology.h): */
#define TRACKER_RSS_PREFIX		       "http://www.tracker-project.org/temp/rss#"

#define METADATA_RSS			       TRACKER_RSS_PREFIX "RSS"
#define METADATA_RSSDATA_OBJECT		       TRACKER_RSS_PREFIX "RSSDataObject"

#define METADATA_RSS_SOMETHING		       TRACKER_RSS_PREFIX "something"

#define NIE_DATASOURCE 			       TRACKER_NIE_PREFIX "DataSource"
#define NIE_DATASOURCE_P 		       TRACKER_NIE_PREFIX "dataSource"

#define RDF_TYPE			       TRACKER_RDF_PREFIX "type"

#define NAO_TAG				       TRACKER_NAO_PREFIX "Tag"
#define NAO_PREFLABEL			       TRACKER_NAO_PREFIX "prefLabel"

#define DATASOURCE_URN			       "urn:nepomuk:datasource:670e2cd0-1241-11de-8c30-0800200c9a66"

typedef struct TrackerRssPushRegistrar TrackerRssPushRegistrar;
typedef struct TrackerRssPushRegistrarClass TrackerRssPushRegistrarClass;

struct TrackerRssPushRegistrar {
	TrackerPushRegistrar parent_instance;
};

struct TrackerRssPushRegistrarClass {
	TrackerPushRegistrarClass parent_class;
};


typedef struct {
	gpointer dummy;
} TrackerRssRegistrarPrivate;

enum {
	PROP_0,
};

static GType tracker_rss_push_registrar_get_type (void) G_GNUC_CONST;

G_DEFINE_TYPE (TrackerRssRegistrar, tracker_rss_registrar, G_TYPE_OBJECT)
G_DEFINE_TYPE (TrackerRssPushRegistrar, tracker_rss_push_registrar, TRACKER_TYPE_PUSH_REGISTRAR);

static void
tracker_rss_registrar_finalize (GObject *object)
{
	G_OBJECT_CLASS (tracker_rss_registrar_parent_class)->finalize (object);
}

static void
tracker_rss_registrar_set_property (GObject      *object,
					  guint         prop_id,
					  const GValue *value,
					  GParamSpec   *pspec)
{
	switch (prop_id) {
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
tracker_rss_registrar_get_property (GObject    *object,
					  guint       prop_id,
					  GValue     *value,
					  GParamSpec *pspec)
{
	switch (prop_id) {
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
tracker_rss_registrar_class_init (TrackerRssRegistrarClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = tracker_rss_registrar_finalize;
	object_class->set_property = tracker_rss_registrar_set_property;
	object_class->get_property = tracker_rss_registrar_get_property;

	g_type_class_add_private (object_class, sizeof (TrackerRssRegistrarPrivate));
}

static void
tracker_rss_registrar_init (TrackerRssRegistrar *object)
{
}


static void
perform_set (TrackerRssRegistrar *object, 
	     const gchar *subject, 
	     const GStrv predicates, 
	     const GStrv values)
{
	guint i = 0;

	/* TODO */
	return;

	if (!tracker_data_query_resource_exists (DATASOURCE_URN, NULL, NULL)) {
		tracker_data_insert_statement (DATASOURCE_URN, RDF_TYPE,
					       NIE_DATASOURCE);
	}

	tracker_data_insert_statement (subject, RDF_TYPE,
		                       METADATA_RSS);

	tracker_data_insert_statement (subject, RDF_TYPE,
		                       METADATA_RSSDATA_OBJECT);

	tracker_data_insert_statement (subject, NIE_DATASOURCE_P,
		                       DATASOURCE_URN);

	while (predicates [i] != NULL && values[i] != NULL) {

		if (g_strcmp0 (predicates[i], TRACKER_RSS_PREDICATE_THING) == 0) {
			tracker_data_insert_statement (subject,
						       METADATA_RSS_SOMETHING, 
						       values[i]);
		}

		i++;
	}
}

static void 
perform_unset (TrackerRssRegistrar *object, 
	       const gchar *subject)
{
	/* TODO */
	return;

	tracker_data_delete_resource (subject); 
}

static void
perform_cleanup (TrackerRssRegistrar *object)
{
	GError *error = NULL;

	/* TODO */
	return;

	tracker_data_update_sparql ("DELETE { ?s ?p ?o } WHERE { ?s nie:dataSource <" DATASOURCE_URN "> }", &error);

	if (error) {
		g_warning ("%s", error->message);
		g_error_free (error);
	}
}

static void
set_stored_last_modseq (guint last_modseq)
{
	tracker_data_manager_set_db_option_int ("RssLastModseq", (gint) last_modseq);
}


void
tracker_rss_registrar_set (TrackerRssRegistrar *object, 
			   const gchar *subject, 
			   const GStrv predicates,
			   const GStrv values,
			   const guint modseq,
			   DBusGMethodInvocation *context,
			   GError *derror)
{
	dbus_async_return_if_fail (subject != NULL, context);

	if (predicates && values) {

		dbus_async_return_if_fail (g_strv_length (predicates) == 
					   g_strv_length (values), context);

		perform_set (object, subject, predicates, values);
	}

	set_stored_last_modseq (modseq);

	dbus_g_method_return (context);
}

void
tracker_rss_registrar_set_many (TrackerRssRegistrar *object, 
				const GStrv subjects, 
				const GPtrArray *predicates,
				const GPtrArray *values,
				const guint modseq,
				DBusGMethodInvocation *context,
				GError *derror)
{
	guint len;
	guint i = 0;

	dbus_async_return_if_fail (subjects != NULL, context);
	dbus_async_return_if_fail (predicates != NULL, context);
	dbus_async_return_if_fail (values != NULL, context);

	len = g_strv_length (subjects);

	dbus_async_return_if_fail (len == predicates->len, context);
	dbus_async_return_if_fail (len == values->len, context);

	while (subjects[i] != NULL) {
		GStrv preds = g_ptr_array_index (predicates, i);
		GStrv vals = g_ptr_array_index (values, i);

		perform_set (object, subjects[i], preds, vals);

		i++;
	}

	set_stored_last_modseq (modseq);

	dbus_g_method_return (context);
}

void
tracker_rss_registrar_unset_many (TrackerRssRegistrar *object, 
				  const GStrv subjects, 
				  const guint modseq,
				  DBusGMethodInvocation *context,
				  GError *derror)
{
	guint i = 0;

	dbus_async_return_if_fail (subjects != NULL, context);

	while (subjects[i] != NULL) {

		perform_unset (object, subjects[i]);

		i++;
	}

	set_stored_last_modseq (modseq);

	dbus_g_method_return (context);
}

void
tracker_rss_registrar_unset (TrackerRssRegistrar *object, 
				   const gchar *subject, 
				   const guint modseq,
				   DBusGMethodInvocation *context,
				   GError *derror)
{
	dbus_async_return_if_fail (subject != NULL, context);

	perform_unset (object, subject);

	dbus_g_method_return (context);
}

void
tracker_rss_registrar_cleanup (TrackerRssRegistrar *object, 
				     const guint modseq,
				     DBusGMethodInvocation *context,
				     GError *derror)
{
	perform_cleanup (object);

	set_stored_last_modseq (modseq);

	dbus_g_method_return (context);
}


static void
on_manager_destroy (DBusGProxy *proxy, gpointer user_data)
{
	return;
}

static void
tracker_rss_push_registrar_enable (TrackerPushRegistrar *registrar, 
					 DBusGConnection      *connection,
					 DBusGProxy           *dbus_proxy, 
					 GError              **error)
{
	GError *nerror = NULL;
	guint result;
	DBusGProxy *manager_proxy;
	GObject *object;

	tracker_push_registrar_set_object (registrar, NULL);
	tracker_push_registrar_set_manager (registrar, NULL);

	manager_proxy = dbus_g_proxy_new_for_name (connection,
						   TRACKER_RSS_MANAGER_SERVICE,
						   TRACKER_RSS_MANAGER_PATH,
						   TRACKER_RSS_MANAGER_INTERFACE);

	/* Creation of the registrar */
	if (!org_freedesktop_DBus_request_name (dbus_proxy, 
						TRACKER_RSS_REGISTRAR_SERVICE,
						DBUS_NAME_FLAG_DO_NOT_QUEUE,
						&result, &nerror)) {

		g_critical ("Could not setup DBus, %s in use\n", 
			    TRACKER_RSS_REGISTRAR_SERVICE);

		if (nerror) {
			g_propagate_error (error, nerror);
			return;
		}
	}

	if (nerror) {
		g_propagate_error (error, nerror);
		return;
	}

	object = g_object_new (TRACKER_TYPE_RSS_REGISTRAR, NULL);

	dbus_g_object_type_install_info (G_OBJECT_TYPE (object), 
					 &dbus_glib_tracker_rss_registrar_object_info);

	dbus_g_connection_register_g_object (connection, 
					     TRACKER_RSS_REGISTRAR_PATH, 
					     object);

	/* Registration of the registrar to the manager */
	dbus_g_proxy_call_no_reply (manager_proxy, "Register",
				    G_TYPE_OBJECT, object, 
				    G_TYPE_UINT, (guint) tracker_data_manager_get_db_option_int ("RssLastModseq"),
				    G_TYPE_INVALID,
				    G_TYPE_INVALID);

	/* If while we had a proxy for the manager the manager shut itself down,
	 * then we'll get rid of our registrar too, in on_manager_destroy */

	g_signal_connect (manager_proxy, "destroy",
			  G_CALLBACK (on_manager_destroy), registrar);

	tracker_push_registrar_set_object (registrar, object);
	tracker_push_registrar_set_manager (registrar, manager_proxy);

	g_object_unref (object); /* sink own */
	g_object_unref (manager_proxy);  /* sink own */
}

static void
tracker_rss_push_registrar_disable (TrackerPushRegistrar *registrar)
{
	tracker_push_registrar_set_object (registrar, NULL);
	tracker_push_registrar_set_manager (registrar, NULL);
}

static void
tracker_rss_push_registrar_class_init (TrackerRssPushRegistrarClass *klass)
{
	TrackerPushRegistrarClass *p_class = TRACKER_PUSH_REGISTRAR_CLASS (klass);

	p_class->enable = tracker_rss_push_registrar_enable;
	p_class->disable = tracker_rss_push_registrar_disable;
}

static void
tracker_rss_push_registrar_init (TrackerRssPushRegistrar *registrar)
{
	return;
}

TrackerPushRegistrar *
tracker_push_module_init (void)
{
	GObject *object;

	object = g_object_new (TRACKER_TYPE_RSS_PUSH_REGISTRAR, NULL);

	tracker_push_registrar_set_service (TRACKER_PUSH_REGISTRAR (object),
					    TRACKER_RSS_MANAGER_SERVICE);

	return TRACKER_PUSH_REGISTRAR (object);
}

void
tracker_push_module_shutdown (TrackerPushRegistrar *registrar)
{
	tracker_rss_push_registrar_disable (registrar);
}
