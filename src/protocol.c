#include "protocol.h"
#include "ir_lora.h"
#include <msp430.h>
#include "power_clk.h"
unsigned char msg[256];
unsigned char hub_sn_tmp[32]={0};
int  parse_data(unsigned char * buf,int len)
{
  int total_len=0;
  int dest_addr_len,msg_len;
  unsigned char dest_addr[32]={0},port=0;
  unsigned short cal_crc=0,data_crc=0;
  if(buf==NULL)
    return -1;
  //check head,end
  if((buf[0]=='#')&&(buf[1]=='$')&&(buf[len-1]=='!')&&(buf[len-2]=='@'))
  {
    //printf("get lora data\r\n");
    //check len
    total_len=buf[2]+(buf[3]<<8);
//    printf("total_len=%d,len=%d\r\n",total_len,len);
    if(total_len!=len)
    {
      printf("error packet len\r\n");
      return -1;
    }
    //check crc
    data_crc=buf[len-4]+(buf[len-3]<<8);
    cal_crc=CRC16(buf,len-4);
    if(data_crc!=cal_crc)
    {
      printf("crc error,data_crc=0x%02x,cal_crc=0x%02x\r\n",data_crc,cal_crc);
      //return -1;
    }
    //get addr info
    dest_addr_len=buf[4];
  //  printf("dest addr len=%d\r\n",dest_addr_len);
    memcpy(dest_addr,&buf[5],dest_addr_len);
   // printf("dest_addr %s\r\n",dest_addr);

    port=buf[HEAD_F_SIZE+TOTAL_F_SIZE+ADDR_LEN_F_SIZE+dest_addr_len];
    msg_len=buf[HEAD_F_SIZE+TOTAL_F_SIZE+ADDR_LEN_F_SIZE+dest_addr_len+PORT_F_SIZE];
   // printf("port=%d,msg_len=%d\r\n",port,msg_len);
    //#$[totoal_len][dest_addr_len][dest_addr][port][msg_len][msg][crc]@!
    memcpy(msg,&buf[HEAD_F_SIZE+TOTAL_F_SIZE+ADDR_LEN_F_SIZE+dest_addr_len+PORT_F_SIZE+MSG_LEN_F_SIZE],msg_len);
   
    if(port==PORT_IR)
    {
        process_ir_msg(msg,msg_len);//透传而已
    }
    else if(port==PORT_LORA)
    {        
        if(strstr(msg,"ctrl:search"))
        {
          memset(hub_sn_tmp,0,sizeof(hub_sn_tmp));
          memcpy(hub_sn_tmp,dest_addr,strlen(dest_addr));
         // printf("get search,new hub addr %s\r\n",hub_sn_tmp);
        }
        process_lora_msg(msg,msg_len);//组网学习相关，可能来自云端，也可能来自配置串口
    }
    else if(port==PORT_MCU)
    {
        process_mcu_msg(msg,msg_len);//序列号读写
        printf("%s %d\r\n",__func__,__LINE__);
    }
    return 0;
  }
  printf("error packet head or end %c %c %c %c\r\n",buf[0],buf[1],buf[2],buf[3]);
  return -1;
}
int get_search_cmd=0;
/*433M，这里的lora其实应该是配置lora相关的，本身lora的收发里面有对应的port，应该放在mcu里面，入网相关可以用PORT_lora*/
extern unsigned char g_hub_sn[SN_LEN];
void update_hub_sn(void)
{
     printf("cur hub sn:%s,new:%s\r\n",g_hub_sn,hub_sn_tmp);
     if(strcmp(g_hub_sn,hub_sn_tmp)&&strlen(hub_sn_tmp))
     {
        printf("update new hub sn\r\n");
        write_hub_sn(hub_sn_tmp);
     }
}
int process_lora_msg(unsigned char *buf,int len)
{
  unsigned short hub_addr;
  if(buf)
  {
    //考虑dongle和node
    //from config uart，配置初始网路信息
    //from connet,广播学习组网，回复组网相关信息
    if(strstr(buf,"ctrl:query"))//I am dongle，收到一个查询。
    {
      //any data to node
    }
 
    if(strstr(buf,"ctrl:join"))//I am dongle，收到一个设备入网sao扫描。
    {
      //send ctrl:ack
    }
      
    if(strstr(buf,"ctrl:q_ack"))//I am dongle，收到一个设备消息收到应答。
    {
      //svve sn to whiete list
    }
  
    if(strstr(buf,"ctrl:fail"))//I am dongle or node，错误码，node也使用
    {
      //any data to node
    }
//only for node;
    if(strstr(buf,"ctrl:q_yes"))//I am node，收到一个查询反馈，表示有消息。
    {
      //any data to node
      printf("Yes,I am waiting for msg\r\n");
      send_ack();
    }
    
    if(strstr(buf,"ctrl:q_no"))//I am node，收到一个查询反馈，表示没有消息。
    {
      //goto sleep
      printf("nothing to do ,goto sleep\r\n");
      device_sleep();
      send_ack();
    }

    if(strstr(buf,"ctrl:search"))//I am node，收到一个设备入网sao扫描。
    {
      //if key pressed
//   printf("get search I will join\r\n");
      if(get_search_cmd==0)
      {
        get_search_cmd=1;
        flash_led0_slow(20);
      }
    }

    //参数lora获取和设置
    if(strstr(buf,"ctrl:p_get"))
    {
      //not support now
    }
    
    if(strstr(buf,"ctrl:p_set"))
    {
       //not support now
    }
   }
}

/*IR,学习到模块中，支持学习开 制冷26度，开制冷27度，关，因此透传处理即可，使用0x40,0x41,0x42*/
int process_ir_msg(unsigned char *buf,int len)
{
  int read=0;
  unsigned char cmd=0xe3;
  printf("%s %d\r\n",__func__,len);
  if(buf)
  {
    ir_write(&cmd,1);
    ir_write(buf,len);
    return 0;
  }
  return -1;
}

/*mcu*/
int process_mcu_msg(unsigned char *buf,int len)
{
  char * SN=NULL;
  char sn[40];
  char sn_tmp[30];
    printf("%s %d\r\n",__func__,__LINE__);
  memset(sn,0,sizeof(sn));
  if(buf)
  {
    //is ctrl msg
    if(strstr(buf,"ctrl"))
    {
        printf("%s\r\n",buf);    
        if(strstr(buf,"HUB_SN_SET="))//write hub SN,ok
        { 
          SN=strstr(buf,"=");
          memcpy(sn,SN+1,SN_LEN);
          printf("to write HUB SN:%s\r\n",sn);
          //update addr;
          write_hub_sn(sn);
          //要返回OK；
          send_msg("ctrl:ack",strlen("ctrl:ack"),PORT_MCU,sn,SN_LEN,UART_CHANNEL_CONFIG_3);
          return 0;
        }
        if(strstr(buf,"HUB_SN_GET"))//get hub SN,ok
        {
           strcat(sn,"ctrl:HUB_SN_GET=");
           read_hub_sn(sn_tmp);
           memcpy(&sn[strlen("ctrl:HUB_SN_GET=")],sn_tmp,SN_LEN);
           send_msg(sn,SN_LEN+strlen("ctrl:HUB_SN_GET="),PORT_MCU,sn_tmp,SN_LEN,UART_CHANNEL_CONFIG_3);
             return 0;
        }
        if(strstr(buf,"SN_SET="))//write SN,ok
        { 
          SN=strstr(buf,"=");
          memcpy(sn,SN+1,SN_LEN);
          printf("to write SN:%s %d\r\n",sn,strlen(sn));
          //update addr;
          write_sn(sn);
          //要返回OK；
         send_msg("ctrl:ack",strlen("ctrl:ack"),PORT_MCU,sn,SN_LEN,UART_CHANNEL_CONFIG_3);
            return 0;
        }
        if(strstr(buf,"SN_GET"))//get SN,ok
        {
           strcat(sn,"ctrl:SN_GET=");
           read_sn(sn_tmp);
           memcpy(&sn[strlen("ctrl:SN_GET=")],sn_tmp,SN_LEN);
           send_msg(sn,SN_LEN+strlen("ctrl:SN_GET="),PORT_MCU,sn_tmp,SN_LEN,UART_CHANNEL_CONFIG_3);
             return 0;
        } 
        //write sn;
        return 0;
    }
  }
  return -1;
}

int parse_sn(unsigned char *buf,int len)
{
  struct serial_num;
}
extern unsigned char g_hub_sn[SN_LEN];
int send_msg(unsigned char *msg,int msg_len,char port,unsigned char * sn,int sn_len,char uart_channel)
{
	unsigned short buf_len=0,crc=0;
        int dongle_len_opt=0;
        if(UART_CHANNEL_LORA_2==uart_channel)
        {
          dongle_len_opt=SN_LEN+1;
        }
	unsigned char * msg_buf=NULL;
	buf_len=HEAD_F_SIZE+END_F_SIZE+TOTAL_F_SIZE+ADDR_LEN_F_SIZE+PORT_F_SIZE+MSG_LEN_F_SIZE+sn_len+msg_len+CRC_F_SIZE+dongle_len_opt;
	printf("send msg len=%d\r\n",buf_len);
	msg_buf=malloc(buf_len);
	if(msg_buf==NULL)
	{
		return -1;
	}
	/*head and end*/
	msg_buf[0]='#';msg_buf[1]='$';msg_buf[buf_len-2]='@';msg_buf[buf_len-1]='!';
	msg_buf[2]=buf_len&0xff;msg_buf[3]=(buf_len>>8)&0xff;
        //for dongle addr
        if(UART_CHANNEL_LORA_2==uart_channel)
        {
           msg_buf[4]=SN_LEN;
           memcpy(&msg_buf[5],g_hub_sn,SN_LEN);
        }
  
	msg_buf[4+dongle_len_opt]=sn_len;
	memcpy(&msg_buf[5+dongle_len_opt],sn,sn_len);
	msg_buf[5+sn_len+dongle_len_opt]=port;
	msg_buf[5+sn_len+1+dongle_len_opt]=msg_len&0xff;
	msg_buf[5+sn_len+2+dongle_len_opt]=(msg_len>>8)&0xff;
	memcpy(&msg_buf[5+sn_len+3+dongle_len_opt],msg,msg_len);
        
	//crc
        crc=CRC16(msg_buf,buf_len-4);
	msg_buf[5+sn_len+2+1+msg_len+dongle_len_opt]=crc&0xff;
	msg_buf[5+sn_len+2+1+msg_len+1+dongle_len_opt]=(crc>>8)&0xff;
    //mabey lora_send
    
    switch(uart_channel)
    {
      case UART_CHANNEL_IR_1:uart1_write(msg_buf,buf_len);break;
      case UART_CHANNEL_LORA_2:lora_send(msg_buf,buf_len); break;
      case UART_CHANNEL_CONFIG_3:uart3_write(msg_buf,buf_len); break;
      default:printf("error\r\n");break;
    }
	free(msg_buf);
        delay_ms(50);
	return 0;
}

int send_data_query(void)
{
 send_msg("ctrl:query",strlen("ctrl:query"),PORT_LORA,g_sn,SN_LEN,UART_CHANNEL_LORA_2);
}

int send_ack(void)
{
  send_msg("ctrl:ack",strlen("ctrl:ack"),PORT_LORA,g_sn,SN_LEN,UART_CHANNEL_LORA_2);
}

int send_search(void)
{
    send_msg("ctrl:search",strlen("ctrl:search"),PORT_LORA,g_sn,SN_LEN,UART_CHANNEL_LORA_2);
}

int send_join(void)
{
    printf("send join\r\n");
    send_msg("ctrl:join",strlen("ctrl:join"),PORT_LORA,g_sn,SN_LEN,UART_CHANNEL_LORA_2);
}


int send_fail(void)
{
    send_msg("ctrl:fail",strlen("ctrl:fail"),PORT_LORA,g_sn,SN_LEN,UART_CHANNEL_LORA_2);
}

int char2int(char c);
//you bug
unsigned short get_addr_from_sn(unsigned char * sn,int len)
{
  int a,b,c,d,e;

  e=char2int(sn[len-1]);
//  printf("addr e=%c %d\r\n",sn[len-1],e);
  if(e<0)
    return -1;
  d=char2int(sn[len-2]);
  //printf("addr d=%c %d\r\n",sn[len-2],d);  
  if(d<0)
    return -1;  
  c=char2int(sn[len-3]);
 //printf("addr c=%c %d\r\n",sn[len-3],c);   
  if(c<0)
    return -1;
  b=char2int(sn[len-4]);
 //printf("addr b=%c %d\r\n",sn[len-4],b); 
  if(b<0)
    return -1;
  a=char2int(sn[len-5]);
 //printf("addr a=%c %d\r\n",sn[len-5],a); 
  if(a<0)
    return -1;
  return a*10000+b*1000+c*100+d*10+e;
}

int char2int(char c)
{
  char num[10]={'0','1','2','3','4','5','6','7','8','9'};
  int i=0;
  for (i=0;i<10;i++)
  {
    if(c==num[i])
      return i;
  }
  return -1;
}