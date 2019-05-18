
/**
 * \page cmds_ref_lldp LLDP
 * - \subpage lldp_management_address_cmd_vtysh
 * - \subpage lldp_packet_interval_cmd_vtysh
 * - \subpage lldp_neighbour_hold_period_cmd_vtysh
 * - \subpage interface_lldp_enable_cmd_vtysh
 * - \subpage interface_tlv_enable_basic_tlv_cmd_vtysh
 * - \subpage lldp_management_address_arp_learning_cmd_vtysh
 * - \subpage show_lldp_statistics_trunk_cmd_vtysh
 * - \subpage show_lldp_statistics_ethernet_cmd_vtysh
 * - \subpage show_lldp_local_trunk_cmd_vtysh
 * - \subpage show_lldp_local_ethernet_cmd_vtysh
 * - \subpage show_lldp_neighbor_trunk_cmd_vtysh
 * - \subpage show_lldp_neighbor_ethernet_cmd_vtysh
 * - \subpage show_lldp_tlv_config_trunk_cmd_vtysh
 * - \subpage show_lldp_tlv_config_ethernet_cmd_vtysh

 */


/**
 * \page modify_log_l2 Modify Log
 * \section l2-v007r004 HiOS-V007R004
 *  -# 
 * \section l2-v007r003 HiOS-V007R003
 *  -# 
 */




#include <zebra.h>
#include <lib/ifm_common.h>
#include "lib/command.h"
#include "vtysh.h"



static struct cmd_node lldp_node =
{
  LLDP_NODE,
  "%s(config-lldp)# ",
};



/**
 * \page interface_tlv_enable_basic_tlv_cmd_vtysh [no]lldp tlv-enable basic-tlv (all | port-description | system-name | system-description | system-capability | management-address)
 * - 功能说明 \n
 *	 config port lldp tlv
 * - 命令格式 \n
 *	 [no]lldp tlv-enable basic-tlv (all | port-description | system-name | system-description | system-capability | management-address)
 * - 参数说明 \n
 *   |参数 	          |说明  	             |
 *   |----------------|-----------------|
 *   | all | Enable basic all TLV|
 *   | port-description | Enable basic port description TLV|
 *   | system-name | Enable basic system name TLV|
 *   | port-description | Enable basic port description TLV|
 *   | system-capability | Enable basic system capability TLV|
 *   | management-address | Enable basic management address TLV| 
 * - 缺省情况 \n
 *   支持基础tlv
 * - 命令模式 \n
 *	 interface view mode
 * - 用户等级 \n
 *	 5
 * - 使用指南 \n
 *	 - 应用场景 \n
 *     config port lldp tlv
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   no lldp tlv-enable basic-tlv (all | port-description | system-name | system-description | system-capability | management-address)
 * - 使用举例 \n
 *	 无
 *
 */

/**
* \page show_lldp_statistics_ethernet_cmd_vtysh show lldp statistics interface (ethernet |gigabitethernet |xgigabitethernet) USP
* - 功能说明 \n
*	show lldp trunk statistics info
* - 命令格式 \n
*	show lldp statistics interface (ethernet |gigabitethernet |xgigabitethernet) USP
* - 参数说明 \n
*	|参数 			 |说明					  |
*	|----------------|----------------------------|
*	|USP			 |format: <0-7>/<0-31>/<1-255>|
* - 缺省情况 \n
*   show all interface statistics
* - 命令模式 \n
*	config view mode
*   interface view mode
* - 用户等级 \n
*	5
* - 使用指南 \n
*	- 应用场景 \n
*	  show statistics
*	- 前置条件 \n
*	  无
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	  无
* - 使用举例 \n
* Hios(config)# show lldp statistics interface				   \n																	
													   \n																			
* Statistics for gigabitethernet 1/1/5 :					   \n																	
* Transmitted Frames Total		:  1					   \n																		
* Received Frames Total 			:  1					   \n																		
* Frames Discarded Total		:  0						   \n																	
* Frames Error Total			:  0						   \n																	
* TLVs Discarded Total			:  0						   \n																	
* TLVs Unrecognized Total		:  0					   \n																		
* Neighbors Expired Total		:  0					   \n																		
* Hios(config)# 										   \n																			
* Hios(config)# 										  \n
*\n
* \n
* Hios(config)# show lldp statistics interface gigabitethernet 1/1/5   \n																
*																							  \n									   
*Statistics for gigabitethernet 1/1/5 : 										 \n 												   
*Transmitted Frames Total		 :	2											  \n												   
*Received Frames Total			 :	1											   \n												   
*Frames Discarded Total 		 :	0											 \n 												   
*Frames Error Total 			 :	0												\n												   
*TLVs Discarded Total			 :	0											  \n												   
*TLVs Unrecognized Total		 :	0										   \n													   
*Neighbors Expired Total		 :	0											\n													   
*Hios(config)#																		 \n 											   
*Hios(config)#																		\n												   
*Hios(config)#																		\n
*/

/**
 * \page show_lldp_local_trunk_cmd_vtysh show  lldp local interface { trunk TRUNK}
 * - 功能说明 \n
 *	 show lldp local interface 
 * - 命令格式 \n
 *	 show lldp local interface { trunk TRUNK}
 * - 参数说明 \n
 *   |参数 	          |说明  	       |
 *   |----------------|----------------|
 *   |TRUNK     	  |format: <1-128>|
 * - 缺省情况 \n
 *   show all interface info
 * - 命令模式 \n
 *	 config view mode
 *	 interface view mode
 * - 用户等级 \n
 *	 5
 * - 使用指南 \n
 *	 - 应用场景 \n
 *     show interface info
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *     无
 * - 使用举例 \n
 *  Hios(config)# show lldp local interface                                                   \n                                          
 *                                                                                                           \n                         
 *sys infomation:                                                                                       \n                              
 *Chassis type       		: macAddress                                                           \n                                          
 *Chassis ID         		: 00:1d:80:01:fc:03                                                    \n                                          
 *System name        	: h20rn-161                                                                 \n                                     
 *System description 	: huahuan h20rn-161, HuaHuan Versatile Routing Platform, Soft \n                                                   
 *ware version: v000r001c074sp000                                                                        \n                             
 *                                                                                                                     \n               
 *System configuration:                                                                                     \n                          
 *LLDP Message Tx Interval                	: 4             (default is 30s)                    \n                                        
 *LLDP Message Tx Hold Multiplier         	: 4             (default is 4)                       \n                                       
 *LLDP Management Address                 	: MAC[00:1d:80:01:fc:03]        (default    \n                                                
 * is MAC Address)                                                                                \n                                    
 *                                                                                                \n                                   
 *Port information:                                                                      \n                                             
 *Interface gigabitethernet 1/1/5 :                                               \n                                                    
 *LLDP Enable Status              	: enable                                     \n                                                       
 *Port link Status                		: link up                                     \n                                                      
 *Total Neighbors                 	: 1                                              \n                                                   
 *Port ID subtype                 	: interface name                            \n                                                        
 *Port ID                         		: gigabitethernet 1/1/5                 \n                                                            
 *                                                                                              \n                                      
 *Interface trunk 1       			:                                                \n                                                           
 *LLDP Enable Status              	: enable                                       \n                                                     
 *Port link Status                		: link up                                      \n                                                     
 *Total Neighbors                 	: 1                                            \n                                                     
 *Port ID subtype                 	: interface name                        \n                                                            
 *Port ID                         		: trunk 1                                \n                                                           
 *                                                                          \n
*/

/**
 * \page show_lldp_local_ethernet_cmd_vtysh show lldp local interface (ethernet |gigabitethernet |xgigabitethernet) USP
 * - 功能说明 \n
 *	 show lldp local interface 
 * - 命令格式 \n
 *	 show lldp local interface (ethernet |gigabitethernet |xgigabitethernet) USP
 * - 参数说明 \n
 *   |参数 	          |说明  	                                  |
 *   |----------------|----------------------------|
 *   |USP     	          |format: <0-7>/<0-31>/<1-255>|
 * - 缺省情况 \n
 *   show all interface info
 * - 命令模式 \n
 *	 config view mode
 *	 interface view mode
 * - 用户等级 \n
 *	 5
 * - 使用指南 \n
 *	 - 应用场景 \n
 *     show interface info
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *     无
 * - 使用举例 \n
 * Hios(config)# show lldp local interface                                                    \n                                         
 *                                                                                                            \n                        
 *sys infomation:                                                                                     \n                                
 *Chassis type       		: macAddress                                                          \n                                           
 *Chassis ID         		: 00:1d:80:01:fc:03                                                 \n                                             
 *System name        	: h20rn-161                                                                           \n                           
 *System description 	: huahuan h20rn-161, HuaHuan Versatile Routing Platform, Soft    \n                                                
 *ware version: v000r001c074sp000                                                                          \n                           
 *                                                                                                                         \n           
 *System configuration:                                                                                         \n                      
 *LLDP Message Tx Interval                	: 4             (default is 30s)                        \n                                    
 *LLDP Message Tx Hold Multiplier         	: 4             (default is 4)                          \n                                    
 *LLDP Management Address                 	: MAC[00:1d:80:01:fc:03]        (default       \n                                             
 * is MAC Address)                                                                      \n                                              
 *                                                                                           \n                                         
 *Port information:                                                                    \n                                               
 *Interface gigabitethernet 1/1/5 :                                              \n                                                     
 *LLDP Enable Status              	: enable                                    \n                                                        
 *Port link Status                		: link up                                    \n                                                       
 *Total Neighbors                 	: 1                                            \n                                                     
 *Port ID subtype                 	: interface name                          \n                                                          
 *Port ID                         		: gigabitethernet 1/1/5                \n                                                             
 *                                                                                            \n                                        
 *Interface trunk 1       			:                                               \n                                                            
 *LLDP Enable Status              	: enable                                     \n                                                       
 *Port link Status                		: link up                                    \n                                                       
 *Total Neighbors                 	: 1                                           \n                                                      
 *Port ID subtype                 	: interface name                        \n                                                            
 *Port ID                         		: trunk 1                                 \n                                                          
 *                                                                                 
*/


/**
 * \page show_lldp_neighbor_trunk_cmd_vtysh show lldp neighbor interface {trunk TRUNK}
 * - 功能说明 \n
 *	 show lldp neighbor interface 
 * - 命令格式 \n
 *	 show lldp neighbor interface { trunk TRUNK}
 * - 参数说明 \n
 *   |参数 	          |说明  	                                  |
 *   |----------------|----------------------------|
 *   |TRUNK     	          |format: <1-128>|
 * - 缺省情况 \n
 *   show all interface neighbor info
 * - 命令模式 \n
 *	 config view mode
 *	 interface view mode
 * - 用户等级 \n
 *	 5
 * - 使用指南 \n
 *	 - 应用场景 \n
 *     show interface neighbor info
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *     无
 * - 使用举例 \n
 *Hios(config)#                                                                           \n                                            
 *Hios(config)# show lldp neighbor interface                                     \n                                                     
 *                                                                                                \n                                    
 *Interface gigabitethernet 1/1/5 has 1 neighbors:                            \n                                                        
 *                                                                                                \n                                    
 *                                                                                                \n                                    
 *Neighbor index                  	:  1                                                 \n                                               
 *svlan type                      		:                                                      \n                                             
 *svlan id                        		:                                                      \n                                             
 *cvlan type                      		:                                                      \n                                             
 *cvlan id                        		:                                                    \n                                               
 *Chassis type                    		:  macAddress                                   \n                                                    
 *Chassis ID                      		:  00:1d:80:01:fc:25                          \n                                                      
 *Port ID type                    		:  InterfaceName                                \n                                                    
 *Port ID                         		:  gigabitethernet 1/1/5                       \n                                                     
 *Expired time                    		:  120s                                                \n                                             
 *Port description                		:  gigabitethernet 1/1/5                             \n                                               
 *System name                     	:  HT201                                                       \n                                     
 *System description              	:  HuaHuan HT201, HuaHuan Versatile Routing Plat        \n                                            
 *form, Software version: V001R001C73                                               \n                                                  
 *System capabilities supported   :  Bridge   Router                               \n                                                   
 *System capabilities enabled     	:  Bridge   Router                                \n                                                  
 *Management address type      	:  MAC Address                                   \n                                                   
 *Management address           	:  00:1d:80:01:fc:25                           \n                                                     
 *                                                                                                       \n                             
 *Interface trunk 1 has 1 neighbors:                                                       \n                                           
 *                                                                                                        \n                            
 *                                                                                                        \n                            
 *Neighbor index                  	:  1                                                      \n                                          
 *svlan type                      		:                                                         \n                                          
 *svlan id                        		:                                                         \n                                          
 *cvlan type                      		:                                                         \n                                          
 *cvlan id                        		:                                                         \n                                          
 *Chassis type                    		:  macAddress                                        \n                                               
 *Chassis ID                      		:  00:1d:80:01:fc:25                               \n                                                 
 *Port ID type                    		:  InterfaceName                                       \n                                             
 *Port ID                         		:  trunk 1                                                 \n                                         
 *Expired time                    		:  120s                                                      \n                                       
 *Port description                		:  trunk 1                                                    \n                                      
 *System name                     	:  HT201                                                         \n                                   
 *System description              	:  HuaHuan HT201, HuaHuan Versatile Routing Plat   \n                                                 
 *form, Software version: V001R001C73                                         \n                                                        
 *System capabilities supported  	:  Bridge   Router                        \n                                                          
 *System capabilities enabled    	:  Bridge   Router                       \n                                                           
 *Management address type     	:  MAC Address                          \n                                                            
 *Management address            	:  00:1d:80:01:fc:25                 \n                                                               
 *Hios(config)#                                                                       \n                                                
 *Hios(config)#                                                                      \n                                                 
 *Hios(config)#                                             \n
*/

/**
 * \page show_lldp_neighbor_ethernet_cmd_vtysh show lldp neighbor interface (ethernet |gigabitethernet |xgigabitethernet) USP
 * - 功能说明 \n
 *	 show lldp neighbor interface 
 * - 命令格式 \n
 *	 show lldp neighbor interface (ethernet |gigabitethernet |xgigabitethernet) USP
 * - 参数说明 \n
 *   |参数 	          |说明  	                                  |
 *   |----------------|----------------------------|
 *   |USP     	          |format: <0-7>/<0-31>/<1-255>|
 * - 缺省情况 \n
 *   show all interface neighbor info
 * - 命令模式 \n
 *	 config view mode
 *	 interface view mode
 * - 用户等级 \n
 *	 5
 * - 使用指南 \n
 *	 - 应用场景 \n
 *     show interface neighbor info
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *     无
 * - 使用举例 \n
 *Hios(config)#                                                                               \n                                        
 *Hios(config)# show lldp neighbor interface                                        \n                                                  
 *                                                                                                      \n                              
 *Interface gigabitethernet 1/1/5 has 1 neighbors:                                     \n                                               
 *                                                                                                          \n                          
 *                                                                                                           \n                         
 *Neighbor index                  	:  1                                                          \n                                      
 *svlan type                      		:                                                              \n                                     
 *svlan id                        		:                                                               \n                                    
 *cvlan type                      		:                                                                \n                                   
 *cvlan id                        		:                                                               \n                                    
 *Chassis type                    		:  macAddress                                            \n                                           
 *Chassis ID                      		:  00:1d:80:01:fc:25                                   \n                                             
 *Port ID type                    		:  InterfaceName                                         \n                                           
 *Port ID                         		:  gigabitethernet 1/1/5                                \n                                            
 *Expired time                    		:  120s                                                         \n                                    
 *Port description                		:  gigabitethernet 1/1/5                                  \n                                          
 *System name                     	:  HT201                                                        \n                                    
 *System description              	:  HuaHuan HT201, HuaHuan Versatile Routing Plat    \n                                                
 *form, Software version: V001R001C73                                               \n                                                  
 *System capabilities supported   :  Bridge   Router                               \n                                                   
 *System capabilities enabled     	:  Bridge   Router                               \n                                                   
 *Management address type      	:  MAC Address                                   \n                                                   
 *Management address           	:  00:1d:80:01:fc:25                           \n                                                     
 *                                                                                                     \n                              
 *Interface trunk 1 has 1 neighbors:                                                       \n                                           
 *                                                                                                    \n                              
 *                                                                                                    \n                              
 *Neighbor index                  	:  1                                                      \n                                          
 *svlan type                      		:                                                           \n                                        
 *svlan id                        		:                                                          \n                                         
 *cvlan type                      		:                                                           \n                                        
 *cvlan id                        		:                                                           \n                                        
 *Chassis type                    		:  macAddress                                          \n                                             
 *Chassis ID                      		:  00:1d:80:01:fc:25                                  \n                                              
 *Port ID type                    		:  InterfaceName                                         \n                                           
 *Port ID                         		:  trunk 1                                                   \n                                       
 *Expired time                    		:  120s                                                       \n                                      
 *Port description                		:  trunk 1                                                     \n                                     
 *System name                     	:  HT201                                                         \n                                   
 *System description              	:  HuaHuan HT201, HuaHuan Versatile Routing Plat   \n                                                 
 *form, Software version: V001R001C73                                            \n                                                     
 *System capabilities supported  	:  Bridge   Router                          \n                                                        
 *System capabilities enabled    	:  Bridge   Router                          \n                                                        
 *Management address type     	:  MAC Address                              \n                                                        
 *Management address            	:  00:1d:80:01:fc:25                      \n                                                          
 *Hios(config)#                                                                           \n                                            
 *Hios(config)#                                                                           \n                                            
 *Hios(config)#                                                                           \n                                
*/


/**
 * \page show_lldp_tlv_config_trunk_cmd_vtysh show lldp tlv-config interface { trunk TRUNK}
 * - 功能说明 \n
 *	 show lldp  interface  tlv config
 * - 命令格式 \n
 *	 show lldp tlv-config interface { trunk TRUNK}
 * - 参数说明 \n
 *   |参数 	          |说明  	      |
 *   |----------------|-------------- |
 *   |TRUNK     	  |format: <1-128>|
 * - 缺省情况 \n
 *   show all interface  tlv config info
 * - 命令模式 \n
 *	 config view mode
 *	 interface view mode
 * - 用户等级 \n
 *	 5
 * - 使用指南 \n
 *	 - 应用场景 \n
 *     show interface  tlv config info
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *     无
 * - 使用举例 \n                                                       
 *Hios(config)# show lldp tlv-config interface                          \n                                                              
 *LLDP tlv-config of port[gigabitethernet 1/1/5]:                       \n                                                              
 *name                                    			state          Default      \n                                                               
 *Basic optional TLV:                                                              \n                                                   
 *Port Description TLV                      		Yes            Yes            \n                                                            
 *System Name TLV                           	Yes            Yes           \n                                                             
 *System Description TLV                    	Yes            Yes           \n                                                             
 *System Capabilities TLV                   	Yes            Yes           \n                                                             
 *Management Address TLV                    	Yes            Yes           \n                                                             
 *LLDP tlv-config of port[trunk 1]:                                            \n                                                       
 *name                                     		state          Default      \n                                                               
 *Basic optional TLV:                                                              \n                                                   
 *Port Description TLV                      		Yes            Yes           \n                                                             
 *System Name TLV                           	Yes            Yes          \n                                                              
 *System Description TLV                    	Yes            Yes          \n                                                              
 *System Capabilities TLV                   	Yes            Yes          \n                                                              
 *Management Address TLV                    	Yes            Yes          \n
*/

/**
 * \page show_lldp_tlv_config_ethernet_cmd_vtysh show lldp tlv-config interface (ethernet | gigabitethernet | xgigabitethernet) USP
 * - 功能说明 \n
 *	 show lldp  interface tlv config
 * - 命令格式 \n
 *	 show lldp tlv-config interface (ethernet |gigabitethernet |xgigabitethernet) USP
 * - 参数说明 \n
 *   |参数 	          |说明  	                   |
 *   |----------------|----------------------------|
 *   |USP     	      |format: <0-7>/<0-31>/<1-255>|
 * - 缺省情况 \n
 *   show all interface tlv config info
 * - 命令模式 \n
 *	 config view mode
 *	 interface view mode
 * - 用户等级 \n
 *	 5
 * - 使用指南 \n
 *	 - 应用场景 \n
 *     show interface tlv config info
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *     无
 * - 使用举例 \n
 * Hios(config)# show lldp tlv-config interface                             \n                                                           
 * LLDP tlv-config of port[gigabitethernet 1/1/5]:                        \n                                                             
 * name                                     		state          Default        \n                                                             
 * Basic optional TLV:                                                                 \n                                                
 * Port Description TLV                      		Yes            Yes             \n                                                           
 * System Name TLV                           	Yes            Yes             \n                                                           
 * System Description TLV                   	Yes            Yes              \n                                                          
 * System Capabilities TLV                   	Yes            Yes             \n                                                           
 * Management Address TLV                    	Yes            Yes             \n                                                           
 * LLDP tlv-config of port[trunk 1]:                                              \n                                                     
 * name                                     		state          Default        \n                                                             
 * Basic optional TLV:                                                                 \n                                                
 * Port Description TLV                      		Yes            Yes              \n                                                          
 * System Name TLV                           	Yes            Yes              \n                                                          
 * System Description TLV                    	Yes            Yes              \n                                                          
 * System Capabilities TLV                   	Yes            Yes              \n                                                          
 * Management Address TLV                    	Yes            Yes              \n                                                          
 * Hios(config)#                                                                        \n                                               
 * Hios(config)#                                                          \n
 * Hios(config)#                                                                           \n                                            
 * Hios(config)# show lldp tlv-config interface gigabitethernet 1/1/5  \n                                                                
 * LLDP tlv-config of port[gigabitethernet 1/1/5]:                            \n                                                         
 * name                                     		state          Default           \n                                                          
 * Basic optional TLV:                                                                   \n                                              
 * Port Description TLV                     	 	Yes            Yes               \n                                                         
 * System Name TLV                           	Yes            Yes               \n                                                         
 * System Description TLV                    	Yes            Yes                \n                                                        
 * System Capabilities TLV                   	Yes            Yes                 \n                                                       
 * Management Address TLV                    	Yes            Yes                \n                                                        
 * Hios(config)#                                                                  \n
*/

/**
 * \page show_lldp_statistics_trunk_cmd_vtysh show lldp statistics interface { trunk TRUNK}
 * - 功能说明 \n
 *	 show lldp trunk statistics info
 * - 命令格式 \n
 *	 show lldp statistics interface { trunk TRUNK}
 * - 参数说明 \n
 *   |参数 	          |说明  	             |
 *   |----------------|-----------------|
 *   |TRUNK     	         |range of TRUNK id format: <1-128>|
 * - 缺省情况 \n
 *   show all interface statistics
 * - 命令模式 \n
 *	 config view mode
 *	 interface view mode
 * - 用户等级 \n
 *	 5
 * - 使用指南 \n
 *	 - 应用场景 \n
 *     show statistics
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *     无
 * - 使用举例 \n
 *Hios(config)# show lldp statistics interface					\n																	 
 *														\n																			 
 * Statistics for gigabitethernet 1/1/5 : 						\n																	 
 *Transmitted Frames Total		 :	8						\n																	 
 * Received Frames Total		 :	5							\n																	 
 * Frames Discarded Total 		 :	0							\n																	 
 * Frames Error Total 			 :	0							\n																	 
 * TLVs Discarded Total			 :	0							\n																	 
 * TLVs Unrecognized Total		 :	0							\n																	 
 * Neighbors Expired Total		 :	0							\n																	 
 *															\n																		 
 * Statistics for trunk 1 :										\n																	 
 * Transmitted Frames Total		 :	3							\n																	 
 * Received Frames Total		 :	1							\n																	 
 * Frames Discarded Total 		 :	0							\n																	 
 * Frames Error Total 			 :	0							\n																	 
 * TLVs Discarded Total			 :	0							\n																	 
 * TLVs Unrecognized Total		 :	0							\n																	 
 * Neighbors Expired Total		 :	0							\n																	 
 * Hios(config)#											\n				   
 * \n
 *\n
 * Hios(config)#                                             \n                                                 
 *Hios(config)# show lldp statistics interface trunk 1                 \n                                                               
 *                                                        \n                                                
 *Statistics for trunk 1 :                                  \n                                                        
 *Transmitted Frames Total        	:  7                                  \n                                                              
 *Received Frames Total           	:  2                                 \n                                                               
 *Frames Discarded Total          	:  0                                \n                                                                
 *Frames Error Total              	:  0                                 \n                                                               
 *TLVs Discarded Total            	:  0                                \n                                                                
 *TLVs Unrecognized Total         	:  0                                 \n                                                               
 *Neighbors Expired Total         	:  0                                \n                                                                
 *Hios(config)#                                                  \n
 */


/**
 * \page  lldp_management_address_cmd_vtysh lldp management-address (ip | mac)
 * - 功能说明 \n
 *	 config management address type
 * - 命令格式 \n
 *	 lldp management-address (ip | mac)
 * - 参数说明 \n
 *   |参数 	          |说明  	 |
 *   |----------------|----------|
 *   |ip              | ip addr |
 *   |mac   		  | mac addr |
 * - 缺省情况 \n
 *   mac addr
 * - 命令模式 \n
 *	 config view mode
 * - 用户等级 \n
 *	 5
 * - 使用指南 \n
 *	 - 应用场景 \n
 *     config management address type
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */


/**
 * \page interface_lldp_enable_cmd_vtysh [no]lldp enable
 * - 功能说明 \n
 *	 config port enable
 * - 命令格式 \n
 *	 [no]lldp enable
 * - 参数说明 \n
 *   无
 * - 缺省情况 \n
 *   无
 * - 命令模式 \n
 *   interface view mode
 * - 用户等级 \n
 * 5
 * - 使用指南 \n
 *	 - 应用场景 \n
 *     config port enable
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *     no lldp enable
 * - 使用举例 \n
 * 无
 *
 */

/**
 * \page lldp_packet_interval_cmd_vtysh [no]lldp packet interval <1-3600>
 * - 功能说明 \n
 *   config lldp packet interval
 * - 命令格式 \n
 *   lldp packet interval <1-3600>
 * - 参数说明 \n
 *   |参数 	          |说明  	        |
 *   |----------------|-----------------|
 *   |<1-3600>     	  |range of packet interval |
 * - 缺省情况 \n
 *   30
 * - 命令模式 \n
 *	 config view mode
 * - 用户等级 \n
 *	 5
 * - 使用指南 \n
 *	 - 应用场景 \n
 *     config lldp packet interval
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   no lldp packet interval
 * - 使用举例 \n
 *	   无
 *
 */

/**
 * \page lldp_neighbour_hold_period_cmd_vtysh [no]lldp neighbour hold-period <2-10>
 * - 功能说明 \n
 *   config lldp hold period
 * - 命令格式 \n
 *   lldp neighbour hold-period <2-10>
 * - 参数说明 \n
 *   |参数 	          |说明  	       |
 *   |----------------|-----------------|
 *   |<2-10>     	  |range of hold period|
 * - 缺省情况 \n
 *  4
 * - 命令模式 \n
 *	 config view mode
 * - 用户等级 \n
 *	 5
 * - 使用指南 \n
 *	 - 应用场景 \n
 *     config lldp hold period
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   no lldp neighbour hold-period
 * - 使用举例 \n
 *	   无
 *
 */


 /**
 * \page  lldp_management_address_arp_learning_cmd_vtysh [no]lldp management-address arp-learning {vlan <1-4094>}
 * - 功能说明 \n
 *	  配置接口收到LLDP报文后下发ARP表项
 * - 命令格式 \n
 *	 lldp management-address arp-learning {vlan <1-4094>}
 *   no lldp management-address arp-learning {vlan <1-4094>}
 * - 参数说明 \n
 *   |参数 	          |说明  	 |
 *   |----------------|----------|
 *   |vlan              | 指定Dot1q终结中三层以太网子接口关联的VLAN ID |
 *   |<1-4094>   		  | 取值范围为1~4094 |
 * - 缺省情况 \n
 *  接口收到LLDP报文后不下发ARP表项
 * - 命令模式 \n
 *	 以太网接口视图
 * - 用户等级 \n
 *	 5
 * - 使用指南 \n
 *	 - 应用场景 \n
 *     配置本命令后，当接口收到携带IPv4格式Management Address TLV的LLDP报文后，
 *     会生成该报文携带的管理地址与报文源MAC地址组成的ARP表项。
 *	 - 前置条件 \n
 *	   需要配置LLDP管理地址为IP类型，使能LLDP且使能管理地址tlv。
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *     命令配置仅支持三层接口配置
 *	   指定vlan参数后，下发ARP表项到该VLAN ID关联的三层以太网子接口。
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */

/*set management address type*/
DEFSH(VTYSH_L2, lldp_management_address_cmd_vtysh,
        "lldp management-address (ip | mac)",
        LLDP_STR
        "Management address for Network managment\n"
        "ip address\n"
        "mac address\n")




/*set / clear packet interval*/        
DEFSH(VTYSH_L2, lldp_packet_interval_cmd_vtysh,
        "lldp packet interval <1-3600>",
        LLDP_STR
        "LLDP packet\n"
        "The interval at which LLDP frames are transmitted\n"
        "The default value is 30s. Please input an integer from 1 to 3600\n")

DEFSH(VTYSH_L2, 
		no_lldp_packet_interval_cmd_vtysh,
        "no lldp packet interval",
        NO_STR
        LLDP_STR
        "LLDP packet\n"
        "The interval at which LLDP frames are transmitted\n")



/*set / clear hold-period*/ 	   
DEFSH(VTYSH_L2, 
		lldp_neighbour_hold_period_cmd_vtysh,
        "lldp neighbour hold-period <2-10>",
        LLDP_STR
        "a equipment or port which support LLDP function\n"	
        "A multiplier on the msgTxInterval that determines the actual TTL value use in a LLDPDU\n"
        "The default value is 4. Please input an integer from 2 to 10\n")

DEFSH(VTYSH_L2, 
		no_lldp_neighbour_hold_period_cmd_vtysh,
        "no lldp neighbour hold-period",
        NO_STR
        LLDP_STR
        "A equipment or port which support LLDP function\n"	
        "A multiplier on the msgTxInterval that determines the actual TTL value use in a LLDPDU\n")

/*set / clear trap-interval*/ 	
#if 0
DEFSH(VTYSH_L2, 
		lldp_trap_interval_cmd_vtysh,
        "lldp trap-interval <5-3600>",
        LLDP_STR
        "This parameter controls the trap-interval of LLDP notifications\n"	
        "The default value is 5s. Please input an integer from 5 to 3600\n")

DEFSH(VTYSH_L2, 
		no_lldp_trap_interval_cmd_vtysh,
        "no lldp trap-interval",   
        NO_STR
        LLDP_STR
		"This parameter controls the trap-interval of LLDP notifications\n")
#endif		


DEFSH(VTYSH_L2, 
		interface_lldp_enable_cmd_vtysh,
		"lldp enable",
		LLDP_STR
		"Enable LLDP protocol\n")


DEFSH(VTYSH_L2, 
		no_interface_lldp_enable_cmd_vtysh,
        "no lldp enable",
        NO_STR
        LLDP_STR
        "Enable LLDP protocol\n")



 
DEFSH(VTYSH_L2, 
		interface_tlv_enable_basic_tlv_cmd_vtysh,
		"lldp tlv-enable basic-tlv (all | port-description | \
									system-name | system-description \
									| system-capability | management-address)",
		LLDP_STR
		"Enable optional TLV\n"
		"LLDP basic TLV configuration\n"
		"Enable basic all TLV\n"	
		"Enable basic port description TLV\n"
		"Enable basic system name TLV\n"
		"Enable basic system description TLV\n"
		"Enable basic system capability TLV\n"
		"Enable basic management address TLV\n")

DEFSH(VTYSH_L2, 
		no_interface_tlv_enable_basic_tlv_cmd_vtysh,
		"no lldp tlv-enable basic-tlv (all | port-description | \
									system-name | system-description \
									| system-capability | management-address)",
		NO_STR
		LLDP_STR
		"Enable optional TLV\n"
		"LLDP basic TLV configuration\n"
		"Enable basic all TLV\n"	
		"Enable basic port description TLV\n"
		"Enable basic system name TLV\n"
		"Enable basic system description TLV\n"
		"Enable basic system capability TLV\n"
		"Enable basic management address TLV\n")








DEFSH (VTYSH_L2, show_lldp_statistics_trunk_cmd_vtysh,
        "show lldp statistics interface { trunk TRUNK}",
        SHOW_STR
        LLDP_STR        
        "LLDP statistics information\n"
        "Interface\n"
	    CLI_INTERFACE_TRUNK_STR
        "The port of trunk, format: <1-128>\n")


		
DEFSH (VTYSH_L2, show_lldp_statistics_ethernet_cmd_vtysh,
        "show lldp statistics interface (ethernet |gigabitethernet |xgigabitethernet) USP",
        SHOW_STR
        LLDP_STR        
        "LLDP statistics information\n"
        "Interface\n"
         "Ethernet interface type\n"
        "GigabitEthernet interface type\n"
        "10GigabitEthernet interface type\n"
        "The port/subport of the interface,  format: <0-7>/<0-31>/<1-255>\n")






DEFSH (VTYSH_L2, show_lldp_local_trunk_cmd_vtysh,
        "show lldp local interface { trunk TRUNK}",
        SHOW_STR
        LLDP_STR        
        "LLDP information about the local device or ports\n"
        "Interface\n"
		CLI_INTERFACE_TRUNK_STR
        "The port of trunk, format: <1-128>\n")




DEFSH (VTYSH_L2, show_lldp_local_ethernet_cmd_vtysh,
        "show lldp local interface (ethernet |gigabitethernet |xgigabitethernet) USP",
        SHOW_STR
        LLDP_STR        
        "LLDP information about the local device or ports\n"
        "Interface\n"
	 "Ethernet interface type\n"
	"GigabitEthernet interface type\n"
	"10GigabitEthernet interface type\n"
        "The port/subport of the interface,  format: <0-7>/<0-31>/<1-255>\n")




DEFSH (VTYSH_L2, show_lldp_neighbor_trunk_cmd_vtysh,
        "show lldp neighbor interface {trunk TRUNK}",
        SHOW_STR
        LLDP_STR        
        "Neighbor information\n"
        "Interface\n"
		CLI_INTERFACE_TRUNK_STR
        "The port of trunk, format: <1-128>\n")





DEFSH (VTYSH_L2, show_lldp_neighbor_ethernet_cmd_vtysh,
        "show lldp neighbor interface (ethernet |gigabitethernet |xgigabitethernet) USP",
        SHOW_STR
        LLDP_STR        
        "Neighbor information\n"
        "Interface\n"
	 "Ethernet interface type\n"
	"GigabitEthernet interface type\n"
	"10GigabitEthernet interface type\n"
        "The port/subport of the interface,  format: <0-7>/<0-31>/<1-255>\n")



DEFSH (VTYSH_L2, show_lldp_tlv_config_trunk_cmd_vtysh,
        "show lldp tlv-config interface { trunk TRUNK}",
        SHOW_STR
        LLDP_STR        
        "Enable TLV information\n"
        "Interface\n"
		CLI_INTERFACE_TRUNK_STR
        "The port of trunk, format: <1-128>\n")



DEFSH (VTYSH_L2, show_lldp_tlv_config_ethernet_cmd_vtysh,
        "show lldp tlv-config interface (ethernet |gigabitethernet |xgigabitethernet) USP",
        SHOW_STR
        LLDP_STR        
        "Enable TLV information\n"
        "Interface\n"
	 "Ethernet interface type\n"
	"GigabitEthernet interface type\n"
	"10GigabitEthernet interface type\n"
        "The port/subport of the interface,  format: <0-7>/<0-31>/<1-255>\n")
        


#if 0
DEFSH(VTYSH_L2, 
		interface_tlv_enable_dot1_tlv_cmd_vtysh,
		"lldp tlv-enable dot1-tlv(all | port-vlan-id | protocol-vlan-id \
									| vlan-name | protocol-identity)",
		LLDP_STR
		"Enable optional TLV\n"
		"Enable 802.1 TLV\n"
		"Enable 802.1 all TLV\n"
		"Enable 802.1 port vlan id TLV\n"	
		"Enable 802.1 protocol vlan TLV\n"
		"Enable 802.1 vlan name TLV\n"
		"Enable 802.1 protocol identity TLV\n")

DEFSH(VTYSH_L2, 
		no_interface_tlv_enable_dot1_tlv_cmd_vtysh,
		"no lldp tlv-enable dot1-tlv(all | port-vlan-id | protocol-vlan-id \
									| vlan-name | protocol-identity)",
		NO_STR
		LLDP_STR
		"Enable optional TLV\n"
		"Enable 802.1 TLV\n"
		"Enable 802.1 all TLV\n"
		"Enable 802.1 port vlan id TLV\n"	
		"Enable 802.1 protocol vlan TLV\n"
		"Enable 802.1 vlan name TLV\n"
		"Enable 802.1 protocol identity TLV\n")

DEFSH(VTYSH_L2, 
		interface_tlv_enable_dot3_tlv_cmd_vtysh,
		"lldp tlv-enable dot3-tlv(all | mac-physic | power | link-aggregation \
									| max-frame-size)",
		LLDP_STR
		"Enable optional TLV\n"
		"Enable 802.3 TLV\n"
		"Enable 802.3 all TLV\n"
		"Enable 802.3 MAC/PHY configuration TLV\n"	
		"Enable 802.3 power TLV\n"
		"Enable 802.3 link aggregation TLV\n"
		"Enable 802.3 max frame size TLV\n")

DEFSH(VTYSH_L2, 
		no_interface_tlv_enable_dot3_tlv_cmd_vtysh,
		"no lldp tlv-enable dot3-tlv(all | mac-physic | power | link-aggregation \
									| max-frame-size)",
		NO_STR
		LLDP_STR
		"Enable optional TLV\n"
		"Enable 802.3 TLV\n"
		"Enable 802.3 all TLV\n"
		"Enable 802.3 MAC/PHY configuration TLV\n"	
		"Enable 802.3 power TLV\n"
		"Enable 802.3 link aggregation TLV\n"
		"Enable 802.3 max frame size TLV\n")
#endif		

/*HT157 cmd, match h3c cmd*/

/*set / clear packet interval*/        
DEFSH(VTYSH_L2, h3c_lldp_timer_tx_interval_cmd_vtysh,
		"lldp timer tx-interval <5-32768>",
		LLDP_STR
		"LLDP timer\n"
		"The interval at which LLDP frames are transmitted\n"
		"The default value is 30s. Please input an integer from 5 to 32768\n")

DEFSH(VTYSH_L2, 
		undo_h3c_lldp_timer_tx_interval_cmd_vtysh,
        "undo lldp timer tx-interval",
        NO_STR
        LLDP_STR
        "LLDP timer\n"
        "The interval at which LLDP frames are transmitted\n")

/*set / clear hold-period*/ 	   
DEFSH(VTYSH_L2, 
		h3c_lldp_hold_multiplier_cmd_vtysh,
        "lldp hold-multiplier <2-10>",
        LLDP_STR
        "A multiplier on the msgTxInterval that determines the actual TTL value use in a LLDPDU\n"
        "The default value is 4. Please input an integer from 2 to 10\n")

DEFSH(VTYSH_L2, 
		undo_h3c_lldp_hold_multiplier_cmd_vtysh,
        "undo lldp hold-multiplier",
        NO_STR
        LLDP_STR
        "A multiplier on the msgTxInterval that determines the actual TTL value use in a LLDPDU\n")

DEFSH(VTYSH_L2, 
		undo_h3c_interface_lldp_enable_cmd_vtysh,
        "undo lldp enable",
        NO_STR
        LLDP_STR
        "Enable LLDP protocol\n")


DEFSH(VTYSH_L2, 
		h3c_interface_management_tlv_cmd_vtysh,
		"lldp tlv-enable basic-tlv management-address-tlv",
		LLDP_STR
		"Enable optional TLV\n"
		"LLDP basic TLV configuration\n"
		"Enable basic management address TLV\n")

DEFSH(VTYSH_L2, 
		undo_h3c_interface_basic_tlv_cmd_vtysh,
		"undo lldp tlv-enable basic-tlv (all | port-description | \
									system-name | system-description \
									| system-capability | management-address-tlv)",
		NO_STR
		LLDP_STR
		"Enable optional TLV\n"
		"LLDP basic TLV configuration\n"
		"Enable basic all TLV\n"	
		"Enable basic port description TLV\n"
		"Enable basic system name TLV\n"
		"Enable basic system description TLV\n"
		"Enable basic system capability TLV\n"
		"Enable basic management address TLV\n")

DEFSH(VTYSH_L2, 
		h3c_lldp_global_enable_cmd_vtysh,
		"lldp global enable",
		LLDP_STR
		"Global configuration\n"
		"Enable LLDP protocol\n")

DEFSH(VTYSH_L2, 
		undo_h3c_lldp_global_enable_cmd_vtysh,
		"undo lldp global enable",
		NO_STR
		LLDP_STR
		"Global configuration\n"
		"Enable LLDP protocol\n")

DEFSH(VTYSH_L2, 
		lldp_management_address_arp_learning_cmd_vtysh,
        "lldp management-address arp-learning {vlan <1-4094>}",
        LLDP_STR
        "Management address for Network managment\n"
        "LLDP arp learning\n"
        "VLAN\n"
        "VLAN value, 1-4094\n")

DEFSH(VTYSH_L2, 
		undo_h3c_lldp_management_address_arp_learning_cmd_vtysh,
        "undo lldp management-address arp-learning",
        NO_STR
        LLDP_STR
        "Management address for Network managment\n"
        "LLDP arp learning\n")

DEFSH(VTYSH_L2, 
		no_lldp_management_address_arp_learning_cmd_vtysh,
        "no lldp management-address arp-learning",
        NO_STR
        LLDP_STR
        "Management address for Network managment\n"
        "LLDP arp learning\n")


void vtysh_init_lldp_cmd (void)
{
	/*lldp whole config */
	install_node (&lldp_node, NULL);
	vtysh_install_default (LLDP_NODE);

	install_element_level (CONFIG_NODE, &lldp_management_address_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &lldp_packet_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &no_lldp_packet_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &lldp_neighbour_hold_period_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &no_lldp_neighbour_hold_period_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/*show information*/		
	install_element_level (CONFIG_NODE, &show_lldp_statistics_trunk_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_lldp_local_trunk_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_lldp_neighbor_trunk_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_lldp_tlv_config_trunk_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level (CONFIG_NODE, &show_lldp_statistics_ethernet_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_lldp_local_ethernet_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_lldp_neighbor_ethernet_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_lldp_tlv_config_ethernet_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
		
	/*lldp interface*/
	install_element_level (PHYSICAL_IF_NODE, &show_lldp_statistics_ethernet_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (PHYSICAL_IF_NODE, &show_lldp_local_ethernet_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (PHYSICAL_IF_NODE, &show_lldp_neighbor_ethernet_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (PHYSICAL_IF_NODE, &show_lldp_tlv_config_ethernet_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
		
	/*interface config*/
	install_element_level(PHYSICAL_IF_NODE, &interface_lldp_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &no_interface_lldp_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
		
	/*interface option TLV config*/
	install_element_level(PHYSICAL_IF_NODE, &interface_tlv_enable_basic_tlv_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &no_interface_tlv_enable_basic_tlv_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	/*lldp trunk interface config*/
	install_element_level (TRUNK_IF_NODE, &show_lldp_statistics_trunk_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (TRUNK_IF_NODE, &show_lldp_local_trunk_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (TRUNK_IF_NODE, &show_lldp_neighbor_trunk_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (TRUNK_IF_NODE, &show_lldp_tlv_config_trunk_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	/*interface config*/
	install_element_level(TRUNK_IF_NODE, &interface_lldp_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &no_interface_lldp_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/*interface option TLV config*/
	install_element_level(TRUNK_IF_NODE, &interface_tlv_enable_basic_tlv_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &no_interface_tlv_enable_basic_tlv_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/*HT157 cmd, match h3c cmd*/
	install_element_level (CONFIG_NODE, &h3c_lldp_timer_tx_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &undo_h3c_lldp_timer_tx_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &h3c_lldp_hold_multiplier_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &undo_h3c_lldp_hold_multiplier_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	install_element_level(PHYSICAL_IF_NODE, &undo_h3c_interface_lldp_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &undo_h3c_interface_lldp_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level(PHYSICAL_IF_NODE, &h3c_interface_management_tlv_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &undo_h3c_interface_basic_tlv_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &h3c_interface_management_tlv_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &undo_h3c_interface_basic_tlv_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (CONFIG_NODE, &h3c_lldp_global_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &undo_h3c_lldp_global_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level(PHYSICAL_IF_NODE, &lldp_management_address_arp_learning_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &undo_h3c_lldp_management_address_arp_learning_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &no_lldp_management_address_arp_learning_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level(TRUNK_IF_NODE, &lldp_management_address_arp_learning_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &undo_h3c_lldp_management_address_arp_learning_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &no_lldp_management_address_arp_learning_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

}





