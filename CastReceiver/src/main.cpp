/* main.c - Main program routines
 *
 * Copyright(C) 2005-2007   Ivo Clarysse
 *

 * This file is part of GMediaRender.
 *
 * GMediaRender is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License,or
 *(at your option) any later version.
 *
 * GMediaRender is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GMediaRender; if not,write to the Free Software
 * Foundation,Inc.,51 Franklin Street,Fifth Floor,Boston,
 * MA 02110-1301,USA.
 *
 */

/*
 *
 * ©K. D. Hedger. Thu  8 Aug 16:14:59 BST 2019 keithdhedger@gmail.com

 * This file (main.c) is part of gmrender-resurrect.

 * gmrender-resurrect is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * gmrender-resurrect is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with gmrender-resurrect.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "common.h"

// IP-address seems strange in libupnp: they actually don't bind to
// that address,but to INADDR_ANY(miniserver.c in upnp library).
// Apparently they just use this for the advertisement ? Anyway,0.0.0.0 would
// not work.
const gchar			*ipAddress=NULL;
int					listenPort=49494;
struct upnp_device	*device;
bool				showVersion=false;
bool				startUpTalk=false;
const gchar			*friendlyName=PACKAGE_NAME;

const gchar			*uuid=GMRENDER_UUID;

/* Generic CastReceiver options */
GOptionEntry		optionEntries[]=
{
	{"version",0,0,G_OPTION_ARG_NONE,&showVersion,"Output version information and exit",NULL},
	{"friendly-name",'f',0,G_OPTION_ARG_STRING,&friendlyName,"Friendly name to advertise.",NULL},
	{"exit-on-stop",'e',0,G_OPTION_ARG_NONE,&exitOnStop,"Exit when file stopped.",NULL},
	{"verbose",'v',0,G_OPTION_ARG_NONE,&startUpTalk,"Show renderer details on start.",NULL},
	{NULL}
};

void do_showVersion(void)
{
	fprintf(stderr,"%s - %s\n",PACKAGE_NAME,PACKAGE_VERSION);
}

gboolean process_cmdline(int argc,char **argv)
{
	GOptionContext	*ctx;
	GError			*err=NULL;
	int				rc;

	ctx=g_option_context_new("- CastReceiver");
	g_option_context_add_main_entries(ctx,optionEntries,NULL);

	if(!g_option_context_parse(ctx,&argc,&argv,&err))
		{
			fprintf(stderr,"Failed to initialize: %s\n",err->message);
			g_error_free(err);
			return(false);
		}

	return(true);
}

int main(int argc,char **argv)
{
	struct upnp_device_descriptor	*upnp_renderer;
	if(!process_cmdline(argc,argv))
		return(EXIT_FAILURE);

	if(showVersion)
		{
			do_showVersion();
			exit(EXIT_SUCCESS);
		}

	upnp_renderer=upnp_renderer_descriptor(friendlyName,uuid);
	if(upnp_renderer==NULL)
		return(EXIT_FAILURE);

	device=upnp_device_init(upnp_renderer,ipAddress,listenPort);
	if(device==NULL)
		{
			fprintf(stderr,"ERROR: Failed to initialize UPnP device");
			return(EXIT_FAILURE);
		}

	if(startUpTalk==true)
		{
			fprintf(stderr,"Using UUID %s\n",uuid);
			fprintf(stderr,"\"%s\" Ready for rendering ...\n",friendlyName);
		}

	output_loop();

	// We're here,because the loop exited. Probably due to catching
	// a signal.
	upnp_device_shutdown(device);

	return(EXIT_SUCCESS);
}
