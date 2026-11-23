/*
 * 09_psi.c.c  --  annotated decompilation of libdvbpsi-1.1.0/src/psi.c
 * ======================================================================
 *
 * Original source : /var/lib/tftpboot/Meld_AV_Script/2p08b/avstreamer/src/libdvbpsi-1.1.0/src/psi.c
 *                   (DWARF CU @ 0x41a770..0x41aba4, compiled with GNU C 4.5.3, MIPS32-LE)
 * Library / API   : VideoLAN libdvbpsi 1.1.0 (LGPLv2.1+), statically linked.
 *                   Matches the libdvbpsi 1.x API.
 *
 * Role in av_streamer:
 *   Allocates, frees, validates, CRC-32s, and serializes raw PSI/SI section
 *   buffers (dvbpsi_psi_section_t). Called by table generators (NIT, SDT,
 *   EIT, TOT/TDT) via dvbpsi_NewPSISection, dvbpsi_BuildPSISection, and
 *   dvbpsi_DeletePSISections (in 01_libxcode4encoder.c.c and tables/*.c).
 *
 * Main data structures (from DWARF):
 *   dvbpsi_psi_section_t (struct dvbpsi_psi_section_s, 32 bytes = 0x20):
 *     +0x00  uint8_t  i_table_id
 *     +0x01  _Bool    b_syntax_indicator
 *     +0x02  _Bool    b_private_indicator
 *     +0x04  uint16_t i_length
 *     +0x06  uint16_t i_extension        (TS ID / service ID / etc.)
 *     +0x08  uint8_t  i_version          (5-bit version_number)
 *     +0x09  _Bool    b_current_next
 *     +0x0a  uint8_t  i_number           (section_number)
 *     +0x0b  uint8_t  i_last_number      (last_section_number)
 *     +0x0c  uint8_t *p_data             (raw section buffer)
 *     +0x10  uint8_t *p_payload_start    (start of table-specific payload)
 *     +0x14  uint8_t *p_payload_end      (end of payload, before 4-byte CRC32)
 *     +0x18  uint32_t i_crc              (computed MPEG-2 CRC-32)
 *     +0x1c  dvbpsi_psi_section_s *p_next (linked list next pointer)
 *
 * Globals referenced: dvbpsi_crc32_table (rodata, 256-entry MPEG-2 CRC-32 LUT).
 * Upstream fidelity: unmodified libdvbpsi 1.1.0 psi.c.
 */

/* dvbpsi_NewPSISection @ 0x0041a770
 * Purpose  : Allocate a dvbpsi_psi_section_t (0x20 bytes) and its backing
 *            raw data buffer of i_max_size bytes.
 * Params   : i_max_size - maximum section buffer size in bytes (e.g. 1024/4096)
 * Returns  : Newly allocated section with p_payload_end == p_data, or NULL on OOM.
 * Callers  : dvbpsi_packet_push (08_dvbpsi.c.c), table *_sections_generate()
 * Callees  : calloc, free
 */
// ==== dvbpsi_NewPSISection @ 0041a770 ====

dvbpsi_psi_section_t * dvbpsi_NewPSISection(int i_max_size)

{
  dvbpsi_psi_section_t *__ptr; // p_section (0x20 bytes)
  uint8_t *puVar1;             // allocated p_data buffer of size i_max_size
  
                    /* Unresolved local var: dvbpsi_psi_section_t * p_section@[???] */
  __ptr = calloc(1,0x20);
  if (__ptr != (dvbpsi_psi_section_t *)0x0) {
    puVar1 = calloc(1,i_max_size);
    __ptr->p_data = puVar1;
    if (puVar1 == (uint8_t *)0x0) {
      free(__ptr);
      __ptr = (dvbpsi_psi_section_t *)0x0;
    }
    else {
      __ptr->p_payload_end = puVar1;
      __ptr->p_next = (dvbpsi_psi_section_s *)0x0;
    }
  }
  return __ptr;
}



/* dvbpsi_DeletePSISections @ 0x0041a800
 * Purpose  : Walk a linked list of dvbpsi_psi_section_t and free each
 *            section's p_data buffer and struct.
 * Params   : p_section - head of section list (may be NULL)
 * Returns  : void
 * Callers  : genNIT, genSDT, genTDT, genEIT (01_libxcode4encoder.c.c),
 *            dvbpsi_decoder_reset, dvbpsi_decoder_delete, etc.
 * Callees  : free
 */
// ==== dvbpsi_DeletePSISections @ 0041a800 ====

void dvbpsi_DeletePSISections(dvbpsi_psi_section_t *p_section)

{
  dvbpsi_psi_section_s *pdVar1; // p_next
  
  while (p_section != (dvbpsi_psi_section_t *)0x0) {
                    /* Unresolved local var: dvbpsi_psi_section_t * p_next@[???] */
    pdVar1 = p_section->p_next;
    if (p_section->p_data != (uint8_t *)0x0) {
      free(p_section->p_data);
    }
    free(p_section);
    p_section = pdVar1;
  }
  return;
}



/* dvbpsi_CheckPSISection @ 0x0041a86c
 * Purpose  : Validate that a gathered PSI section has the expected table_id
 *            and (except for TOT 0x73) has section_syntax_indicator == 1,
 *            then log section metadata at debug level.
 * Params   : p_dvbpsi       - handle (asserted non-NULL; DAT_004244f0="p_dvbpsi")
 *            p_section      - section to check (asserted non-NULL)
 *            table_id       - expected MPEG/DVB table_id
 *            psz_table_name - table name string for error/debug messages
 * Returns  : true if valid, false on table_id or syntax_indicator mismatch.
 * Callers  : table *_sections_gather() (SDT, EIT, NIT)
 * Callees  : __assert_fail, dvbpsi_error, dvbpsi_debug
 */
// ==== dvbpsi_CheckPSISection @ 0041a86c ====

_Bool dvbpsi_CheckPSISection
                (dvbpsi_t *p_dvbpsi,dvbpsi_psi_section_t *p_section,uint8_t table_id,
                char *psz_table_name)

{
  uint uVar1;                // p_section->i_table_id
  
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,(char *)((int)&DAT_004244f9 + 3),0xc4,
                  "dvbpsi_CheckPSISection"); // "p_dvbpsi", "psi.c" (inferred)
  }
  if (p_section == (dvbpsi_psi_section_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_section",(char *)((int)&DAT_004244f9 + 3),0xc5,"dvbpsi_CheckPSISection");
  }
  uVar1 = (uint)p_section->i_table_id;
  if (uVar1 != table_id) {
    dvbpsi_error(p_dvbpsi,psz_table_name,"invalid section (table_id == 0x%02x expected 0x%02x)",
                 uVar1,(uint)table_id);
    return false;
  }
  if ((p_section->b_syntax_indicator == false) && (uVar1 != 0x73)) { // 0x73 == DVB TOT
    dvbpsi_error(p_dvbpsi,psz_table_name,"invalid section (section_syntax_indicator == 0)");
    return false;
  }
  dvbpsi_debug(p_dvbpsi,psz_table_name,
               "Table version %2d, i_extension %5d, section %3d up to %3d, current %1d",
               (uint)p_section->i_version,(uint)p_section->i_extension,(uint)p_section->i_number,
               (uint)p_section->i_last_number,(uint)p_section->b_current_next);
  return true;
}



/* dvbpsi_ValidPSISection @ 0x0041a98c
 * Purpose  : Verify the MPEG-2 CRC-32 over [p_data, p_payload_end + 4).
 *            Uses standard MPEG-2 CRC-32 (poly 0x04C11DB7, MSB-first, init
 *            0xFFFFFFFF, no final XOR), so a valid section + CRC sums to 0.
 * Params   : p_section - section whose p_data..p_payload_end+4 holds section+CRC
 * Returns  : true if CRC-32 residue is 0, false otherwise.
 * Callers  : dvbpsi_packet_push (08_dvbpsi.c.c), dvbpsi_BuildPSISection
 */
// ==== dvbpsi_ValidPSISection @ 0041a98c ====

_Bool dvbpsi_ValidPSISection(dvbpsi_psi_section_t *p_section)

{
  byte bVar1;                // current byte
  bool bVar2;                // true if residue == 0
  uint uVar3;                // i_crc running accumulator (init 0xffffffff)
  byte *pbVar4;              // p_byte cursor
  
                    /* Unresolved local var: uint32_t i_crc@[???]
                       Unresolved local var: uint8_t * p_byte@[???] */
  pbVar4 = p_section->p_data;
  bVar2 = false;
  if (pbVar4 < p_section->p_payload_end + 4) {
    uVar3 = 0xffffffff;
    do {
      bVar1 = *pbVar4;
      pbVar4 = pbVar4 + 1;
      uVar3 = dvbpsi_crc32_table[uVar3 >> 0x18 ^ (uint)bVar1] ^ uVar3 << 8;
    } while (pbVar4 < p_section->p_payload_end + 4);
    bVar2 = uVar3 == 0;
  }
  return bVar2;
}



/* dvbpsi_CalculateCRC32 @ 0x0041a9e8
 * Purpose  : Compute the MPEG-2 CRC-32 over [p_data, p_payload_end), store it
 *            in p_section->i_crc, and write it big-endian to p_payload_end[0..3].
 * Params   : p_section - section with populated header/payload up to p_payload_end
 * Returns  : void
 * Callers  : dvbpsi_BuildPSISection
 */
// ==== dvbpsi_CalculateCRC32 @ 0041a9e8 ====

void dvbpsi_CalculateCRC32(dvbpsi_psi_section_t *p_section)

{
  byte *pbVar1;              // p_byte cursor
  byte bVar2;                // current byte, then MSB of final CRC
  uint uVar3;                // running CRC-32 accumulator
  byte *pbVar4;              // p_section->p_payload_end (4-byte CRC destination)
  
                    /* Unresolved local var: uint8_t * p_byte@[???] */
  pbVar1 = p_section->p_data;
  pbVar4 = p_section->p_payload_end;
  p_section->i_crc = 0xffffffff;
  bVar2 = 0xff;
  if (pbVar1 < pbVar4) {
    uVar3 = 0xffffffff;
    do {
      bVar2 = *pbVar1;
      pbVar1 = pbVar1 + 1;
      uVar3 = dvbpsi_crc32_table[uVar3 >> 0x18 ^ (uint)bVar2] ^ uVar3 << 8;
      p_section->i_crc = uVar3;
    } while (pbVar1 < pbVar4);
    bVar2 = (byte)(uVar3 >> 0x18);
  }
  // Write CRC-32 big-endian at p_payload_end[0..3] (note MIPS32-LE +2 offset = bits 23..16)
  *pbVar4 = bVar2;
  p_section->p_payload_end[1] = (uint8_t)*(undefined2 *)((int)&p_section->i_crc + 2);
  p_section->p_payload_end[2] = (uint8_t)(p_section->i_crc >> 8);
  p_section->p_payload_end[3] = (uint8_t)p_section->i_crc;
  return;
}



/* dvbpsi_BuildPSISection @ 0x0041aa74
 * Purpose  : Serialize the PSI section header fields (table_id, syntax/private
 *            indicators, reserved bits 0x30, 12-bit section_length, and for
 *            long sections the 5-byte extension/version/current_next/number/
 *            last_number) into p_section->p_data, then compute and verify the
 *            trailing CRC-32 when required by dvbpsi_has_CRC32().
 * Params   : p_dvbpsi  - handle for error reporting
 *            p_section - section to serialize
 * Returns  : void
 * Callers  : table *_sections_generate() (NIT, SDT, EIT, TOT/TDT)
 * Callees  : dvbpsi_CalculateCRC32, dvbpsi_ValidPSISection, dvbpsi_error
 */
// ==== dvbpsi_BuildPSISection @ 0041aa74 ====

void dvbpsi_BuildPSISection(dvbpsi_t *p_dvbpsi,dvbpsi_psi_section_t *p_section)

{
  uint8_t uVar1;             // p_section->i_table_id
  byte bVar2;                // private_indicator bit (0x40 or 0)
  _Bool _Var3;               // result of dvbpsi_ValidPSISection()
  undefined3 extraout_var;   // MIPS ABI upper 24 bits of _Bool return
  byte bVar4;                // section_syntax_indicator bit (0x80 or 0)
  
  bVar4 = 0x80;
  bVar2 = 0x40;
  *p_section->p_data = p_section->i_table_id;
  if (p_section->b_syntax_indicator == false) {
    bVar4 = 0;
  }
  if (p_section->b_private_indicator == false) {
    bVar2 = 0;
  }
  // Byte 1: syntax(1) | private(1) | reserved('11' = 0x30) | section_length[11:8]
  p_section->p_data[1] = bVar2 | bVar4 | (byte)(p_section->i_length >> 8) & 0xf | 0x30;
  p_section->p_data[2] = (uint8_t)p_section->i_length;
  if (p_section->b_syntax_indicator != false) {
    // Bytes 3..7: extended PSI header
    p_section->p_data[3] = (uint8_t)(p_section->i_extension >> 8);
    p_section->p_data[4] = (uint8_t)p_section->i_extension;
    p_section->p_data[5] = p_section->b_current_next | 0xc0U | (p_section->i_version & 0x1f) << 1;
    p_section->p_data[6] = p_section->i_number;
    p_section->p_data[7] = p_section->i_last_number;
  }
  uVar1 = p_section->i_table_id;
  // dvbpsi_has_CRC32(): skip CRC for TDT(0x70), RST(0x71), ST(0x72), DIT(0x7e);
  // include CRC for long sections or TOT(0x73).
  if (((2 < (byte)(uVar1 + 0x90)) && (uVar1 != 0x7e)) &&
     ((p_section->b_syntax_indicator != false || (uVar1 == 0x73)))) {
    dvbpsi_CalculateCRC32(p_section);
    _Var3 = dvbpsi_ValidPSISection(p_section);
    if (CONCAT31(extraout_var,_Var3) == 0) {
      dvbpsi_error(p_dvbpsi,"misc PSI","********************************************");
      dvbpsi_error(p_dvbpsi,"misc PSI","* Generated PSI section has a bad CRC_32.  *");
      dvbpsi_error(p_dvbpsi,"misc PSI","* THIS IS A BUG, PLEASE REPORT TO THE LIST *");
      dvbpsi_error(p_dvbpsi,"misc PSI","*  ---  libdvbpsi-devel@videolan.org  ---  *");
      dvbpsi_error(p_dvbpsi,"misc PSI","********************************************");
      return;
    }
  }
  return;
}


