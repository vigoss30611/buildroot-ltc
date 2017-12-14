/*****************************************************************************
**
**  Name:           bridge_info.c
**
**  Description:    Bluetooth BLE mesh bridge application
**
**  Copyright (c) 2014, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "bridge_info.h"
#include "udp_task.h"
#include "tcp_task.h"
#include "flood_bridge_utils.h"

#include "app_utils.h"


#define NODES_FILE_NAME "flood_mesh_nodes.config"


bridge_info_t Bridge_info;
bridge_info_p pBridge_info = &Bridge_info;

void flood_bridge_search_nodes();

void flood_bridge_init()
{    
    memset(pBridge_info, 0, sizeof(bridge_info_t));

    pBridge_info->mesh_enabled = TRUE;
    pBridge_info->max_rssi = MIN_RSSI;

	flood_bridge_load_nodes_info();

	flood_bridge_get_nodes_num();

#ifdef ENABLE_UDP_BROADCAST_ADDR
    start_upd_task();
#endif
	start_tcp_task();
}


void flood_bridge_load_nodes_info()
{
	int pfd;
	int file_len = 0;

	if ((pfd = open(NODES_FILE_NAME, O_RDONLY)) >= 0)
	{
		/* Get the length of the device file */
	    file_len = app_file_size(pfd);
	    if (file_len > 0)
	    {
			int read_len = 0;
			read_len = read(pfd, (void *) pBridge_info->devices, file_len);
			if (read_len < 0)
			{
				APP_INFO0("failed to load mesh nodes\n");
			}
	    }

		close(pfd);
    }
	else
	{
		perror("No config file\n");
	}
}

void flood_bridge_store_nodes_info()
{
	int pfd;

	if ((pfd = open(NODES_FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0666)) >= 0)
	{
		int write_len = write(pfd, (void *) pBridge_info->devices, MAX_FLOOD_MESH_NODES*sizeof(tFloodMeshNode));

		if (write_len != MAX_FLOOD_MESH_NODES*sizeof(tFloodMeshNode))
		{
			APP_INFO0("failed to store mesh nodes\n");
		}
		close(pfd);
	}
}


void flood_bridge_add_node(UINT8 addr_type, INT16 node_id, BD_ADDR address)
{
	if (is_valid_bdaddr(address))
	{
		int i = 0;
		int valid_nodes_num=0,  valid_sensor_num= 0;
		int empty_slot = -1;
	    for (i = 0; i < MAX_FLOOD_MESH_NODES; i++)
	    {
			if (!is_valid_bdaddr(pBridge_info->devices[i].address))
			{
	            if (empty_slot == -1)
	            {
					empty_slot = i;
	            }
			}
            else if (0 == memcmp(address, pBridge_info->devices[i].address, BD_ADDR_LEN))
            {
				if (empty_slot == -1)
	            {
					empty_slot = i;
	            }
				memset(&pBridge_info->devices[i], 0, sizeof(tFloodMeshNode));
            }
			else
			{
			    if(pBridge_info->devices[i].addr_type == BLE_SENSOR_NODE)
                    valid_sensor_num++;
                else if(pBridge_info->devices[i].addr_type == BLE_MESH_NODE)
				    valid_nodes_num++;
			}

	    }

        APP_DEBUG1("flood_bridge_add_node addr_type:%d, empty_slot:%d, valid_sensor_num:%d, valid_nodes_num:%d", 
            addr_type, empty_slot, valid_sensor_num, valid_nodes_num);
		if ((empty_slot != -1) && (empty_slot < MAX_FLOOD_MESH_NODES))
		{
		    if(    ((addr_type == BLE_SENSOR_NODE) && (valid_sensor_num < QIWO_SENSOR_NUM))
                || ((addr_type == BLE_MESH_NODE) && (valid_nodes_num < (MAX_FLOOD_MESH_NODES-QIWO_SENSOR_NUM))) 
            )
            {      
    			memcpy(pBridge_info->devices[empty_slot].address, address, BD_ADDR_LEN);
                pBridge_info->devices[empty_slot].addr_type = addr_type;
                pBridge_info->devices[empty_slot].node_id = node_id;

			    if(addr_type == BLE_SENSOR_NODE)
                    valid_sensor_num++;
                else if(addr_type == BLE_MESH_NODE)
				    valid_nodes_num++;
             }
		}

        pBridge_info->sensor_num = valid_sensor_num;
        pBridge_info->nodes_num = valid_nodes_num;
        
		flood_bridge_store_nodes_info();
	}

	flood_bridge_search_nodes();
}

void flood_bridge_delete_node(BD_ADDR address)
{
	if (is_valid_bdaddr(address))
	{
		int i = 0, j = 0;
		int valid_nodes_num = 0;

	    for (i = 0; i < MAX_FLOOD_MESH_NODES; i++)
	    {
			if (0 == memcmp(address, pBridge_info->devices[i].address, BD_ADDR_LEN))
            {
                if(pBridge_info->devices[i].addr_type== BLE_SENSOR_NODE)
                {
                    pBridge_info->sensor_num--;
                    for(j = 0; j < QIWO_SENSOR_NUM; j++)
                    {
                        if(pBridge_info->sensor_connected[j] && (0 == memcmp(address, pBridge_info->sensor_address[j], BD_ADDR_LEN)))
                        {
                            //app_qiwo_ble_client_unpair(j + 1);
                            app_qiwo_ble_client_unpair(app_ble_client_find_index_by_conn_id(pBridge_info->sensor_conn_id[j]));
                        }
                    } 
                }
                else if(pBridge_info->devices[i].addr_type == BLE_MESH_NODE)
                {
                    pBridge_info->nodes_num--;
                    if(pBridge_info->proxy_connected && (0 == memcmp(address, pBridge_info->proxy_address, BD_ADDR_LEN)))
                    {
                        //app_qiwo_ble_client_unpair(0);
                        app_qiwo_ble_client_unpair(app_ble_client_find_index_by_conn_id(pBridge_info->proxy_conn_id));
                    }
                }
				memset(&pBridge_info->devices[i], 0, sizeof(tFloodMeshNode));
                break;
            }

	    }

	    flood_bridge_store_nodes_info();
	}
}

void flood_brige_clear_all_nodes()
{
    int i;
    
    APP_INFO0("Reset flood mesh nodes\n");
    
	memset(pBridge_info->devices, 0, sizeof(tFloodMeshNode)*MAX_FLOOD_MESH_NODES);
	flood_bridge_store_nodes_info();

	pBridge_info->nodes_num = 0;
    pBridge_info->sensor_num = 0;

	if (pBridge_info->proxy_connected)
	{
		app_qiwo_ble_client_unpair(app_ble_client_find_index_by_conn_id(pBridge_info->proxy_address));
	}
    for(i = 0; i < QIWO_SENSOR_NUM; i++)
    {
        if(pBridge_info->sensor_connected[i])
            app_qiwo_ble_client_unpair(app_ble_client_find_index_by_conn_id(pBridge_info->sensor_conn_id[i]));
    }
}


int flood_bridge_get_nodes_num()
{
	int i = 0;
	int valid_nodes_num = 0, valid_sensor_num = 0;

    for (i = 0; i < MAX_FLOOD_MESH_NODES; i++)
    {
		if (is_valid_bdaddr(pBridge_info->devices[i].address))
		{
		    if(pBridge_info->devices[i].addr_type == 1)
                valid_nodes_num++;
            else if(pBridge_info->devices[i].addr_type == 2)
                valid_sensor_num++;
		}

    }

	pBridge_info->nodes_num = valid_nodes_num;
    pBridge_info->sensor_num = valid_nodes_num;

	return 0;

}


BOOLEAN is_flood_mesh_nodes(UINT8 addr_type, BD_ADDR address)
{
    BOOLEAN result = FALSE;
    int i = 0;

    for (i=0;i<MAX_FLOOD_MESH_NODES;i++)
	{
	    //APP_INFO1("Load Address : 0x%02x%02x%02x%02x%02x%02x\n", pBridge_info->devices[i].address[5], pBridge_info->devices[i].address[4],
        //     pBridge_info->devices[i].address[3], pBridge_info->devices[i].address[2], pBridge_info->devices[i].address[1],
        //     pBridge_info->devices[i].address[0]);
		if ((0 == memcmp(pBridge_info->devices[i].address, address, BD_ADDR_LEN)) 
		                      && (addr_type == pBridge_info->devices[i].addr_type))
		{
		    APP_INFO0("Address Matched \n");
		    result = TRUE;
		    break;
		}
	}


    return result;
}

int get_flood_mesh_nodes_index(BD_ADDR address)
{
    int result = -1;
    int i = 0;

    for (i=0;i<MAX_FLOOD_MESH_NODES;i++)
	{
		if (0 == memcmp(pBridge_info->devices[i].address, address, BD_ADDR_LEN))
		{
		    result = i;
		    break;
		}
	}


    return result;
}

