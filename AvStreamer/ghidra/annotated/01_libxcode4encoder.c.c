/*
 * 01_libxcode4encoder.c.c  --  annotated decompilation of libxcode4encoder.c
 * ============================================================================
 *
 * Original source path (from DWARF):
 *   /media/WD_passport/Meld_AV_Script/2p13_a/avstreamer/src/libxcode4encoder/libxcode4encoder.c
 * Address range: 0x00405420 .. 0x0040bdfc (30 functions, 1735 raw lines)
 *
 * Role in av_streamer:
 *   High-level ViXS XCode4 encoder wrapper, thread manager, and ATSC PSIP /
 *   DVB-T SI table generator. Called by main.c (start_encoder(), main()) and
 *   delegates low-level /dev/vixs/xcodedrv ioctls to vixs_encoder.c.
 *
 * Main data structures (DWARF):
 *   - vixs_encoder_t (0x1980 = 6528 bytes): encoder instance allocated by
 *     xcode4_open_encoder(). Key offsets left raw by Ghidra:
 *       +0x003c  outputMode (1=UDP, 2=DVO, 3=FILE)
 *       +0x00d4  bSendData  (non-zero when -g PSIP/SI injection is enabled)
 *       +0x125c  ioctl_ts_ctrl (IOCTLSDEVTSCTRL)
 *       +0x1324  ioctl_ts_ctrl.ioctl_param.tsinput_lock (TS status bitmask)
 *   - fileconf (t_conf, 68 bytes): parsed from /etc/config/queryInfo
 *   - vixs_encoder_ts_info_t (36 bytes): unpacked TS lock/output status
 *
 * Globals owned by this unit:
 *   - crc32_tab[256] @ 0x00436170: byte-swapped MPEG-2 CRC-32 (0x04c11db7) LUT
 *   - sti_minor      @ 0x00436574: uint16_t PSIP TVCT minor channel (default 1)
 */

// ==== message @ 00405420 ====

/*
 * message(): libdvbpsi logging callback (dvbpsi_message_cb).
 *   Params: handle (unused), level (DVBPSI_MSG_ERROR/WARN/DEBUG), msg
 *   Returns: void
 *   Callers: passed to dvbpsi_new() in genNIT, genSDT, genTDT, genEIT.
 *   Side effects: writes formatted log line to stderr; ignores DVBPSI_MSG_NONE (-1).
 */
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

/*
 * writePSI(): packs a libdvbpsi PSI section list into a single 188-byte (0xbc)
 *             MPEG-2 TS packet whose 4-byte TS header (PID, CC) was already
 *             written by the caller.
 *   Params: p_packet (188-byte TS buffer), p_section (linked list of sections)
 *   Returns: void
 *   Callers: genNIT, genSDT, genTDT, genEIT
 *   Locals:
 *     local_res4 = p_section loop cursor
 *     local_14   = p_byte (source pointer into p_section->p_data)
 *     puVar2     = p_end  (p_section->p_payload_end)
 *     iVar1      = trailing CRC length (4 if b_syntax_indicator, else 0)
 *     local_18   = p_pos_in_ts (destination pointer starting at p_packet + 5)
 *   Quirk: if p_section->p_next != NULL, subsequent sections overwrite the same
 *          188-byte packet buffer from offset 5 instead of advancing packets.
 */
void writePSI(uint8_t *p_packet,dvbpsi_psi_section_t *p_section)

{
  int iVar1;
  uint8_t *puVar2;
  dvbpsi_psi_section_t *local_res4;
  uint8_t *local_18;
  uint8_t *local_14;
  
  *p_packet = 0x47;                                // TS sync byte
  for (local_res4 = p_section; local_res4 != (dvbpsi_psi_section_t *)0x0;
      local_res4 = local_res4->p_next) {
                    /* Unresolved local var: uint8_t * p_pos_in_ts@[???]
                       Unresolved local var: uint8_t * p_byte@[???]
                       Unresolved local var: uint8_t * p_end@[???] */
    local_14 = local_res4->p_data;
    puVar2 = local_res4->p_payload_end;
    if (local_res4->b_syntax_indicator == false) {
      iVar1 = 0;                                   // short-syntax section (e.g. TDT): no CRC32
    }
    else {
      iVar1 = 4;                                   // long-syntax section: include 4-byte CRC32
    }
    p_packet[1] = p_packet[1] | 0x40;              // set Payload Unit Start Indicator (PUSI = 1)
    p_packet[4] = 0;                               // PSI pointer_field = 0
    local_18 = p_packet + 5;
    for (; (local_18 < p_packet + 0xbc && (local_14 < puVar2 + iVar1)); local_14 = local_14 + 1) {
      *local_18 = *local_14;
      local_18 = local_18 + 1;
    }
    for (; local_18 < p_packet + 0xbc; local_18 = local_18 + 1) {
      *local_18 = 0xff;                            // pad remainder of 188-byte TS packet with 0xff stuffing
    }
  }
  return;
}



// ==== bswap_32 @ 004056dc ====

/*
 * bswap_32(): byte-swaps a 32-bit unsigned integer (endian conversion).
 *   Params: x (32-bit value)
 *   Returns: byte-swapped uint32_t
 *   Callers: FormATSC_MGT, FormATSC_ETT, FormATSC_EIT, FormATSC_TVCT, FormATSC_STT
 *   Locals: uVar1 = intermediate value with adjacent bytes swapped within 16-bit halves
 */
uint32_t bswap_32(uint32_t x)

{
  uint uVar1;
  
  uVar1 = (x & 0xff00ff) << 8 | x >> 8 & 0xff00ff; // swap bytes within each 16-bit half
  return (uVar1 >> 0x10) + uVar1 * 0x10000;        // swap the two 16-bit halves
}



// ==== crc32 @ 00405740 ====

/*
 * crc32(): computes MPEG-2 PSI/PSIP CRC-32 (polynomial 0x04c11db7, MSB-first).
 *   Params: crc (initial seed, callers pass 0xffffffff), buffer, length
 *   Returns: byte-swapped 32-bit CRC (callers pass result to bswap_32())
 *   Callers: FormATSC_MGT, FormATSC_ETT, FormATSC_EIT, FormATSC_TVCT, FormATSC_STT
 *   Locals: local_res0 = running CRC; local_res4 = byte pointer into buffer
 *   Note: crc32_tab @ 0x00436170 stores byte-swapped table entries (e.g.
 *         crc32_tab[1] = 0xb71dc104 in LE = bswap_32(0x04c11db7)), allowing a
 *         right-shift loop on little-endian MIPS followed by bswap_32().
 */
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

/*
 * FormTSHeader(): writes a 4-byte MPEG-2 TS header + 1-byte PSI pointer_field.
 *   Params: dat8 (output buffer), pid (13-bit PID), counter (4-bit continuity counter)
 *   Returns: 5 (bytes written)
 *   Callers: encoder_send_data_thread_entry (for ATSC PSIP packets)
 */
int FormTSHeader(uint8_t *dat8,uint16_t pid,uint8_t counter)

{
  *dat8 = 0x47;                                    // TS sync byte
  dat8[1] = (byte)(pid >> 8) & 0x1f | 0x40;        // TEI=0, PUSI=1 (0x40), priority=0, PID[12:8]
  dat8[2] = (uint8_t)pid;                          // PID[7:0]
  dat8[3] = counter & 0xf | 0x10;                  // scrambling=00, adaptation=01 (payload only), CC
  dat8[4] = 0;                                     // PSI pointer_field = 0
  return 5;
}



// ==== FormATSC_MGT @ 00405898 ====

/*
 * FormATSC_MGT(): builds an ATSC A/65 Master Guide Table (MGT, table_id=0xC7)
 *                 section listing the TVCT, EIT-0..N, and ETT-0..M tables.
 *   Params: data (output buffer), vctlen (TVCT section length),
 *           eitlen/eitpid/eitcnt, ettlen/ettpid/ettcnt
 *   Returns: total section length in bytes (including 4-byte CRC32)
 *   Callers: encoder_send_data_thread_entry (ATSC mode, PID 0x1ffb)
 *   Locals:
 *     local_30 = tables_defined count
 *     local_38 = write offset i into data
 *     local_34 = loop index j over EIT / ETT arrays
 *     uVar1    = current eitpid[j] or ettpid[j]
 *     length   = byte length before the 4-byte CRC32
 *     uVar2    = computed CRC32
 */
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
  *data = 199;                                     // table_id = 0xC7 (MGT)
  data[1] = 0xf0;                                  // section_syntax=1, private=1, reserved=11 (length patched later)
  data[2] = 0;
  data[3] = 0;                                     // table_id_extension = 0x0000
  data[4] = 0;
  data[5] = 0xef;                                  // reserved=11, version_number=0x17 (23), current_next=1
  data[6] = 0;                                     // section_number = 0
  data[7] = 0;                                     // last_section_number = 0
  data[8] = 0;                                     // protocol_version = 0
  data[9] = (uint8_t)(local_30 >> 8);              // tables_defined MSB
  data[10] = (uint8_t)local_30;                    // tables_defined LSB
  local_38 = 0xb;
  if (vctlen != 0) {
    data[0xb] = 0;                                 // table_type = 0x0000 (Terrestrial VCT, current_next=1)
    data[0xc] = 0;
    data[0xd] = 0xff;                              // reserved=111, PID=0x1ffb (PSIP base PID)
    data[0xe] = 0xfb;
    data[0xf] = 0xf7;                              // reserved=111, version_number=0x17
    data[0x10] = (uint8_t)(vctlen >> 0x18);        // number_bytes (32-bit BE)
    data[0x11] = (uint8_t)(vctlen >> 0x10);
    data[0x12] = (uint8_t)(vctlen >> 8);
    data[0x13] = (uint8_t)vctlen;
    data[0x14] = 0xf0;                             // reserved=1111, table_type_descriptors_length=0
    data[0x15] = 0;
    local_38 = 0x16;
  }
  if (eitcnt != 0) {
    for (local_34 = 0; local_34 < (int)(uint)eitcnt; local_34 = local_34 + 1) {
      uVar1 = eitpid[local_34];
      data[local_38] = 1;                          // table_type = 0x0100 + j (EIT-j)
      data[local_38 + 1] = (uint8_t)local_34;
      data[local_38 + 2] = (byte)(uVar1 >> 8) & 0x1f | 0xe0; // reserved=111 | PID[12:8]
      data[local_38 + 3] = (uint8_t)uVar1;         // PID[7:0]
      data[local_38 + 4] = 0xf7;                   // reserved=111, version_number=0x17
      data[local_38 + 5] = (uint8_t)(eitlen[local_34] >> 0x18); // number_bytes (32-bit BE)
      data[local_38 + 6] = (uint8_t)(eitlen[local_34] >> 0x10);
      data[local_38 + 7] = (uint8_t)(eitlen[local_34] >> 8);
      data[local_38 + 8] = (uint8_t)eitlen[local_34];
      data[local_38 + 9] = 0xf0;                   // descriptors_length = 0
      data[local_38 + 10] = 0;
      local_38 = local_38 + 0xb;
    }
  }
  if (ettcnt != 0) {
    for (local_34 = 0; local_34 < (int)(uint)ettcnt; local_34 = local_34 + 1) {
      uVar1 = ettpid[local_34];
      data[local_38] = 2;                          // table_type = 0x0200 + j (Event ETT-j)
      data[local_38 + 1] = (uint8_t)local_34;
      data[local_38 + 2] = (byte)(uVar1 >> 8) & 0x1f | 0xe0; // reserved=111 | PID[12:8]
      data[local_38 + 3] = (uint8_t)uVar1;         // PID[7:0]
      data[local_38 + 4] = 0xf7;                   // reserved=111, version_number=0x17
      data[local_38 + 5] = (uint8_t)(ettlen[local_34] >> 0x18); // number_bytes (32-bit BE)
      data[local_38 + 6] = (uint8_t)(ettlen[local_34] >> 0x10);
      data[local_38 + 7] = (uint8_t)(ettlen[local_34] >> 8);
      data[local_38 + 8] = (uint8_t)ettlen[local_34];
      data[local_38 + 9] = 0xf0;                   // descriptors_length = 0
      data[local_38 + 10] = 0;
      local_38 = local_38 + 0xb;
    }
  }
  data[local_38] = 0xf0;                           // trailing MGT descriptors_length = 0
  data[local_38 + 1] = 0;
  length = local_38 + 2;
  data[1] = (byte)((uint)(local_38 + 3) >> 8) | 0xf0; // patch section_length = (length + 4) - 3 = local_38 + 3
  data[2] = (char)length + 1;
  uVar2 = crc32(0xffffffff,data,length);
  uVar2 = bswap_32(uVar2);
  data[length] = (uint8_t)(uVar2 >> 0x18);         // append 4-byte CRC32 (big-endian)
  data[local_38 + 3] = (uint8_t)(uVar2 >> 0x10);
  data[local_38 + 4] = (uint8_t)(uVar2 >> 8);
  data[local_38 + 5] = (uint8_t)uVar2;
  return local_38 + 6;
}



// ==== setMinor @ 004063e4 ====

/*
 * setMinor(): sets the global PSIP TVCT minor channel number (sti_minor).
 *   Params: minor (16-bit minor channel number; default sti_minor = 1)
 *   Returns: minor
 *   Callers: main() in 00_main.c.c on CLI option '-M <minor>'
 */
uint16_t setMinor(uint16_t minor)

{
  sti_minor = minor;
  return minor;
}



// ==== FormATSC_ETT @ 00406420 ====

/*
 * FormATSC_ETT(): builds an ATSC A/65 Extended Text Table (ETT, table_id=0xCC)
 *                 section with a single "eng" Multiple String Structure (MSS).
 *   Params: data (output buffer), addinfo (text), addinfolen, source_id, event_id
 *   Returns: total section length in bytes (including 4-byte CRC32)
 *   Callers: encoder_send_data_thread_entry (PIDs 0x1100..0x1103)
 *   Locals:
 *     iVar2    = event_id << 2 (ETM_id bits [15:2])
 *     bVar1    = ETM_id[1:0] (0 for channel ETM if event_id==0, 2 for event ETM)
 *     local_30 = write offset i into data
 *     local_2c = MSS string loop index l (0..0)
 *     local_28 = MSS segment loop index m (0..0)
 *     local_24 = byte copy loop index n (0..addinfolen-1)
 *     uVar3    = computed CRC32
 *   Quirk: data[10] ORs (iVar2 >> 16) into source_id LSB, which would corrupt
 *          source_id if event_id >= 0x4000 (safe for event_id 10..40 used here).
 */
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
    bVar1 = 0;                                     // channel ETM (ETM_id[1:0] = 0b00)
  }
  else {
    bVar1 = 2;                                     // event ETM (ETM_id[1:0] = 0b10)
  }
  *data = 0xcc;                                    // table_id = 0xCC (ETT)
  data[1] = 0xf0;
  data[2] = 0;
  data[3] = (uint8_t)(event_id >> 8);              // ETT_table_id_extension = event_id
  data[4] = (uint8_t)event_id;
  data[5] = 0xef;                                  // reserved=11, version_number=0x17, current_next=1
  data[6] = 0;                                     // section_number = 0
  data[7] = 0;                                     // last_section_number = 0
  data[8] = 0;                                     // protocol_version = 0
  data[9] = (uint8_t)(source_id >> 8);             // ETM_id[31:16] = source_id
  data[10] = (byte)source_id | (byte)((uint)iVar2 >> 0x10);
  data[0xb] = (uint8_t)((uint)iVar2 >> 8);         // ETM_id[15:2] = event_id, [1:0] = bVar1
  data[0xc] = (byte)iVar2 | bVar1;
  data[0xd] = 1;                                   // MSS: number_strings = 1
  local_30 = 0xe;
  for (local_2c = 0; local_2c < 1; local_2c = local_2c + 1) {
    data[local_30] = 0x65;                         // ISO_639_language_code = "eng"
    data[local_30 + 1] = 0x6e;
    data[local_30 + 2] = 0x67;
    data[local_30 + 3] = 1;                        // number_segments = 1
    local_30 = local_30 + 4;
    for (local_28 = 0; local_28 < 1; local_28 = local_28 + 1) {
      data[local_30] = 0;                          // compression_type = 0 (uncompressed)
      data[local_30 + 1] = 0;                      // mode = 0 (ISO-8859-1 / ASCII)
      data[local_30 + 2] = (uint8_t)addinfolen;    // number_bytes
      local_30 = local_30 + 3;
      for (local_24 = 0; local_24 < addinfolen; local_24 = local_24 + 1) {
        data[local_30] = addinfo[local_24];
        local_30 = local_30 + 1;
      }
    }
  }
  data[1] = (byte)(local_30 + 1 >> 8) | 0xf0;      // patch section_length = (local_30 + 4) - 3
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

/*
 * FormATSC_EIT(): builds an ATSC A/65 Event Information Table (EIT, table_id=0xCB)
 *                 section containing 1 hardcoded 6-hour event for source_id=3.
 *   Params: data (output buffer), title, title_len, timeoffs (sec from UTC midnight),
 *           evt_id (event_id), etm (2-bit ETM_location)
 *   Returns: total section length in bytes (including 4-byte CRC32)
 *   Callers: encoder_send_data_thread_entry (4 EITs on PIDs 0x1000..0x1003)
 *   Locals:
 *     tVar3    = current Unix time
 *     iVar4    = GPS start_time: UTC midnight in GPS seconds + timeoffs
 *                (0x12d53d70 = 315,964,800 = Unix-to-GPS epoch 1970->1980)
 *     local_50 = write offset i into data
 *     uVar5    = title_len_pos (offset of event title_length byte)
 *     uVar2    = computed event descriptors_length
 *     bVar1    = single-iteration loop guard
 *     local_4c/48/44 = MSS loop indices l/m/n
 *     uVar6    = computed CRC32
 *   Quirks:
 *     - Masks uVar5 and local_50 with 0xff when patching lengths (breaks if >= 256).
 *     - Empty dead loop at end of descriptor block.
 *     - Hardcodes source_id=3, duration=0x5460 (21600s = 6h), and Caption Service
 *       Descriptor (0x86, "eng", digital_cc=1).
 */
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
  iVar4 = (tVar3 / 0x15180) * 0x15180 + -0x12d53d70 + timeoffs; // 0x15180=86400s/day; -0x12d53d70=-315964800s (GPS epoch)
  *data = 0xcb;                                    // table_id = 0xCB (EIT)
  data[1] = 0xf0;
  data[2] = 0;
  data[3] = 0;                                     // source_id = 0x0003
  data[4] = 3;
  data[5] = 0xef;                                  // reserved=11, version_number=0x17, current_next=1
  data[6] = 0;                                     // section_number = 0
  data[7] = 0;                                     // last_section_number = 0
  data[8] = 0;                                     // protocol_version = 0
  data[9] = 1;                                     // num_events_in_section = 1
  local_50 = 10;
  bVar1 = false;
  while (!bVar1) {
    data[local_50] = (byte)(evt_id >> 8) | 0xc0;   // reserved=11 | event_id[13:8]
    data[local_50 + 1] = (uint8_t)evt_id;          // event_id[7:0]
    data[local_50 + 2] = (uint8_t)((uint)iVar4 >> 0x18); // start_time (32-bit BE GPS seconds)
    data[local_50 + 3] = (uint8_t)((uint)iVar4 >> 0x10);
    data[local_50 + 4] = (uint8_t)((uint)iVar4 >> 8);
    data[local_50 + 5] = (uint8_t)iVar4;
    data[local_50 + 6] = ((byte)etm & 3) << 4 | 0xc0; // reserved=11, ETM_location=etm, duration[19:16]=0
    data[local_50 + 7] = 0x54;                     // length_in_seconds = 0x05460 (21600s = 6 hours)
    data[local_50 + 8] = 0x60;
    uVar5 = local_50 + 9;                          // title_len_pos
    data[uVar5] = 0;                               // placeholder for title_length
    data[local_50 + 10] = 1;                       // MSS: number_strings = 1
    local_50 = local_50 + 0xb;
    for (local_4c = 0; local_4c < 1; local_4c = local_4c + 1) {
      data[local_50] = 0x65;                       // "eng"
      data[local_50 + 1] = 0x6e;
      data[local_50 + 2] = 0x67;
      data[local_50 + 3] = 1;                      // number_segments = 1
      local_50 = local_50 + 4;
      for (local_48 = 0; local_48 < 1; local_48 = local_48 + 1) {
        data[local_50] = 0;                        // compression_type = 0
        data[local_50 + 1] = 0;                    // mode = 0
        data[local_50 + 2] = (uint8_t)title_len;   // number_bytes
        local_50 = local_50 + 3;
        for (local_44 = 0; local_44 < title_len; local_44 = local_44 + 1) {
          data[local_50] = title[local_44];
          local_50 = local_50 + 1;
        }
      }
    }
    data[uVar5 & 0xff] = ((byte)local_50 - (char)uVar5) - 1; // patch title_length
    data[local_50] = 0;                            // descriptors_length placeholder (2 bytes)
    data[local_50 + 1] = 0;
    data[local_50 + 2] = 0x86;                     // descriptor_tag = 0x86 (ATSC Caption Service Descriptor)
    data[local_50 + 3] = 7;                        // descriptor_length = 7
    data[local_50 + 4] = 0xe1;                     // reserved=111, number_of_services=1
    data[local_50 + 5] = 0x65;                     // language = "eng"
    data[local_50 + 6] = 0x6e;
    data[local_50 + 7] = 0x67;
    data[local_50 + 8] = 0xc1;                     // digital_cc=1, reserved=1, caption_service_number=1
    data[local_50 + 9] = 0x3f;                     // easy_reader=0, wide_aspect_ratio=0, reserved=111111
    data[local_50 + 10] = 0xff;                    // reserved=11111111
    uVar2 = ((short)(local_50 + 0xb) - (ushort)(byte)local_50) - 2; // = 9 bytes
    data[local_50 & 0xff] = (byte)(uVar2 >> 8) & 0xf | 0xf0; // patch descriptors_length
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

/*
 * FormATSC_TVCT(): builds an ATSC A/65 Terrestrial Virtual Channel Table (TVCT,
 *                  table_id=0xC8) section defining 1 virtual channel ("MELD300",
 *                  major=mjchannel, minor=mnchannel).
 *   Params: data (output buffer), mjchannel (from conf.chan_freq), mnchannel
 *           (from sti_minor), extended_name, ext_name_len, etm
 *   Returns: total section length in bytes (including 4-byte CRC32)
 *   Callers: encoder_send_data_thread_entry (ATSC mode, PID 0x1ffb)
 *   Locals:
 *     local_68 = write offset i into data
 *     sVar1    = saved channel-entry base offset (10)
 *     local_64 = channel loop index j (0..0)
 *     local_5c = channel descriptors_length accumulator
 *     iVar2    = desc_len_pos (offset of 10-bit channel descriptors_length)
 *     iVar3    = descriptor length byte offset (for 0xA0 and 0xA1 descriptors)
 *     local_58/54/50 = MSS loop indices l/m/n for Extended Channel Name (0xA0)
 *     local_60 = dead loop index k (0 < 0)
 *     uVar4    = computed CRC32
 *   Note:
 *     - Short name is hardcoded UTF-16BE "MELD300".
 *     - Service Location Descriptor (0xA1) hardcodes PCR_PID=0x0030, MPEG-2 video
 *       (stream_type=0x02, PID=0x0030), and AC-3 audio (stream_type=0x81, PID=0x0031).
 *     - Minor channel mask (& 0xf) at data[+0xf] is 2 bits wider than ATSC 10-bit minor.
 */
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
  *data = 200;                                     // table_id = 0xC8 (TVCT)
  data[1] = 0xf0;
  data[2] = 0;
  data[3] = 0;                                     // transport_stream_id = 0x0000
  data[4] = 0;
  data[5] = 0xef;                                  // reserved=11, version_number=0x17, current_next=1
  data[6] = 0;                                     // section_number = 0
  data[7] = 0;                                     // last_section_number = 0
  data[8] = 0;                                     // protocol_version = 0
  data[9] = 1;                                     // num_channels_in_section = 1
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
    data[local_68] = 0;                            // short_name[0..6] = UTF-16BE "MELD300" (14 bytes)
    data[local_68 + 1] = 0x4d;                     // 'M'
    data[local_68 + 2] = 0;
    data[local_68 + 3] = 0x45;                     // 'E'
    data[local_68 + 4] = 0;
    data[local_68 + 5] = 0x4c;                     // 'L'
    data[local_68 + 6] = 0;
    data[local_68 + 7] = 0x44;                     // 'D'
    data[local_68 + 8] = 0;
    data[local_68 + 9] = 0x33;                     // '3'
    data[local_68 + 10] = 0;
    data[local_68 + 0xb] = 0x30;                   // '0'
    data[local_68 + 0xc] = 0;
    data[local_68 + 0xd] = 0x30;                   // '0'
    data[local_68 + 0xe] = (byte)(mjchannel >> 6) | 0xf0; // reserved=1111, major_channel_number[9:6]
    data[local_68 + 0xf] = (char)mjchannel << 2 | (byte)(mnchannel >> 8) & 0xf; // major[5:0] | minor[9:8]
    data[local_68 + 0x10] = (uint8_t)mnchannel;    // minor_channel_number[7:0]
    data[local_68 + 0x11] = 4;                     // modulation_mode = 4 (8-VSB)
    data[local_68 + 0x12] = 0;                     // carrier_frequency = 0 (4 bytes)
    data[local_68 + 0x13] = 0;
    data[local_68 + 0x14] = 0;
    data[local_68 + 0x15] = 0;
    data[local_68 + 0x16] = 0;                     // channel_TSID = 0x0000
    data[local_68 + 0x17] = 0;
    data[local_68 + 0x18] = 0;                     // program_number = 0x0003
    data[local_68 + 0x19] = 3;
    data[local_68 + 0x1a] = etm << 6 | 9;          // ETM_location=etm, access_ctrl=0, hidden=0, hide_guide=1, reserved=001
    data[local_68 + 0x1b] = 0xc2;                  // reserved=11, service_type=2 (ATSC Digital TV)
    data[local_68 + 0x1c] = 0;                     // source_id = 0x0003
    data[local_68 + 0x1d] = 3;
    iVar2 = local_68 + 0x1e;                       // desc_len_pos
    data[iVar2] = (byte)((ushort)local_5c >> 8) & 3 | 0xfc;
    data[local_68 + 0x1f] = (uint8_t)local_5c;
    data[local_68 + 0x20] = 0xa0;                  // descriptor_tag = 0xA0 (ATSC Extended Channel Name Descriptor)
    iVar3 = local_68 + 0x21;                       // ext_name_pos (descriptor_length)
    data[iVar3] = 0;
    data[local_68 + 0x22] = 1;                     // MSS: number_strings = 1
    local_68 = local_68 + 0x23;
    for (local_58 = 0; local_58 < 1; local_58 = local_58 + 1) {
      data[local_68] = 0x65;                       // "eng"
      data[local_68 + 1] = 0x6e;
      data[local_68 + 2] = 0x67;
      data[local_68 + 3] = 1;                      // number_segments = 1
      local_68 = local_68 + 4;
      for (local_54 = 0; local_54 < 1; local_54 = local_54 + 1) {
        data[local_68] = 0;                        // compression_type = 0
        data[local_68 + 1] = 0;                    // mode = 0
        data[local_68 + 2] = (uint8_t)ext_name_len; // number_bytes
        local_68 = local_68 + 3;
        for (local_50 = 0; local_50 < ext_name_len; local_50 = local_50 + 1) {
          data[local_68] = extended_name[local_50];
          local_68 = local_68 + 1;
        }
      }
    }
    data[iVar3] = ((char)local_68 - (char)iVar3) - 1; // patch 0xA0 descriptor_length
    data[local_68] = 0xa1;                         // descriptor_tag = 0xA1 (ATSC Service Location Descriptor)
    iVar3 = local_68 + 1;                          // serv_loc_pos (descriptor_length)
    data[iVar3] = 0;
    data[local_68 + 2] = 0xe0;                     // reserved=111, PCR_PID = 0x0030
    data[local_68 + 3] = 0x30;
    data[local_68 + 4] = 2;                        // number_elements = 2
    data[local_68 + 5] = 2;                        // element 0: stream_type = 0x02 (MPEG-2 video)
    data[local_68 + 6] = 0xe0;                     // elementary_PID = 0x0030
    data[local_68 + 7] = 0x30;
    data[local_68 + 8] = 0x65;                     // ISO_639_language_code = "eng"
    data[local_68 + 9] = 0x6e;
    data[local_68 + 10] = 0x67;
    data[local_68 + 0xb] = 0x81;                   // element 1: stream_type = 0x81 (ATSC AC-3 audio)
    data[local_68 + 0xc] = 0xe0;                   // elementary_PID = 0x0031
    data[local_68 + 0xd] = 0x31;
    data[local_68 + 0xe] = 0x65;                   // ISO_639_language_code = "eng"
    data[local_68 + 0xf] = 0x6e;
    data[local_68 + 0x10] = 0x67;
    local_68 = local_68 + 0x11;
    data[iVar3] = ((char)local_68 - (char)iVar3) - 1; // patch 0xA1 descriptor_length (15 bytes)
    local_5c = ((short)local_68 - (short)iVar2) + -2; // total channel descriptors_length
    data[iVar2] = (byte)((ushort)local_5c >> 8) & 3 | 0xfc;
    data[sVar1 + 0x1f] = (uint8_t)local_5c;
  }
  data[local_68] = 0xfc;                           // reserved=111111, additional_descriptors_length = 0
  data[local_68 + 1] = 0;
  local_68 = local_68 + 2;
  for (local_60 = 0; local_60 < 0; local_60 = local_60 + 1) { // dead loop
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

/*
 * FormATSC_STT(): builds an ATSC A/65 System Time Table (STT, table_id=0xCD)
 *                 section (fixed 20 bytes = 0x14).
 *   Params: data (output buffer)
 *   Returns: 20 (0x14)
 *   Callers: encoder_send_data_thread_entry (ATSC mode, PID 0x1ffb)
 *   Locals: tVar1 = time(NULL); iVar2 = GPS system_time; uVar3 = CRC32
 *   Quirk: subtracts 0x12d4db1b (315,939,611) instead of 0x12d53d70 (315,964,800)
 *          used in FormATSC_EIT, making STT system_time ~7 hours (25,189s) ahead.
 *          GPS_UTC_offset is hardcoded to 15 (0x0f).
 */
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
  iVar2 = tVar1 + -0x12d4db1b;                     // Unix time -> GPS seconds (offset by ~7h vs 0x12d53d70)
  *data = 0xcd;                                    // table_id = 0xCD (STT)
  data[1] = 0xf0;
  data[2] = 0;
  data[3] = 0;                                     // table_id_extension = 0x0000
  data[4] = 0;
  data[5] = 0xef;                                  // reserved=11, version_number=0x17, current_next=1
  data[6] = 0;                                     // section_number = 0
  data[7] = 0;                                     // last_section_number = 0
  data[8] = 0;                                     // protocol_version = 0
  data[9] = (uint8_t)((uint)iVar2 >> 0x18);        // system_time (32-bit BE GPS seconds)
  data[10] = (uint8_t)((uint)iVar2 >> 0x10);
  data[0xb] = (uint8_t)((uint)iVar2 >> 8);
  data[0xc] = (uint8_t)iVar2;
  data[0xd] = 0xf;                                 // GPS_UTC_offset = 15 seconds
  data[0xe] = 0;                                   // daylight_savings = 0x0000
  data[0xf] = 0;
  data[1] = 0xf0;                                  // section_length = 0x011 (17 bytes after byte 2)
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

/*
 * genNIT(): builds a DVB-T Network Information Table (NIT actual, table_id=0x40)
 *           on PID 0x0010 into a 188-byte TS packet using libdvbpsi.
 *   Params: packet (188-byte output), counter (CC), freq (from conf.chan_freq),
 *           bandwidth (5/6/7/8 MHz), lngt (longitude), lttd (latitude)
 *   Returns: 0 on success, 1 if dvbpsi_new() fails
 *   Callers: encoder_send_data_thread_entry (when conf.modulationmode == 0, DVB-T)
 *   Locals:
 *     local_4c[0..3] = network_name ("MELD")
 *     local_4c[4..8], uStack_43..41, local_40 = cell_list_descriptor (0x6c) buffer
 *     local_3c, uStack_3a..39, local_38, uStack_35, local_34 = terrestrial_delivery_system_descriptor (0x5a)
 *     local_30 = cell_extent_of_longitude (0xff)
 *     local_2c..20 = service_list_descriptor (0x41) buffer (service_id=3, type=1)
 *     dStack_1c = dvbpsi_nit_t
 *     local_68 = DVB-T bandwidth bits (0=8MHz, 1=7MHz, 2=6MHz, 3=5MHz)
 *     iVar2 = hz_freq (freq * 100000)
 *     fVar5 = scaled lat/lon; uVar4 = lttd16, uVar3 = lngt16
 */
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
  
  local_4c[0] = 0x4d;                              // "MELD" (network_name_descriptor 0x40)
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
  local_4c[4] = 0x6c;                              // descriptor 0x6c header (unused by dvbpsi_nit_ts_descriptor_add)
  local_4c[5] = 10;
  local_4c[6] = 0;                                 // cell_id = 0x0000
  local_4c[7] = 0;
  local_40 = 0;
  local_30 = 0xff;
  local_24 = 0;
  local_20 = 0;
  local_2c = 0x3000341;                            // LE: tag=0x41, len=3, service_id=0x0003..
  local_28 = 1;                                    // service_type=1 (digital TV)
  local_68 = '\0';
  iVar2 = freq * 100000;
  if (lngt < 0.0) {
    lngt = lngt + 360.0;                           // wrap negative longitude into [0, 360)
  }
  fVar5 = (lngt / 180.0) * 32768.0;                // EN 300 468 cell_longitude (2^15 / 180)
  if (fVar5 < 2.1474836e+09) {
    uVar3 = (undefined2)(int)fVar5;
  }
  else {
    uVar3 = (undefined2)(int)(fVar5 - 2.1474836e+09);
  }
  if (lttd < 0.0) {
    lttd = lttd + 360.0;
  }
  fVar5 = (lttd / 90.0) * 32768.0;                 // EN 300 468 cell_latitude (2^15 / 90)
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
    local_68 = '\x02';                             // 6 MHz
  }
  else if (bandwidth < 7) {
    if (bandwidth == 5) {
      local_68 = '\x03';                           // 5 MHz
    }
  }
  else if (bandwidth == 7) {
    local_68 = '\x01';                             // 7 MHz
  }
  else if (bandwidth == 8) {
    local_68 = '\0';                               // 8 MHz
  }
  uVar1 = CONCAT12(local_68 << 5,local_38._0_2_);
  _local_38 = CONCAT13(0x40,uVar1 | 0x1f0000);     // bandwidth[7:5] | priority=1, Time_Slicing=1, MPE-FEC=1, res=11
  local_34 = 0xffffff1c;
  p_dvbpsi = dvbpsi_new(message,DVBPSI_MSG_DEBUG);
  if (p_dvbpsi != (dvbpsi_t *)0x0) {
    dvbpsi_nit_init(&dStack_1c,0x40,3,0x2114,0x17,true); // table_id=0x40, network_id=3, version=0x17, current_next=1
    dvbpsi_nit_descriptor_add(&dStack_1c,0x40,4,local_4c); // 0x40: network_name_descriptor "MELD"
    p_ts = dvbpsi_nit_ts_add(&dStack_1c,0,0x2114); // ts_id=0, orig_network_id=0x2114
    dvbpsi_nit_ts_descriptor_add(p_ts,0x5a,0xb,&uStack_3a); // 0x5a: terrestrial_delivery_system_descriptor
    dvbpsi_nit_ts_descriptor_add(p_ts,0x6c,10,local_4c + 6); // 0x6c: cell_list_descriptor
    dvbpsi_nit_ts_descriptor_add(p_ts,0x41,3,(uint8_t *)((int)&local_2c + 2)); // 0x41: service_list_descriptor
    dStack_1c.b_current_next = true;
    p_section = dvbpsi_nit_sections_generate(p_dvbpsi,&dStack_1c,0x40);
    *packet = 0x47;                                // TS header: PID 0x0010 (NIT)
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

/*
 * genSDT(): builds a DVB Service Description Table (SDT actual, table_id=0x42)
 *           on PID 0x0011 into a 188-byte TS packet using libdvbpsi.
 *   Params: packet (188-byte output), counter (CC), company, service_name (fcc),
 *           sn, lttd, lngt
 *   Returns: 0 on success, 1 if dvbpsi_new() fails
 *   Callers: encoder_send_data_thread_entry (when conf.modulationmode == 0, DVB-T)
 *   Locals:
 *     local_d8, local_d7, local_d6 = td48[128] (service_descriptor 0x48 payload)
 *     dStack_58 = dvbpsi_sdt_t
 *     acStack_4c = formatted service_name string "<company>,<fcc>(<sn>,<lttd>,<lngt>)"
 *     p_service = dvbpsi_sdt_service_t* (service_id=3, running_status=4)
 *     sVar1 = strlen(acStack_4c)
 */
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
  local_d8 = 0x48;                                 // tag 0x48 (service_descriptor)
  local_d7 = 0xe;
  local_d6[0] = 1;                                 // service_type = 1 (digital television service)
  local_d6[1] = 4;                                 // service_provider_name_length = 4
  local_d6[2] = 0x4d;                              // "MELD"
  local_d6[3] = 0x45;
  local_d6[4] = 0x4c;
  local_d6[5] = 0x44;
  local_d6[6] = 8;                                 // default service_name_length (overwritten below)
  local_d6[7] = 0x4d;                              // default "MKVMT300" (overwritten below)
  local_d6[8] = 0x4b;
  local_d6[9] = 0x56;
  local_d6[10] = 0x4d;
  local_d6[0xb] = 0x54;
  local_d6[0xc] = 0x33;
  local_d6[0xd] = 0x30;
  local_d6[0xe] = 0x30;
  p_dvbpsi = dvbpsi_new(message,DVBPSI_MSG_DEBUG);
  if (p_dvbpsi != (dvbpsi_t *)0x0) {
    dvbpsi_sdt_init(&dStack_58,0x42,0,0x10,false,0x2114); // table_id=0x42, ts_id=0, ver=0x10, orig_net=0x2114
    p_service = dvbpsi_sdt_service_add(&dStack_58,3,false,false,4,false); // service_id=3, running_status=4
    sprintf(acStack_4c,"%s,%s(%s,%3.6f,%3.6f)",company,service_name,sn,(double)lttd,(double)lngt);
    sVar1 = strlen(acStack_4c);
    local_d6[6] = (char)sVar1;                     // service_name_length
    memcpy(local_d6 + 7,acStack_4c,(size_t)sVar1);
    dvbpsi_sdt_service_descriptor_add(p_service,0x48,local_d6[6] + 7,local_d6);
    dStack_58.b_current_next = true;
    p_section = dvbpsi_sdt_sections_generate(p_dvbpsi,&dStack_58);
    *packet = 0x47;                                // TS header: PID 0x0011 (SDT)
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

/*
 * genTDT(): builds a DVB Time and Date Table (TDT, table_id=0x70) on PID 0x0014
 *           into a 188-byte TS packet using libdvbpsi.
 *   Params: packet (188-byte output), counter (CC)
 *   Returns: 0 on success, 1 if dvbpsi_new() fails
 *   Callers: encoder_send_data_thread_entry (when conf.modulationmode == 0, DVB-T)
 *   Locals:
 *     local_1c  = current time_t; ptVar1 = gmtime(&local_1c)
 *     iVar2     = L (1 for Jan/Feb, else 0) in EN 300 468 Annex C MJD formula
 *     uVar3     = 16-bit Modified Julian Date (MJD)
 *     uVar4     = 24-bit UTC time (HH<<16 | MM<<8 | SS)
 *     dStack_30 = dvbpsi_tot_t (used for TDT with table_id=0x70)
 *   Quirk: packs HH/MM/SS as raw binary bytes rather than BCD as required by EN 300 468.
 */
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
          (int)((double)(ptVar1->tm_mon + 2 + iVar2 * 0xc) * 30.6001); // 0x3a6c = 14956 (MJD formula)
  uVar4 = ptVar1->tm_hour << 0x10 | ptVar1->tm_min << 8 | ptVar1->tm_sec;
  p_dvbpsi = dvbpsi_new(message,DVBPSI_MSG_DEBUG);
  if (p_dvbpsi != (dvbpsi_t *)0x0) {
    dvbpsi_tot_init(&dStack_30,0x70,0,0x10,false,  // table_id=0x70 (TDT), version=0x10
                    CONCAT44((int)uVar4 >> 0x1f | uVar3 >> 8 | ((int)uVar3 >> 0x1f) << 0x18,
                             uVar4 | uVar3 * 0x1000000));
    p_section = dvbpsi_tot_sections_generate(p_dvbpsi,&dStack_30);
    *packet = 0x47;                                // TS header: PID 0x0014 (TDT/TOT)
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

/*
 * genEIT(): builds a DVB Event Information Table (EIT present/following actual,
 *           table_id=0x4E) on PID 0x0012 into a 188-byte TS packet using libdvbpsi.
 *   Params: packet (188-byte output), counter (CC)
 *   Returns: 0 on success, 1 if dvbpsi_new() fails
 *   Callers: encoder_send_data_thread_entry (when conf.modulationmode == 0, DVB-T)
 *   Locals:
 *     dStack_98  = dvbpsi_eit_t
 *     local_88   = time(NULL); ptVar2 = gmtime(&local_88)
 *     uVar3/uVar4 = MJD / UTC start time (same EN 300 468 formula as genTDT)
 *     local_84..68, auStack_82 = td4e[32] (0x4e extended_event_descriptor, "test of EIT")
 *     local_64, local_60 = td54[8] (0x54 content_descriptor, 0x09 0x20)
 *     local_5c, local_5b, local_5a = td4d[64] (0x4d short_event_descriptor, "test event-name" / "experiment")
 */
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
    local_84 = 0x6500114e;                         // LE: tag=0x4e, len=0x11, desc_num/last=0x00, "e.."
    local_80 = 0xb00756e;                          // "nu", items_len=0, text_len=11
    local_7c = 0x74736574;                         // "test"
    local_78 = 0x20666f20;                         // " of "
    local_74 = 0x544945;                           // "EIT\0"
    local_60 = 0;
    local_64 = 0x920254;                           // LE: tag=0x54, len=2, content_nibble=0x09, user=0x20
    memset(&local_5c,0,0x40);
    local_5c = 0x4d;                               // tag 0x4d (short_event_descriptor), len=0x1e
    local_5b = 0x1e;
    local_5a[0] = 0x65;                            // "enu"
    local_5a[1] = 0x6e;
    local_5a[2] = 0x75;
    local_5a[3] = 0xf;                             // event_name_length = 15
    local_5a[4] = 0x74;                            // "test event-name"
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
    local_5a[0x13] = 10;                           // text_length = 10
    local_5a[0x14] = 0x65;                         // "experiment"
    local_5a[0x15] = 0x78;
    local_5a[0x16] = 0x70;
    local_5a[0x17] = 0x65;
    local_5a[0x18] = 0x72;
    local_5a[0x19] = 0x69;
    local_5a[0x1a] = 0x6d;
    local_5a[0x1b] = 0x65;
    local_5a[0x1c] = 0x6e;
    local_5a[0x1d] = 0x74;
    dvbpsi_eit_init(&dStack_98,0x4e,3,0x10,false,0,0x2114,0,0); // table_id=0x4e, service_id=3, ver=0x10
    p_event = dvbpsi_eit_event_add
                        (&dStack_98,0,
                         CONCAT44((int)uVar4 >> 0x1f | uVar3 >> 8 | ((int)uVar3 >> 0x1f) << 0x18,
                                  uVar4 | uVar3 * 0x1000000),0x5dc,0,false,0x32);
    dvbpsi_eit_event_descriptor_add(p_event,0x4e,0x11,auStack_82);
    dvbpsi_eit_event_descriptor_add(p_event,0x54,2,(uint8_t *)((int)&local_64 + 2));
    dvbpsi_eit_event_descriptor_add(p_event,0x4d,0x1e,local_5a);
    p_section = dvbpsi_eit_sections_generate(p_dvbpsi,&dStack_98,0x4e);
    *packet = 0x47;                                // TS header: PID 0x0012 (EIT)
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

/*
 * encoder_recv_video_thread_entry(): pthread entry for receiving encoded MPEG-TS
 *                                    DMA buffers from /dev/vixs/xcodedrv and
 *                                    forwarding them to UDP (-m UDP) or capture
 *                                    file (-m FILE / -f / -F). Not started in
 *                                    DVO RF-modulator mode (-m DVO).
 *   Params: arg (util_thread_arg_t* -> {pThread, pEnCoder})
 *   Returns: NULL
 *   Callers: launched by launch_encoder_recv_thead()
 *   Callees: util_thread_notify_ready, vixs_encoder_recv_stream, util_sendto, write
 *   Locals:
 *     pEnCoder = (vixs_encoder_t*)((util_thread_arg_t*)arg)->arg
 *     mThread  = ((util_thread_arg_t*)arg)->pThread
 *     local_50 = txLen (bytes sent over UDP on this iteration)
 *     local_34 = bufSize (bytes received from driver)
 *     vStack_30 = vixs_xcode_recv_strm_info_t (driver recv metadata/status)
 *     uVar2    = curBufIdx (ioctl_sdev_recv.dmabuffer_idx)
 *     rVar1    = return_code_t scratch
 *     piVar3, param3 = errno / strerror() on UDP send failure
 */
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
      if (((uint)vStack_30.result & 0x1000) != 0) { // bit 12 = EOS / last frame flag
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
        pEnCoder->stopRecv = 1;                    // fatal driver error codes: stop immediately
      }
      else {
        pEnCoder->recvTimeoutCnt = pEnCoder->recvTimeoutCnt - 1;
        if (pEnCoder->recvTimeoutCnt == 0) {
          pEnCoder->stopRecv = 1;                  // exhausted retry/timeout budget (initially 10)
        }
      }
    }
  }
  printf("%s:Exit recv thread Entry \n","encoder_recv_video_thread_entry");
  return (void *)0x0;
}



// ==== terminate_encoder_recv_thread @ 00409d30 ====

/*
 * terminate_encoder_recv_thread(): signals and joins recvVideoThread.
 *   Params: pEnCoder
 *   Returns: RET_OK or RET_ERR
 *   Callers: xcode4_start_encoder (error rollback), xcode4_stop_encoder
 */
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

/*
 * launch_encoder_recv_thead(): spawns recvVideoThread (note original "thead" typo).
 *   Params: pEnCoder
 *   Returns: RET_OK or RET_ERR
 *   Callers: xcode4_start_encoder (when outputMode is UDP or FILE)
 */
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

/*
 * readCfgFile(): parses /etc/config/queryInfo key=value lines into fileconf *conf.
 *   Params: pth (path, "/etc/config/queryInfo"), conf (output fileconf struct)
 *   Returns: 0 on success, -1 if fopen64() fails (defaults still populated)
 *   Callers: encoder_send_data_thread_entry
 *   Defaults: modulationmode=1 (ATSC), bandwidth=0, chan_freq=27 (0x1b),
 *             txpower=0x35, dacgain=0xab, fwversion=" v0.034 Tx v2",
 *             fcc="OKVMT300", sn="00000467".
 *   Locals:
 *     __stream = FILE*; local_14 = getline() buffer (not freed); local_10[0] = linesize
 *     local_1c = result (0 or -1); sVar2 = string length; lVar3 = strtol() result
 *   Quirk: DVB-T bandwidth check tests local_10[0] (allocated getline capacity)
 *          rather than _Var4 (line length) before reading local_14[0x19].
 */
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
  conf->modulationmode = 1;                        // default: ATSC
  conf->bandwidth = 0;
  conf->chan_freq = 0x1b;                          // default channel: 27
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
              conf->bandwidth = local_14[0x19] + -0x30; // e.g. "DVB-T 8MHz" -> 8
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

/*
 * readSbiFile(): parses /etc/config/sbi (UCI config) for GPS coordinates.
 *   Params: pth ("/etc/config/sbi"), lngt (out longitude), lttd (out latitude)
 *   Returns: 0 on success, -1 on open failure (outputs default to 0.0)
 *   Callers: encoder_send_data_thread_entry
 *   Locals: __stream = FILE*; local_10 = getline() buffer; local_c = linesize;
 *           local_18 = result (0 or -1)
 */
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

/*
 * readCompanyFile(): reads the first line of /etc/config/company (max 32 chars).
 *   Params: pth ("/etc/config/company"), company_name (out buffer >= 32 bytes)
 *   Returns: 32 (0x20) on success, -1 on open failure (writes "Meldtech")
 *   Callers: encoder_send_data_thread_entry
 *   Locals: __stream = FILE*; local_10 = getline() buffer; local_c = linesize; __n = copy len
 */
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
      __n = _Var2 - 1;                             // strip trailing newline
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

/*
 * encoder_send_data_thread_entry(): pthread entry that periodically builds and
 *                                   injects ATSC PSIP (11 TS packets, 2068 B) or
 *                                   DVB-T SI (4 TS packets, 752 B) tables into
 *                                   the XCode4 TS muxer via hXcodeSendData.
 *   Params: arg (util_thread_arg_t* -> {pThread, pEnCoder})
 *   Returns: NULL
 *   Callers: launched by launch_encoder_send_data_thead() when -g is set
 *   MT300 behaviour (PSIP virtual-channel latch):
 *     /etc/config/queryInfo, /etc/config/sbi, and /etc/config/company are read
 *     ONCE at thread startup before the while loop. Consequently, conf.chan_freq
 *     (used as the PSIP TVCT major_channel_number) latches the channel present
 *     when av_streamer starts and never updates if whitespaceagent later changes
 *     the RF channel.
 *   Locals:
 *     pEnCoder   = (vixs_encoder_t*)((util_thread_arg_t*)arg)->arg
 *     mThread    = ((util_thread_arg_t*)arg)->pThread
 *     local_128  = psicounter (CC for 0x1ffb / DVB SI)
 *     local_127  = ettcounter (CC for ETT PIDs 0x1100..0x1103)
 *     local_126  = eitcounter (CC for EIT PIDs 0x1000..0x1003)
 *     local_124  = kk (0xff TS-packet stuffing loop index)
 *     local_e8   = bufSize (pEnCoder->sendDataSize)
 *     fStack_e4  = fileconf parsed from /etc/config/queryInfo
 *     local_a0   = lngt (longitude); local_9c = lttd (latitude)
 *     acStack_98 = company[32]
 *     local_78   = eitpid[4] = {0x1000, 0x1001, 0x1002, 0x1003}
 *     local_70..64 = eitlen[4]
 *     local_60   = ettpid[4] = {0x1100, 0x1101, 0x1102, 0x1103}
 *     local_58..4c = ettlen[4]
 *     acStack_48 = extended_name[64] ("<company>,<fcc>(<sn>,<lngt>, <lttd>)")
 *     ext_name_len = strlen(acStack_48)
 *     dat8       = (uint8_t*)pEnCoder->pSendDataBuf
 *     vctlen     = TVCT section length
 */
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
  readCfgFile("/etc/config/queryInfo",&fStack_e4); // latched once at thread start!
  readSbiFile("/etc/config/sbi",&local_a0,&local_9c);
  readCompanyFile("/etc/config/company",acStack_98);
  sprintf(acStack_48,"%s,%s(%s,%3.6f, %3.6f)",acStack_98,fStack_e4.fcc,fStack_e4.sn,(double)local_a0
          ,(double)local_9c);
  ext_name_len = strlen(acStack_48);
  printf("%s:Enter Encoder %x send data thread Entry \n","encoder_send_data_thread_entry",
         pEnCoder->idx);
  while (mThread->toExit == 0) {
    local_e8 = pEnCoder->sendDataSize;
    if (fStack_e4.modulationmode == 0) {           // DVB-T SI: 4 TS packets (NIT, SDT, TDT, EIT)
      genNIT((uint8_t *)pEnCoder->pSendDataBuf,local_128,fStack_e4.chan_freq,fStack_e4.bandwidth,
             local_a0,local_9c);
      genSDT((uint8_t *)(pEnCoder->pSendDataBuf + 0xbc),local_128,acStack_98,fStack_e4.fcc,
             fStack_e4.sn,local_a0,local_9c);
      genTDT((uint8_t *)(pEnCoder->pSendDataBuf + 0x178),local_128);
      genEIT((uint8_t *)(pEnCoder->pSendDataBuf + 0x234),local_128);
    }
    else if (fStack_e4.modulationmode == 1) {      // ATSC PSIP: 11 TS packets (TVCT, 4xEIT, 4xETT, MGT, STT)
      dat8 = (uint8_t *)pEnCoder->pSendDataBuf;
      FormTSHeader(dat8,0x1ffb,local_128);         // Packet 0 (+0x000): TVCT on PID 0x1ffb
      vctlen = FormATSC_TVCT((uint8_t *)(pEnCoder->pSendDataBuf + 5),(uint16_t)fStack_e4.chan_freq,
                             sti_minor,acStack_48,ext_name_len,0);
      for (local_124 = vctlen + 5; local_124 < 0xbc; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0xbc,local_78[0],local_126); // Packet 1 (+0x0bc): EIT-0 on PID 0x1000
      local_70 = FormATSC_EIT((uint8_t *)(pEnCoder->pSendDataBuf + 0xc1),acStack_48,ext_name_len,0,
                              10,1);
      for (local_124 = local_70 + 0xc1; local_124 < 0x178; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x178,local_78[1],local_126); // Packet 2 (+0x178): EIT-1 on PID 0x1001 (+6h)
      local_6c = FormATSC_EIT((uint8_t *)(pEnCoder->pSendDataBuf + 0x17d),acStack_48,ext_name_len,
                              0x5460,0x14,1);
      for (local_124 = local_6c + 0x17d; local_124 < 0x234; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x234,local_78[2],local_126); // Packet 3 (+0x234): EIT-2 on PID 0x1002 (+12h)
      local_68 = FormATSC_EIT((uint8_t *)(pEnCoder->pSendDataBuf + 0x239),acStack_48,ext_name_len,
                              0xa8c0,0x1e,1);
      for (local_124 = local_68 + 0x239; local_124 < 0x2f0; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x2f0,local_78[3],local_126); // Packet 4 (+0x2f0): EIT-3 on PID 0x1003 (+18h)
      local_64 = FormATSC_EIT((uint8_t *)(pEnCoder->pSendDataBuf + 0x2f5),acStack_48,ext_name_len,
                              0xfd20,0x28,1);
      for (local_124 = local_64 + 0x2f5; local_124 < 0x3ac; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      local_126 = local_126 + 1;
      FormTSHeader(dat8 + 0x3ac,local_60[0],local_127); // Packet 5 (+0x3ac): ETT-0 on PID 0x1100
      local_58 = FormATSC_ETT((uint8_t *)(pEnCoder->pSendDataBuf + 0x3b1),acStack_48,ext_name_len,3,
                              10);
      for (local_124 = local_58 + 0x3b1; local_124 < 0x468; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x468,local_60[1],local_127); // Packet 6 (+0x468): ETT-1 on PID 0x1101
      local_54 = FormATSC_ETT((uint8_t *)(pEnCoder->pSendDataBuf + 0x46d),acStack_48,ext_name_len,3,
                              0x14);
      for (local_124 = local_54 + 0x46d; local_124 < 0x524; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x524,local_60[2],local_127); // Packet 7 (+0x524): ETT-2 on PID 0x1102
      local_50 = FormATSC_ETT((uint8_t *)(pEnCoder->pSendDataBuf + 0x529),acStack_48,ext_name_len,3,
                              0x1e);
      for (local_124 = local_50 + 0x529; local_124 < 0x5e0; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x5e0,local_60[3],local_127); // Packet 8 (+0x5e0): ETT-3 on PID 0x1103
      local_4c = FormATSC_ETT((uint8_t *)(pEnCoder->pSendDataBuf + 0x5e5),acStack_48,ext_name_len,3,
                              0x28);
      for (local_124 = local_4c + 0x5e5; local_124 < 0x69c; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      local_127 = local_127 + 1;
      FormTSHeader(dat8 + 0x69c,0x1ffb,local_128 + 1); // Packet 9 (+0x69c): MGT on PID 0x1ffb
      iVar1 = FormATSC_MGT((uint8_t *)(pEnCoder->pSendDataBuf + 0x6a1),vctlen,&local_70,local_78,4,
                           &local_58,local_60,4);
      for (local_124 = iVar1 + 0x6a1; local_124 < 0x758; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      local_128 = local_128 + 2;
      FormTSHeader(dat8 + 0x758,0x1ffb,local_128); // Packet 10 (+0x758): STT on PID 0x1ffb
      iVar1 = FormATSC_STT((uint8_t *)(pEnCoder->pSendDataBuf + 0x75d));
      for (local_124 = iVar1 + 0x75d; local_124 < 0x814; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
    }
    local_128 = local_128 + 1;
    rVar2 = vixs_encoder_send_stream(pEnCoder,(int *)&local_e8,0x8000000); // IOCTLSDEVSEND (flags=0x8000000)
    if (rVar2 != RET_OK) {
      printf("%s:Xcoder %x Send data to AV driver error\n","encoder_send_data_thread_entry",
             pEnCoder->idx);
      mThread->toExit = 1;
    }
    if (pEnCoder->sendDataInterval == 0) {
      mThread->toExit = 1;
    }
    else {
      usleep(pEnCoder->sendDataInterval * 1000);   // -t <ms> (e.g. 1000 ms in avs_start.sh)
    }
  }
  printf("%s:Exit recv thread Entry \n","encoder_send_data_thread_entry");
  local_e8 = 0;
  vixs_encoder_send_stream(pEnCoder,(int *)&local_e8,0x8400000); // final 0-byte EOS/flush send
  return (void *)0x0;
}



// ==== terminate_encoder_send_data_thread @ 0040b57c ====

/*
 * terminate_encoder_send_data_thread(): signals and joins sendDataThread.
 *   Params: pEnCoder
 *   Returns: RET_OK or RET_ERR
 *   Callers: xcode4_start_encoder (error rollback), xcode4_stop_encoder
 */
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

/*
 * launch_encoder_send_data_thead(): spawns sendDataThread (note original "thead" typo).
 *   Params: pEnCoder
 *   Returns: RET_OK or RET_ERR
 *   Callers: xcode4_start_encoder (when pEnCoder->bSendData != 0)
 */
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

/*
 * xcode4_open_encoder(): allocates a vixs_encoder_t (0x1980 = 6528 bytes) and
 *                        initializes it via vixs_encoder_init().
 *   Params: handle (out int* receiving vixs_encoder_t*), param (init params)
 *   Returns: RET_OK or RET_ERR
 *   Callers: start_encoder() in 00_main.c.c
 */
return_code_t xcode4_open_encoder(int *handle,vixs_encoder_init_param_t *param)

{
  vixs_encoder_t *pEnCoder;
  return_code_t rVar1;
  
                    /* Unresolved local var: return_code_t rc@[???]
                       Unresolved local var: vixs_encoder_t * pEnCoder@[???] */
  pEnCoder = malloc(0x1980);                       // sizeof(vixs_encoder_t) = 6528
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

/*
 * xcode4_start_encoder(): opens the XCode4 encoder stream and launches the
 *                         recv (for UDP/FILE) and send-data (for -g) threads.
 *   Params: handle (vixs_encoder_t*)
 *   Returns: RET_OK or RET_ERR
 *   Callers: start_encoder() in 00_main.c.c
 *   Locals: local_10 = rc; handle+0x3c = outputMode; handle+0xd4 = bSendData
 */
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
      if ((*(int *)(handle + 0x3c) == 1) || (*(int *)(handle + 0x3c) == 3)) { // outputMode: UDP(1) or FILE(3)
        local_10 = launch_encoder_recv_thead((vixs_encoder_t *)handle);
        if (local_10 != RET_OK) {
          printf("%s:Launch Encode Thread failed\n","xcode4_start_encoder");
          vixs_encoder_close_stream((vixs_encoder_t *)handle);
        }
      }
      if (*(int *)(handle + 0xd4) != 0) {          // bSendData != 0 (-g table generation)
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

/*
 * xcode4_stop_encoder(): stops recvVideoThread (or flushes DVO stream), stops
 *                        sendDataThread if active, and closes the XCode4 stream.
 *   Params: handle (vixs_encoder_t*)
 *   Returns: RET_OK or RET_ERR (Bug: uninitialized local_10 if outputMode==DVO and bSendData==0)
 *   Callers: sample_event_callback() and main() in 00_main.c.c
 */
return_code_t xcode4_stop_encoder(int handle)

{
  return_code_t local_10;
  
                    /* Unresolved local var: return_code_t rc@[???]
                       Unresolved local var: vixs_encoder_t * pEnCoder@[???] */
  printf("%s:E\n","xcode4_stop_encoder");
  if ((*(int *)(handle + 0x3c) == 1) || (*(int *)(handle + 0x3c) == 3)) { // outputMode: UDP(1) or FILE(3)
    local_10 = terminate_encoder_recv_thread((vixs_encoder_t *)handle);
    if (local_10 != RET_OK) {
      printf("%s:Fail to terminate encode_thread, return value %x\n","xcode4_stop_encoder",local_10)
      ;
    }
  }
  else {
    vixs_encoder_flush_stream((vixs_encoder_t *)handle); // DVO(2): flush instead of recv thread
  }
  if (*(int *)(handle + 0xd4) != 0) {              // bSendData != 0
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

/*
 * xcode4_close_encoder(): frees the vixs_encoder_t handle.
 *   Params: handle (vixs_encoder_t*)
 *   Returns: RET_OK
 *   Callers: main() in 00_main.c.c
 *   Note: does not invoke vixs_encoder_deinit() (defined in 02_vixs_encoder.c.c).
 */
return_code_t xcode4_close_encoder(int handle)

{
                    /* Unresolved local var: vixs_encoder_t * pEnCoder@[???] */
  free((void *)handle);
  return RET_OK;
}



// ==== xcode4_update_encoder_param @ 0040bc58 ====

/*
 * xcode4_update_encoder_param(): updates encoder input/output video standard or
 *                                bitrate via vixs_encoder_update_stream().
 *   Params: handle (vixs_encoder_t*), param (vixs_encoder_update_param_t*)
 *   Returns: RET_OK or RET_ERR
 *   Callers: start_encoder() in 00_main.c.c (on restart != 0)
 */
return_code_t xcode4_update_encoder_param(int handle,vixs_encoder_update_param_t *param)

{
  return_code_t rVar1;
  
                    /* Unresolved local var: vixs_encoder_t * pEnCoder@[???] */
  rVar1 = vixs_encoder_update_stream((vixs_encoder_t *)handle,param);
  return rVar1;
}



// ==== xcode4_get_encoder_ts_info @ 0040bcb8 ====

/*
 * xcode4_get_encoder_ts_info(): queries TS lock and output status from the
 *                               XCode4 driver and unpacks the status bitmask at
 *                               handle+0x1324 (ioctl_ts_ctrl.ioctl_param.tsinput_lock).
 *   Params: handle (vixs_encoder_t*), info (out vixs_encoder_ts_info_t*)
 *   Returns: RET_OK or RET_ERR
 *   Callers: main() in 00_main.c.c (periodic status poll)
 */
return_code_t xcode4_get_encoder_ts_info(int handle,vixs_encoder_ts_info_t *info)

{
  return_code_t rVar1;
  
                    /* Unresolved local var: vixs_encoder_t * pEnCoder@[???]
                       Unresolved local var: return_code_t rc@[???] */
  rVar1 = vixs_encoder_get_ts_info((vixs_encoder_t *)handle);
  if (rVar1 == RET_OK) {
    info->inputPsiLock = *(uint *)(handle + 0x1324) & 1;           // bit 0: PSI lock
    info->inputVideoLock = (uint)((*(uint *)(handle + 0x1324) & 2) != 0); // bit 1: video lock
    info->inputAudioLock = (uint)((*(uint *)(handle + 0x1324) & 4) != 0); // bit 2: audio lock
    info->inputPCRLock = (uint)((*(uint *)(handle + 0x1324) & 8) != 0);   // bit 3: PCR lock
    info->inputVideoCCErr = (uint)((*(uint *)(handle + 0x1324) & 0x10) != 0); // bit 4: video CC error
    info->inputAudioCCErr = (uint)((*(uint *)(handle + 0x1324) & 0x20) != 0); // bit 5: audio CC error
    info->tsOutput = (uint)((*(uint *)(handle + 0x1324) & 0x40) == 0);    // bit 6 (inverted): TS output active
    info->videoOutput = (uint)((*(uint *)(handle + 0x1324) & 0x80) == 0); // bit 7 (inverted): video output active
    info->audioOutput = (uint)((*(uint *)(handle + 0x1324) & 0x100) == 0); // bit 8 (inverted): audio output active
  }
  return rVar1;
}


