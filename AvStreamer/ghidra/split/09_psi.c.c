// ==== dvbpsi_NewPSISection @ 0041a770 ====

dvbpsi_psi_section_t * dvbpsi_NewPSISection(int i_max_size)

{
  dvbpsi_psi_section_t *__ptr;
  uint8_t *puVar1;
  
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



// ==== dvbpsi_DeletePSISections @ 0041a800 ====

void dvbpsi_DeletePSISections(dvbpsi_psi_section_t *p_section)

{
  dvbpsi_psi_section_s *pdVar1;
  
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



// ==== dvbpsi_CheckPSISection @ 0041a86c ====

_Bool dvbpsi_CheckPSISection
                (dvbpsi_t *p_dvbpsi,dvbpsi_psi_section_t *p_section,uint8_t table_id,
                char *psz_table_name)

{
  uint uVar1;
  
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,(char *)((int)&DAT_004244f9 + 3),0xc4,
                  "dvbpsi_CheckPSISection");
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
  if ((p_section->b_syntax_indicator == false) && (uVar1 != 0x73)) {
    dvbpsi_error(p_dvbpsi,psz_table_name,"invalid section (section_syntax_indicator == 0)");
    return false;
  }
  dvbpsi_debug(p_dvbpsi,psz_table_name,
               "Table version %2d, i_extension %5d, section %3d up to %3d, current %1d",
               (uint)p_section->i_version,(uint)p_section->i_extension,(uint)p_section->i_number,
               (uint)p_section->i_last_number,(uint)p_section->b_current_next);
  return true;
}



// ==== dvbpsi_ValidPSISection @ 0041a98c ====

_Bool dvbpsi_ValidPSISection(dvbpsi_psi_section_t *p_section)

{
  byte bVar1;
  bool bVar2;
  uint uVar3;
  byte *pbVar4;
  
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



// ==== dvbpsi_CalculateCRC32 @ 0041a9e8 ====

void dvbpsi_CalculateCRC32(dvbpsi_psi_section_t *p_section)

{
  byte *pbVar1;
  byte bVar2;
  uint uVar3;
  byte *pbVar4;
  
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
  *pbVar4 = bVar2;
  p_section->p_payload_end[1] = (uint8_t)*(undefined2 *)((int)&p_section->i_crc + 2);
  p_section->p_payload_end[2] = (uint8_t)(p_section->i_crc >> 8);
  p_section->p_payload_end[3] = (uint8_t)p_section->i_crc;
  return;
}



// ==== dvbpsi_BuildPSISection @ 0041aa74 ====

void dvbpsi_BuildPSISection(dvbpsi_t *p_dvbpsi,dvbpsi_psi_section_t *p_section)

{
  uint8_t uVar1;
  byte bVar2;
  _Bool _Var3;
  undefined3 extraout_var;
  byte bVar4;
  
  bVar4 = 0x80;
  bVar2 = 0x40;
  *p_section->p_data = p_section->i_table_id;
  if (p_section->b_syntax_indicator == false) {
    bVar4 = 0;
  }
  if (p_section->b_private_indicator == false) {
    bVar2 = 0;
  }
  p_section->p_data[1] = bVar2 | bVar4 | (byte)(p_section->i_length >> 8) & 0xf | 0x30;
  p_section->p_data[2] = (uint8_t)p_section->i_length;
  if (p_section->b_syntax_indicator != false) {
    p_section->p_data[3] = (uint8_t)(p_section->i_extension >> 8);
    p_section->p_data[4] = (uint8_t)p_section->i_extension;
    p_section->p_data[5] = p_section->b_current_next | 0xc0U | (p_section->i_version & 0x1f) << 1;
    p_section->p_data[6] = p_section->i_number;
    p_section->p_data[7] = p_section->i_last_number;
  }
  uVar1 = p_section->i_table_id;
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



