#ifndef SOURCE_SHARED_SDK_H
#define SOURCE_SHARED_SDK_H

/*
 * This is the base SDK for all Source Engine games.
 * All the contents inside this directory should be backwards compatible to Source 2007.
 * Other games may need some changes and thus everything has to be defined in their respective directory.
 * For example, CSGO has some changes to the interfaces and structures, thus you would include source_csgo/sdk.h
 * instead of this one.
*/

#include "../framework/utils/memutils.h"
#include "../framework/g_defines.h"

#include "misc.h"
#include "types.h"
#include "enums.h"
#include "offsets.h"
#include "utlvector.h"
#include "utlmemory.h"
#include "recv.h"
#include "send_table.h"
#include "usercmd.h"
#include "../source_2007/icliententity.h"
#include "prediction.h"
#include "inetchannelinfo.h"
#include "globalvars.h"
#include "clientclass.h"
#include "serverclass.h"
#include "appsystem.h"
#include "iclientmode.h"
#include "baseclient.h"
#include "servergame.h"
#include "icliententitylist.h"
#include "basehandle.h"
#include "ehandle.h"
#include "varmapping.h"
#include "baseentity.h"
#include "interfaces.h"
#include "netvars.h"
#include "iconvar.h"
#include "convar.h"
#include "cvar.h"
#include "effects.h"
#include "effects_hook.h"
#include "color.h"
#include "surface.h"
#include "panel.h"
#include "viewrender.h"
#include "eventmanager.h"
#include "studio.h"
#include "mdl.h"
#include "mdlcache.h"
#include "ienginetrace.h"
#include "spatialpartition.h"
#include "staticpropmgr.h"
#include "modelloader.h"
#include "physicssurfaceprops.h"
#include "voxeltree.h"
#include "glow_object.h"
#include "glow_object_manager.h"
#include "host_state.h"

#endif
