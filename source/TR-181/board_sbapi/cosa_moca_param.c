/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#include <errno.h>
#include <string.h>
#include <msgpack.h>
#include <stdarg.h>
#include "cosa_moca_helpers.h"
#include "cosa_moca_param.h"
#include "ccsp_trace.h"
/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */
/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
enum {
    MO_OK                       = HELPERS_OK,
    MO_OUT_OF_MEMORY            = HELPERS_OUT_OF_MEMORY,
    MO_INVALID_FIRST_ELEMENT    = HELPERS_INVALID_FIRST_ELEMENT,
    MO_INVALID_OBJECT,
    MO_INVALID_VERSION,
};
/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* none */
/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
int process_mocaparams( mocaparam_t *e, msgpack_object_map *map );
int process_mocadoc( mocadoc_t *md, int num, ...); 
/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

mocadoc_t* mocadoc_convert( const void *buf, size_t len )
{
	return comp_helper_convert( buf, len, sizeof(mocadoc_t), "moca", 
                            MSGPACK_OBJECT_MAP, true,
                           (process_fn_t) process_mocadoc,
                           (destroy_fn_t) mocadoc_destroy );
}

void mocadoc_destroy( mocadoc_t *md )
{
	if( NULL != md )
	{
        if( NULL != md->param )
        {
            free( md->param );
        }

		if( NULL != md->subdoc_name )
		{
			free( md->subdoc_name );
		}
		free( md );
	}
}
/* See webcfgdoc.h for details. */
const char* mocadoc_strerror( int errnum )
{
    struct error_map {
        int v;
        const char *txt;
    } map[] = {
        { .v = MO_OK,                               .txt = "No errors." },
        { .v = MO_OUT_OF_MEMORY,                    .txt = "Out of memory." },
        { .v = MO_INVALID_FIRST_ELEMENT,            .txt = "Invalid first element." },
        { .v = MO_INVALID_VERSION,                 .txt = "Invalid 'version' value." },
        { .v = MO_INVALID_OBJECT,                .txt = "Invalid 'value' array." },
        { .v = 0, .txt = NULL }
    };
    int i = 0;
    while( (map[i].v != errnum) && (NULL != map[i].txt) ) { i++; }
    if( NULL == map[i].txt )
    {
	CcspTraceWarning(("----mocadoc_strerror----\n"));
        return "Unknown error.";
    }
    return map[i].txt;
}
/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
/**
 *  Convert the msgpack map into the doc_t structure.
 *
 *  @param e    the entry pointer
 *  @param map  the msgpack map pointer
 *
 *  @return 0 on success, error otherwise
 */
int process_mocaparams( mocaparam_t *e, msgpack_object_map *map )
{
    int left = map->size;
    uint8_t objects_left = 0x01;
    msgpack_object_kv *p;
    p = map->ptr;
    while( (0 < objects_left) && (0 < left--) )
    {
        if( MSGPACK_OBJECT_STR == p->key.type )
        {
              if( MSGPACK_OBJECT_BOOLEAN == p->val.type )
              {
                if( 0 == match(p, "Enable") )
                {
                    e->enable = p->val.via.boolean;
                    objects_left &= ~(1 << 0);
                }
            }
        }
           p++;
    }
        
    
    if( 1 & objects_left ) {
    } else {
        errno = MO_OK;
    }
   
    return (0 == objects_left) ? 0 : -1;
}
int process_mocadoc( mocadoc_t *md,int num, ... )
{
//To access the variable arguments use va_list 
	va_list valist;
	va_start(valist, num);//start of variable argument loop

	msgpack_object *obj = va_arg(valist, msgpack_object *);//each usage of va_arg fn argument iterates by one time
	msgpack_object_map *mapobj = &obj->via.map;

	msgpack_object *obj1 = va_arg(valist, msgpack_object *);
	md->subdoc_name = strndup( obj1->via.str.ptr, obj1->via.str.size );

	msgpack_object *obj2 = va_arg(valist, msgpack_object *);
	md->version = (uint32_t) obj2->via.u64;

	msgpack_object *obj3 = va_arg(valist, msgpack_object *);
	md->transaction_id = (uint16_t) obj3->via.u64;
	va_end(valist);//End of variable argument loop


	md->param = (mocaparam_t *) malloc( sizeof(mocaparam_t) );
        if( NULL == md->param )
        {
	    CcspTraceWarning(("entries count malloc failed\n"));
            return -1;
        }
        memset( md->param, 0, sizeof(mocaparam_t));


	if( 0 != process_mocaparams(md->param, mapobj) )
	{
		CcspTraceWarning(("process_portdocparams failed\n"));
		return -1;
	}

    return 0;
}

