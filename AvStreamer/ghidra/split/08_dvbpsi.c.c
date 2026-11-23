// ==== dvbpsi_new @ 00419a00 ====

dvbpsi_t * dvbpsi_new(dvbpsi_message_cb callback,dvbpsi_msg_level level)

{
  dvbpsi_t *pdVar1;
  
                    /* Unresolved local var: dvbpsi_t * p_dvbpsi@[DW_OP_reg2(v0)] */
  pdVar1 = calloc(1,0x10);
  if (pdVar1 != (dvbpsi_t *)0x0) {
    pdVar1->p_decoder = (dvbpsi_decoder_t *)0x0;
    pdVar1->pf_message = callback;
    pdVar1->i_msg_level = level;
  }
  return pdVar1;
}



// ==== dvbpsi_delete @ 00419a5c ====

void dvbpsi_delete(dvbpsi_t *p_dvbpsi)

{
  if (p_dvbpsi->p_decoder != (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder == ((void *)0)","dvbpsi.c",0x44,"dvbpsi_delete");
  }
  p_dvbpsi->pf_message = (dvbpsi_message_cb)0x0;
  free(p_dvbpsi);
  return;
}



// ==== dvbpsi_decoder_new @ 00419ab0 ====

void * dvbpsi_decoder_new(dvbpsi_callback_gather_t pf_gather,int i_section_max_size,
                         _Bool b_discontinuity,size_t psi_size)

{
  undefined2 *puVar1;
  undefined2 *puVar2;
  
                    /* Unresolved local var: dvbpsi_decoder_t * p_decoder@[???] */
  if (psi_size < 0x1c) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("psi_size >= sizeof(dvbpsi_decoder_t)","dvbpsi.c",0x51,"dvbpsi_decoder_new");
  }
  puVar1 = calloc(1,psi_size);
  puVar2 = (undefined2 *)0x0;
  if (puVar1 != (undefined2 *)0x0) {
    *(dvbpsi_callback_gather_t *)(puVar1 + 8) = pf_gather;
    *puVar1 = 0x7370;
    *(undefined1 *)(puVar1 + 1) = 0x69;
    *(undefined1 *)(puVar1 + 3) = 0xff;
    *(undefined4 *)(puVar1 + 4) = 0;
    *(int *)(puVar1 + 10) = i_section_max_size;
    *(_Bool *)(puVar1 + 2) = b_discontinuity;
    *(undefined1 *)((int)puVar1 + 5) = 0;
    *(undefined1 *)((int)puVar1 + 7) = 0;
    *(undefined4 *)(puVar1 + 6) = 0;
    *(undefined1 *)((int)puVar1 + 3) = 0;
    puVar2 = puVar1;
  }
  return puVar2;
}



// ==== dvbpsi_decoder_reset @ 00419b80 ====

void dvbpsi_decoder_reset(dvbpsi_decoder_t *p_decoder,_Bool b_force)

{
  if (p_decoder == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_decoder","dvbpsi.c",0x6c,"dvbpsi_decoder_reset");
  }
  if (b_force) {
    p_decoder->b_current_valid = false;
  }
  dvbpsi_DeletePSISections(p_decoder->p_sections);
  p_decoder->p_sections = (dvbpsi_psi_section_t *)0x0;
  return;
}



// ==== dvbpsi_decoder_psi_sections_completed @ 00419bf0 ====

_Bool dvbpsi_decoder_psi_sections_completed(dvbpsi_decoder_t *p_decoder)

{
  _Bool _Var1;
  dvbpsi_psi_section_t *pdVar2;
  uint uVar3;
  uint uVar4;
  
                    /* Unresolved local var: _Bool b_complete@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p@[???]
                       Unresolved local var: uint32_t prev_nr@[???] */
  if (p_decoder == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_decoder","dvbpsi.c",0x7c,"dvbpsi_decoder_psi_sections_completed");
  }
  pdVar2 = p_decoder->p_sections;
  _Var1 = false;
  if ((pdVar2 != (dvbpsi_psi_section_t *)0x0) && (pdVar2->i_number == 0)) {
    uVar3 = 0;
    do {
      pdVar2 = pdVar2->p_next;
      if (p_decoder->i_last_section_number == uVar3) {
        _Var1 = true;
      }
      if (pdVar2 == (dvbpsi_psi_section_s *)0x0) {
        return _Var1;
      }
      uVar4 = uVar3 + 1;
      if (uVar4 == 0x100) {
                    /* WARNING: Subroutine does not return */
        __assert_fail(s_prev_nr_<_256_00424318,"dvbpsi.c",0x84,
                      "dvbpsi_decoder_psi_sections_completed");
      }
      uVar3 = (uint)pdVar2->i_number;
    } while (uVar3 == uVar4);
  }
  return _Var1;
}



// ==== dvbpsi_decoder_psi_section_add @ 00419cac ====

_Bool dvbpsi_decoder_psi_section_add(dvbpsi_decoder_t *p_decoder,dvbpsi_psi_section_t *p_section)

{
  byte bVar1;
  _Bool _Var2;
  dvbpsi_psi_section_t *p_section_00;
  dvbpsi_psi_section_t *p_section_01;
  
                    /* Unresolved local var: dvbpsi_psi_section_t * p@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_prev@[???]
                       Unresolved local var: _Bool b_overwrite@[???] */
  if (p_decoder == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_decoder","dvbpsi.c",0x95,"dvbpsi_decoder_psi_section_add");
  }
  if (p_section == (dvbpsi_psi_section_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_section","dvbpsi.c",0x96,"dvbpsi_decoder_psi_section_add");
  }
  if (p_section->p_next != (dvbpsi_psi_section_s *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_section->p_next == ((void *)0)","dvbpsi.c",0x97,
                  "dvbpsi_decoder_psi_section_add");
  }
  p_section_01 = p_decoder->p_sections;
  if (p_section_01 == (dvbpsi_psi_section_t *)0x0) {
    p_decoder->p_sections = p_section;
    p_section->p_next = (dvbpsi_psi_section_s *)0x0;
    return false;
  }
  bVar1 = p_section->i_number;
  if (p_section_01->i_number == bVar1) {
    p_section->p_next = p_section_01->p_next;
    p_section_01->p_next = (dvbpsi_psi_section_s *)0x0;
    dvbpsi_DeletePSISections(p_section_01);
    _Var2 = true;
    p_decoder->p_sections = p_section;
  }
  else if (bVar1 < p_section_01->i_number) {
    p_section->p_next = p_section_01;
    _Var2 = false;
    p_decoder->p_sections = p_section;
  }
  else {
    while (p_section_00 = p_section_01->p_next, p_section_00 != (dvbpsi_psi_section_t *)0x0) {
      if (p_section_00->i_number == bVar1) {
        p_section_01->p_next = p_section;
        p_section->p_next = p_section_00->p_next;
        p_section_00->p_next = (dvbpsi_psi_section_s *)0x0;
        dvbpsi_DeletePSISections(p_section_00);
        return true;
      }
      if (bVar1 < p_section_00->i_number) {
        p_section_01->p_next = p_section;
        p_section->p_next = p_section_00;
        return false;
      }
      p_section_01 = p_section_00;
    }
    _Var2 = false;
    if (p_section_01->i_number < bVar1) {
      p_section_01->p_next = p_section;
      p_section->p_next = (dvbpsi_psi_section_s *)0x0;
    }
  }
  return _Var2;
}



// ==== dvbpsi_decoder_delete @ 00419e54 ====

void dvbpsi_decoder_delete(dvbpsi_decoder_t *p_decoder)

{
  if (p_decoder == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_decoder","dvbpsi.c",0xe2,"dvbpsi_decoder_delete");
  }
  if (p_decoder->p_sections != (dvbpsi_psi_section_t *)0x0) {
    dvbpsi_DeletePSISections(p_decoder->p_sections);
    p_decoder->p_sections = (dvbpsi_psi_section_t *)0x0;
  }
  dvbpsi_DeletePSISections(p_decoder->p_current_section);
  free(p_decoder);
  return;
}



// ==== dvbpsi_decoder_present @ 00419edc ====

_Bool dvbpsi_decoder_present(dvbpsi_t *p_dvbpsi)

{
  bool bVar1;
  
  bVar1 = false;
  if (p_dvbpsi != (dvbpsi_t *)0x0) {
    bVar1 = p_dvbpsi->p_decoder != (dvbpsi_decoder_t *)0x0;
  }
  return bVar1;
}



// ==== dvbpsi_error @ 00419ef4 ====

void dvbpsi_error(dvbpsi_t *dvbpsi,char *src,char *fmt,...)

{
  int iVar1;
  int iVar2;
  undefined4 in_a3;
  undefined4 local_resc;
  char *local_1c;
  char *local_18 [2];
  
  if (DVBPSI_MSG_NONE < dvbpsi->i_msg_level) {
                    /* Unresolved local var: va_list ap@[???]
                       Unresolved local var: char * tmp@[???]
                       Unresolved local var: int err@[???]
                       Unresolved local var: char * msg@[???] */
    local_1c = (char *)0x0;
    local_resc = in_a3;
    iVar1 = vasprintf(&local_1c,fmt,&local_resc);
    if (-1 < iVar1) {
      local_18[0] = (char *)0x0;
      iVar2 = asprintf(local_18,"libdvbpsi (%s): %s",src,local_1c);
      if (iVar2 < 0) {
        free(local_1c);
      }
      else {
        free(local_1c);
        if ((iVar1 != 0) && (dvbpsi->pf_message != (dvbpsi_message_cb)0x0)) {
          (*dvbpsi->pf_message)(dvbpsi,DVBPSI_MSG_ERROR,local_18[0]);
        }
        free(local_18[0]);
      }
    }
  }
  return;
}



// ==== dvbpsi_packet_push @ 00419fe4 ====

_Bool dvbpsi_packet_push(dvbpsi_t *p_dvbpsi,uint8_t *p_data)

{
  uint8_t uVar1;
  bool bVar2;
  _Bool _Var3;
  uint uVar4;
  int iVar5;
  uint8_t *puVar6;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  uint8_t *puVar7;
  char cVar8;
  uint uVar9;
  dvbpsi_psi_section_t *p_section;
  dvbpsi_decoder_t *pdVar10;
  uint8_t *__n;
  byte *__src;
  byte *pbVar11;
  byte *pbVar12;
  
                    /* Unresolved local var: uint8_t i_expected_counter@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_section@[???]
                       Unresolved local var: uint8_t * p_payload_pos@[???]
                       Unresolved local var: uint8_t * p_new_pos@[???]
                       Unresolved local var: int i_available@[???]
                       Unresolved local var: dvbpsi_decoder_t * p_decoder@[???]
                       Unresolved local var: _Bool b_first@[???] */
  pdVar10 = p_dvbpsi->p_decoder;
  if (pdVar10 == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_decoder","dvbpsi.c",0x10a,"dvbpsi_packet_push");
  }
  if (*p_data != 0x47) {
    dvbpsi_error(p_dvbpsi,"PSI decoder","not a TS packet");
    return false;
  }
  if (pdVar10->i_continuity_counter == 0xff) {
    pdVar10->i_continuity_counter = p_data[3] & 0xf;
  }
  else {
    uVar4 = pdVar10->i_continuity_counter + 1 & 0xf;
    uVar9 = p_data[3] & 0xf;
    pdVar10->i_continuity_counter = (uint8_t)uVar9;
    if ((uVar4 == (uVar9 + 1 & 0xf)) && (pdVar10->b_discontinuity == false)) {
      dvbpsi_error(p_dvbpsi,"PSI decoder","TS duplicate (received %d, expected %d) for PID %d",uVar9
                   ,uVar4,(p_data[1] & 0x1f) << 8 | (uint)p_data[2]);
      return false;
    }
    if (uVar4 != uVar9) {
      dvbpsi_error(p_dvbpsi,"PSI decoder","TS discontinuity (received %d, expected %d) for PID %d",
                   uVar9,uVar4,(p_data[1] & 0x1f) << 8 | (uint)p_data[2]);
      pdVar10->b_discontinuity = true;
      if (pdVar10->p_current_section != (dvbpsi_psi_section_t *)0x0) {
        dvbpsi_DeletePSISections(pdVar10->p_current_section);
        pdVar10->p_current_section = (dvbpsi_psi_section_t *)0x0;
      }
    }
  }
  if ((p_data[3] & 0x10) == 0) {
    return false;
  }
  __src = p_data + 4;
  if ((p_data[3] & 0x20) != 0) {
    __src = p_data + p_data[4] + 5;
  }
  pbVar11 = (byte *)0x0;
  if ((p_data[1] & 0x40) != 0) {
    pbVar11 = __src + *__src + 1;
    __src = __src + 1;
  }
  p_section = pdVar10->p_current_section;
  pbVar12 = pbVar11;
  if (p_section == (dvbpsi_psi_section_t *)0x0) {
    if (pbVar11 == (byte *)0x0) {
      return false;
    }
    p_section = dvbpsi_NewPSISection(pdVar10->i_section_max_size);
    pdVar10->p_current_section = p_section;
    if (p_section == (dvbpsi_psi_section_t *)0x0) {
      return false;
    }
    pdVar10->i_need = 3;
    pdVar10->b_complete_header = false;
    pbVar12 = (byte *)0x0;
    __src = pbVar11;
  }
  __n = p_data + (0xbc - (int)__src);
  if ((int)__n < 1) {
    return true;
  }
  do {
    while( true ) {
      if ((int)__n < pdVar10->i_need) {
        memcpy(p_section->p_payload_end,__src,(size_t)__n);
        iVar5 = pdVar10->i_need;
        p_section->p_payload_end = p_section->p_payload_end + (int)__n;
        pdVar10->i_need = iVar5 - (int)__n;
        return true;
      }
      memcpy(p_section->p_payload_end,__src,pdVar10->i_need);
      iVar5 = pdVar10->i_need;
      _Var3 = pdVar10->b_complete_header;
      puVar7 = p_section->p_payload_end + iVar5;
      __src = __src + iVar5;
      p_section->p_payload_end = puVar7;
      __n = __n + -iVar5;
      if (_Var3 == false) break;
      uVar1 = p_section->i_table_id;
      bVar2 = false;
      if (((2 < (byte)(uVar1 + 0x90)) && (uVar1 != 0x7e)) &&
         (bVar2 = true, p_section->b_syntax_indicator == false)) {
        bVar2 = uVar1 == 0x73;
      }
      puVar6 = p_section->p_data;
      _Var3 = (_Bool)-((char)puVar6[1] >> 7);
      p_section->b_syntax_indicator = _Var3;
      p_section->b_private_indicator = SUB41((puVar6[1] & 0x7f) >> 6,0);
      if (_Var3 == false) {
        cVar8 = '\0';
        if (!bVar2) {
LAB_0041a39c:
          p_section->i_table_id = *puVar6;
          if (cVar8 != '\0') goto LAB_0041a3a8;
LAB_0041a424:
          p_section->i_extension = 0;
          p_section->i_version = 0;
          p_section->b_current_next = true;
          p_section->i_number = 0;
          p_section->i_last_number = 0;
          p_section->p_payload_start = puVar6 + 3;
          goto LAB_0041a3ec;
        }
        p_section->p_payload_end = puVar7 + -4;
LAB_0041a314:
        _Var3 = dvbpsi_ValidPSISection(p_section);
        if (CONCAT31(extraout_var,_Var3) != 0) {
          puVar6 = p_section->p_data;
          cVar8 = p_section->b_syntax_indicator;
          goto LAB_0041a39c;
        }
        _Var3 = dvbpsi_ValidPSISection(p_section);
        if (CONCAT31(extraout_var_00,_Var3) == 0) {
          dvbpsi_error(p_dvbpsi,"misc PSI","Bad CRC_32 table 0x%x !!!",(uint)*p_section->p_data);
        }
        else {
          dvbpsi_error(p_dvbpsi,"misc PSI","table 0x%x",(uint)*p_section->p_data);
        }
        dvbpsi_DeletePSISections(p_section);
      }
      else {
        p_section->p_payload_end = puVar7 + -4;
        if (bVar2) goto LAB_0041a314;
        p_section->i_table_id = *puVar6;
        if (_Var3 == false) goto LAB_0041a424;
LAB_0041a3a8:
        p_section->i_extension = CONCAT11(puVar6[3],puVar6[4]);
        p_section->i_version = (uint8_t)((puVar6[5] & 0x3f) >> 1);
        p_section->b_current_next = (_Bool)(puVar6[5] & 1);
        p_section->i_number = puVar6[6];
        uVar1 = puVar6[7];
        p_section->p_payload_start = puVar6 + 8;
        p_section->i_last_number = uVar1;
LAB_0041a3ec:
        if (pdVar10->pf_gather != (dvbpsi_callback_gather_t)0x0) {
          (*pdVar10->pf_gather)(p_dvbpsi,p_section);
        }
      }
      pdVar10->p_current_section = (dvbpsi_psi_section_t *)0x0;
      if (pbVar12 == (byte *)0x0) {
        if (__n == (uint8_t *)0x0) {
          return true;
        }
        pbVar12 = __src;
        if (*__src == 0xff) {
          return true;
        }
      }
LAB_0041a218:
      __src = pbVar12;
      p_section = dvbpsi_NewPSISection(pdVar10->i_section_max_size);
      pdVar10->p_current_section = p_section;
      if (p_section == (dvbpsi_psi_section_t *)0x0) {
        return false;
      }
      pdVar10->i_need = 3;
      __n = p_data + (0xbc - (int)__src);
      pdVar10->b_complete_header = false;
      pbVar12 = (byte *)0x0;
      if ((int)__n < 1) {
        return true;
      }
    }
    puVar7 = p_section->p_data;
    pdVar10->b_complete_header = true;
    iVar5 = pdVar10->i_section_max_size;
    uVar4 = (puVar7[1] & 0xf) << 8 | (uint)puVar7[2];
    p_section->i_length = (uint16_t)uVar4;
    pdVar10->i_need = uVar4;
    if (iVar5 + -2 <= (int)uVar4) {
      dvbpsi_error(p_dvbpsi,"PSI decoder","PSI section too long");
      dvbpsi_DeletePSISections(p_section);
      pdVar10->p_current_section = (dvbpsi_psi_section_t *)0x0;
      if (pbVar12 == (byte *)0x0) {
        return true;
      }
      goto LAB_0041a218;
    }
    if ((int)__n < 1) {
      return true;
    }
  } while( true );
}



// ==== dvbpsi_warning @ 0041a588 ====

void dvbpsi_warning(dvbpsi_t *dvbpsi,char *src,char *fmt,...)

{
  int iVar1;
  int iVar2;
  undefined4 in_a3;
  undefined4 local_resc;
  char *local_1c;
  char *local_18 [2];
  
  if (DVBPSI_MSG_ERROR < dvbpsi->i_msg_level) {
                    /* Unresolved local var: va_list ap@[???]
                       Unresolved local var: char * tmp@[???]
                       Unresolved local var: int err@[???]
                       Unresolved local var: char * msg@[???] */
    local_1c = (char *)0x0;
    local_resc = in_a3;
    iVar1 = vasprintf(&local_1c,fmt,&local_resc);
    if (-1 < iVar1) {
      local_18[0] = (char *)0x0;
      iVar2 = asprintf(local_18,"libdvbpsi (%s): %s",src,local_1c);
      if (iVar2 < 0) {
        free(local_1c);
      }
      else {
        free(local_1c);
        if ((iVar1 != 0) && (dvbpsi->pf_message != (dvbpsi_message_cb)0x0)) {
          (*dvbpsi->pf_message)(dvbpsi,DVBPSI_MSG_WARN,local_18[0]);
        }
        free(local_18[0]);
      }
    }
  }
  return;
}



// ==== dvbpsi_debug @ 0041a678 ====

void dvbpsi_debug(dvbpsi_t *dvbpsi,char *src,char *fmt,...)

{
  int iVar1;
  int iVar2;
  undefined4 in_a3;
  undefined4 local_resc;
  char *local_1c;
  char *local_18 [2];
  
  if (DVBPSI_MSG_WARN < dvbpsi->i_msg_level) {
                    /* Unresolved local var: va_list ap@[???]
                       Unresolved local var: char * tmp@[???]
                       Unresolved local var: int err@[???]
                       Unresolved local var: char * msg@[???] */
    local_1c = (char *)0x0;
    local_resc = in_a3;
    iVar1 = vasprintf(&local_1c,fmt,&local_resc);
    if (-1 < iVar1) {
      local_18[0] = (char *)0x0;
      iVar2 = asprintf(local_18,"libdvbpsi (%s): %s",src,local_1c);
      if (iVar2 < 0) {
        free(local_1c);
      }
      else {
        free(local_1c);
        if ((iVar1 != 0) && (dvbpsi->pf_message != (dvbpsi_message_cb)0x0)) {
          (*dvbpsi->pf_message)(dvbpsi,DVBPSI_MSG_DEBUG,local_18[0]);
        }
        free(local_18[0]);
      }
    }
  }
  return;
}



