#ifndef _LIBANXT_PRIVATE_H_
#define _LIBANXT_PRIVATE_H_

#define test(func)             do { if ((func)==NXT_FAIL) return NXT_FAIL; } while (0)

#define set_word(buf,word)     do { ((unsigned char*)(buf))[0] = ((uint16_t)(word))%0x100; ((unsigned char*)(buf))[1] = ((uint16_t)(word))/0x100; } while (0)
#define set_dword(buf,dword)   do { set_word((buf),((uint32_t)(dword))%0x10000); set_word((buf)+2,((uint32_t)(dword))/0x10000); } while (0)
#define get_word(buf)          (((uint16_t)(((unsigned char*)(buf))[0]))+((unsigned char*)(buf))[1]*0x100)
#define get_dword(buf)         (((uint32_t)get_word(buf))+get_word(buf+2)*0x10000)

#define FLAG_ISSET(val,flag) (((val)&(flag))==(flag))

/// Packet type
typedef enum {
  /// Invalid packet type
  NXT_TYPE_NONE = -1,
  /// Direct command telegram, response required
  NXT_TYPE_DIRECT_RESP = 0x00,
  /// System command telegram, response required
  NXT_TYPE_SYSTEM_RESP = 0x01,
  /// Reply telegram
  NXT_TYPE_REPLY       = 0x02,
  /// Direct command telegram, no response
  NXT_TYPE_DIRECT_NORE = 0x80,
  /// System command telegram, no response
  NXT_TYPE_SYSTEM_NORE = 0x81
} nxt_type_t;

/// Packet command
typedef enum {
  NXT_CMD_NONE = -1
} nxt_cmd_t;

ssize_t nxt_con_send(nxt_t *nxt);
ssize_t nxt_con_recv(nxt_t *nxt,size_t size);
void nxt_pack_byte(nxt_t *nxt,uint8_t val);
void nxt_pack_word(nxt_t *nxt,uint16_t val);
void nxt_pack_dword(nxt_t *nxt,uint32_t val);
void nxt_pack_start(nxt_t *nxt,nxt_cmd_t cmd);
void nxt_pack_mem(nxt_t *nxt,void *buf,size_t len);
void nxt_pack_str(nxt_t *nxt,const char *str,size_t maxlen);
uint8_t nxt_unpack_byte(nxt_t *nxt);
uint16_t nxt_unpack_word(nxt_t *nxt);
uint32_t nxt_unpack_dword(nxt_t *nxt);
int nxt_unpack_start(nxt_t *nxt,nxt_cmd_t cmd);
int nxt_unpack_error(nxt_t *nxt);
void *nxt_unpack_mem(nxt_t *nxt,size_t len);
void *nxt_unpack_str(nxt_t *nxt,size_t len);

#endif /* _LIBANXT_PRIVATE_H_ */
