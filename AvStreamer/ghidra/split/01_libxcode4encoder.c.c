// ==== message @ 00405420 ====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void message(dvbpsi_t *handle,dvbpsi_msg_level_t level,char *msg)

{
  if (level == DVBPSI_MSG_WARN) {
    fwrite("Warning: ",1,9,_stderr);
  }
  else if (level == DVBPSI_MSG_DEBUG) {
    fwrite("Debug: ",1,7,_stderr);
  }
  else {
    if (level != DVBPSI_MSG_ERROR) {
      return;
    }
    fwrite("Error: ",1,7,_stderr);
  }
  fprintf(_stderr,"%s\n",msg);
  return;
}



// ==== writePSI @ 00405574 ====

void writePSI(uint8_t *p_packet,dvbpsi_psi_section_t *p_section)

{
  int iVar1;
  uint8_t *puVar2;
  dvbpsi_psi_section_t *local_res4;
  uint8_t *local_18;
  uint8_t *local_14;
  
  *p_packet = 0x47;
  for (local_res4 = p_section; local_res4 != (dvbpsi_psi_section_t *)0x0;
      local_res4 = local_res4->p_next) {
                    /* Unresolved local var: uint8_t * p_pos_in_ts@[???]
                       Unresolved local var: uint8_t * p_byte@[???]
                       Unresolved local var: uint8_t * p_end@[???] */
    local_14 = local_res4->p_data;
    puVar2 = local_res4->p_payload_end;
    if (local_res4->b_syntax_indicator == false) {
      iVar1 = 0;
    }
    else {
      iVar1 = 4;
    }
    p_packet[1] = p_packet[1] | 0x40;
    p_packet[4] = 0;
    local_18 = p_packet + 5;
    for (; (local_18 < p_packet + 0xbc && (local_14 < puVar2 + iVar1)); local_14 = local_14 + 1) {
      *local_18 = *local_14;
      local_18 = local_18 + 1;
    }
    for (; local_18 < p_packet + 0xbc; local_18 = local_18 + 1) {
      *local_18 = 0xff;
    }
  }
  return;
}



// ==== bswap_32 @ 004056dc ====

uint32_t bswap_32(uint32_t x)

{
  uint uVar1;
  
  uVar1 = (x & 0xff00ff) << 8 | x >> 8 & 0xff00ff;
  return (uVar1 >> 0x10) + uVar1 * 0x10000;
}



// ==== crc32 @ 00405740 ====

uint32_t crc32(uint32_t crc,uint8_t *buffer,size_t length)

{
  uint local_res0;
  byte *local_res4;
  
                    /* Unresolved local var: uint8_t * end@[???] */
  local_res0 = crc;
  for (local_res4 = buffer; local_res4 < buffer + length; local_res4 = local_res4 + 1) {
    local_res0 = crc32_tab[local_res0 & 0xff ^ (uint)*local_res4] ^ local_res0 >> 8;
  }
  return local_res0;
}



// ==== FormTSHeader @ 004057e8 ====

int FormTSHeader(uint8_t *dat8,uint16_t pid,uint8_t counter)

{
  *dat8 = 0x47;
  dat8[1] = (byte)(pid >> 8) & 0x1f | 0x40;
  dat8[2] = (uint8_t)pid;
  dat8[3] = counter & 0xf | 0x10;
  dat8[4] = 0;
  return 5;
}



// ==== FormATSC_MGT @ 00405898 ====

int FormATSC_MGT(uint8_t *data,uint32_t vctlen,uint32_t *eitlen,uint16_t *eitpid,uint8_t eitcnt,
                uint32_t *ettlen,uint16_t *ettpid,uint8_t ettcnt)

{
  uint16_t uVar1;
  size_t length;
  uint32_t uVar2;
  int local_38;
  int local_34;
  ushort local_30;
  
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int j@[???]
                       Unresolved local var: uint32_t crc@[???]
                       Unresolved local var: uint8_t tableid@[???]
                       Unresolved local var: uint8_t reserved_0@[???]
                       Unresolved local var: uint8_t reserved_2@[???]
                       Unresolved local var: uint8_t reserved_3@[???]
                       Unresolved local var: uint8_t reserved_5@[???]
                       Unresolved local var: uint8_t reserved_6@[???]
                       Unresolved local var: uint8_t current_next_indicator@[???]
                       Unresolved local var: uint8_t version_number@[???]
                       Unresolved local var: uint8_t section_number@[???]
                       Unresolved local var: uint8_t last_section_number@[???]
                       Unresolved local var: uint16_t tables_defined@[???]
                       Unresolved local var: uint16_t table_type_PID@[???]
                       Unresolved local var: uint8_t table_type_version_number@[???]
                       Unresolved local var: uint16_t table_type_descriptors_length@[???]
                       Unresolved local var: uint16_t descriptors_length@[???] */
  local_30 = (ushort)(vctlen != 0);
  if (eitcnt != 0) {
    local_30 = eitcnt + local_30;
  }
  if (ettcnt != 0) {
    local_30 = ettcnt + local_30;
  }
  *data = 199;
  data[1] = 0xf0;
  data[2] = 0;
  data[3] = 0;
  data[4] = 0;
  data[5] = 0xef;
  data[6] = 0;
  data[7] = 0;
  data[8] = 0;
  data[9] = (uint8_t)(local_30 >> 8);
  data[10] = (uint8_t)local_30;
  local_38 = 0xb;
  if (vctlen != 0) {
    data[0xb] = 0;
    data[0xc] = 0;
    data[0xd] = 0xff;
    data[0xe] = 0xfb;
    data[0xf] = 0xf7;
    data[0x10] = (uint8_t)(vctlen >> 0x18);
    data[0x11] = (uint8_t)(vctlen >> 0x10);
    data[0x12] = (uint8_t)(vctlen >> 8);
    data[0x13] = (uint8_t)vctlen;
    data[0x14] = 0xf0;
    data[0x15] = 0;
    local_38 = 0x16;
  }
  if (eitcnt != 0) {
    for (local_34 = 0; local_34 < (int)(uint)eitcnt; local_34 = local_34 + 1) {
      uVar1 = eitpid[local_34];
      data[local_38] = 1;
      data[local_38 + 1] = (uint8_t)local_34;
      data[local_38 + 2] = (byte)(uVar1 >> 8) & 0x1f | 0xe0;
      data[local_38 + 3] = (uint8_t)uVar1;
      data[local_38 + 4] = 0xf7;
      data[local_38 + 5] = (uint8_t)(eitlen[local_34] >> 0x18);
      data[local_38 + 6] = (uint8_t)(eitlen[local_34] >> 0x10);
      data[local_38 + 7] = (uint8_t)(eitlen[local_34] >> 8);
      data[local_38 + 8] = (uint8_t)eitlen[local_34];
      data[local_38 + 9] = 0xf0;
      data[local_38 + 10] = 0;
      local_38 = local_38 + 0xb;
    }
  }
  if (ettcnt != 0) {
    for (local_34 = 0; local_34 < (int)(uint)ettcnt; local_34 = local_34 + 1) {
      uVar1 = ettpid[local_34];
      data[local_38] = 2;
      data[local_38 + 1] = (uint8_t)local_34;
      data[local_38 + 2] = (byte)(uVar1 >> 8) & 0x1f | 0xe0;
      data[local_38 + 3] = (uint8_t)uVar1;
      data[local_38 + 4] = 0xf7;
      data[local_38 + 5] = (uint8_t)(ettlen[local_34] >> 0x18);
      data[local_38 + 6] = (uint8_t)(ettlen[local_34] >> 0x10);
      data[local_38 + 7] = (uint8_t)(ettlen[local_34] >> 8);
      data[local_38 + 8] = (uint8_t)ettlen[local_34];
      data[local_38 + 9] = 0xf0;
      data[local_38 + 10] = 0;
      local_38 = local_38 + 0xb;
    }
  }
  data[local_38] = 0xf0;
  data[local_38 + 1] = 0;
  length = local_38 + 2;
  data[1] = (byte)((uint)(local_38 + 3) >> 8) | 0xf0;
  data[2] = (char)length + 1;
  uVar2 = crc32(0xffffffff,data,length);
  uVar2 = bswap_32(uVar2);
  data[length] = (uint8_t)(uVar2 >> 0x18);
  data[local_38 + 3] = (uint8_t)(uVar2 >> 0x10);
  data[local_38 + 4] = (uint8_t)(uVar2 >> 8);
  data[local_38 + 5] = (uint8_t)uVar2;
  return local_38 + 6;
}



// ==== setMinor @ 004063e4 ====

uint16_t setMinor(uint16_t minor)

{
  sti_minor = minor;
  return minor;
}



// ==== FormATSC_ETT @ 00406420 ====

int FormATSC_ETT(uint8_t *data,char *addinfo,int addinfolen,uint16_t source_id,uint16_t event_id)

{
  byte bVar1;
  int iVar2;
  uint32_t uVar3;
  size_t local_30;
  int local_2c;
  int local_28;
  int local_24;
  
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int l@[???]
                       Unresolved local var: int m@[???]
                       Unresolved local var: int n@[???]
                       Unresolved local var: uint32_t crc@[???]
                       Unresolved local var: uint8_t tableid@[???]
                       Unresolved local var: uint16_t ETT_table_id_ext@[???]
                       Unresolved local var: uint8_t current_next_indicator@[???]
                       Unresolved local var: uint8_t version_number@[???]
                       Unresolved local var: uint8_t reserved_0@[???]
                       Unresolved local var: uint32_t ETM_id@[???] */
  iVar2 = (uint)event_id << 2;
  if (event_id == 0) {
    bVar1 = 0;
  }
  else {
    bVar1 = 2;
  }
  *data = 0xcc;
  data[1] = 0xf0;
  data[2] = 0;
  data[3] = (uint8_t)(event_id >> 8);
  data[4] = (uint8_t)event_id;
  data[5] = 0xef;
  data[6] = 0;
  data[7] = 0;
  data[8] = 0;
  data[9] = (uint8_t)(source_id >> 8);
  data[10] = (byte)source_id | (byte)((uint)iVar2 >> 0x10);
  data[0xb] = (uint8_t)((uint)iVar2 >> 8);
  data[0xc] = (byte)iVar2 | bVar1;
  data[0xd] = 1;
  local_30 = 0xe;
  for (local_2c = 0; local_2c < 1; local_2c = local_2c + 1) {
    data[local_30] = 0x65;
    data[local_30 + 1] = 0x6e;
    data[local_30 + 2] = 0x67;
    data[local_30 + 3] = 1;
    local_30 = local_30 + 4;
    for (local_28 = 0; local_28 < 1; local_28 = local_28 + 1) {
      data[local_30] = 0;
      data[local_30 + 1] = 0;
      data[local_30 + 2] = (uint8_t)addinfolen;
      local_30 = local_30 + 3;
      for (local_24 = 0; local_24 < addinfolen; local_24 = local_24 + 1) {
        data[local_30] = addinfo[local_24];
        local_30 = local_30 + 1;
      }
    }
  }
  data[1] = (byte)(local_30 + 1 >> 8) | 0xf0;
  data[2] = (char)local_30 + 1;
  uVar3 = crc32(0xffffffff,data,local_30);
  uVar3 = bswap_32(uVar3);
  data[local_30] = (uint8_t)(uVar3 >> 0x18);
  data[local_30 + 1] = (uint8_t)(uVar3 >> 0x10);
  data[local_30 + 2] = (uint8_t)(uVar3 >> 8);
  data[local_30 + 3] = (uint8_t)uVar3;
  return local_30 + 4;
}



// ==== FormATSC_EIT @ 00406980 ====

/* WARNING: Heritage AFTER dead removal. Example location: v0xffffffe4 : 0x00406cc4 */
/* WARNING: Restarted to delay deadcode elimination for space: stack */

int FormATSC_EIT(uint8_t *data,char *title,int title_len,uint32_t timeoffs,uint16_t evt_id,int etm)

{
  bool bVar1;
  ushort uVar2;
  time_t tVar3;
  int iVar4;
  uint uVar5;
  uint32_t uVar6;
  size_t local_50;
  int local_4c;
  int local_48;
  int local_44;
  
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int l@[???]
                       Unresolved local var: int m@[???]
                       Unresolved local var: int n@[???]
                       Unresolved local var: uint16_t j@[???]
                       Unresolved local var: uint32_t crc@[???]
                       Unresolved local var: uint8_t tableid@[???]
                       Unresolved local var: uint16_t source_id@[???]
                       Unresolved local var: uint8_t current_next_indicator@[???]
                       Unresolved local var: uint8_t version_number@[???]
                       Unresolved local var: uint8_t reserved_0@[???]
                       Unresolved local var: uint16_t event_id@[???]
                       Unresolved local var: uint8_t reserved_1@[???]
                       Unresolved local var: uint8_t reserved_2@[???]
                       Unresolved local var: uint8_t ETM_location@[???]
                       Unresolved local var: uint8_t title_len_pos@[???]
                       Unresolved local var: uint8_t desc_len_pos@[???]
                       Unresolved local var: uint16_t descriptors_length@[???]
                       Unresolved local var: time_t tt@[???]
                       Unresolved local var: uint8_t num_events_in_section@[???]
                       Unresolved local var: uint32_t[1] gpstime@[???]
                       Unresolved local var: uint32_t[1] len_secs@[???] */
  tVar3 = time((time_t *)0x0);
  iVar4 = (tVar3 / 0x15180) * 0x15180 + -0x12d53d70 + timeoffs;
  *data = 0xcb;
  data[1] = 0xf0;
  data[2] = 0;
  data[3] = 0;
  data[4] = 3;
  data[5] = 0xef;
  data[6] = 0;
  data[7] = 0;
  data[8] = 0;
  data[9] = 1;
  local_50 = 10;
  bVar1 = false;
  while (!bVar1) {
    data[local_50] = (byte)(evt_id >> 8) | 0xc0;
    data[local_50 + 1] = (uint8_t)evt_id;
    data[local_50 + 2] = (uint8_t)((uint)iVar4 >> 0x18);
    data[local_50 + 3] = (uint8_t)((uint)iVar4 >> 0x10);
    data[local_50 + 4] = (uint8_t)((uint)iVar4 >> 8);
    data[local_50 + 5] = (uint8_t)iVar4;
    data[local_50 + 6] = ((byte)etm & 3) << 4 | 0xc0;
    data[local_50 + 7] = 0x54;
    data[local_50 + 8] = 0x60;
    uVar5 = local_50 + 9;
    data[uVar5] = 0;
    data[local_50 + 10] = 1;
    local_50 = local_50 + 0xb;
    for (local_4c = 0; local_4c < 1; local_4c = local_4c + 1) {
      data[local_50] = 0x65;
      data[local_50 + 1] = 0x6e;
      data[local_50 + 2] = 0x67;
      data[local_50 + 3] = 1;
      local_50 = local_50 + 4;
      for (local_48 = 0; local_48 < 1; local_48 = local_48 + 1) {
        data[local_50] = 0;
        data[local_50 + 1] = 0;
        data[local_50 + 2] = (uint8_t)title_len;
        local_50 = local_50 + 3;
        for (local_44 = 0; local_44 < title_len; local_44 = local_44 + 1) {
          data[local_50] = title[local_44];
          local_50 = local_50 + 1;
        }
      }
    }
    data[uVar5 & 0xff] = ((byte)local_50 - (char)uVar5) - 1;
    data[local_50] = 0;
    data[local_50 + 1] = 0;
    data[local_50 + 2] = 0x86;
    data[local_50 + 3] = 7;
    data[local_50 + 4] = 0xe1;
    data[local_50 + 5] = 0x65;
    data[local_50 + 6] = 0x6e;
    data[local_50 + 7] = 0x67;
    data[local_50 + 8] = 0xc1;
    data[local_50 + 9] = 0x3f;
    data[local_50 + 10] = 0xff;
    uVar2 = ((short)(local_50 + 0xb) - (ushort)(byte)local_50) - 2;
    data[local_50 & 0xff] = (byte)(uVar2 >> 8) & 0xf | 0xf0;
    data[(local_50 & 0xff) + 1] = (uint8_t)uVar2;
    for (local_4c = 0; local_4c < (int)(uint)uVar2; local_4c = local_4c + 1) {
    }
    local_50 = local_50 + 0xb;
    bVar1 = true;
  }
  data[1] = (byte)(local_50 + 1 >> 8) | 0xf0;
  data[2] = (char)local_50 + 1;
  uVar6 = crc32(0xffffffff,data,local_50);
  uVar6 = bswap_32(uVar6);
  data[local_50] = (uint8_t)(uVar6 >> 0x18);
  data[local_50 + 1] = (uint8_t)(uVar6 >> 0x10);
  data[local_50 + 2] = (uint8_t)(uVar6 >> 8);
  data[local_50 + 3] = (uint8_t)uVar6;
  return local_50 + 4;
}



// ==== FormATSC_TVCT @ 004073b8 ====

int FormATSC_TVCT(uint8_t *data,uint16_t mjchannel,uint16_t mnchannel,char *extended_name,
                 int ext_name_len,uint8_t etm)

{
  size_t sVar1;
  int iVar2;
  int iVar3;
  uint32_t uVar4;
  size_t local_68;
  int local_64;
  int local_60;
  short local_5c;
  int local_58;
  int local_54;
  int local_50;
  
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int j@[???]
                       Unresolved local var: int k@[???]
                       Unresolved local var: uint32_t crc@[???]
                       Unresolved local var: uint8_t tableid@[???]
                       Unresolved local var: uint16_t transport_stream_id@[???]
                       Unresolved local var: uint8_t current_next_indicator@[???]
                       Unresolved local var: uint8_t version_number@[???]
                       Unresolved local var: uint8_t reserved_0@[???]
                       Unresolved local var: uint8_t reserved_1@[???]
                       Unresolved local var: uint8_t reserved_2@[???]
                       Unresolved local var: uint8_t reserved_3@[???]
                       Unresolved local var: uint8_t reserved_4@[???]
                       Unresolved local var: uint8_t reserved_5@[???]
                       Unresolved local var: uint8_t modulation_mode@[???]
                       Unresolved local var: uint16_t channelTSID@[???]
                       Unresolved local var: uint16_t program_number@[???]
                       Unresolved local var: uint8_t ETM_location@[???]
                       Unresolved local var: uint8_t access_controlled@[???]
                       Unresolved local var: uint8_t hidden@[???]
                       Unresolved local var: uint8_t hide_guide@[???]
                       Unresolved local var: uint8_t service_type@[???]
                       Unresolved local var: uint8_t num_channels_in_section@[???]
                       Unresolved local var: uint16_t major_channel_number@[???]
                       Unresolved local var: uint16_t minor_channel_number@[???]
                       Unresolved local var: uint16_t source_id@[???]
                       Unresolved local var: uint16_t descriptors_length@[???]
                       Unresolved local var: uint16_t add_descriptors_length@[???] */
  local_5c = 0;
  *data = 200;
  data[1] = 0xf0;
  data[2] = 0;
  data[3] = 0;
  data[4] = 0;
  data[5] = 0xef;
  data[6] = 0;
  data[7] = 0;
  data[8] = 0;
  data[9] = 1;
  local_68 = 10;
  for (local_64 = 0; sVar1 = local_68, local_64 < 1; local_64 = local_64 + 1) {
                    /* Unresolved local var: int desc_len_pos@[???]
                       Unresolved local var: int ext_name_pos@[???]
                       Unresolved local var: int l@[???]
                       Unresolved local var: int m@[???]
                       Unresolved local var: int n@[???]
                       Unresolved local var: uint8_t reserved_6@[???]
                       Unresolved local var: uint8_t reserved_7@[???]
                       Unresolved local var: uint16_t PCR_PID@[???]
                       Unresolved local var: int serv_loc_pos@[???]
                       Unresolved local var: int EPID@[???] */
    data[local_68] = 0;
    data[local_68 + 1] = 0x4d;
    data[local_68 + 2] = 0;
    data[local_68 + 3] = 0x45;
    data[local_68 + 4] = 0;
    data[local_68 + 5] = 0x4c;
    data[local_68 + 6] = 0;
    data[local_68 + 7] = 0x44;
    data[local_68 + 8] = 0;
    data[local_68 + 9] = 0x33;
    data[local_68 + 10] = 0;
    data[local_68 + 0xb] = 0x30;
    data[local_68 + 0xc] = 0;
    data[local_68 + 0xd] = 0x30;
    data[local_68 + 0xe] = (byte)(mjchannel >> 6) | 0xf0;
    data[local_68 + 0xf] = (char)mjchannel << 2 | (byte)(mnchannel >> 8) & 0xf;
    data[local_68 + 0x10] = (uint8_t)mnchannel;
    data[local_68 + 0x11] = 4;
    data[local_68 + 0x12] = 0;
    data[local_68 + 0x13] = 0;
    data[local_68 + 0x14] = 0;
    data[local_68 + 0x15] = 0;
    data[local_68 + 0x16] = 0;
    data[local_68 + 0x17] = 0;
    data[local_68 + 0x18] = 0;
    data[local_68 + 0x19] = 3;
    data[local_68 + 0x1a] = etm << 6 | 9;
    data[local_68 + 0x1b] = 0xc2;
    data[local_68 + 0x1c] = 0;
    data[local_68 + 0x1d] = 3;
    iVar2 = local_68 + 0x1e;
    data[iVar2] = (byte)((ushort)local_5c >> 8) & 3 | 0xfc;
    data[local_68 + 0x1f] = (uint8_t)local_5c;
    data[local_68 + 0x20] = 0xa0;
    iVar3 = local_68 + 0x21;
    data[iVar3] = 0;
    data[local_68 + 0x22] = 1;
    local_68 = local_68 + 0x23;
    for (local_58 = 0; local_58 < 1; local_58 = local_58 + 1) {
      data[local_68] = 0x65;
      data[local_68 + 1] = 0x6e;
      data[local_68 + 2] = 0x67;
      data[local_68 + 3] = 1;
      local_68 = local_68 + 4;
      for (local_54 = 0; local_54 < 1; local_54 = local_54 + 1) {
        data[local_68] = 0;
        data[local_68 + 1] = 0;
        data[local_68 + 2] = (uint8_t)ext_name_len;
        local_68 = local_68 + 3;
        for (local_50 = 0; local_50 < ext_name_len; local_50 = local_50 + 1) {
          data[local_68] = extended_name[local_50];
          local_68 = local_68 + 1;
        }
      }
    }
    data[iVar3] = ((char)local_68 - (char)iVar3) - 1;
    data[local_68] = 0xa1;
    iVar3 = local_68 + 1;
    data[iVar3] = 0;
    data[local_68 + 2] = 0xe0;
    data[local_68 + 3] = 0x30;
    data[local_68 + 4] = 2;
    data[local_68 + 5] = 2;
    data[local_68 + 6] = 0xe0;
    data[local_68 + 7] = 0x30;
    data[local_68 + 8] = 0x65;
    data[local_68 + 9] = 0x6e;
    data[local_68 + 10] = 0x67;
    data[local_68 + 0xb] = 0x81;
    data[local_68 + 0xc] = 0xe0;
    data[local_68 + 0xd] = 0x31;
    data[local_68 + 0xe] = 0x65;
    data[local_68 + 0xf] = 0x6e;
    data[local_68 + 0x10] = 0x67;
    local_68 = local_68 + 0x11;
    data[iVar3] = ((char)local_68 - (char)iVar3) - 1;
    local_5c = ((short)local_68 - (short)iVar2) + -2;
    data[iVar2] = (byte)((ushort)local_5c >> 8) & 3 | 0xfc;
    data[sVar1 + 0x1f] = (uint8_t)local_5c;
  }
  data[local_68] = 0xfc;
  data[local_68 + 1] = 0;
  local_68 = local_68 + 2;
  for (local_60 = 0; local_60 < 0; local_60 = local_60 + 1) {
    data[local_68] = 1;
    local_68 = local_68 + 1;
  }
  data[1] = (byte)(local_68 + 1 >> 8) | 0xf0;
  data[2] = (char)local_68 + 1;
  uVar4 = crc32(0xffffffff,data,local_68);
  uVar4 = bswap_32(uVar4);
  data[local_68] = (uint8_t)(uVar4 >> 0x18);
  data[local_68 + 1] = (uint8_t)(uVar4 >> 0x10);
  data[local_68 + 2] = (uint8_t)(uVar4 >> 8);
  data[local_68 + 3] = (uint8_t)uVar4;
  return local_68 + 4;
}



// ==== FormATSC_STT @ 004082c8 ====

int FormATSC_STT(uint8_t *data)

{
  time_t tVar1;
  int iVar2;
  uint32_t uVar3;
  
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: uint32_t crc@[???]
                       Unresolved local var: uint8_t tableid@[???]
                       Unresolved local var: uint8_t reserved_0@[???]
                       Unresolved local var: uint8_t current_next_indicator@[???]
                       Unresolved local var: uint8_t version_number@[???]
                       Unresolved local var: time_t tt@[???]
                       Unresolved local var: uint32_t gpstime@[???] */
  tVar1 = time((time_t *)0x0);
  iVar2 = tVar1 + -0x12d4db1b;
  *data = 0xcd;
  data[1] = 0xf0;
  data[2] = 0;
  data[3] = 0;
  data[4] = 0;
  data[5] = 0xef;
  data[6] = 0;
  data[7] = 0;
  data[8] = 0;
  data[9] = (uint8_t)((uint)iVar2 >> 0x18);
  data[10] = (uint8_t)((uint)iVar2 >> 0x10);
  data[0xb] = (uint8_t)((uint)iVar2 >> 8);
  data[0xc] = (uint8_t)iVar2;
  data[0xd] = 0xf;
  data[0xe] = 0;
  data[0xf] = 0;
  data[1] = 0xf0;
  data[2] = 0x11;
  uVar3 = crc32(0xffffffff,data,0x10);
  uVar3 = bswap_32(uVar3);
  data[0x10] = (uint8_t)(uVar3 >> 0x18);
  data[0x11] = (uint8_t)(uVar3 >> 0x10);
  data[0x12] = (uint8_t)(uVar3 >> 8);
  data[0x13] = (uint8_t)uVar3;
  return 0x14;
}



// ==== genNIT @ 004086b4 ====

int genNIT(uint8_t *packet,uint8_t counter,uint32_t freq,int bandwidth,float lngt,float lttd)

{
  uint3 uVar1;
  int iVar2;
  dvbpsi_t *p_dvbpsi;
  dvbpsi_nit_ts_t *p_ts;
  dvbpsi_psi_section_t *p_section;
  undefined2 uVar3;
  undefined2 uVar4;
  float fVar5;
  char local_68;
  uint8_t local_4c [9];
  undefined1 uStack_43;
  undefined1 uStack_42;
  undefined1 uStack_41;
  undefined4 local_40;
  undefined2 local_3c;
  uint8_t uStack_3a;
  undefined1 uStack_39;
  undefined1 local_38 [3];
  undefined1 uStack_35;
  undefined4 local_34;
  undefined2 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined2 local_20;
  dvbpsi_nit_t dStack_1c;
  
  local_4c[0] = 0x4d;
  local_4c[1] = 0x45;
  local_4c[2] = 0x4c;
  local_4c[3] = 0x44;
                    /* Unresolved local var: uint8_t[4] name@[???]
                       Unresolved local var: uint8_t[12] cell@[???]
                       Unresolved local var: uint8_t[14] td5a@[???]
                       Unresolved local var: uint8_t[14] td41@[???]
                       Unresolved local var: dvbpsi_nit_t nit@[???]
                       Unresolved local var: dvbpsi_nit_ts_t * p_es@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_section1@[???]
                       Unresolved local var: uint8_t bw_mod@[???]
                       Unresolved local var: uint32_t hz_freq@[???]
                       Unresolved local var: float tmp@[???]
                       Unresolved local var: uint16_t lngt16@[???]
                       Unresolved local var: uint16_t lttd16@[???]
                       Unresolved local var: dvbpsi_t * p_dvbpsi@[???] */
  local_4c[4] = 0x6c;
  local_4c[5] = 10;
  local_4c[6] = 0;
  local_4c[7] = 0;
  local_40 = 0;
  local_30 = 0xff;
  local_24 = 0;
  local_20 = 0;
  local_2c = 0x3000341;
  local_28 = 1;
  local_68 = '\0';
  iVar2 = freq * 100000;
  if (lngt < 0.0) {
    lngt = lngt + 360.0;
  }
  fVar5 = (lngt / 180.0) * 32768.0;
  if (fVar5 < 2.1474836e+09) {
    uVar3 = (undefined2)(int)fVar5;
  }
  else {
    uVar3 = (undefined2)(int)(fVar5 - 2.1474836e+09);
  }
  if (lttd < 0.0) {
    lttd = lttd + 360.0;
  }
  fVar5 = (lttd / 90.0) * 32768.0;
  if (fVar5 < 2.1474836e+09) {
    uVar4 = (undefined2)(int)fVar5;
  }
  else {
    uVar4 = (undefined2)(int)(fVar5 - 2.1474836e+09);
  }
  stack0xffffffbc =
       CONCAT13((char)uVar3,
                CONCAT12((char)((ushort)uVar3 >> 8),CONCAT11((char)uVar4,(char)((ushort)uVar4 >> 8))
                        ));
  _local_3c = CONCAT13((char)((uint)iVar2 >> 0x10),CONCAT12((char)((uint)iVar2 >> 0x18),0xb5a));
  local_38._0_2_ = CONCAT11((char)iVar2,(char)((uint)iVar2 >> 8));
  if (bandwidth == 6) {
    local_68 = '\x02';
  }
  else if (bandwidth < 7) {
    if (bandwidth == 5) {
      local_68 = '\x03';
    }
  }
  else if (bandwidth == 7) {
    local_68 = '\x01';
  }
  else if (bandwidth == 8) {
    local_68 = '\0';
  }
  uVar1 = CONCAT12(local_68 << 5,local_38._0_2_);
  _local_38 = CONCAT13(0x40,uVar1 | 0x1f0000);
  local_34 = 0xffffff1c;
  p_dvbpsi = dvbpsi_new(message,DVBPSI_MSG_DEBUG);
  if (p_dvbpsi != (dvbpsi_t *)0x0) {
    dvbpsi_nit_init(&dStack_1c,0x40,3,0x2114,0x17,true);
    dvbpsi_nit_descriptor_add(&dStack_1c,0x40,4,local_4c);
    p_ts = dvbpsi_nit_ts_add(&dStack_1c,0,0x2114);
    dvbpsi_nit_ts_descriptor_add(p_ts,0x5a,0xb,&uStack_3a);
    dvbpsi_nit_ts_descriptor_add(p_ts,0x6c,10,local_4c + 6);
    dvbpsi_nit_ts_descriptor_add(p_ts,0x41,3,(uint8_t *)((int)&local_2c + 2));
    dStack_1c.b_current_next = true;
    p_section = dvbpsi_nit_sections_generate(p_dvbpsi,&dStack_1c,0x40);
    *packet = 0x47;
    packet[1] = 0;
    packet[2] = 0x10;
    packet[3] = counter & 0xf | 0x10;
    writePSI(packet,p_section);
    dvbpsi_nit_empty(&dStack_1c);
    dvbpsi_DeletePSISections(p_section);
    dvbpsi_delete(p_dvbpsi);
  }
  return (uint)(p_dvbpsi == (dvbpsi_t *)0x0);
}



// ==== genSDT @ 00408cc4 ====

int genSDT(uint8_t *packet,uint8_t counter,char *company,char *service_name,char *sn,float lttd,
          float lngt)

{
  dvbpsi_t *p_dvbpsi;
  dvbpsi_sdt_service_t *p_service;
  size_t sVar1;
  dvbpsi_psi_section_t *p_section;
  undefined1 local_d8;
  undefined1 local_d7;
  uint8_t local_d6 [126];
  dvbpsi_sdt_t dStack_58;
  char acStack_4c [68];
  
                    /* Unresolved local var: uint8_t[128] td48@[???]
                       Unresolved local var: dvbpsi_sdt_t sdt@[???]
                       Unresolved local var: dvbpsi_sdt_service_t * p_ss@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_section1@[???]
                       Unresolved local var: char[64] fullname@[???]
                       Unresolved local var: uint8_t fullnamelen@[???]
                       Unresolved local var: dvbpsi_t * p_dvbpsi@[???] */
  memset(&local_d8,0,0x80);
  local_d8 = 0x48;
  local_d7 = 0xe;
  local_d6[0] = 1;
  local_d6[1] = 4;
  local_d6[2] = 0x4d;
  local_d6[3] = 0x45;
  local_d6[4] = 0x4c;
  local_d6[5] = 0x44;
  local_d6[6] = 8;
  local_d6[7] = 0x4d;
  local_d6[8] = 0x4b;
  local_d6[9] = 0x56;
  local_d6[10] = 0x4d;
  local_d6[0xb] = 0x54;
  local_d6[0xc] = 0x33;
  local_d6[0xd] = 0x30;
  local_d6[0xe] = 0x30;
  p_dvbpsi = dvbpsi_new(message,DVBPSI_MSG_DEBUG);
  if (p_dvbpsi != (dvbpsi_t *)0x0) {
    dvbpsi_sdt_init(&dStack_58,0x42,0,0x10,false,0x2114);
    p_service = dvbpsi_sdt_service_add(&dStack_58,3,false,false,4,false);
    sprintf(acStack_4c,"%s,%s(%s,%3.6f,%3.6f)",company,service_name,sn,(double)lttd,(double)lngt);
    sVar1 = strlen(acStack_4c);
    local_d6[6] = (char)sVar1;
    memcpy(local_d6 + 7,acStack_4c,(size_t)sVar1);
    dvbpsi_sdt_service_descriptor_add(p_service,0x48,local_d6[6] + 7,local_d6);
    dStack_58.b_current_next = true;
    p_section = dvbpsi_sdt_sections_generate(p_dvbpsi,&dStack_58);
    *packet = 0x47;
    packet[1] = 0;
    packet[2] = 0x11;
    packet[3] = counter & 0xf | 0x10;
    writePSI(packet,p_section);
    dvbpsi_sdt_empty(&dStack_58);
    dvbpsi_DeletePSISections(p_section);
    dvbpsi_delete(p_dvbpsi);
  }
  return (uint)(p_dvbpsi == (dvbpsi_t *)0x0);
}



// ==== genTDT @ 00409024 ====

int genTDT(uint8_t *packet,uint8_t counter)

{
  tm *ptVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  dvbpsi_t *p_dvbpsi;
  dvbpsi_psi_section_t *p_section;
  dvbpsi_tot_t dStack_30;
  time_t local_1c;
  
                    /* Unresolved local var: dvbpsi_tot_t tdt@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_section1@[???]
                       Unresolved local var: time_t tt@[???]
                       Unresolved local var: tm * ttx@[???]
                       Unresolved local var: int L@[???]
                       Unresolved local var: uint64_t mjd@[???]
                       Unresolved local var: uint64_t utc@[???]
                       Unresolved local var: dvbpsi_t * p_dvbpsi@[???] */
  local_1c = time((time_t *)0x0);
  ptVar1 = gmtime(&local_1c);
  if ((ptVar1->tm_mon == 0) || (ptVar1->tm_mon == 1)) {
    iVar2 = 1;
  }
  else {
    iVar2 = 0;
  }
  uVar3 = ptVar1->tm_mday + 0x3a6c + (int)((double)(ptVar1->tm_year - iVar2) * 365.25) +
          (int)((double)(ptVar1->tm_mon + 2 + iVar2 * 0xc) * 30.6001);
  uVar4 = ptVar1->tm_hour << 0x10 | ptVar1->tm_min << 8 | ptVar1->tm_sec;
  p_dvbpsi = dvbpsi_new(message,DVBPSI_MSG_DEBUG);
  if (p_dvbpsi != (dvbpsi_t *)0x0) {
    dvbpsi_tot_init(&dStack_30,0x70,0,0x10,false,
                    CONCAT44((int)uVar4 >> 0x1f | uVar3 >> 8 | ((int)uVar3 >> 0x1f) << 0x18,
                             uVar4 | uVar3 * 0x1000000));
    p_section = dvbpsi_tot_sections_generate(p_dvbpsi,&dStack_30);
    *packet = 0x47;
    packet[1] = 0;
    packet[2] = 0x14;
    packet[3] = counter & 0xf | 0x10;
    writePSI(packet,p_section);
    dvbpsi_tot_empty(&dStack_30);
    dvbpsi_DeletePSISections(p_section);
    dvbpsi_delete(p_dvbpsi);
  }
  return (uint)(p_dvbpsi == (dvbpsi_t *)0x0);
}



// ==== genEIT @ 00409330 ====

int genEIT(uint8_t *packet,uint8_t counter)

{
  dvbpsi_t *p_dvbpsi;
  int iVar1;
  tm *ptVar2;
  uint uVar3;
  uint uVar4;
  dvbpsi_eit_event_t *p_event;
  dvbpsi_psi_section_t *p_section;
  dvbpsi_eit_t dStack_98;
  time_t local_88;
  undefined4 local_84;
  uint8_t auStack_82 [2];
  undefined4 local_80;
  undefined4 local_7c;
  undefined4 local_78;
  undefined4 local_74;
  undefined4 local_70;
  undefined4 local_6c;
  undefined4 local_68;
  undefined4 local_64;
  undefined4 local_60;
  undefined1 local_5c;
  undefined1 local_5b;
  uint8_t local_5a [66];
  
                    /* Unresolved local var: dvbpsi_psi_section_t * p_section1@[???]
                       Unresolved local var: dvbpsi_eit_t eit@[???]
                       Unresolved local var: dvbpsi_eit_event_t * et@[???]
                       Unresolved local var: dvbpsi_t * p_dvbpsi@[???]
                       Unresolved local var: time_t tt@[???]
                       Unresolved local var: tm * ttx@[???]
                       Unresolved local var: int L@[???]
                       Unresolved local var: uint64_t mjd@[???]
                       Unresolved local var: uint64_t utc@[???]
                       Unresolved local var: uint8_t[32] td4e@[???]
                       Unresolved local var: uint8_t[8] td54@[???]
                       Unresolved local var: uint8_t[64] td4d@[???] */
  p_dvbpsi = dvbpsi_new(message,DVBPSI_MSG_DEBUG);
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
    iVar1 = 1;
  }
  else {
    local_88 = time((time_t *)0x0);
    ptVar2 = gmtime(&local_88);
    if ((ptVar2->tm_mon == 0) || (ptVar2->tm_mon == 1)) {
      iVar1 = 1;
    }
    else {
      iVar1 = 0;
    }
    uVar3 = ptVar2->tm_mday + 0x3a6c + (int)((double)(ptVar2->tm_year - iVar1) * 365.25) +
            (int)((double)(ptVar2->tm_mon + 2 + iVar1 * 0xc) * 30.6001);
    uVar4 = ptVar2->tm_hour << 0x10 | ptVar2->tm_min << 8 | ptVar2->tm_sec;
    local_70 = 0;
    local_6c = 0;
    local_68 = 0;
    local_84 = 0x6500114e;
    local_80 = 0xb00756e;
    local_7c = 0x74736574;
    local_78 = 0x20666f20;
    local_74 = 0x544945;
    local_60 = 0;
    local_64 = 0x920254;
    memset(&local_5c,0,0x40);
    local_5c = 0x4d;
    local_5b = 0x1e;
    local_5a[0] = 0x65;
    local_5a[1] = 0x6e;
    local_5a[2] = 0x75;
    local_5a[3] = 0xf;
    local_5a[4] = 0x74;
    local_5a[5] = 0x65;
    local_5a[6] = 0x73;
    local_5a[7] = 0x74;
    local_5a[8] = 0x20;
    local_5a[9] = 0x65;
    local_5a[10] = 0x76;
    local_5a[0xb] = 0x65;
    local_5a[0xc] = 0x6e;
    local_5a[0xd] = 0x74;
    local_5a[0xe] = 0x2d;
    local_5a[0xf] = 0x6e;
    local_5a[0x10] = 0x61;
    local_5a[0x11] = 0x6d;
    local_5a[0x12] = 0x65;
    local_5a[0x13] = 10;
    local_5a[0x14] = 0x65;
    local_5a[0x15] = 0x78;
    local_5a[0x16] = 0x70;
    local_5a[0x17] = 0x65;
    local_5a[0x18] = 0x72;
    local_5a[0x19] = 0x69;
    local_5a[0x1a] = 0x6d;
    local_5a[0x1b] = 0x65;
    local_5a[0x1c] = 0x6e;
    local_5a[0x1d] = 0x74;
    dvbpsi_eit_init(&dStack_98,0x4e,3,0x10,false,0,0x2114,0,0);
    p_event = dvbpsi_eit_event_add
                        (&dStack_98,0,
                         CONCAT44((int)uVar4 >> 0x1f | uVar3 >> 8 | ((int)uVar3 >> 0x1f) << 0x18,
                                  uVar4 | uVar3 * 0x1000000),0x5dc,0,false,0x32);
    dvbpsi_eit_event_descriptor_add(p_event,0x4e,0x11,auStack_82);
    dvbpsi_eit_event_descriptor_add(p_event,0x54,2,(uint8_t *)((int)&local_64 + 2));
    dvbpsi_eit_event_descriptor_add(p_event,0x4d,0x1e,local_5a);
    p_section = dvbpsi_eit_sections_generate(p_dvbpsi,&dStack_98,0x4e);
    *packet = 0x47;
    packet[1] = 0;
    packet[2] = 0x12;
    packet[3] = counter & 0xf | 0x10;
    writePSI(packet,p_section);
    dvbpsi_eit_empty(&dStack_98);
    dvbpsi_DeletePSISections(p_section);
    dvbpsi_delete(p_dvbpsi);
    iVar1 = 0;
  }
  return iVar1;
}



// ==== encoder_recv_video_thread_entry @ 00409900 ====

void * encoder_recv_video_thread_entry(void *arg)

{
  size_t param2;
  vixs_encoder_t *pEnCoder;
  util_thread_t *mThread;
  return_code_t rVar1;
  uint32 uVar2;
  int *piVar3;
  char *param3;
  uint local_50;
  size_t local_34;
  vixs_xcode_recv_strm_info_t vStack_30;
  
                    /* Unresolved local var: util_thread_arg_t * tArg@[???]
                       Unresolved local var: vixs_encoder_t * pEnCoder@[???]
                       Unresolved local var: util_thread_t * mThread@[???]
                       Unresolved local var: int bufSize@[???]
                       Unresolved local var: int consumedIdx@[???]
                       Unresolved local var: int curBufIdx@[???]
                       Unresolved local var: vixs_xcode_recv_strm_info_t recvInfo@[???]
                       Unresolved local var: int txLen@[???]
                       Unresolved local var: return_code_t rc@[???] */
  pEnCoder = *(vixs_encoder_t **)((int)arg + 4);
  mThread = *(util_thread_t **)arg;
  local_50 = 0;
  util_thread_notify_ready(mThread,RET_OK);
  printf("%s:Enter Encoder %x recv thread Entry \n","encoder_recv_video_thread_entry",pEnCoder->idx)
  ;
  while ((pEnCoder->stopRecv == 0 && (mThread->toExit == 0))) {
    rVar1 = vixs_encoder_recv_stream
                      (pEnCoder,(int *)&local_34,pEnCoder->lastConsumedRecvBufIdx,&vStack_30);
    if (rVar1 == RET_OK) {
      if (((uint)vStack_30.result & 0x1000) != 0) {
        printf("%s:Received last frame\n","encoder_recv_video_thread_entry");
        pEnCoder->stopRecv = 1;
        pEnCoder->stopEncode = 1;
      }
      uVar2 = (pEnCoder->ioctl_sdev_recv).dmabuffer_idx;
      if (pEnCoder->outputMode == VIXS_ENCODER_OUT_MODE_UDP) {
        if (pEnCoder->dstSock != 0) {
          rVar1 = util_sendto(pEnCoder->dstSock,pEnCoder->pRecvBuf[uVar2],local_34,0,
                              (sockaddr *)&pEnCoder->dstSockAddr,0x10);
          param2 = local_34;
          if (rVar1 == RET_OK) {
            local_50 = local_34;
          }
          else {
            piVar3 = __errno_location();
            param3 = strerror(*piVar3);
            printf("%s:Send error Payload %x: %s\n","encoder_recv_video_thread_entry",param2,param3)
            ;
            local_50 = 0;
            pEnCoder->totalUdpOutError = pEnCoder->totalUdpOutError + 1;
          }
        }
      }
      else if (pEnCoder->outputMode == VIXS_ENCODER_OUT_MODE_FILE) {
        write(pEnCoder->captureOutFileFd,pEnCoder->pRecvBuf[uVar2],local_34);
      }
      pEnCoder->totalOutputSize = pEnCoder->totalOutputSize + local_50;
      pEnCoder->lastConsumedRecvBufIdx = uVar2;
    }
    else {
      printf("%s:Xcoder %x Receive from AV driver error\n","encoder_recv_video_thread_entry",
             pEnCoder->idx);
      if ((((vStack_30.returnValue == 0x1b) || (vStack_30.returnValue == 0x1e)) ||
          (vStack_30.returnValue == 5)) || (vStack_30.returnValue == 0x24)) {
        pEnCoder->stopRecv = 1;
      }
      else {
        pEnCoder->recvTimeoutCnt = pEnCoder->recvTimeoutCnt - 1;
        if (pEnCoder->recvTimeoutCnt == 0) {
          pEnCoder->stopRecv = 1;
        }
      }
    }
  }
  printf("%s:Exit recv thread Entry \n","encoder_recv_video_thread_entry");
  return (void *)0x0;
}



// ==== terminate_encoder_recv_thread @ 00409d30 ====

return_code_t terminate_encoder_recv_thread(vixs_encoder_t *pEnCoder)

{
  return_code_t param2;
  
                    /* Unresolved local var: return_code_t rc@[???] */
  param2 = util_destroy_thread(&pEnCoder->recvVideoThread);
  if (param2 != RET_OK) {
    printf("%s:Fail to terminate encode recv thread, return value %x\n",
           "terminate_encoder_recv_thread",param2);
  }
  return param2;
}



// ==== launch_encoder_recv_thead @ 00409dc8 ====

return_code_t launch_encoder_recv_thead(vixs_encoder_t *pEnCoder)

{
  return_code_t param2;
  
                    /* Unresolved local var: return_code_t rc@[???] */
  param2 = util_create_thread(&pEnCoder->recvVideoThread,encoder_recv_video_thread_entry,pEnCoder);
  if (param2 != RET_OK) {
    printf("%s:Fail to create encode recv thread, return value %x\n","launch_encoder_recv_thead",
           param2);
  }
  return param2;
}



// ==== readCfgFile @ 00409e6c ====

int readCfgFile(char *pth,fileconf *conf)

{
  FILE *__stream;
  int iVar1;
  size_t sVar2;
  long lVar3;
  __ssize_t _Var4;
  int local_1c;
  char *local_14;
  size_t local_10 [2];
  
                    /* Unresolved local var: FILE * config@[???]
                       Unresolved local var: char * line@[???]
                       Unresolved local var: size_t linesize@[???]
                       Unresolved local var: size_t size@[???]
                       Unresolved local var: int result@[???] */
  local_10[0] = 0;
  local_1c = 0;
  __stream = fopen64(pth,"r");
  conf->modulationmode = 1;
  conf->bandwidth = 0;
  conf->chan_freq = 0x1b;
  conf->txpower = 0x35;
  conf->dacgain = 0xab;
  memcpy(conf," v0.034 Tx v2",0xe);
  memcpy(conf->fcc,"OKVMT300",9);
  memcpy(conf->sn,"00000467",9);
  if (__stream == (FILE *)0x0) {
    local_1c = -1;
  }
  else {
    while( true ) {
      _Var4 = getline(&local_14,local_10,__stream);
      if (_Var4 == -1) break;
      iVar1 = strncmp(local_14,"Modulation mode = ",0x12);
      if (iVar1 == 0) {
        conf->bandwidth = 0;
        iVar1 = strncmp(local_14 + 0x12,"ATSC",4);
        if (iVar1 == 0) {
          conf->modulationmode = 1;
        }
        else {
          iVar1 = strncmp(local_14 + 0x12,"DVB-T",5);
          if (iVar1 == 0) {
            conf->modulationmode = 0;
            if (local_10[0] < 0x1a) {
              conf->bandwidth = 8;
            }
            else {
              conf->bandwidth = local_14[0x19] + -0x30;
            }
          }
          else {
            conf->modulationmode = 2;
          }
        }
      }
      iVar1 = strncmp(local_14,"MT300-AV Firmware version = ",0x1c);
      if (iVar1 == 0) {
        sVar2 = _Var4 - 0x1d;
        if (0x10 < sVar2) {
          sVar2 = 0x10;
        }
        memcpy(conf,local_14 + 0x1c,sVar2);
      }
      iVar1 = strncmp(local_14,"S/N = ",6);
      if (iVar1 == 0) {
        sVar2 = _Var4 - 7;
        if (0x10 < sVar2) {
          sVar2 = 0x10;
        }
        memcpy(conf->sn,local_14 + 6,sVar2);
      }
      iVar1 = strncmp(local_14,"FCC ID = ",9);
      if (iVar1 == 0) {
        sVar2 = _Var4 - 10;
        if (0x10 < sVar2) {
          sVar2 = 0x10;
        }
        memcpy(conf->fcc,local_14 + 9,sVar2);
      }
      iVar1 = strncmp(local_14,"Channel = ",10);
      if (iVar1 == 0) {
        iVar1 = atoi(local_14 + 10);
        conf->chan_freq = iVar1;
      }
      iVar1 = strncmp(local_14,"Frequency = ",0xc);
      if (iVar1 == 0) {
        iVar1 = atoi(local_14 + 0xc);
        conf->chan_freq = iVar1;
      }
      iVar1 = strncmp(local_14,"TX power = ",0xb);
      if (iVar1 == 0) {
        lVar3 = strtol(local_14 + 0xb,(char **)0x0,0x10);
        conf->txpower = lVar3;
      }
      iVar1 = strncmp(local_14,"DAC gain = ",0xb);
      if (iVar1 == 0) {
        lVar3 = strtol(local_14 + 0xb,(char **)0x0,0x10);
        conf->dacgain = lVar3;
      }
    }
    fclose(__stream);
  }
  return local_1c;
}



// ==== readSbiFile @ 0040a3f8 ====

int readSbiFile(char *pth,float *lngt,float *lttd)

{
  FILE *__stream;
  int iVar1;
  __ssize_t _Var2;
  float fVar3;
  int local_18;
  char *local_10;
  size_t local_c;
  
                    /* Unresolved local var: FILE * config@[???]
                       Unresolved local var: char * line@[???]
                       Unresolved local var: size_t linesize@[???]
                       Unresolved local var: int result@[???] */
  local_c = 0;
  local_18 = 0;
  __stream = fopen64(pth,"r");
  *lngt = 0.0;
  *lttd = 0.0;
  if (__stream == (FILE *)0x0) {
    local_18 = -1;
  }
  else {
    while( true ) {
      _Var2 = getline(&local_10,&local_c,__stream);
      if (_Var2 == -1) break;
      iVar1 = strncmp(local_10,"    option \'Longitude\' \'",0x18);
      if (iVar1 == 0) {
        fVar3 = strtof(local_10 + 0x18,(char **)0x0);
        *lngt = fVar3;
      }
      iVar1 = strncmp(local_10,"    option \'Latitude\' \'",0x17);
      if (iVar1 == 0) {
        fVar3 = strtof(local_10 + 0x17,(char **)0x0);
        *lttd = fVar3;
      }
    }
    fclose(__stream);
  }
  return local_18;
}



// ==== readCompanyFile @ 0040a5ac ====

int readCompanyFile(char *pth,char *company_name)

{
  FILE *__stream;
  int iVar1;
  __ssize_t _Var2;
  size_t __n;
  char *local_10;
  size_t local_c;
  
                    /* Unresolved local var: FILE * config@[???]
                       Unresolved local var: char * line@[???]
                       Unresolved local var: size_t linesize@[???]
                       Unresolved local var: size_t size@[???] */
  local_c = 0;
  __stream = fopen64(pth,"r");
  if (__stream == (FILE *)0x0) {
    memcpy(company_name,"Meldtech",9);
    iVar1 = -1;
  }
  else {
    _Var2 = getline(&local_10,&local_c,__stream);
    if (_Var2 != 0) {
      __n = _Var2 - 1;
      if (0x20 < __n) {
        __n = 0x20;
      }
      memcpy(company_name,local_10,__n);
    }
    fclose(__stream);
    iVar1 = 0x20;
  }
  return iVar1;
}



// ==== encoder_send_data_thread_entry @ 0040a6ec ====

void * encoder_send_data_thread_entry(void *arg)

{
  vixs_encoder_t *pEnCoder;
  util_thread_t *mThread;
  size_t ext_name_len;
  uint8_t *dat8;
  uint32_t vctlen;
  int iVar1;
  return_code_t rVar2;
  uint8_t local_128;
  uint8_t local_127;
  uint8_t local_126;
  uint local_124;
  uint32 local_e8;
  fileconf fStack_e4;
  float local_a0;
  float local_9c;
  char acStack_98 [32];
  uint16_t local_78 [4];
  uint32_t local_70;
  int local_6c;
  int local_68;
  int local_64;
  uint16_t local_60 [4];
  uint32_t local_58;
  int local_54;
  int local_50;
  int local_4c;
  char acStack_48 [64];
  
                    /* Unresolved local var: util_thread_arg_t * tArg@[???]
                       Unresolved local var: vixs_encoder_t * pEnCoder@[???]
                       Unresolved local var: util_thread_t * mThread@[???]
                       Unresolved local var: int bufSize@[???]
                       Unresolved local var: uint64 flag@[???]
                       Unresolved local var: return_code_t rc@[???]
                       Unresolved local var: uint8_t psicounter@[???]
                       Unresolved local var: uint8_t ettcounter@[???]
                       Unresolved local var: uint8_t eitcounter@[???]
                       Unresolved local var: fileconf conf@[???]
                       Unresolved local var: float lngt@[???]
                       Unresolved local var: float lttd@[???]
                       Unresolved local var: char[32] company@[???]
                       Unresolved local var: uint32_t timeoffset@[???]
                       Unresolved local var: uint16_t pid@[???]
                       Unresolved local var: uint16_t[4] eitpid@[???]
                       Unresolved local var: uint32_t[4] eitlen@[???]
                       Unresolved local var: uint16_t[4] ettpid@[???]
                       Unresolved local var: uint32_t[4] ettlen@[???]
                       Unresolved local var: uint8_t eitcnt@[???]
                       Unresolved local var: uint8_t ettcnt@[???]
                       Unresolved local var: uint32_t vctlen@[???]
                       Unresolved local var: uint8_t * dat8@[???]
                       Unresolved local var: uint32_t start_pos@[???]
                       Unresolved local var: int len@[???]
                       Unresolved local var: int kk@[???]
                       Unresolved local var: char[64] extended_name@[???]
                       Unresolved local var: int ext_name_len@[???] */
  pEnCoder = *(vixs_encoder_t **)((int)arg + 4);
  mThread = *(util_thread_t **)arg;
  local_128 = 0;
  local_127 = 0;
  local_126 = 0;
  local_78[0] = 0x1000;
  local_78[1] = 0x1001;
  local_78[2] = 0x1002;
  local_78[3] = 0x1003;
  local_70 = 0;
  local_6c = 0;
  local_68 = 0;
  local_64 = 0;
  local_60[0] = 0x1100;
  local_60[1] = 0x1101;
  local_60[2] = 0x1102;
  local_60[3] = 0x1103;
  local_58 = 0;
  local_54 = 0;
  local_50 = 0;
  local_4c = 0;
  util_thread_notify_ready(mThread,RET_OK);
  memset(&fStack_e4,0,0x44);
  memset(acStack_98,0,0x20);
  readCfgFile("/etc/config/queryInfo",&fStack_e4);
  readSbiFile("/etc/config/sbi",&local_a0,&local_9c);
  readCompanyFile("/etc/config/company",acStack_98);
  sprintf(acStack_48,"%s,%s(%s,%3.6f, %3.6f)",acStack_98,fStack_e4.fcc,fStack_e4.sn,(double)local_a0
          ,(double)local_9c);
  ext_name_len = strlen(acStack_48);
  printf("%s:Enter Encoder %x send data thread Entry \n","encoder_send_data_thread_entry",
         pEnCoder->idx);
  while (mThread->toExit == 0) {
    local_e8 = pEnCoder->sendDataSize;
    if (fStack_e4.modulationmode == 0) {
      genNIT((uint8_t *)pEnCoder->pSendDataBuf,local_128,fStack_e4.chan_freq,fStack_e4.bandwidth,
             local_a0,local_9c);
      genSDT((uint8_t *)(pEnCoder->pSendDataBuf + 0xbc),local_128,acStack_98,fStack_e4.fcc,
             fStack_e4.sn,local_a0,local_9c);
      genTDT((uint8_t *)(pEnCoder->pSendDataBuf + 0x178),local_128);
      genEIT((uint8_t *)(pEnCoder->pSendDataBuf + 0x234),local_128);
    }
    else if (fStack_e4.modulationmode == 1) {
      dat8 = (uint8_t *)pEnCoder->pSendDataBuf;
      FormTSHeader(dat8,0x1ffb,local_128);
      vctlen = FormATSC_TVCT((uint8_t *)(pEnCoder->pSendDataBuf + 5),(uint16_t)fStack_e4.chan_freq,
                             sti_minor,acStack_48,ext_name_len,0);
      for (local_124 = vctlen + 5; local_124 < 0xbc; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0xbc,local_78[0],local_126);
      local_70 = FormATSC_EIT((uint8_t *)(pEnCoder->pSendDataBuf + 0xc1),acStack_48,ext_name_len,0,
                              10,1);
      for (local_124 = local_70 + 0xc1; local_124 < 0x178; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x178,local_78[1],local_126);
      local_6c = FormATSC_EIT((uint8_t *)(pEnCoder->pSendDataBuf + 0x17d),acStack_48,ext_name_len,
                              0x5460,0x14,1);
      for (local_124 = local_6c + 0x17d; local_124 < 0x234; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x234,local_78[2],local_126);
      local_68 = FormATSC_EIT((uint8_t *)(pEnCoder->pSendDataBuf + 0x239),acStack_48,ext_name_len,
                              0xa8c0,0x1e,1);
      for (local_124 = local_68 + 0x239; local_124 < 0x2f0; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x2f0,local_78[3],local_126);
      local_64 = FormATSC_EIT((uint8_t *)(pEnCoder->pSendDataBuf + 0x2f5),acStack_48,ext_name_len,
                              0xfd20,0x28,1);
      for (local_124 = local_64 + 0x2f5; local_124 < 0x3ac; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      local_126 = local_126 + 1;
      FormTSHeader(dat8 + 0x3ac,local_60[0],local_127);
      local_58 = FormATSC_ETT((uint8_t *)(pEnCoder->pSendDataBuf + 0x3b1),acStack_48,ext_name_len,3,
                              10);
      for (local_124 = local_58 + 0x3b1; local_124 < 0x468; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x468,local_60[1],local_127);
      local_54 = FormATSC_ETT((uint8_t *)(pEnCoder->pSendDataBuf + 0x46d),acStack_48,ext_name_len,3,
                              0x14);
      for (local_124 = local_54 + 0x46d; local_124 < 0x524; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x524,local_60[2],local_127);
      local_50 = FormATSC_ETT((uint8_t *)(pEnCoder->pSendDataBuf + 0x529),acStack_48,ext_name_len,3,
                              0x1e);
      for (local_124 = local_50 + 0x529; local_124 < 0x5e0; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x5e0,local_60[3],local_127);
      local_4c = FormATSC_ETT((uint8_t *)(pEnCoder->pSendDataBuf + 0x5e5),acStack_48,ext_name_len,3,
                              0x28);
      for (local_124 = local_4c + 0x5e5; local_124 < 0x69c; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      local_127 = local_127 + 1;
      FormTSHeader(dat8 + 0x69c,0x1ffb,local_128 + 1);
      iVar1 = FormATSC_MGT((uint8_t *)(pEnCoder->pSendDataBuf + 0x6a1),vctlen,&local_70,local_78,4,
                           &local_58,local_60,4);
      for (local_124 = iVar1 + 0x6a1; local_124 < 0x758; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      local_128 = local_128 + 2;
      FormTSHeader(dat8 + 0x758,0x1ffb,local_128);
      iVar1 = FormATSC_STT((uint8_t *)(pEnCoder->pSendDataBuf + 0x75d));
      for (local_124 = iVar1 + 0x75d; local_124 < 0x814; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
    }
    local_128 = local_128 + 1;
    rVar2 = vixs_encoder_send_stream(pEnCoder,(int *)&local_e8,0x8000000);
    if (rVar2 != RET_OK) {
      printf("%s:Xcoder %x Send data to AV driver error\n","encoder_send_data_thread_entry",
             pEnCoder->idx);
      mThread->toExit = 1;
    }
    if (pEnCoder->sendDataInterval == 0) {
      mThread->toExit = 1;
    }
    else {
      usleep(pEnCoder->sendDataInterval * 1000);
    }
  }
  printf("%s:Exit recv thread Entry \n","encoder_send_data_thread_entry");
  local_e8 = 0;
  vixs_encoder_send_stream(pEnCoder,(int *)&local_e8,0x8400000);
  return (void *)0x0;
}



// ==== terminate_encoder_send_data_thread @ 0040b57c ====

return_code_t terminate_encoder_send_data_thread(vixs_encoder_t *pEnCoder)

{
  return_code_t param2;
  
                    /* Unresolved local var: return_code_t rc@[???] */
  param2 = util_destroy_thread(&pEnCoder->sendDataThread);
  if (param2 != RET_OK) {
    printf("%s:Fail to terminate encoder send data thread, return value %x\n",
           "terminate_encoder_send_data_thread",param2);
  }
  return param2;
}



// ==== launch_encoder_send_data_thead @ 0040b614 ====

return_code_t launch_encoder_send_data_thead(vixs_encoder_t *pEnCoder)

{
  return_code_t param2;
  
                    /* Unresolved local var: return_code_t rc@[???] */
  param2 = util_create_thread(&pEnCoder->sendDataThread,encoder_send_data_thread_entry,pEnCoder);
  if (param2 != RET_OK) {
    printf("%s:Fail to create encoder send data thread, return value %x\n",
           "launch_encoder_send_data_thead",param2);
  }
  return param2;
}



// ==== xcode4_open_encoder @ 0040b6b8 ====

return_code_t xcode4_open_encoder(int *handle,vixs_encoder_init_param_t *param)

{
  vixs_encoder_t *pEnCoder;
  return_code_t rVar1;
  
                    /* Unresolved local var: return_code_t rc@[???]
                       Unresolved local var: vixs_encoder_t * pEnCoder@[???] */
  pEnCoder = malloc(0x1980);
  if (pEnCoder == (vixs_encoder_t *)0x0) {
    printf("%s:No memory\n","xcode4_open_encoder");
    rVar1 = RET_ERR;
  }
  else {
    rVar1 = vixs_encoder_init(pEnCoder,param);
    if (rVar1 == RET_OK) {
      printf("%s:Init Encoder Sucess\n","xcode4_open_encoder");
      *handle = (int)pEnCoder;
    }
    else {
      printf("%s:Init Encoder Failed\n","xcode4_open_encoder");
      free(pEnCoder);
      *handle = 0;
    }
  }
  return rVar1;
}



// ==== xcode4_start_encoder @ 0040b804 ====

return_code_t xcode4_start_encoder(int handle)

{
  return_code_t local_10;
  
                    /* Unresolved local var: return_code_t rc@[???]
                       Unresolved local var: vixs_encoder_t * pEnCoder@[???] */
  printf("%s:E\n","xcode4_start_encoder");
  if (handle == 0) {
    printf("%s:Invalid Handle\n","xcode4_start_encoder");
    local_10 = RET_ERR;
  }
  else {
    local_10 = vixs_encoder_open_stream((vixs_encoder_t *)handle);
    if (local_10 == RET_OK) {
      if ((*(int *)(handle + 0x3c) == 1) || (*(int *)(handle + 0x3c) == 3)) {
        local_10 = launch_encoder_recv_thead((vixs_encoder_t *)handle);
        if (local_10 != RET_OK) {
          printf("%s:Launch Encode Thread failed\n","xcode4_start_encoder");
          vixs_encoder_close_stream((vixs_encoder_t *)handle);
        }
      }
      if (*(int *)(handle + 0xd4) != 0) {
        local_10 = launch_encoder_send_data_thead((vixs_encoder_t *)handle);
        if (local_10 != RET_OK) {
          printf("%s:Launch Send Thread failed\n","xcode4_start_encoder");
          if ((*(int *)(handle + 0x3c) == 1) || (*(int *)(handle + 0x3c) == 3)) {
            terminate_encoder_recv_thread((vixs_encoder_t *)handle);
          }
          vixs_encoder_close_stream((vixs_encoder_t *)handle);
        }
      }
    }
    else {
      printf("%s:Open Encoder stream failed\n","xcode4_start_encoder");
    }
    printf("%s:X\n","xcode4_start_encoder");
  }
  return local_10;
}



// ==== xcode4_stop_encoder @ 0040ba60 ====

return_code_t xcode4_stop_encoder(int handle)

{
  return_code_t local_10;
  
                    /* Unresolved local var: return_code_t rc@[???]
                       Unresolved local var: vixs_encoder_t * pEnCoder@[???] */
  printf("%s:E\n","xcode4_stop_encoder");
  if ((*(int *)(handle + 0x3c) == 1) || (*(int *)(handle + 0x3c) == 3)) {
    local_10 = terminate_encoder_recv_thread((vixs_encoder_t *)handle);
    if (local_10 != RET_OK) {
      printf("%s:Fail to terminate encode_thread, return value %x\n","xcode4_stop_encoder",local_10)
      ;
    }
  }
  else {
    vixs_encoder_flush_stream((vixs_encoder_t *)handle);
  }
  if (*(int *)(handle + 0xd4) != 0) {
    local_10 = terminate_encoder_send_data_thread((vixs_encoder_t *)handle);
    if (local_10 != RET_OK) {
      printf("%s:Fail to terminate encoder_send_data_thread, return value %x\n",
             "xcode4_stop_encoder",local_10);
    }
  }
  vixs_encoder_close_stream((vixs_encoder_t *)handle);
  printf("%s:X\n","xcode4_stop_encoder");
  return local_10;
}



// ==== xcode4_close_encoder @ 0040bbfc ====

return_code_t xcode4_close_encoder(int handle)

{
                    /* Unresolved local var: vixs_encoder_t * pEnCoder@[???] */
  free((void *)handle);
  return RET_OK;
}



// ==== xcode4_update_encoder_param @ 0040bc58 ====

return_code_t xcode4_update_encoder_param(int handle,vixs_encoder_update_param_t *param)

{
  return_code_t rVar1;
  
                    /* Unresolved local var: vixs_encoder_t * pEnCoder@[???] */
  rVar1 = vixs_encoder_update_stream((vixs_encoder_t *)handle,param);
  return rVar1;
}



// ==== xcode4_get_encoder_ts_info @ 0040bcb8 ====

return_code_t xcode4_get_encoder_ts_info(int handle,vixs_encoder_ts_info_t *info)

{
  return_code_t rVar1;
  
                    /* Unresolved local var: vixs_encoder_t * pEnCoder@[???]
                       Unresolved local var: return_code_t rc@[???] */
  rVar1 = vixs_encoder_get_ts_info((vixs_encoder_t *)handle);
  if (rVar1 == RET_OK) {
    info->inputPsiLock = *(uint *)(handle + 0x1324) & 1;
    info->inputVideoLock = (uint)((*(uint *)(handle + 0x1324) & 2) != 0);
    info->inputAudioLock = (uint)((*(uint *)(handle + 0x1324) & 4) != 0);
    info->inputPCRLock = (uint)((*(uint *)(handle + 0x1324) & 8) != 0);
    info->inputVideoCCErr = (uint)((*(uint *)(handle + 0x1324) & 0x10) != 0);
    info->inputAudioCCErr = (uint)((*(uint *)(handle + 0x1324) & 0x20) != 0);
    info->tsOutput = (uint)((*(uint *)(handle + 0x1324) & 0x40) == 0);
    info->videoOutput = (uint)((*(uint *)(handle + 0x1324) & 0x80) == 0);
    info->audioOutput = (uint)((*(uint *)(handle + 0x1324) & 0x100) == 0);
  }
  return rVar1;
}



