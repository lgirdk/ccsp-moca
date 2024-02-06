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

/**********************************************************************
   Copyright [2014] [Cisco Systems, Inc.]
 
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
 
       http://www.apache.org/licenses/LICENSE-2.0
 
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
**********************************************************************/


/**************************************************************************

    module: cosa_moca_dml.c

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    description:

        This file implementes back-end apis for the COSA Data Model Library

    -------------------------------------------------------------------

    environment:

        platform independent

    -------------------------------------------------------------------

    author:

        COSA XML TOOL CODE GENERATOR 1.0

    -------------------------------------------------------------------

    revision:

        01/17/2011    initial revision.

**************************************************************************/

#include "cosa_moca_dml.h"
#include "dml_tr181_custom_cfg.h"
#include "cosa_moca_network_info.h"
#include "safec_lib_common.h"
#include "syscfg/syscfg.h"
#include "cosa_moca_apis.h"

#ifdef CONFIG_SYSTEM_MOCA

extern ANSC_HANDLE g_MoCAObject ;

#if 0
#ifdef AnscTraceWarning
#undef AnscTraceWarning
#define AnscTraceWarning(a) printf("%s:%d> ", __FUNCTION__, __LINE__); printf a
#endif
#endif

/***********************************************************************
 IMPORTANT NOTE:

 According to TR69 spec:
 On successful receipt of a SetParameterValues RPC, the CPE MUST apply 
 the changes to all of the specified Parameters atomically. That is, either 
 all of the value changes are applied together, or none of the changes are 
 applied at all. In the latter case, the CPE MUST return a fault response 
 indicating the reason for the failure to apply the changes. 
 
 The CPE MUST NOT apply any of the specified changes without applying all 
 of them.

 In order to set parameter values correctly, the back-end is required to
 hold the updated values until "Validate" and "Commit" are called. Only after
 all the "Validate" passed in different objects, the "Commit" will be called.
 Otherwise, "Rollback" will be called instead.

 The sequence in COSA Data Model will be:

 SetParamBoolValue/SetParamIntValue/SetParamUlongValue/SetParamStringValue
 -- Backup the updated values;

 if( Validate_XXX())
 {
     Commit_XXX();    -- Commit the update all together in the same object
 }
 else
 {
     Rollback_XXX();  -- Remove the update at backup;
 }
 
***********************************************************************/

ANSC_STATUS
CosaSListPushEntryByInsNum
    (
        PSLIST_HEADER               pListHead,
        PCOSA_CONTEXT_LINK_OBJECT   pCosaContext
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContextEntry = (PCOSA_CONTEXT_LINK_OBJECT)NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry       = (PSINGLE_LINK_ENTRY       )NULL;
    ULONG                           ulIndex           = 0;

    if ( pListHead->Depth == 0 )
    {
        AnscSListPushEntryAtBack(pListHead, &pCosaContext->Linkage);
    }
    else
    {
        pSLinkEntry = AnscSListGetFirstEntry(pListHead);

        for ( ulIndex = 0; ulIndex < pListHead->Depth; ulIndex++ )
        {
            pCosaContextEntry = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);
            pSLinkEntry       = AnscSListGetNextEntry(pSLinkEntry);

            if ( pCosaContext->InstanceNumber < pCosaContextEntry->InstanceNumber )
            {
                AnscSListPushEntryByIndex(pListHead, &pCosaContext->Linkage, ulIndex);

                return ANSC_STATUS_SUCCESS;
            }
        }

        AnscSListPushEntryAtBack(pListHead, &pCosaContext->Linkage);
    }

    return ANSC_STATUS_SUCCESS;
}

/***********************************************************************

 APIs for Object:

    MoCA.

    *  MoCA_GetParamBoolValue
    *  MoCA_GetParamIntValue
    *  MoCA_GetParamUlongValue
    *  MoCA_GetParamStringValue
    *  MoCA_SetParamBoolValue
    *  MoCA_SetParamIntValue
    *  MoCA_SetParamUlongValue
    *  MoCA_SetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        MoCA_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
MoCA_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_MOCA            pMyObject     = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_CFG              pCfg          = &pMyObject->MoCACfg;
    errno_t                         rc       = -1;
    int                             ind      = -1;
    
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */

    rc = strcmp_s("X_RDKCENTRAL-COM_MoCAHost_Sync", strlen("X_RDKCENTRAL-COM_MoCAHost_Sync"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
    
        *pBool = FALSE;
        return TRUE;
    }

    rc = strcmp_s("X_RDKCENTRAL-COM_ForceEnable", strlen("X_RDKCENTRAL-COM_ForceEnable"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        *pBool = pCfg->bForceEnabled;
        return TRUE;
    }

    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        MoCA_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
MoCA_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        MoCA_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
MoCA_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_MOCA            pMyObject     = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_CFG              pCfg          = &pMyObject->MoCACfg;
    errno_t                         rc       = -1;
    int                             ind      = -1;
    
    /* check the parameter name and return the corresponding value */
    rc = strcmp_s("X_CISCO_COM_ProvisioningServerAddressType", strlen("X_CISCO_COM_ProvisioningServerAddressType"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))    
    {
        /* collect value */
        *puLong = pCfg->X_CISCO_COM_ProvisioningServerAddressType;
        return TRUE;
    }
    rc = strcmp_s("MocaResetCount", strlen("MocaResetCount"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
	{
		if (CosaDmlMocaGetResetCount(NULL,puLong) != ANSC_STATUS_SUCCESS)
			return FALSE;

		return TRUE;
	}


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        MoCA_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
MoCA_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_MOCA            pMyObject     = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_CFG              pCfg          = &pMyObject->MoCACfg;
    errno_t                         rc       = -1;
    int                             ind      = -1;

    /* check the parameter name and return the corresponding value */
    rc = strcmp_s("X_CISCO_COM_ProvisioningFilename", strlen("X_CISCO_COM_ProvisioningFilename"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        rc = strcpy_s(pValue, *pUlSize, (char *)pCfg->X_CISCO_COM_ProvisioningFilename);
        if (rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
	*pUlSize = AnscSizeOfString(pValue);
        return 0;
    }

    rc = strcmp_s("X_CISCO_COM_ProvisioningServerAddress", strlen("X_CISCO_COM_ProvisioningServerAddress"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        rc = strcpy_s(pValue, *pUlSize, (char *)pCfg->X_CISCO_COM_ProvisioningServerAddress);
        if (rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
	*pUlSize = AnscSizeOfString(pValue);
        return 0;
    }

    rc = strcmp_s("Data", strlen("Data"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        // To Do
        /* collect value */
       CcspTraceWarning(("Data Get Not supported\n"));
        return 0;
    }
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}


/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        MoCA_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
MoCA_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_MOCA            pMyObject     = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    errno_t                         rc       = -1;
    int                             ind      = -1;

    /* check the parameter name and set the corresponding value */
    rc = strcmp_s("X_RDKCENTRAL-COM_MoCAHost_Sync", strlen("X_RDKCENTRAL-COM_MoCAHost_Sync"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        
        Send_Update_to_LMLite(TRUE);
        return TRUE;
    }
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    rc = strcmp_s("X_RDKCENTRAL-COM_ForceEnable", strlen("X_RDKCENTRAL-COM_ForceEnable"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
		if(MoCA_SetForceEnable(&pMyObject->MoCAIfFullTable[0].MoCAIfFull.Cfg, &pMyObject->MoCACfg, bValue)){
			return TRUE;
		}
    }
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        MoCA_SetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int                         iValue
            );

    description:

        This function is called to set integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int                         iValue
                The updated integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
MoCA_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(iValue);
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        MoCA_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG                       uValue
            );

    description:

        This function is called to set ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG                       uValue
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
MoCA_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{    
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_MOCA            pMyObject     = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_CFG              pCfg          = &pMyObject->MoCACfg;
    errno_t                         rc       = -1;
    int                             ind      = -1;
    
    /* check the parameter name and set the corresponding value */
    rc = strcmp_s("X_CISCO_COM_ProvisioningServerAddressType", strlen("X_CISCO_COM_ProvisioningServerAddressType"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* save update to backup */
        pCfg->X_CISCO_COM_ProvisioningServerAddressType = uValue;
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        MoCA_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pString
            );

    description:

        This function is called to set string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pString
                The updated string value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
MoCA_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_MOCA            pMyObject     = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_CFG              pCfg          = &pMyObject->MoCACfg;
    errno_t                         rc       = -1;
    int                             ind      = -1;
    
    /* check the parameter name and set the corresponding value */
    rc = strcmp_s("X_CISCO_COM_ProvisioningFilename", strlen("X_CISCO_COM_ProvisioningFilename"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* save update to backup */
	rc = strcpy_s((PCHAR)pCfg->X_CISCO_COM_ProvisioningFilename, sizeof(pCfg->X_CISCO_COM_ProvisioningFilename), pString);
        if (rc != EOK)
        {
            ERR_CHK(rc);
            return FALSE;
        }
        return TRUE;
    }

    rc = strcmp_s("X_CISCO_COM_ProvisioningServerAddress", strlen("X_CISCO_COM_ProvisioningServerAddress"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* save update to backup */
        rc = strcpy_s((PCHAR)pCfg->X_CISCO_COM_ProvisioningServerAddress, sizeof(pCfg->X_CISCO_COM_ProvisioningServerAddress), pString);
        if (rc != EOK)
        {
            ERR_CHK(rc);
            return FALSE;
        }
        return TRUE;
    }
    rc = strcmp_s("Data", strlen("Data"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
 	CcspTraceWarning(("Data received from webconfig\n"));
        if( CosaDmlmocaUnpack( pString))
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }  
    }
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        MoCA_Validate
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pReturnParamName,
                ULONG*                      puLength
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pReturnParamName,
                The buffer (128 bytes) of parameter name if there's a validation. 

                ULONG*                      puLength
                The output length of the param name. 

    return:     TRUE if there's no validation.

**********************************************************************/
BOOL
MoCA_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(pReturnParamName);
    UNREFERENCED_PARAMETER(puLength);

    /* We should add some validation code here*/

    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        MoCA_Commit
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
MoCA_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_MOCA            pMyObject     = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_CFG              pCfg          = &pMyObject->MoCACfg;
    
    CosaDmlMocaSetCfg((ANSC_HANDLE)NULL, pCfg);
    
    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        MoCA_Rollback
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to roll back the update whenever there's a 
        validation found.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
MoCA_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_MOCA            pMyObject     = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_CFG              pCfg          = &pMyObject->MoCACfg;

    CosaDmlMocaGetCfg((ANSC_HANDLE)NULL, pCfg);
    
    return 0;
}

/**********************************************************************
    caller:     owner of this object
    prototype:
        BOOL
        MoCAReset_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );
    description:
        This function is called to retrieve Boolean parameter value;
    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;
                char*                       ParamName,
                The parameter name;
                BOOL*                       pBool
                The buffer of returned boolean value;
    return:     TRUE if succeeded.
**********************************************************************/
BOOL
MoCAReset_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    errno_t                         rc       = -1;
    int                             ind      = -1;
    
    CcspTraceWarning(("ParamName: %s pBool: %p\n", ParamName, pBool));
    rc = strcmp_s("Reset", strlen("Reset"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        *pBool = FALSE;
        return TRUE;
    }
    return FALSE;
}



/**********************************************************************
    caller:     owner of this object
    prototype:
        BOOL
        MoCAReset_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );
    description:
        This function is called to set BOOL parameter value;
    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;
                char*                       ParamName,
                The parameter name;
                BOOL                        bValue
                The updated BOOL value;
    return:     TRUE if succeeded.
**********************************************************************/
BOOL
MoCAReset_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    pthread_t tid ;
    int res;
    errno_t                         rc       = -1;
    int                             ind      = -1;
    
    CcspTraceWarning(("MoCAReset_SetParamBoolValue () ParamName: %s bValue: %d\n", ParamName, bValue));

    rc = strcmp_s("Reset", strlen("Reset"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
	{
		CcspTraceWarning(("Starts MoCA_Interface_Reset -- CosaDmlMocaIfReset() call\n"));
		res = pthread_create ( &tid, NULL, &MoCA_Interface_Reset, NULL );
		if(res != 0)
		{
			CcspTraceWarning(("Creating MoCA_Interface_Reset Thread error %d\n", res));
		}
		else
		{
			CcspTraceWarning(("MoCA Interface Reset Thread Created\n"));
		}
		return TRUE;
	}
    return FALSE;
}

/***********************************************************************

 APIs for Object:

    MoCA.Interface.{i}.

    *  Interface1_GetEntryCount
    *  Interface1_GetEntry
    *  Interface1_GetParamBoolValue
    *  Interface1_GetParamIntValue
    *  Interface1_GetParamUlongValue
    *  Interface1_GetParamStringValue
    *  Interface1_SetParamBoolValue
    *  Interface1_SetParamIntValue
    *  Interface1_SetParamUlongValue
    *  Interface1_SetParamStringValue
    *  Interface1_Validate
    *  Interface1_Commit
    *  Interface1_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Interface1_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
Interface1_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    
    return CosaDmlMocaGetNumberOfIfs((ANSC_HANDLE)NULL);
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        Interface1_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
Interface1_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_MOCA            pMyObject     = (PCOSA_DATAMODEL_MOCA)g_MoCAObject;

    *pInsNumber  = pMyObject->MoCAIfFullTable[nIndex].MoCAIfFull.Cfg.InstanceNumber;

    return &pMyObject->MoCAIfFullTable[nIndex];
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Interface1_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Interface1_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    errno_t                         rc       = -1;
    int                             ind      = -1;
    PCOSA_DML_MOCA_IF_FULL          pMoCAIfFull = &((PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext)->MoCAIfFull;
    
#if defined (_XB6_PRODUCT_REQ_)
    //ARRISXB6-3829: RDKB DB Not Syncing if set through SNMP
    CosaDmlMocaIfGetCfg(NULL, pMoCAIfFull->Cfg.InstanceNumber-1, &pMoCAIfFull->Cfg);
#endif

    CosaDmlMocaIfGetDinfo(NULL, pMoCAIfFull->Cfg.InstanceNumber-1, &pMoCAIfFull->DynamicInfo);

    /* check the parameter name and return the corresponding value */
    rc = strcmp_s("Enable", strlen("Enable"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *pBool = pMoCAIfFull->Cfg.bEnabled;
        return TRUE;
    }

    rc = strcmp_s("Upstream", strlen("Upstream"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *pBool = FALSE ; /*For an Internet Gateway Device, Upstream will be false for all LAN interfaces*/
        return TRUE;
    }

    rc = strcmp_s("MaxNodes", strlen("MaxNodes"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *pBool = pMoCAIfFull->DynamicInfo.MaxNodes;
        return TRUE;
    }

    rc = strcmp_s("PreferredNC", strlen("PreferredNC"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *pBool = pMoCAIfFull->Cfg.bPreferredNC;
        return TRUE;
    }

    rc = strcmp_s("PrivacyEnabledSetting", strlen("PrivacyEnabledSetting"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *pBool = pMoCAIfFull->Cfg.PrivacyEnabledSetting;
        return TRUE;
    }

    rc = strcmp_s("PrivacyEnabled", strlen("PrivacyEnabled"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *pBool = pMoCAIfFull->DynamicInfo.PrivacyEnabled;
        return TRUE;
    }

    rc = strcmp_s("QAM256Capable", strlen("QAM256Capable"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *pBool = pMoCAIfFull->StaticInfo.QAM256Capable;
        return TRUE;
    }

    rc = strcmp_s("X_CISCO_COM_Reset", strlen("X_CISCO_COM_Reset"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *pBool = pMoCAIfFull->Cfg.X_CISCO_COM_Reset;
        return TRUE;
    }

    rc = strcmp_s("X_CISCO_COM_MixedMode", strlen("X_CISCO_COM_MixedMode"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *pBool = pMoCAIfFull->Cfg.X_CISCO_COM_MixedMode;
        return TRUE;
    }

    rc = strcmp_s("X_CISCO_COM_ChannelScanning", strlen("X_CISCO_COM_ChannelScanning"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *pBool = pMoCAIfFull->Cfg.X_CISCO_COM_ChannelScanning;
        return TRUE;
    }

    rc = strcmp_s("X_CISCO_COM_AutoPowerControlEnable", strlen("X_CISCO_COM_AutoPowerControlEnable"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *pBool = pMoCAIfFull->Cfg.X_CISCO_COM_AutoPowerControlEnable;
        return TRUE;
    }

    rc = strcmp_s("X_CISCO_COM_EnableTabooBit", strlen("X_CISCO_COM_EnableTabooBit"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *pBool = pMoCAIfFull->Cfg.X_CISCO_COM_EnableTabooBit;
        return TRUE;
    }

    rc = strcmp_s("X_CISCO_COM_CycleMaster", strlen("X_CISCO_COM_CycleMaster"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *pBool = pMoCAIfFull->StaticInfo.X_CISCO_COM_CycleMaster;
        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Interface1_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Interface1_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    errno_t                         rc       = -1;
	int                             ind      = -1;
    PCOSA_DML_MOCA_IF_FULL          pMoCAIfFull = &((PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext)->MoCAIfFull;

    /* check the parameter name and return the corresponding value */

    AnscTraceWarning(("ParamName: %s: pMoCAIfFull->Cfg.TxPowerLimit: %d\n", ParamName, pMoCAIfFull->Cfg.TxPowerLimit));

    rc = strcmp_s("TxPowerLimit", strlen("TxPowerLimit"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *pInt = pMoCAIfFull->Cfg.TxPowerLimit;
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Interface1_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Interface1_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{    
    errno_t                         rc       = -1;
    int                             ind      = -1;
    PCOSA_DML_MOCA_IF_FULL          pMoCAIfFull = &((PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext)->MoCAIfFull;
    
    AnscTraceWarning(("ParamName: %s\n", ParamName));
#if defined (_XB6_PRODUCT_REQ_)
    //ARRISXB6-3829: RDKB DB Not Syncing if set through SNMP
    CosaDmlMocaIfGetCfg(NULL, pMoCAIfFull->Cfg.InstanceNumber-1, &pMoCAIfFull->Cfg);
#endif

    CosaDmlMocaIfGetDinfo(NULL, pMoCAIfFull->Cfg.InstanceNumber-1, &pMoCAIfFull->DynamicInfo);


    /* check the parameter name and return the corresponding value */
    rc = strcmp_s("Status", strlen("Status"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAIfFull->DynamicInfo.Status;
        return TRUE;
    }

    rc = strcmp_s("LastChange", strlen("LastChange"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAIfFull->DynamicInfo.LastChange;
        return TRUE;
    }

    rc = strcmp_s("MaxBitRate", strlen("MaxBitRate"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAIfFull->StaticInfo.MaxBitRate;
        return TRUE;
    }

    rc = strcmp_s("MaxIngressBW", strlen("MaxIngressBW"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAIfFull->DynamicInfo.MaxIngressBW;
        return TRUE;
    }

    rc = strcmp_s("MaxEgressBW", strlen("MaxEgressBW"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAIfFull->DynamicInfo.MaxEgressBW;
        return TRUE;
    }

    rc = strcmp_s("NetworkCoordinator", strlen("NetworkCoordinator"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAIfFull->DynamicInfo.NetworkCoordinator;
        return TRUE;
    }

    rc = strcmp_s("NodeID", strlen("NodeID"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAIfFull->DynamicInfo.NodeID;
        return TRUE;
    }

    rc = strcmp_s("BackupNC", strlen("BackupNC"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAIfFull->DynamicInfo.BackupNC;
        return TRUE;
    }

    rc = strcmp_s("CurrentOperFreq", strlen("CurrentOperFreq"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        *puLong = pMoCAIfFull->DynamicInfo.CurrentOperFreq;
        return TRUE;
    }

    rc = strcmp_s("LastOperFreq", strlen("LastOperFreq"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAIfFull->DynamicInfo.LastOperFreq;
        return TRUE;
    }

    rc = strcmp_s("PowerCntlPhyTarget", strlen("PowerCntlPhyTarget"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAIfFull->Cfg.PowerCntlPhyTarget;
        return TRUE;
    }

    rc = strcmp_s("BeaconPowerLimit", strlen("BeaconPowerLimit"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        AnscTraceWarning(("pMoCAIfFull->Cfg.BeaconPowerLimit: %lu\n", pMoCAIfFull->Cfg.BeaconPowerLimit));

        *puLong = pMoCAIfFull->Cfg.BeaconPowerLimit;
        return TRUE;
    }

    rc = strcmp_s("TxBcastRate", strlen("TxBcastRate"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAIfFull->DynamicInfo.TxBcastRate;
        return TRUE;
    }

    rc = strcmp_s("TxBcastPowerReduction", strlen("TxBcastPowerReduction"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAIfFull->StaticInfo.TxBcastPowerReduction;
        return TRUE;
    }

    rc = strcmp_s("PacketAggregationCapability", strlen("PacketAggregationCapability"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAIfFull->StaticInfo.PacketAggregationCapability;
        return TRUE;
    }

    rc = strcmp_s("AutoPowerControlPhyRate", strlen("AutoPowerControlPhyRate"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */

        AnscTraceWarning(("ParamName: %s: pMoCAIfFull->Cfg.AutoPowerControlPhyRate: %lu\n", ParamName, 
                          pMoCAIfFull->Cfg.AutoPowerControlPhyRate));
        *puLong = pMoCAIfFull->Cfg.AutoPowerControlPhyRate;
        return TRUE;
    }

    rc = strcmp_s("X_CISCO_COM_BestNetworkCoordinatorID", strlen("X_CISCO_COM_BestNetworkCoordinatorID"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAIfFull->DynamicInfo.X_CISCO_COM_BestNetworkCoordinatorID;
        return TRUE;
    }

    rc = strcmp_s("X_CISCO_COM_NumberOfConnectedClients", strlen("X_CISCO_COM_NumberOfConnectedClients"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAIfFull->DynamicInfo.X_CISCO_COM_NumberOfConnectedClients;
        return TRUE;
    }
#ifdef MOCA_DIAGONISTIC
    rc = strcmp_s("LinkUpTime", strlen("LinkUpTime"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAIfFull->DynamicInfo.LinkUpTime;
        return TRUE;
    }
#endif

    /* check the parameter name and set the corresponding value */
    rc = strcmp_s("TxPowerLimit", strlen("TxPowerLimit"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        AnscTraceWarning(("ParamName: %s: pMoCAIfFull->Cfg.TxPowerLimit: %d\n", ParamName, pMoCAIfFull->Cfg.TxPowerLimit));
        *puLong = pMoCAIfFull->Cfg.TxPowerLimit;
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Interface1_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
Interface1_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{    
    errno_t                         rc       = -1;
    int                             ind      = -1;
    PCOSA_DML_MOCA_IF_FULL          pMoCAIfFull = &((PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext)->MoCAIfFull;
        
    AnscTraceWarning(("ParamName: %s\n", ParamName));
#if defined (_XB6_PRODUCT_REQ_)
    //ARRISXB6-3829: RDKB DB Not Syncing if set through SNMP
    CosaDmlMocaIfGetCfg(NULL, pMoCAIfFull->Cfg.InstanceNumber-1, &pMoCAIfFull->Cfg);
#endif

    CosaDmlMocaIfGetDinfo(NULL, pMoCAIfFull->Cfg.InstanceNumber-1, &pMoCAIfFull->DynamicInfo);


    /* check the parameter name and return the corresponding value */
    rc = strcmp_s("Alias", strlen("Alias"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        rc = strcpy_s(pValue, *pUlSize, pMoCAIfFull->Cfg.Alias);
        if (rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
        *pUlSize = AnscSizeOfString(pValue);
        return 0;
    }

    rc = strcmp_s("Name", strlen("Name"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        rc = strcpy_s(pValue, *pUlSize, pMoCAIfFull->StaticInfo.Name);
        if (rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
        *pUlSize = AnscSizeOfString(pValue);
        return 0;
    }

    rc = strcmp_s("LowerLayers", strlen("LowerLayers"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
       // Not using "LowerLayers" parameter
        return 0;
    }

    rc = strcmp_s("MACAddress", strlen("MACAddress"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        _ansc_sprintf
            (
                pValue,
                "%02X:%02X:%02X:%02X:%02X:%02X",
                pMoCAIfFull->StaticInfo.MacAddress[0],
                pMoCAIfFull->StaticInfo.MacAddress[1],
                pMoCAIfFull->StaticInfo.MacAddress[2],
                pMoCAIfFull->StaticInfo.MacAddress[3],
                pMoCAIfFull->StaticInfo.MacAddress[4],
                pMoCAIfFull->StaticInfo.MacAddress[5]
            );

        *pUlSize = AnscSizeOfString(pValue);
        return 0;
    }

    rc = strcmp_s("FirmwareVersion", strlen("FirmwareVersion"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        rc = strcpy_s(pValue, *pUlSize, pMoCAIfFull->StaticInfo.FirmwareVersion);
        if (rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
        *pUlSize = AnscSizeOfString(pValue);
        return 0;
    }

    rc = strcmp_s("HighestVersion", strlen("HighestVersion"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        rc = strcpy_s(pValue, *pUlSize, pMoCAIfFull->StaticInfo.HighestVersion);
        if (rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
        *pUlSize = AnscSizeOfString(pValue);
        return 0;
    }

    rc = strcmp_s("CurrentVersion", strlen("CurrentVersion"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        rc = strcpy_s(pValue, *pUlSize, pMoCAIfFull->DynamicInfo.CurrentVersion);
        if (rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
        *pUlSize = AnscSizeOfString(pValue);
        return 0;
    }

    rc = strcmp_s("FreqCapabilityMask", strlen("FreqCapabilityMask"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        _ansc_sprintf
            (
                pValue,
                "%02X%02X%02X%02X%02X%02X%02X%02X",
                pMoCAIfFull->StaticInfo.FreqCapabilityMask[0],
                pMoCAIfFull->StaticInfo.FreqCapabilityMask[1],
                pMoCAIfFull->StaticInfo.FreqCapabilityMask[2],
                pMoCAIfFull->StaticInfo.FreqCapabilityMask[3],
                pMoCAIfFull->StaticInfo.FreqCapabilityMask[4],
                pMoCAIfFull->StaticInfo.FreqCapabilityMask[5],
                pMoCAIfFull->StaticInfo.FreqCapabilityMask[6],
                pMoCAIfFull->StaticInfo.FreqCapabilityMask[7]
            );
        *pUlSize = AnscSizeOfString(pValue);
        return 0;
    }

    rc = strcmp_s("FreqCurrentMaskSetting", strlen("FreqCurrentMaskSetting"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        rc = strcpy_s(pValue, *pUlSize, (PCHAR)pMoCAIfFull->Cfg.FreqCurrentMaskSetting);
        if (rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
        *pUlSize = AnscSizeOfString((PCHAR)pMoCAIfFull->Cfg.FreqCurrentMaskSetting);
        return 0;
    }

    rc = strcmp_s("X_CISCO_COM_ChannelScanMask", strlen("X_CISCO_COM_ChannelScanMask"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        rc = strcpy_s(pValue, *pUlSize, (PCHAR)pMoCAIfFull->Cfg.X_CISCO_COM_ChannelScanMask);
        if (rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
        *pUlSize = AnscSizeOfString((PCHAR)pMoCAIfFull->Cfg.X_CISCO_COM_ChannelScanMask);
        return 0;
    }

    rc = strcmp_s("FreqCurrentMask", strlen("FreqCurrentMask"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        _ansc_sprintf
            (
                pValue,
                "%02X%02X%02X%02X%02X%02X%02X%02X",
                pMoCAIfFull->DynamicInfo.FreqCurrentMask[0],
                pMoCAIfFull->DynamicInfo.FreqCurrentMask[1],
                pMoCAIfFull->DynamicInfo.FreqCurrentMask[2],
                pMoCAIfFull->DynamicInfo.FreqCurrentMask[3],
                pMoCAIfFull->DynamicInfo.FreqCurrentMask[4],
                pMoCAIfFull->DynamicInfo.FreqCurrentMask[5],
                pMoCAIfFull->DynamicInfo.FreqCurrentMask[6],
                pMoCAIfFull->DynamicInfo.FreqCurrentMask[7]
            );

        *pUlSize = AnscSizeOfString(pValue);
        return 0;
    }

    rc = strcmp_s("KeyPassphrase", strlen("KeyPassphrase"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* When read KeyPassphrase should return an empty string */
        rc = strcpy_s(pValue, *pUlSize, pMoCAIfFull->Cfg.KeyPassphrase);
        if (rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
        *pUlSize = AnscSizeOfString(pMoCAIfFull->Cfg.KeyPassphrase);
        return 0;
    }

    rc = strcmp_s("NetworkTabooMask", strlen("NetworkTabooMask"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        rc = strcpy_s(pValue, *pUlSize, (PCHAR)pMoCAIfFull->StaticInfo.NetworkTabooMask);
        if (rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
        *pUlSize = AnscSizeOfString(pValue);
        return 0;
    }

    rc = strcmp_s("NodeTabooMask", strlen("NodeTabooMask"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        rc = strcpy_s(pValue, *pUlSize, (PCHAR)pMoCAIfFull->Cfg.NodeTabooMask);
        if (rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
        *pUlSize = AnscSizeOfString((PCHAR)pMoCAIfFull->Cfg.NodeTabooMask);
        return 0;
    }

    rc = strcmp_s("X_CISCO_NetworkCoordinatorMACAddress", strlen("X_CISCO_NetworkCoordinatorMACAddress"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        rc = strcpy_s(pValue, *pUlSize, (PCHAR)pMoCAIfFull->DynamicInfo.X_CISCO_NetworkCoordinatorMACAddress);
        if (rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
        *pUlSize = AnscSizeOfString(pValue);
        return 0;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Interface1_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Interface1_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_DML_MOCA_IF_FULL          pMoCAIfFull = &((PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext)->MoCAIfFull;
    PCOSA_DATAMODEL_MOCA            pMyObject     = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_CFG              pCfg          = &pMyObject->MoCACfg;
    errno_t                         rc       = -1;
    int                             ind      = -1;

    AnscTraceWarning(("ParamName: %s bvalue %d\n", ParamName, bValue));
    BOOL bridgeId = FALSE;
    /* check the parameter name and set the corresponding value */
    rc = strcmp_s("Enable", strlen("Enable"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* save update to backup */
        char buf[5] = {0};
#if defined (_CM_HIGHSPLIT_SUPPORTED_)
        unsigned char IsHighSplitEnabled = CosaMoCAIsCMHighSplitDiplexerMode();

        if( ( bValue == TRUE ) && ( TRUE == IsHighSplitEnabled ) )
        {
            CcspTraceWarning(("Enabling MOCA is not supported when CM HighSplit mode\n"));
            return FALSE;
        }
#endif /* * _CM_HIGHSPLIT_SUPPORTED_ */

        syscfg_get( NULL, "X_RDKCENTRAL-COM_VIDEOSERVICE", buf, sizeof(buf));
        if( strlen(buf) != 0)
        {
                if (strcmp(buf,"1") == 0)
                {
                    if( ( bValue == FALSE)
#if defined (_CM_HIGHSPLIT_SUPPORTED_) 
                        && ( FALSE == IsHighSplitEnabled ) 
#endif /* * _CM_HIGHSPLIT_SUPPORTED_ */
                      )
                    {
                        CcspTraceWarning(("Disabling MOCA is not supported when VideoService is ENABLED\n"));
                        return FALSE;
                    }
                }
        }
		if((bValue == FALSE) && (pCfg->bForceEnabled == TRUE) && ((ANSC_STATUS_SUCCESS == is_usg_in_bridge_mode(&bridgeId)) && (FALSE == bridgeId)) 
#if defined (_CM_HIGHSPLIT_SUPPORTED_)
                && ( FALSE == IsHighSplitEnabled )
#endif /* * _CM_HIGHSPLIT_SUPPORTED_ */
                ){
			CcspTraceWarning(("MOCA cannot Disabled due to X_RDKCENTRAL-COM_ForceEnable flag\n"));
			return FALSE;
	   }

        pMoCAIfFull->Cfg.bEnabled = bValue;
        return TRUE;
    }

    rc = strcmp_s("PreferredNC", strlen("PreferredNC"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* save update to backup */
        pMoCAIfFull->Cfg.bPreferredNC = bValue;
        return TRUE;
    }

    rc = strcmp_s("PrivacyEnabledSetting", strlen("PrivacyEnabledSetting"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* save update to backup */
        pMoCAIfFull->Cfg.PrivacyEnabledSetting = bValue;
        return TRUE;
    }

    rc = strcmp_s("X_CISCO_COM_Reset", strlen("X_CISCO_COM_Reset"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* save update to backup */
        pMoCAIfFull->Cfg.X_CISCO_COM_Reset = bValue;
        return TRUE;
    }

    rc = strcmp_s("X_CISCO_COM_MixedMode", strlen("X_CISCO_COM_MixedMode"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* save update to backup */
        pMoCAIfFull->Cfg.X_CISCO_COM_MixedMode = bValue;
        return TRUE;
    }

    rc = strcmp_s("X_CISCO_COM_ChannelScanning", strlen("X_CISCO_COM_ChannelScanning"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* save update to backup */
        pMoCAIfFull->Cfg.X_CISCO_COM_ChannelScanning = bValue;
        return TRUE;
    }

    rc = strcmp_s("X_CISCO_COM_AutoPowerControlEnable", strlen("X_CISCO_COM_AutoPowerControlEnable"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* save update to backup */
        pMoCAIfFull->Cfg.X_CISCO_COM_AutoPowerControlEnable = bValue;
        return TRUE;
    }

    rc = strcmp_s("X_CISCO_COM_EnableTabooBit", strlen("X_CISCO_COM_EnableTabooBit"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* save update to backup */
        pMoCAIfFull->Cfg.X_CISCO_COM_EnableTabooBit = bValue;
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Interface1_SetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int                         iValue
            );

    description:

        This function is called to set integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int                         iValue
                The updated integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Interface1_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    PCOSA_DML_MOCA_IF_FULL          pMoCAIfFull = &((PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext)->MoCAIfFull;
    errno_t                         rc       = -1;
    int                             ind      = -1;

    AnscTraceWarning(("ParamName: %s iValue: %d\n", ParamName, iValue));

    /* check the parameter name and set the corresponding value */
    rc = strcmp_s("TxPowerLimit", strlen("TxPowerLimit"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* save update to backup */
        pMoCAIfFull->Cfg.TxPowerLimit = iValue;
        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Interface1_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG                       uValue
            );

    description:

        This function is called to set ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG                       uValue
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Interface1_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{    
    PCOSA_DML_MOCA_IF_FULL          pMoCAIfFull = &((PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext)->MoCAIfFull;
    errno_t                         rc       = -1;
    int                             ind      = -1;
    
    AnscTraceWarning(("ParamName: %s uValue: %lu\n", ParamName, uValue));
    #define LIMIT 15
    #define MIN 3

    /* check the parameter name and set the corresponding value */
    rc = strcmp_s("PowerCntlPhyTarget", strlen("PowerCntlPhyTarget"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* save update to backup */
        pMoCAIfFull->Cfg.PowerCntlPhyTarget = uValue;
        return TRUE;
    }

    rc = strcmp_s("BeaconPowerLimit", strlen("BeaconPowerLimit"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        if(((uValue%MIN)==0) && (uValue<=LIMIT))
        {
            /* save update to backup */
            pMoCAIfFull->Cfg.BeaconPowerLimit = uValue;
            return TRUE;
        }
    }

    rc = strcmp_s("AutoPowerControlPhyRate", strlen("AutoPowerControlPhyRate"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* save update to backup */
        pMoCAIfFull->Cfg.AutoPowerControlPhyRate = uValue;
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Interface1_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pString
            );

    description:

        This function is called to set string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pString
                The updated string value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Interface1_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    PCOSA_DML_MOCA_IF_FULL          pMoCAIfFull = &((PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext)->MoCAIfFull;
    errno_t                         rc          = -1;
    int                             ind         =  -1;

    if((pString == NULL) || (ParamName == NULL))
    {
        AnscTraceWarning(("RDK_LOG_WARN,%s %s:%d\n",__FILE__, __FUNCTION__,__LINE__));
        return FALSE;
    }

   /*KeyPassphrase is sensitive information and should not be printed in log*/
    if (!(rc = strcmp_s("KeyPassphrase", strlen("KeyPassphrase"), ParamName, &ind)))
    {
        if(!ind)
        {
             AnscTraceWarning(("ParamName: %s\n",ParamName));
        }
        else
        {
             AnscTraceWarning(("ParamName: %s pString: %s\n", ParamName, pString));
        }
    }
    else if(rc != EOK)
    {
        AnscTraceWarning(("RDK_LOG_WARN, %s-%d rc =%d \n",__FUNCTION__,__LINE__,rc));
        return FALSE;
    }

    /* check the parameter name and set the corresponding value */
    if (!(rc = strcmp_s("Alias", strlen("Alias"), ParamName, &ind)))
    {
        if(!ind)
        {
            /* save update to backup */
            rc = strcpy_s(pMoCAIfFull->Cfg.Alias, sizeof( pMoCAIfFull->Cfg.Alias ), pString);
            if(rc != EOK)
            {
                AnscTraceWarning(("RDK_LOG_WARN, %s-%d rc =%d \n",__FUNCTION__,__LINE__,rc));
                return FALSE;
            }
            return TRUE;
       }
    }
    else if(rc != EOK)
    {
        AnscTraceWarning(("RDK_LOG_WARN, %s-%d rc =%d \n",__FUNCTION__,__LINE__,rc));
        return FALSE;
    }

    if (!(rc = strcmp_s("LowerLayers", strlen("LowerLayers"), ParamName, &ind)))
    {
        if(!ind)
        {
            /* save update to backup */
            /* Interface is a layer 1 interface. LowerLayers will not be used. */
            return FALSE;
        }
    }
    else if(rc != EOK)
    {
        AnscTraceWarning(("RDK_LOG_WARN, %s-%d rc =%d \n",__FUNCTION__,__LINE__,rc));
        return FALSE;
    }

    if (!(rc = strcmp_s("FreqCurrentMaskSetting", strlen("FreqCurrentMaskSetting"), ParamName, &ind)))
    {
        if(!ind)
        {
            /* save update to backup */
            rc = strcpy_s((PCHAR)pMoCAIfFull->Cfg.FreqCurrentMaskSetting, sizeof( pMoCAIfFull->Cfg.FreqCurrentMaskSetting ), pString);
            if(rc != EOK)
            {
                AnscTraceWarning(("RDK_LOG_WARN, %s-%d rc =%d \n",__FUNCTION__,__LINE__,rc));
                return FALSE;
            }
            return TRUE;
        }
    }
    else if(rc != EOK)
    {
        AnscTraceWarning(("RDK_LOG_WARN, %s-%d rc =%d \n",__FUNCTION__,__LINE__,rc));
        return FALSE;
    }

    if (!(rc = strcmp_s("X_CISCO_COM_ChannelScanMask", strlen("X_CISCO_COM_ChannelScanMask"), ParamName, &ind)))
    {
        if(!ind)
        {
            /* save update to backup */
            rc = strcpy_s((PCHAR)pMoCAIfFull->Cfg.X_CISCO_COM_ChannelScanMask, sizeof( pMoCAIfFull->Cfg.X_CISCO_COM_ChannelScanMask ), pString);
            if(rc != EOK)
            {
                 AnscTraceWarning(("RDK_LOG_WARN, %s-%d rc =%d \n",__FUNCTION__,__LINE__,rc));
                 return FALSE;
            }
            return TRUE;
        }
    }
    else if(rc != EOK)
    {
        AnscTraceWarning(("RDK_LOG_WARN, %s-%d rc =%d \n",__FUNCTION__,__LINE__,rc));
        return FALSE;
    }

    if (!(rc = strcmp_s("KeyPassphrase", strlen("KeyPassphrase"), ParamName, &ind)))
    {
        if(!ind)
        {
            /* save update to backup */
            rc = strcpy_s(pMoCAIfFull->Cfg.KeyPassphrase, sizeof( pMoCAIfFull->Cfg.KeyPassphrase ), pString);
            if(rc != EOK)
            {
                 AnscTraceWarning(("RDK_LOG_WARN, %s-%d rc =%d \n",__FUNCTION__,__LINE__,rc));
                 return FALSE;
            }
            return TRUE;
         }
     }
     else if(rc != EOK)
     {
         AnscTraceWarning(("RDK_LOG_WARN, %s-%d rc =%d \n",__FUNCTION__,__LINE__,rc));
         return FALSE;
     }

    if (!(rc = strcmp_s("NodeTabooMask", strlen("NodeTabooMask"), ParamName, &ind)))
    {
        if(!ind)
        {
             /* save update to backup */
             rc = strcpy_s((PCHAR)pMoCAIfFull->Cfg.NodeTabooMask, sizeof( pMoCAIfFull->Cfg.NodeTabooMask ), pString);
             if(rc != EOK)
             {
                 AnscTraceWarning(("RDK_LOG_WARN, %s-%d rc =%d \n",__FUNCTION__,__LINE__,rc));
                 return FALSE;
             }
             return TRUE;
        }
    }
    else if(rc != EOK)
    {
        AnscTraceWarning(("RDK_LOG_WARN, %s-%d rc =%d \n",__FUNCTION__,__LINE__,rc));
        return FALSE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Interface1_Validate
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pReturnParamName,
                ULONG*                      puLength
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pReturnParamName,
                The buffer (128 bytes) of parameter name if there's a validation. 

                ULONG*                      puLength
                The output length of the param name. 

    return:     TRUE if there's no validation.

**********************************************************************/
BOOL
Interface1_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    PCOSA_DML_MOCA_IF_FULL_TABLE    pMoCAIfFullTable        = (PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext;
    ULONG                           keyPassLen              = 0;
    PCHAR                           tempKeyPassphrase       = NULL;

    AnscTraceWarning(("\n"));

    CosaDmlMocaGetNumberOfIfs((ANSC_HANDLE)NULL/*pPoamMoCADm*/);
    errno_t                         rc       = -1;
            
    keyPassLen = AnscSizeOfString(pMoCAIfFullTable->MoCAIfFull.Cfg.KeyPassphrase);
    if( (keyPassLen < 12) || (keyPassLen > 17) )
    {
       rc = strcpy_s(pReturnParamName,  *puLength,"KeyPassphrase");
        if (rc != EOK)
        {
            ERR_CHK(rc);
            return FALSE;
        }
       *puLength = AnscSizeOfString("KeyPassphrase");
       AnscTraceWarning(("%s: KeyPassphrase length must be from 12 to 17 characters\n", __FUNCTION__));
       return FALSE;
    }
    tempKeyPassphrase = pMoCAIfFullTable->MoCAIfFull.Cfg.KeyPassphrase;
    while(*tempKeyPassphrase)
    {
       if (*tempKeyPassphrase < 48 || *tempKeyPassphrase > 57)
       {
          AnscTraceWarning(("%s: KeyPassphrase must be composed of numeric digits 0-9\n", __FUNCTION__));
          return FALSE;
       }
       tempKeyPassphrase++;
    }

    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Interface1_Commit
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Interface1_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DML_MOCA_IF_FULL          pMoCAIfFull = &((PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext)->MoCAIfFull;
    ANSC_STATUS                     ReturnStatus  = ANSC_STATUS_SUCCESS;

    AnscTraceWarning(("\n"));

    if ( (ReturnStatus = CosaDmlMocaIfSetCfg((ANSC_HANDLE)NULL, pMoCAIfFull->Cfg.InstanceNumber-1, &pMoCAIfFull->Cfg)) != ANSC_STATUS_SUCCESS)
    {
       AnscTraceWarning(("%s: CosaDmlMocaIfSetCfg retunrs Error Calling Interface1_Rollback\n", __FUNCTION__));
       Interface1_Rollback(hInsContext); 
       return ANSC_STATUS_DISCARD;
    }
    
    return ReturnStatus;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Interface1_Rollback
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to roll back the update whenever there's a 
        validation found.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Interface1_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DML_MOCA_IF_FULL          pMoCAIfFull = &((PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext)->MoCAIfFull;
    
    CosaDmlMocaIfGetCfg((ANSC_HANDLE)NULL, pMoCAIfFull->Cfg.InstanceNumber-1, &pMoCAIfFull->Cfg);

    return 0;
}

/***********************************************************************

 APIs for Object:

    MoCA.Interface.{i}.X_CISCO_COM_PeerTable.{i}.

    *  X_CISCO_COM_PeerTable_GetEntryCount
    *  X_CISCO_COM_PeerTable_GetEntry
    *  X_CISCO_COM_PeerTable_IsUpdated
    *  X_CISCO_COM_PeerTable_Synchronize
    *  X_CISCO_COM_PeerTable_GetParamBoolValue
    *  X_CISCO_COM_PeerTable_GetParamIntValue
    *  X_CISCO_COM_PeerTable_GetParamUlongValue
    *  X_CISCO_COM_PeerTable_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        X_CISCO_COM_PeerTable_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
X_CISCO_COM_PeerTable_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MOCA            pMyObject       = (PCOSA_DATAMODEL_MOCA)g_MoCAObject;
    PCOSA_DML_MOCA_IF_FULL          pIf             = (PCOSA_DML_MOCA_IF_FULL)hInsContext;
    ULONG                           InterfaceIndex  = pIf->Cfg.InstanceNumber - 1;
    
    return pMyObject->MoCAIfFullTable[InterfaceIndex].ulMoCAPeerTableCount;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        X_CISCO_COM_PeerTable_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
X_CISCO_COM_PeerTable_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{   
    PCOSA_DATAMODEL_MOCA            pMyObject       = (PCOSA_DATAMODEL_MOCA)g_MoCAObject;
    PCOSA_DML_MOCA_IF_FULL          pIf             = (PCOSA_DML_MOCA_IF_FULL)hInsContext;
    ULONG                           InterfaceIndex  = pIf->Cfg.InstanceNumber - 1;

    if ( pMyObject->MoCAIfFullTable[InterfaceIndex].ulMoCAPeerTableCount <= nIndex )
        return NULL;

    *pInsNumber               = nIndex+1;

    return (ANSC_HANDLE)&pMyObject->MoCAIfFullTable[InterfaceIndex].pMoCAPeerTable[nIndex];
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_PeerTable_IsUpdated
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is checking whether the table is updated or not.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     TRUE or FALSE.

**********************************************************************/
BOOL
X_CISCO_COM_PeerTable_IsUpdated
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    static ULONG last_tick = 0;
    
    if ( !last_tick ) 
    {
        last_tick = AnscGetTickInSeconds();

        return TRUE;
    }
    
    if ( last_tick >= TIME_NO_NEGATIVE(AnscGetTickInSeconds() - MOCA_REFRESH_INTERVAL) )
    {
        return FALSE;
    }
    else 
    {
        last_tick = AnscGetTickInSeconds();

        return TRUE;
    }
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        X_CISCO_COM_PeerTable_Synchronize
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to synchronize the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
X_CISCO_COM_PeerTable_Synchronize
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MOCA            pMyObject       = (PCOSA_DATAMODEL_MOCA)g_MoCAObject;
    PCOSA_DML_MOCA_IF_FULL          pIf             = (PCOSA_DML_MOCA_IF_FULL)hInsContext;
    ULONG                           InterfaceIndex  = pIf->Cfg.InstanceNumber - 1;
    PCOSA_DML_MOCA_PEER             pConf           = NULL;
    ULONG                           Count           = 0;
    ANSC_STATUS                     ret             = ANSC_STATUS_SUCCESS;
    
    if ( pMyObject->MoCAIfFullTable[InterfaceIndex].pMoCAPeerTable )
    {
        AnscFreeMemory(pMyObject->MoCAIfFullTable[InterfaceIndex].pMoCAPeerTable);
        pMyObject->MoCAIfFullTable[InterfaceIndex].pMoCAPeerTable       = NULL;
        pMyObject->MoCAIfFullTable[InterfaceIndex].ulMoCAPeerTableCount = 0;
    }

    ret = CosaDmlMocaIfPeerTableGetTable((ANSC_HANDLE)NULL, InterfaceIndex, &pConf,&Count );
    if ( ret != ANSC_STATUS_SUCCESS )
    {
        return ret;
    }

    pMyObject->MoCAIfFullTable[InterfaceIndex].pMoCAPeerTable       = pConf;
    pMyObject->MoCAIfFullTable[InterfaceIndex].ulMoCAPeerTableCount = Count;

    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_PeerTable_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_CISCO_COM_PeerTable_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pBool);
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_PeerTable_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_CISCO_COM_PeerTable_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_PeerTable_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_CISCO_COM_PeerTable_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_DML_MOCA_PEER             pConf           = (PCOSA_DML_MOCA_PEER)hInsContext;
    errno_t                         rc       = -1;
    int                             ind      = -1;

    /* check the parameter name and return the corresponding value */
    rc = strcmp_s("PeerSpeed", strlen("PeerSpeed"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->PeerSpeed;
        
        return TRUE;
    }
    
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        X_CISCO_COM_PeerTable_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
X_CISCO_COM_PeerTable_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pValue);
    UNREFERENCED_PARAMETER(pUlSize);
    return -1;
}

#if 0
static ANSC_STATUS
_MeshTxNodeTable_Release
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus    = ANSC_STATUS_SUCCESS;
    PCOSA_DML_MOCA_IF_FULL_TABLE    pMoCAIfFull     = (PCOSA_DML_MOCA_IF_FULL_TABLE)hThisObject;
    PCOSA_DML_MOCA_MeshTxNode       pMeshTxNode     = NULL;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext    = NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry     = AnscSListGetFirstEntry(&pMoCAIfFull->MoCAMeshTxNodeTable);
    
    while ( pSLinkEntry )
    {
        pCosaContext = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);
        pSLinkEntry = AnscSListGetNextEntry(pSLinkEntry);

        AnscSListPopEntryByLink(&pMoCAIfFull->MoCAMeshTxNodeTable, &pCosaContext->Linkage);


        pMeshTxNode = (PCOSA_DML_MOCA_MeshTxNode)pCosaContext->hContext;

        if ( AnscSListQueryDepth(&pMeshTxNode->MoCAMeshRxNodeTable) )
        {
            PCOSA_CONTEXT_LINK_OBJECT       pCosaContext    = NULL;
            PSINGLE_LINK_ENTRY              pSLinkEntry     = AnscSListGetFirstEntry(&pMeshTxNode->MoCAMeshRxNodeTable);
            
            while ( pSLinkEntry )
            {
                pCosaContext = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);
                pSLinkEntry = AnscSListGetNextEntry(pSLinkEntry);
            
                AnscSListPopEntryByLink(&pMeshTxNode->MoCAMeshRxNodeTable, &pCosaContext->Linkage);
                AnscFreeMemory(pCosaContext->hContext);
                AnscFreeMemory(pCosaContext);
            }
        }
        
        AnscFreeMemory(pCosaContext->hContext);
        AnscFreeMemory(pCosaContext);
    }
    
    AnscSListInitializeHeader(&pMoCAIfFull->MoCAMeshTxNodeTable);
    return returnStatus;
}

static PCOSA_DML_MOCA_MeshTxNode
_MeshTxNodeTable_StoreTxNode
    (
        PSLIST_HEADER               pMoCAMeshTxNodeTable,
        ULONG                       ulInterfaceIndex,
        ULONG                       TxNodeID
    )
{
    ULONG                           instNum         = 0;
    PCOSA_DML_MOCA_MeshTxNode       pMeshTxNode     = NULL;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext    = NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry     = AnscSListGetFirstEntry(pMoCAMeshTxNodeTable);
    PCOSA_CONTEXT_MOCA_LINK_OBJECT  pMocaLink       = NULL;

    //check duplicate
    while ( pSLinkEntry )
    {
        pCosaContext = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);
        pSLinkEntry = AnscSListGetNextEntry(pSLinkEntry);

        pMeshTxNode = (PCOSA_DML_MOCA_MeshTxNode)pCosaContext->hContext;
        //printf("HOUJIN: %s: Existing InstanceNumber=%u and TxNodeID=%u, New TxNodeID=%u\n",
        //    __func__, pCosaContext->InstanceNumber, pMeshTxNode->TxNodeID, TxNodeID);
        
        if ( instNum < pCosaContext->InstanceNumber )
            instNum = pCosaContext->InstanceNumber;

        if ( pMeshTxNode->TxNodeID == TxNodeID )
            return pMeshTxNode;
    }


    //append entity
    pMeshTxNode = (PCOSA_DML_MOCA_MeshTxNode)AnscAllocateMemory(sizeof(COSA_DML_MOCA_MeshTxNode));
    if ( !pMeshTxNode )
        return NULL;

    AnscSListInitializeHeader(&pMeshTxNode->MoCAMeshRxNodeTable);
    pMeshTxNode->TxNodeID = TxNodeID;

    
    //append context
    pMocaLink = (PCOSA_CONTEXT_MOCA_LINK_OBJECT)AnscAllocateMemory(sizeof(COSA_CONTEXT_MOCA_LINK_OBJECT));
    if ( !pMocaLink )
    {
        AnscFreeMemory(pMeshTxNode);
        return NULL;
    }

    pMocaLink->InterfaceIndex = ulInterfaceIndex;
    pMocaLink->Index          = instNum++;
    pMocaLink->InstanceNumber = instNum;
    pMocaLink->hContext       = (ANSC_HANDLE)pMeshTxNode;

    //printf("HOUJIN: %s: Created InstanceNumber=%u and TxNodeID=%u\n",
    //    __func__, instNum, TxNodeID);

    CosaSListPushEntryByInsNum(pMoCAMeshTxNodeTable, (PCOSA_CONTEXT_LINK_OBJECT)pMocaLink);
    
    return pMeshTxNode;
}

static PCOSA_DML_MOCA_MeshRxNode
_MeshTxNodeTable_StoreRxNode
    (
        PSLIST_HEADER               pMoCAMeshRxNodeTable,
        ULONG                       ulInterfaceIndex,
        ULONG                       RxNodeID,
        ULONG                       TxRate
    )
{
    ULONG                           instNum         = 0;
    PCOSA_DML_MOCA_MeshRxNode       pMeshRxNode     = NULL;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext    = NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry     = AnscSListGetFirstEntry(pMoCAMeshRxNodeTable);
    PCOSA_CONTEXT_MOCA_LINK_OBJECT  pMocaLink       = NULL;

    //check duplicate
    while ( pSLinkEntry )
    {
        pCosaContext = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);
        pSLinkEntry = AnscSListGetNextEntry(pSLinkEntry);

        pMeshRxNode = (PCOSA_DML_MOCA_MeshRxNode)pCosaContext->hContext;
        //printf("HOUJIN: %s: Existing InstanceNumber=%u and RxNodeID=%u, New RxNodeID=%u\n",
        //    __func__, pCosaContext->InstanceNumber, pMeshRxNode->RxNodeID, RxNodeID);
        
        if ( instNum < pCosaContext->InstanceNumber )
            instNum = pCosaContext->InstanceNumber;

        if ( pMeshRxNode->RxNodeID == RxNodeID )
            return pMeshRxNode;
    }


    //append entity
    pMeshRxNode = (PCOSA_DML_MOCA_MeshRxNode)AnscAllocateMemory(sizeof(COSA_DML_MOCA_MeshRxNode));
    if ( !pMeshRxNode )
        return NULL;

    pMeshRxNode->RxNodeID = RxNodeID;
    pMeshRxNode->TxRate = TxRate;

    
    //append context
    pMocaLink = (PCOSA_CONTEXT_MOCA_LINK_OBJECT)AnscAllocateMemory(sizeof(COSA_CONTEXT_MOCA_LINK_OBJECT));
    if ( !pMocaLink )
    {
        AnscFreeMemory(pMeshRxNode);
        return NULL;
    }

    pMocaLink->InterfaceIndex = ulInterfaceIndex;
    pMocaLink->Index          = instNum++;
    pMocaLink->InstanceNumber = instNum;
    pMocaLink->hContext       = (ANSC_HANDLE)pMeshRxNode;

    //printf("HOUJIN: %s: Created InstanceNumber=%u, RxNodeID=%u, TxRate=%u\n",
    //    __func__, instNum, RxNodeID, TxRate);

    CosaSListPushEntryByInsNum(pMoCAMeshRxNodeTable, (PCOSA_CONTEXT_LINK_OBJECT)pMocaLink);
    
    return pMeshRxNode;
}

static ANSC_STATUS
_MeshTxNodeTable_Populate
    (
        ANSC_HANDLE                 hThisObject,
        ULONG                       ulInterfaceIndex,
        const COSA_DML_MOCA_MESH*   pMoCAMesh,
        ULONG                       Count
    )
{
    ANSC_STATUS                     returnStatus    = ANSC_STATUS_SUCCESS;
    PCOSA_DML_MOCA_IF_FULL_TABLE    pMoCAIfFull     = (PCOSA_DML_MOCA_IF_FULL_TABLE)hThisObject;
    PCOSA_DML_MOCA_MeshTxNode       pMeshTxNode     = NULL;
    ULONG                           i               = 0;

    if ( AnscSListQueryDepth(&pMoCAIfFull->MoCAMeshTxNodeTable) )
    {
        _MeshTxNodeTable_Release(hThisObject);
    }

    for ( i = 0; i < Count; ++i )
    {
        pMeshTxNode = _MeshTxNodeTable_StoreTxNode(&pMoCAIfFull->MoCAMeshTxNodeTable, ulInterfaceIndex, pMoCAMesh[i].TxNodeID);
        if ( pMeshTxNode )
        {
            _MeshTxNodeTable_StoreRxNode(&pMeshTxNode->MoCAMeshRxNodeTable, ulInterfaceIndex, pMoCAMesh[i].RxNodeID, pMoCAMesh[i].TxRate);
        }
    }
    
    return returnStatus;
}

/***********************************************************************

 APIs for Object:

    MoCA.Interface.{i}.X_RDKCENTRAL-COM_MeshTable.MeshTxNodeTable.{i}.

    *  MeshTxNodeTable_GetEntryCount
    *  MeshTxNodeTable_GetEntry
    *  MeshTxNodeTable_IsUpdated
    *  MeshTxNodeTable_Synchronize
    *  MeshTxNodeTable_GetParamUlongValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        MeshTxNodeTable_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
MeshTxNodeTable_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DML_MOCA_IF_FULL_TABLE pMoCAIfFull = (PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext;
    return AnscSListQueryDepth(&pMoCAIfFull->MoCAMeshTxNodeTable);
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        MeshTxNodeTable_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
MeshTxNodeTable_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{   
    PCOSA_DML_MOCA_IF_FULL_TABLE    pMoCAIfFull     = (PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext    = NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry     = NULL;

    pSLinkEntry = AnscSListGetEntryByIndex(&pMoCAIfFull->MoCAMeshTxNodeTable, nIndex);

    if ( pSLinkEntry )
    {
        pCosaContext = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);
        *pInsNumber = pCosaContext->InstanceNumber;
    }

    return pCosaContext;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        MeshTxNodeTable_IsUpdated
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is checking whether the table is updated or not.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     TRUE or FALSE.

**********************************************************************/
BOOL
MeshTxNodeTable_IsUpdated
    (
        ANSC_HANDLE                 hInsContext
    )
{
    static ULONG last_tick = 0;
    
    if ( !last_tick ) 
    {
        last_tick = AnscGetTickInSeconds();

        return TRUE;
    }
    
    if ( last_tick >= TIME_NO_NEGATIVE(AnscGetTickInSeconds() - MOCA_REFRESH_INTERVAL) )
    {
        return FALSE;
    }
    else 
    {
        last_tick = AnscGetTickInSeconds();

        return TRUE;
    }
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        MeshTxNodeTable_Synchronize
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to synchronize the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
MeshTxNodeTable_Synchronize
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DML_MOCA_IF_FULL_TABLE    pMoCAIfFull     = (PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext;
    PCOSA_DML_MOCA_IF_FULL          pIf             = (PCOSA_DML_MOCA_IF_FULL)hInsContext;
    ULONG                           InterfaceIndex  = pIf->Cfg.InstanceNumber - 1;
    PCOSA_DML_MOCA_MESH             pMoCAMesh       = NULL;
    ULONG                           Count           = 0;
    ANSC_STATUS                     ret             = ANSC_STATUS_SUCCESS;

    _MeshTxNodeTable_Release(hInsContext);

    //fetch data
    ret = CosaDmlMocaIfMeshTableGetTable((ANSC_HANDLE)NULL, InterfaceIndex, &pMoCAMesh, &Count);
    if ( ret != ANSC_STATUS_SUCCESS )
    {
        goto FIN;
    }

    _MeshTxNodeTable_Populate(hInsContext, InterfaceIndex, pMoCAMesh, Count);

FIN:
    if ( pMoCAMesh )
        AnscFreeMemory(pMoCAMesh);
    return ret;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        MeshTxNodeTable_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
MeshTxNodeTable_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_CONTEXT_LINK_OBJECT pContextLinkObject = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_MOCA_MeshTxNode pConf = (PCOSA_DML_MOCA_MeshTxNode)pContextLinkObject->hContext;
    errno_t                         rc       = -1;
    int                             ind      = -1;

    /* check the parameter name and return the corresponding value */
    rc = strcmp_s("MeshTxNodeId", strlen("MeshTxNodeId"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->TxNodeID;
        
        return TRUE;
    }
    
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/***********************************************************************

 APIs for Object:

    MoCA.Interface.{i}.X_RDKCENTRAL-COM_MeshTable.MeshTxNodeTable.{i}.MeshRxNodeTable.{i}.

    *  MeshRxNodeTable_GetEntryCount
    *  MeshRxNodeTable_GetEntry
    *  MeshRxNodeTable_GetParamUlongValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        MeshRxNodeTable_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
MeshRxNodeTable_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_CONTEXT_LINK_OBJECT pContextLinkObject = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_MOCA_MeshTxNode pMoCAMeshTxNode = (PCOSA_DML_MOCA_MeshTxNode)pContextLinkObject->hContext;
    return AnscSListQueryDepth(&pMoCAMeshTxNode->MoCAMeshRxNodeTable);
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        MeshRxNodeTable_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
MeshRxNodeTable_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{   
    PCOSA_CONTEXT_LINK_OBJECT   pContextLinkObject  = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_MOCA_MeshTxNode   pMoCAMeshTxNode     = (PCOSA_DML_MOCA_MeshTxNode)pContextLinkObject->hContext;
    PCOSA_CONTEXT_LINK_OBJECT   pCosaContext        = NULL;
    PSINGLE_LINK_ENTRY          pSLinkEntry         = NULL;

    pSLinkEntry = AnscSListGetEntryByIndex(&pMoCAMeshTxNode->MoCAMeshRxNodeTable, nIndex);

    if ( pSLinkEntry )
    {
        pCosaContext = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);
        *pInsNumber = pCosaContext->InstanceNumber;
    }

    return pCosaContext;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        MeshRxNodeTable_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
MeshRxNodeTable_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_CONTEXT_LINK_OBJECT   pContextLinkObject  = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_MOCA_MeshRxNode   pConf               = (PCOSA_DML_MOCA_MeshRxNode)pContextLinkObject->hContext;
    errno_t                         rc       = -1;
    int                             ind      = -1;

    /* check the parameter name and return the corresponding value */
    rc = strcmp_s("MeshRxNodeId", strlen("MeshRxNodeId"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->RxNodeID;
        
        return TRUE;
    }
    
    /* check the parameter name and return the corresponding value */
    rc = strcmp_s("MeshPHYTxRate", strlen("MeshPHYTxRate"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->TxRate;
        
        return TRUE;
    }
    
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}
#endif

/***********************************************************************

 APIs for Object:

    MoCA.Interface.{i}.X_RDKCENTRAL-COM_MeshTable.{i}.

    *  MeshTable_GetEntryCount
    *  MeshTable_GetEntry
    *  MeshTable_IsUpdated
    *  MeshTable_Synchronize
    *  MeshTable_GetParamUlongValue

***********************************************************************/
/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        MeshTable_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
MeshTable_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DML_MOCA_IF_FULL_TABLE    pMoCAIfFullTable    = (PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext;
    return pMoCAIfFullTable->ulMoCAMeshTableCount;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        MeshTable_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
MeshTable_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DML_MOCA_IF_FULL_TABLE    pMoCAIfFullTable    = (PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext;

    if (nIndex < pMoCAIfFullTable->ulMoCAMeshTableCount)
    {
        *pInsNumber  = nIndex + 1;

        return &pMoCAIfFullTable->pMoCAMeshTable[nIndex];
    }

    return NULL;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        MeshTable_IsUpdated
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is checking whether the table is updated or not.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     TRUE or FALSE.

**********************************************************************/
BOOL
MeshTable_IsUpdated
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    static ULONG last_tick = 0;

    if ( !last_tick )
    {
        last_tick = AnscGetTickInSeconds();

        return TRUE;
    }

    if ( last_tick >= TIME_NO_NEGATIVE(AnscGetTickInSeconds() - MOCA_REFRESH_INTERVAL) )
    {
        return FALSE;
    }
    else
    {
        last_tick = AnscGetTickInSeconds();

        return TRUE;
    }
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        MeshTable_Synchronize
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to synchronize the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
MeshTable_Synchronize
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DML_MOCA_IF_FULL_TABLE    pMoCAIfFullTable     = (PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext;
    PCOSA_DML_MOCA_IF_FULL          pIf                  = (PCOSA_DML_MOCA_IF_FULL)hInsContext;
    ULONG                           InterfaceIndex       = pIf->Cfg.InstanceNumber - 1;
    ANSC_STATUS                     ret                  = ANSC_STATUS_SUCCESS;
    void*                           ppMoCAMeshTable      = NULL;
    void*                           pulMoCAMeshTableCount = NULL;

    if ( pMoCAIfFullTable->pMoCAMeshTable)
    {
        AnscFreeMemory(pMoCAIfFullTable->pMoCAMeshTable);

        pMoCAIfFullTable->pMoCAMeshTable= NULL;
    }

    ppMoCAMeshTable = &pMoCAIfFullTable->pMoCAMeshTable;
    pulMoCAMeshTableCount = &pMoCAIfFullTable->ulMoCAMeshTableCount;
    //fetch data
    ret = CosaDmlMocaIfMeshTableGetTable
	  (
		(ANSC_HANDLE)NULL,
		InterfaceIndex,
		(PCOSA_DML_MOCA_MESH *)ppMoCAMeshTable,
		(PULONG)pulMoCAMeshTableCount
	  );

    return ret;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        MeshTable_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
MeshTable_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_DML_MOCA_MESH pMoCAMesh = (PCOSA_DML_MOCA_MESH)hInsContext;
    errno_t                         rc       = -1;
    int                             ind      = -1;

    /* check the parameter name and return the corresponding value */
    rc = strcmp_s("MeshTxNodeId", strlen("MeshTxNodeId"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))

    {
        /* collect value */
        *puLong = pMoCAMesh->TxNodeID;

        return TRUE;
    }

    rc = strcmp_s("MeshRxNodeId", strlen("MeshRxNodeId"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAMesh->RxNodeID;

        return TRUE;
    }

    rc = strcmp_s("MeshPHYTxRate", strlen("MeshPHYTxRate"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAMesh->TxRate;

        return TRUE;
    }

   /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/***********************************************************************

 APIs for Object:

    MoCA.Interface.{i}.Stats.

    *  Stats2_GetParamBoolValue
    *  Stats2_GetParamIntValue
    *  Stats2_GetParamUlongValue
    *  Stats2_GetParamStringValue
    *  Stats2_SetParamBoolValue
    *  Stats2_SetParamIntValue
    *  Stats2_SetParamUlongValue
    *  Stats2_SetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Stats2_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Stats2_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    errno_t                         rc       = -1;
    int                             ind      = -1;

    /* check the parameter name and return the corresponding value */
    rc = strcmp_s("Reset", strlen("Reset"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *pBool = FALSE;
        
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Stats2_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Stats2_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Stats2_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Stats2_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_DML_MOCA_IF_FULL          pMoCAIfFull = &((PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext)->MoCAIfFull;
    /*PPOAM_COSAMOCADM_OBJECT         pPoamMoCADm = (PPOAM_COSAMOCADM_OBJECT )pMyObject->pPoamMoCADm;*/
    COSA_DML_MOCA_STATS             Stats;

    CosaDmlMocaIfGetStats((ANSC_HANDLE)NULL/*pPoamMoCADm*/, pMoCAIfFull->Cfg.InstanceNumber - 1, &Stats);
    errno_t                         rc       = -1;
    int                             ind      = -1;

    /* check the parameter name and return the corresponding value */
    rc = strcmp_s("BytesSent", strlen("BytesSent"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = Stats.BytesSent;
        return TRUE;
    }

    rc = strcmp_s("BytesReceived", strlen("BytesReceived"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = Stats.BytesReceived;
        return TRUE;
    }

    rc = strcmp_s("PacketsSent", strlen("PacketsSent"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = Stats.PacketsSent;
        return TRUE;
    }

    rc = strcmp_s("PacketsReceived", strlen("PacketsReceived"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = Stats.PacketsReceived;
        return TRUE;
    }

    rc = strcmp_s("ErrorsSent", strlen("ErrorsSent"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = Stats.ErrorsSent;
        return TRUE;
    }

    rc = strcmp_s("ErrorsReceived", strlen("ErrorsReceived"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = Stats.ErrorsReceived;
        return TRUE;
    }

    rc = strcmp_s("UnicastPacketsSent", strlen("UnicastPacketsSent"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = Stats.UnicastPacketsSent;
        return TRUE;
    }

    rc = strcmp_s("UnicastPacketsReceived", strlen("UnicastPacketsReceived"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = Stats.UnicastPacketsReceived;
        return TRUE;
    }

    rc = strcmp_s("DiscardPacketsSent", strlen("DiscardPacketsSent"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = Stats.DiscardPacketsSent;
        return TRUE;
    }

    rc = strcmp_s("DiscardPacketsReceived", strlen("DiscardPacketsReceived"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = Stats.DiscardPacketsReceived;
        return TRUE;
    }

    rc = strcmp_s("MulticastPacketsSent", strlen("MulticastPacketsSent"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = Stats.MulticastPacketsSent;
        return TRUE;
    }

    rc = strcmp_s("MulticastPacketsReceived", strlen("MulticastPacketsReceived"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = Stats.MulticastPacketsReceived;
        return TRUE;
    }

    rc = strcmp_s("BroadcastPacketsSent", strlen("BroadcastPacketsSent"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = Stats.BroadcastPacketsSent;
        return TRUE;
    }

    rc = strcmp_s("BroadcastPacketsReceived", strlen("BroadcastPacketsReceived"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = Stats.BroadcastPacketsReceived;
        return TRUE;
    }

    rc = strcmp_s("UnknownProtoPacketsReceived", strlen("UnknownProtoPacketsReceived"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = Stats.UnknownProtoPacketsReceived;
        return TRUE;
    }

    rc = strcmp_s("X_CISCO_COM_ExtAggrAverageTx", strlen("X_CISCO_COM_ExtAggrAverageTx"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = Stats.X_CISCO_COM_ExtAggrAverageTx;
        return TRUE;
    }

    rc = strcmp_s("X_CISCO_COM_ExtAggrAverageRx", strlen("X_CISCO_COM_ExtAggrAverageRx"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = Stats.X_CISCO_COM_ExtAggrAverageRx;
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Stats2_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
Stats2_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pValue);
    UNREFERENCED_PARAMETER(pUlSize);
    return -1;
}


/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Stats2_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Stats2_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_DML_MOCA_IF_FULL          pMoCAIfFull = &((PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext)->MoCAIfFull;
    errno_t                         rc       = -1;
    int                             ind      = -1;

    /* check the parameter name and set the corresponding value */
    rc = strcmp_s("Reset", strlen("Reset"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* save update to backup */
        if ( bValue == TRUE )
        {
            CosaDmlMocaIfResetStats( NULL, pMoCAIfFull->Cfg.InstanceNumber-1 );
            return TRUE;
        }
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Stats2_SetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int                         iValue
            );

    description:

        This function is called to set integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int                         iValue
                The updated integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Stats2_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(iValue);
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Stats2_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG                       uValue
            );

    description:

        This function is called to set ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG                       uValue
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Stats2_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{    
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(uValue);   
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Stats2_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pString
            );

    description:

        This function is called to set string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pString
                The updated string value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Stats2_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pString);
    
    /* check the parameter name and set the corresponding value */


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Stats2_Validate
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pReturnParamName,
                ULONG*                      puLength
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pReturnParamName,
                The buffer (128 bytes) of parameter name if there's a validation. 

                ULONG*                      puLength
                The output length of the param name. 

    return:     TRUE if there's no validation.

**********************************************************************/
BOOL
Stats2_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(pReturnParamName);
    UNREFERENCED_PARAMETER(puLength);
    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Stats2_Commit
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Stats2_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Stats2_Rollback
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to roll back the update whenever there's a 
        validation found.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Stats2_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    return 0;
}

/***********************************************************************

 APIs for Object:

    MoCA.Interface.{i}.Stats.X_CISCO_COM_ExtCounterTable.{i}.

    *  X_CISCO_COM_ExtCounterTable_GetEntryCount
    *  X_CISCO_COM_ExtCounterTable_GetEntry
    *  X_CISCO_COM_ExtCounterTable_GetParamBoolValue
    *  X_CISCO_COM_ExtCounterTable_GetParamIntValue
    *  X_CISCO_COM_ExtCounterTable_GetParamUlongValue
    *  X_CISCO_COM_ExtCounterTable_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        X_CISCO_COM_ExtCounterTable_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
X_CISCO_COM_ExtCounterTable_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DML_MOCA_IF_FULL          pIf             = (PCOSA_DML_MOCA_IF_FULL)hInsContext;
    ULONG                           InterfaceIndex  = pIf->Cfg.InstanceNumber - 1;

    return CosaDmlMocaIfExtCounterGetNumber((ANSC_HANDLE)NULL, InterfaceIndex);
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        X_CISCO_COM_ExtCounterTable_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
X_CISCO_COM_ExtCounterTable_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DML_MOCA_IF_FULL          pIf             = (PCOSA_DML_MOCA_IF_FULL)hInsContext;
    ULONG                           InterfaceIndex  = 0;
    PCOSA_DML_MOCA_EXTCOUNTER       pExtCounter     = NULL;
    PCOSA_CONTEXT_MOCA_LINK_OBJECT  pMocaLink       = NULL;
    ANSC_STATUS                     ret             = ANSC_STATUS_FAILURE;
    PCOSA_DATAMODEL_MOCA            pMyObject   = (PCOSA_DATAMODEL_MOCA)g_MoCAObject;
    void*                           pMoCAExtCounterTable = NULL;

    InterfaceIndex  = pIf->Cfg.InstanceNumber - 1;

    /* Get one ExtCounter Entry from backend*/
    pExtCounter = (PCOSA_DML_MOCA_EXTCOUNTER)AnscAllocateMemory( sizeof(COSA_DML_MOCA_EXTCOUNTER) );
    if ( pExtCounter == NULL )
    {
        return NULL;
    }
    
    AnscTraceWarning(("InterfaceIndex: %lu nIndex: %lu pExtCounte: %p\n",  InterfaceIndex, nIndex, pExtCounter));
    ret = CosaDmlMocaIfExtCounterGetEntry((ANSC_HANDLE)NULL, InterfaceIndex, nIndex, pExtCounter);
    if ( ret == ANSC_STATUS_FAILURE )
    {
        AnscFreeMemory(pExtCounter);
        return NULL;
    }

    /* Wrapper the entry and save two index because we need them later */
    pMocaLink = (PCOSA_CONTEXT_MOCA_LINK_OBJECT)AnscAllocateMemory( sizeof( COSA_CONTEXT_MOCA_LINK_OBJECT ) );
    if ( pMocaLink == NULL )
    {
        AnscFreeMemory(pExtCounter);
        return pMocaLink;
    }

    pMocaLink->InterfaceIndex = InterfaceIndex;
    pMocaLink->Index          = nIndex;
    pMocaLink->hContext       = (ANSC_HANDLE)pExtCounter;
    *pInsNumber               = nIndex+1;
    
    /* Put into our list */

    pMoCAExtCounterTable = &pMyObject->MoCAIfFullTable[InterfaceIndex].pMoCAExtCounterTable;
    CosaSListPushEntryByInsNum((PSLIST_HEADER)pMoCAExtCounterTable, (PCOSA_CONTEXT_LINK_OBJECT)pMocaLink);

    return (ANSC_HANDLE)pMocaLink;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_ExtCounterTable_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_CISCO_COM_ExtCounterTable_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pBool);
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_ExtCounterTable_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_CISCO_COM_ExtCounterTable_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{

    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_ExtCounterTable_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_CISCO_COM_ExtCounterTable_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{    
    PCOSA_CONTEXT_MOCA_LINK_OBJECT  pMocaLink       = (PCOSA_CONTEXT_MOCA_LINK_OBJECT)hInsContext;
    PCOSA_DML_MOCA_EXTCOUNTER       pConf     = (PCOSA_DML_MOCA_EXTCOUNTER)pMocaLink->hContext;
    errno_t                         rc       = -1;
    int                             ind      = -1;

    AnscTraceWarning(("InterfaceIndex: %lu Index: %lu pConf: %p\n",  pMocaLink->InterfaceIndex, pMocaLink->Index, pConf));
    /*this is counter object. the information will be changed from time to time.*/
    CosaDmlMocaIfExtCounterGetEntry((ANSC_HANDLE)NULL, pMocaLink->InterfaceIndex, pMocaLink->Index, pConf);

    /* check the parameter name and return the corresponding value */
    rc = strcmp_s("Type", strlen("Type"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
        
    {
        /* collect value */
        *puLong = pConf->Type;
        return TRUE;
    }

    rc = strcmp_s("Map", strlen("Map"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->Map;
        return TRUE;
    }
    
    rc = strcmp_s("Rsrv", strlen("Rsrv"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->Rsrv;
        return TRUE;
    }
    
    rc = strcmp_s("Lc", strlen("Lc"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->Lc;
        return TRUE;
    }
    
    rc = strcmp_s("Adm", strlen("Adm"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->Adm;
        return TRUE;
    }
    
    rc = strcmp_s("Probe", strlen("Probe"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->Probe;
        return TRUE;
    }
    
    rc = strcmp_s("Async", strlen("Async"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->Async;
        return TRUE;
    }
    
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        X_CISCO_COM_ExtCounterTable_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
X_CISCO_COM_ExtCounterTable_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{    

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pValue);
    UNREFERENCED_PARAMETER(pUlSize);
    return -1;
}

/***********************************************************************

 APIs for Object:

    MoCA.Interface.{i}.Stats.X_CISCO_COM_ExtAggrCounterTable.{i}.

    *  X_CISCO_COM_ExtAggrCounterTable_GetEntryCount
    *  X_CISCO_COM_ExtAggrCounterTable_GetEntry
    *  X_CISCO_COM_ExtAggrCounterTable_GetParamBoolValue
    *  X_CISCO_COM_ExtAggrCounterTable_GetParamIntValue
    *  X_CISCO_COM_ExtAggrCounterTable_GetParamUlongValue
    *  X_CISCO_COM_ExtAggrCounterTable_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        X_CISCO_COM_ExtAggrCounterTable_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
X_CISCO_COM_ExtAggrCounterTable_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DML_MOCA_IF_FULL          pIf             = (PCOSA_DML_MOCA_IF_FULL)hInsContext;
    ULONG                           InterfaceIndex  = pIf->Cfg.InstanceNumber - 1;
    
    return CosaDmlMocaIfExtAggrCounterGetNumber((ANSC_HANDLE)NULL, InterfaceIndex);
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        X_CISCO_COM_ExtAggrCounterTable_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
X_CISCO_COM_ExtAggrCounterTable_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DML_MOCA_IF_FULL          pIf             = (PCOSA_DML_MOCA_IF_FULL)hInsContext;
    ULONG                           InterfaceIndex  = 0;
    PCOSA_DML_MOCA_EXTAGGRCOUNTER   pExtAggrCounter = NULL;
    PCOSA_CONTEXT_MOCA_LINK_OBJECT  pMocaLink       = NULL;
    ANSC_STATUS                     ret             = ANSC_STATUS_FAILURE;
    PCOSA_DATAMODEL_MOCA            pMyObject   = (PCOSA_DATAMODEL_MOCA)g_MoCAObject;
    void*                           pMoCAExtAggrCounterTable = NULL;

    InterfaceIndex  = pIf->Cfg.InstanceNumber - 1;

    /* Get one ExtCounter Entry from backend*/
    pExtAggrCounter = (PCOSA_DML_MOCA_EXTAGGRCOUNTER)AnscAllocateMemory( sizeof(COSA_DML_MOCA_EXTAGGRCOUNTER) );
    if ( pExtAggrCounter == NULL )
    {
        return NULL;
    }
    
    ret = CosaDmlMocaIfExtAggrCounterGetEntry((ANSC_HANDLE)NULL, InterfaceIndex, nIndex, pExtAggrCounter);
    if ( ret == ANSC_STATUS_FAILURE )
    {
        AnscFreeMemory(pExtAggrCounter);
        return NULL;
    }

    /* Wrapper the entry and save two index because we need them later */
    pMocaLink = (PCOSA_CONTEXT_MOCA_LINK_OBJECT)AnscAllocateMemory( sizeof( COSA_CONTEXT_MOCA_LINK_OBJECT ) );
    if ( pMocaLink == NULL )
    {
        AnscFreeMemory(pExtAggrCounter);
        return pMocaLink;
    }

    pMocaLink->InterfaceIndex = InterfaceIndex;
    pMocaLink->Index          = nIndex;
    pMocaLink->hContext       = (ANSC_HANDLE)pExtAggrCounter;
    *pInsNumber               = nIndex+1;

    /* Put into our list */
    pMoCAExtAggrCounterTable = &pMyObject->MoCAIfFullTable[InterfaceIndex].pMoCAExtAggrCounterTable;

    CosaSListPushEntryByInsNum((PSLIST_HEADER)pMoCAExtAggrCounterTable, (PCOSA_CONTEXT_LINK_OBJECT)pMocaLink);

    return (ANSC_HANDLE)pMocaLink;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_ExtAggrCounterTable_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_CISCO_COM_ExtAggrCounterTable_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pBool);
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_ExtAggrCounterTable_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_CISCO_COM_ExtAggrCounterTable_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{

    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_ExtAggrCounterTable_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_CISCO_COM_ExtAggrCounterTable_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{    
    PCOSA_CONTEXT_MOCA_LINK_OBJECT  pMocaLink        = (PCOSA_CONTEXT_MOCA_LINK_OBJECT)hInsContext;
    PCOSA_DML_MOCA_EXTAGGRCOUNTER   pConf  = (PCOSA_DML_MOCA_EXTAGGRCOUNTER)pMocaLink->hContext;
    errno_t                         rc       = -1;
    int                             ind      = -1;

    /*this is counter object. the information will be changed from time to time.*/
    CosaDmlMocaIfExtAggrCounterGetEntry((ANSC_HANDLE)NULL, pMocaLink->InterfaceIndex, pMocaLink->Index, pConf);

    /* check the parameter name and return the corresponding value */
    rc = strcmp_s("Tx", strlen("Tx"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->Tx;
        return TRUE;
    }

    rc = strcmp_s("Rx", strlen("Rx"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->Rx;
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        X_CISCO_COM_ExtAggrCounterTable_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
X_CISCO_COM_ExtAggrCounterTable_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{    

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pValue);
    UNREFERENCED_PARAMETER(pUlSize);
    return -1;
}



/***********************************************************************

 APIs for Object:

    MoCA.Interface.{i}.QoS.

    *  QoS_GetParamBoolValue
    *  QoS_GetParamIntValue
    *  QoS_GetParamUlongValue
    *  QoS_GetParamStringValue
    *  QoS_SetParamBoolValue
    *  QoS_SetParamIntValue
    *  QoS_SetParamUlongValue
    *  QoS_SetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        QoS_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
QoS_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_DML_MOCA_IF_FULL_TABLE    pMoCACfg = (PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext;
    errno_t                         rc       = -1;
    int                             ind      = -1;

    /* check the parameter name and return the corresponding value */
    rc = strcmp_s("X_CISCO_COM_Enabled", strlen("X_CISCO_COM_Enabled"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *pBool = pMoCACfg->MoCAIfQos.Enabled;
        
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        QoS_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
QoS_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        QoS_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
QoS_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(puLong);
    errno_t                         rc       = -1;
    int                             ind      = -1;

    /* check the parameter name and return the corresponding value */
    rc = strcmp_s("EgressNumFlows", strlen("EgressNumFlows"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        return TRUE;
    }

    rc = strcmp_s("IngressNumFlows", strlen("IngressNumFlows"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        QoS_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
QoS_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pValue);
    UNREFERENCED_PARAMETER(pUlSize);
    return -1;
}


/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Qos_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Qos_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_DML_MOCA_IF_FULL_TABLE    pMoCACfg = (PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext;
    errno_t                         rc       = -1;
    int                             ind      = -1;

    /* check the parameter name and set the corresponding value */
    rc = strcmp_s("X_CISCO_COM_Enabled", strlen("X_CISCO_COM_Enabled"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* save update to backup */
        pMoCACfg->MoCAIfQos.Enabled = bValue;
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Qos_SetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int                         iValue
            );

    description:

        This function is called to set integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int                         iValue
                The updated integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Qos_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(iValue);
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Qos_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG                       uValue
            );

    description:

        This function is called to set ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG                       uValue
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Qos_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{    
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(uValue);
    
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Qos_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pString
            );

    description:

        This function is called to set string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pString
                The updated string value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Qos_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pString);
 /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Qos_Validate
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pReturnParamName,
                ULONG*                      puLength
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pReturnParamName,
                The buffer (128 bytes) of parameter name if there's a validation. 

                ULONG*                      puLength
                The output length of the param name. 

    return:     TRUE if there's no validation.

**********************************************************************/
BOOL
Qos_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(pReturnParamName);
    UNREFERENCED_PARAMETER(puLength);
    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Qos_Commit
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Qos_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DML_MOCA_IF_FULL          pMoCAIfFull = (PCOSA_DML_MOCA_IF_FULL)hInsContext;
    ULONG                           ulIndex     = pMoCAIfFull->Cfg.InstanceNumber-1;
    
    PCOSA_DATAMODEL_MOCA            pMyObject     = (PCOSA_DATAMODEL_MOCA    )g_MoCAObject;
    PCOSA_DML_MOCA_QOS              pCfg          = &pMyObject->MoCAIfFullTable[ulIndex].MoCAIfQos;
    ANSC_STATUS                     ReturnStatus  = ANSC_STATUS_SUCCESS;

    ReturnStatus = CosaDmlMocaIfSetQos(NULL, ulIndex, pCfg);
    
    return ReturnStatus;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Qos_Rollback
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to roll back the update whenever there's a 
        validation found.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Qos_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    return 0;
}

/***********************************************************************

 APIs for Object:

    MoCA.Interface.{i}.QoS.FlowStats.{i}.

    *  FlowStats_GetEntryCount
    *  FlowStats_GetEntry
    *  FlowStats_IsUpdated
    *  FlowStats_Synchronize
    *  FlowStats_GetParamUlongValue
    *  FlowStats_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        FlowStats_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
FlowStats_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MOCA            pMyObject       = (PCOSA_DATAMODEL_MOCA)g_MoCAObject;
    PCOSA_DML_MOCA_IF_FULL          pIf             = (PCOSA_DML_MOCA_IF_FULL)hInsContext;
    ULONG                           InterfaceIndex  = pIf->Cfg.InstanceNumber - 1;
    
    return pMyObject->MoCAIfFullTable[InterfaceIndex].ulMoCAFlowTableCount;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        FlowStats_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
FlowStats_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{   
    PCOSA_DATAMODEL_MOCA            pMyObject       = (PCOSA_DATAMODEL_MOCA)g_MoCAObject;
    PCOSA_DML_MOCA_IF_FULL          pIf             = (PCOSA_DML_MOCA_IF_FULL)hInsContext;
    ULONG                           InterfaceIndex  = pIf->Cfg.InstanceNumber - 1;

    if ( pMyObject->MoCAIfFullTable[InterfaceIndex].ulMoCAFlowTableCount <= nIndex )
        return NULL;

    *pInsNumber               = nIndex+1;
    
    return (ANSC_HANDLE)&pMyObject->MoCAIfFullTable[InterfaceIndex].pMoCAFlowTable[nIndex];
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        FlowStats_IsUpdated
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is checking whether the table is updated or not.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     TRUE or FALSE.

**********************************************************************/
BOOL
FlowStats_IsUpdated
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    static ULONG last_tick = 0;
    
    if ( !last_tick ) 
    {
        last_tick = AnscGetTickInSeconds();

        return TRUE;
    }
    
    if ( last_tick >= TIME_NO_NEGATIVE(AnscGetTickInSeconds() - MOCA_REFRESH_INTERVAL) )
    {
        return FALSE;
    }
    else 
    {
        last_tick = AnscGetTickInSeconds();

        return TRUE;
    }
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        FlowStats_Synchronize
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to synchronize the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
FlowStats_Synchronize
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_MOCA            pMyObject       = (PCOSA_DATAMODEL_MOCA)g_MoCAObject;
    PCOSA_DML_MOCA_IF_FULL          pIf             = (PCOSA_DML_MOCA_IF_FULL)hInsContext;
    ULONG                           InterfaceIndex  = pIf->Cfg.InstanceNumber - 1;
    PCOSA_DML_MOCA_FLOW             pConf           = NULL;
    ULONG                           Count           = 0;
    ANSC_STATUS                     ret             = ANSC_STATUS_SUCCESS;
    
    if ( pMyObject->MoCAIfFullTable[InterfaceIndex].pMoCAFlowTable )
    {
        AnscFreeMemory(pMyObject->MoCAIfFullTable[InterfaceIndex].pMoCAFlowTable);
        pMyObject->MoCAIfFullTable[InterfaceIndex].pMoCAFlowTable       = NULL;
        pMyObject->MoCAIfFullTable[InterfaceIndex].ulMoCAFlowTableCount = 0;
    }

    ret = CosaDmlMocaIfFlowTableGetTable((ANSC_HANDLE)NULL, InterfaceIndex, &pConf, &Count );
    if ( ret != ANSC_STATUS_SUCCESS )
    {
        return ret;
    }

    pMyObject->MoCAIfFullTable[InterfaceIndex].pMoCAFlowTable       = pConf;
    pMyObject->MoCAIfFullTable[InterfaceIndex].ulMoCAFlowTableCount = Count;

    return ret;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        FlowStats_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
FlowStats_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_DML_MOCA_FLOW             pConf           = (PCOSA_DML_MOCA_FLOW)hInsContext;
    errno_t                         rc       = -1;
    int                             ind      = -1;

    /* check the parameter name and return the corresponding value */
    rc = strcmp_s("FlowID", strlen("FlowID"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->FlowID;
        
        return TRUE;
    }
    
    rc = strcmp_s("IngressNodeID", strlen("IngressNodeID"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->IngressNodeID;
        
        return TRUE;
    }
    
    rc = strcmp_s("EgressNodeID", strlen("EgressNodeID"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->EgressNodeID;
        
        return TRUE;
    }
    
    rc = strcmp_s("LeaseTimeLeft", strlen("LeaseTimeLeft"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->FlowTimeLeft;
        
        return TRUE;
    }
    
    rc = strcmp_s("FlowPackets", strlen("FlowPackets"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->PacketSize;
        
        return TRUE;
    }
    
    rc = strcmp_s("MaxRate", strlen("MaxRate"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->PeakDataRate;
        
        return TRUE;
    }
    
    rc = strcmp_s("MaxBurstSize", strlen("MaxBurstSize"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->BurstSize;
        
        return TRUE;
    }
    
    rc = strcmp_s("FlowTag", strlen("FlowTag"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->FlowTag;
        
        return TRUE;
    }

    rc = strcmp_s("LeaseTime", strlen("LeaseTime"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->LeaseTime;

        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        FlowStats_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
FlowStats_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_DML_MOCA_FLOW             pConf           = (PCOSA_DML_MOCA_FLOW)hInsContext;
    errno_t                         rc       = -1;
    int                             ind      = -1;

    /* check the parameter name and return the corresponding value */
    rc = strcmp_s("PacketDA", strlen("PacketDA"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
	rc = strcpy_s(pValue, *pUlSize, (PCHAR)pConf->DestinationMACAddress);
        if (rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
        *pUlSize = _ansc_strlen(pValue);
        
        return 0;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/***********************************************************************

 APIs for Object:

    MoCA.Interface.{i}.Associated{i}.

    *  AssociatedDevice_GetEntryCount
    *  AssociatedDevice_GetEntry
    *  AssociatedDevice_IsUpdated
    *  AssociatedDevice_Synchronize
    *  AssociatedDevice_GetParamBoolValue
    *  AssociatedDevice_GetParamIntValue
    *  AssociatedDevice_GetParamUlongValue
    *  AssociatedDevice_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        AssociatedDevice_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
AssociatedDevice_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DML_MOCA_IF_FULL_TABLE    pMoCAIfFullTable    = (PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext;
    
    return pMoCAIfFullTable->ulMoCAAssocDeviceCount;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        AssociatedDevice_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
AssociatedDevice_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{    
    PCOSA_DML_MOCA_IF_FULL_TABLE    pMoCAIfFullTable    = (PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext;

    if (nIndex < pMoCAIfFullTable->ulMoCAAssocDeviceCount)
    {
        *pInsNumber  = nIndex + 1;

        return &pMoCAIfFullTable->pMoCAAssocDevice[nIndex];
    }

    return NULL; /* return the handle */
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        AssociatedDevice_IsUpdated
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is checking whether the table is updated or not.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     TRUE or FALSE.

**********************************************************************/
static ULONG last_tick;
#define REFRESH_INTERVAL 120
#define TIME_NO_NEGATIVE(x) ((long)(x) < 0 ? 0 : (x))

BOOL
AssociatedDevice_IsUpdated
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if ( !last_tick ) 
    {
        last_tick = AnscGetTickInSeconds();

        return TRUE;
    }
    
    if ( last_tick >= TIME_NO_NEGATIVE(AnscGetTickInSeconds() - MOCA_REFRESH_INTERVAL) )
    {
        return FALSE;
    }
    else 
    {
        last_tick = AnscGetTickInSeconds();

        return TRUE;
    }
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        AssociatedDevice_Synchronize
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to synchronize the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
AssociatedDevice_Synchronize
    (
        ANSC_HANDLE                 hInsContext
    )
{    
    /*PPOAM_COSAMOCADM_OBJECT         pPoamMoCADm         = (PPOAM_COSAMOCADM_OBJECT )pMyObject->pPoamMoCADm;*/
    PCOSA_DML_MOCA_IF_FULL_TABLE    pMoCAIfFullTable    = (PCOSA_DML_MOCA_IF_FULL_TABLE)hInsContext;
    void* pulMoCAAssocDeviceCount = NULL;
    void* pMoCAAssocDevice = NULL;

    if ( pMoCAIfFullTable->pMoCAAssocDevice)
    {
        AnscFreeMemory(pMoCAIfFullTable->pMoCAAssocDevice);
        
        pMoCAIfFullTable->pMoCAAssocDevice= NULL;
    }    

    pulMoCAAssocDeviceCount = &pMoCAIfFullTable->ulMoCAAssocDeviceCount;
    pMoCAAssocDevice = &pMoCAIfFullTable->pMoCAAssocDevice,
    CosaDmlMocaIfGetAssocDevices
        (
            (ANSC_HANDLE)NULL/*pPoamMoCADm*/, 
            pMoCAIfFullTable->MoCAIfFull.Cfg.InstanceNumber-1, 
            (PULONG)pulMoCAAssocDeviceCount,
            (PCOSA_DML_MOCA_ASSOC_DEVICE *)pMoCAAssocDevice,
            NULL
        );
    
    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        AssociatedDevice_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
AssociatedDevice_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_DML_MOCA_ASSOC_DEVICE     pMoCAAssocDevice    = (PCOSA_DML_MOCA_ASSOC_DEVICE)hInsContext;
    errno_t                         rc       = -1;
    int                             ind      = -1;

    /* check the parameter name and return the corresponding value */
    rc = strcmp_s("PreferredNC", strlen("PreferredNC"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *pBool = pMoCAAssocDevice->PreferredNC;
        return TRUE;
    }

    rc = strcmp_s("QAM256Capable", strlen("QAM256Capable"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *pBool = pMoCAAssocDevice->QAM256Capable;
        return TRUE;
    }

    rc = strcmp_s("Active", strlen("Active"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *pBool = pMoCAAssocDevice->Active;
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        AssociatedDevice_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
AssociatedDevice_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    /* check the parameter name and return the corresponding value */

    PCOSA_DML_MOCA_ASSOC_DEVICE     pMoCAAssocDevice    = (PCOSA_DML_MOCA_ASSOC_DEVICE)hInsContext;
    errno_t                         rc       = -1;
    int                             ind      = -1;

    rc = strcmp_s("RxPowerLevel", strlen("RxPowerLevel"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *pInt = pMoCAAssocDevice->RxPowerLevel;
        return TRUE;
    }

    rc = strcmp_s("RxBcastPowerLevel", strlen("RxBcastPowerLevel"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *pInt = pMoCAAssocDevice->RxBcastPowerLevel;
        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        AssociatedDevice_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
AssociatedDevice_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_DML_MOCA_ASSOC_DEVICE     pMoCAAssocDevice    = (PCOSA_DML_MOCA_ASSOC_DEVICE)hInsContext;
    errno_t                         rc       = -1;
    int                             ind      = -1;
 
    /* check the parameter name and return the corresponding value */
    rc = strcmp_s("NodeID", strlen("NodeID"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAAssocDevice->NodeID;
        return TRUE;
    }

    rc = strcmp_s("PHYTxRate", strlen("PHYTxRate"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAAssocDevice->PHYTxRate;
        return TRUE;
    }

    rc = strcmp_s("PHYRxRate", strlen("PHYRxRate"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAAssocDevice->PHYRxRate;
        return TRUE;
    }

    rc = strcmp_s("TxPowerControlReduction", strlen("TxPowerControlReduction"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAAssocDevice->TxPowerControlReduction;
        return TRUE;
    }

    rc = strcmp_s("TxBcastRate", strlen("TxBcastRate"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAAssocDevice->TxBcastRate;
        return TRUE;
    }

    rc = strcmp_s("TxPackets", strlen("TxPackets"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAAssocDevice->TxPackets;
        return TRUE;
    }

    rc = strcmp_s("RxPackets", strlen("RxPackets"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAAssocDevice->RxPackets;
        return TRUE;
    }

    rc = strcmp_s("RxErroredAndMissedPackets", strlen("RxErroredAndMissedPackets"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAAssocDevice->RxErroredAndMissedPackets;
        return TRUE;
    }

    rc = strcmp_s("PacketAggregationCapability", strlen("PacketAggregationCapability"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAAssocDevice->PacketAggregationCapability;
        return TRUE;
    }

    rc = strcmp_s("RxSNR", strlen("RxSNR"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAAssocDevice->RxSNR;
        return TRUE;
    }

    rc = strcmp_s("X_CISCO_COM_RxBcastRate", strlen("X_CISCO_COM_RxBcastRate"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK)) 
    {
        /* collect value */
        *puLong = pMoCAAssocDevice->X_CISCO_COM_RxBcastRate;
        return TRUE;
    }

    rc = strcmp_s("X_CISCO_COM_NumberOfClients", strlen("X_CISCO_COM_NumberOfClients"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pMoCAAssocDevice->X_CISCO_COM_NumberOfClients;
        return TRUE;
    }
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        AssociatedDevice_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
AssociatedDevice_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_DML_MOCA_ASSOC_DEVICE     pMoCAAssocDevice    = (PCOSA_DML_MOCA_ASSOC_DEVICE)hInsContext;
    errno_t                         rc       = -1;
    int                             ind      = -1;

    /* check the parameter name and return the corresponding value */
    rc = strcmp_s("MACAddress", strlen("MACAddress"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        _ansc_sprintf
            (
                pValue,
                "%02X:%02X:%02X:%02X:%02X:%02X",
                pMoCAAssocDevice->MACAddress[0],
                pMoCAAssocDevice->MACAddress[1],
                pMoCAAssocDevice->MACAddress[2],
                pMoCAAssocDevice->MACAddress[3],
                pMoCAAssocDevice->MACAddress[4],
                pMoCAAssocDevice->MACAddress[5]
            );

        *pUlSize = AnscSizeOfString(pValue);
        return 0;
    }

    rc = strcmp_s("HighestVersion", strlen("HighestVersion"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        rc = strcpy_s(pValue, *pUlSize, pMoCAAssocDevice->HighestVersion);
        if (rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
        *pUlSize = AnscSizeOfString(pValue);
        return 0;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */

    return -1;
}
/***********************************************************************

 APIs for Object:

    DeviceInfo.X_RDKCENTRAL_COM_xOpsDeviceMgmt.Logging.

    *  Logging_GetParamBoolValue
    *  Logging_GetParamUlongValue
    *  Logging_SetParamBoolValue
    *  Logging_SetParamUlongValue
    *  Logging_Validate
    *  Logging_Commit
    *  Logging_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Logging_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
Logging_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    /* check the parameter name and return the corresponding value */
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_MOCA            pMyObject           = (PCOSA_DATAMODEL_MOCA)g_MoCAObject;
    errno_t                         rc       = -1;
    int                             ind      = -1;
	
    rc = strcmp_s("xOpsDMMoCALogEnabled", strlen("xOpsDMMoCALogEnabled"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
	*pBool =  pMyObject->LogStatus.Log_Enable;
       	return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}
  
 /**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
       	Logging_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Logging_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{    
    /* check the parameter name and return the corresponding value */
	UNREFERENCED_PARAMETER(hInsContext);
        PCOSA_DATAMODEL_MOCA			pMyObject			= (PCOSA_DATAMODEL_MOCA)g_MoCAObject;
        errno_t                         rc       = -1;
        int                             ind      = -1;

    rc = strcmp_s("xOpsDMMoCALogPeriod", strlen("xOpsDMMoCALogPeriod"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
	*puLong =  pMyObject->LogStatus.Log_Period;
	return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}


/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Logging_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/


BOOL
Logging_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    /* check the parameter name and return the corresponding value */
    UNREFERENCED_PARAMETER(hInsContext);
    char buf[8]={0};
	PCOSA_DATAMODEL_MOCA			pMyObject			= (PCOSA_DATAMODEL_MOCA)g_MoCAObject;
        errno_t                         rc       = -1;
        int                             ind      = -1;
        
	
    rc = strcmp_s("xOpsDMMoCALogEnabled", strlen("xOpsDMMoCALogEnabled"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
	if(bValue)
    {
		rc = strcpy_s(buf, sizeof(buf), "true");
                if (rc != EOK)
                {
                    ERR_CHK(rc);
                    return FALSE;
                }
    }
	else
    {
		rc = strcpy_s(buf, sizeof(buf), "false");
                if (rc != EOK)
                {
                    ERR_CHK(rc);
                    return FALSE;
                }
	}
	if (syscfg_set_commit(NULL, "moca_log_enabled", buf) != 0) 
	{
		AnscTraceWarning(("syscfg_set failed\n"));
	} 
	else 
	{
		pMyObject->LogStatus.Log_Enable = bValue;
		CosaMocaTelemetryxOpsLogSettingsSync();
	}
        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}



/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Logging_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG                       uValue
            );

    description:

        This function is called to set ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG                       uValue
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Logging_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{    
    /* check the parameter name and return the corresponding value */    
    UNREFERENCED_PARAMETER(hInsContext);
	PCOSA_DATAMODEL_MOCA			pMyObject			= (PCOSA_DATAMODEL_MOCA)g_MoCAObject;
        errno_t                         rc       = -1;
        int                             ind      = -1;

    rc = strcmp_s("xOpsDMMoCALogPeriod", strlen("xOpsDMMoCALogPeriod"), ParamName, &ind );
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
		
	if (syscfg_set_u_commit(NULL, "moca_log_period", uValue) != 0) 
	{
		AnscTraceWarning(("syscfg_set failed\n"));
	} 
	else 
	{
		pMyObject->LogStatus.Log_Period = uValue;
		CosaMocaTelemetryxOpsLogSettingsSync();
	}
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Logging_Validate
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pReturnParamName,
                ULONG*                      puLength
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pReturnParamName,
                The buffer (128 bytes) of parameter name if there's a validation. 

                ULONG*                      puLength
                The output length of the param name. 

    return:     TRUE if there's no validation.

**********************************************************************/
BOOL
Logging_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
   UNREFERENCED_PARAMETER(hInsContext);
   UNREFERENCED_PARAMETER(pReturnParamName);
   UNREFERENCED_PARAMETER(puLength);
   return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Logging_Commit
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Logging_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Logging_Rollback
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to roll back the update whenever there's a 
        validation found.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Logging_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
	UNREFERENCED_PARAMETER(hInsContext);
        return 0;
}
#endif


