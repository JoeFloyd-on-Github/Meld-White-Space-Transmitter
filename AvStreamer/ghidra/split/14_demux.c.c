// ==== dvbpsi_Demux @ 0041eb90 ====

void dvbpsi_Demux(dvbpsi_t *p_dvbpsi,dvbpsi_psi_section_t *p_section)

{
  uint *puVar1;
  uint uVar2;
  dvbpsi_decoder_t *pdVar3;
  code *UNRECOVERED_JUMPTABLE;
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"demux.c",0x68,"dvbpsi_Demux");
  }
  pdVar3 = p_dvbpsi->p_decoder;
  if (pdVar3 == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","demux.c",0x69,"dvbpsi_Demux");
  }
                    /* Unresolved local var: uint32_t i_id@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  puVar1 = *(uint **)(pdVar3 + 1);
  if (puVar1 == (uint *)0x0) {
LAB_0041ec0c:
    (**(code **)&pdVar3[1].b_discontinuity)
              (p_dvbpsi,p_section->i_table_id,p_section->i_extension,pdVar3[1].p_current_section);
                    /* Unresolved local var: uint32_t i_id@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
    puVar1 = *(uint **)(pdVar3 + 1);
    if (puVar1 == (uint *)0x0) {
LAB_0041ec64:
      dvbpsi_DeletePSISections(p_section);
      return;
    }
    uVar2 = (uint)CONCAT12(p_section->i_table_id,p_section->i_extension);
    if (uVar2 == *puVar1) {
      UNRECOVERED_JUMPTABLE = (code *)puVar1[1];
      goto LAB_0041ec88;
    }
    do {
      puVar1 = (uint *)puVar1[4];
      if (puVar1 == (uint *)0x0) goto LAB_0041ec64;
    } while (uVar2 != *puVar1);
  }
  else {
    uVar2 = (uint)CONCAT12(p_section->i_table_id,p_section->i_extension);
    if (uVar2 == *puVar1) {
      UNRECOVERED_JUMPTABLE = (code *)puVar1[1];
      goto LAB_0041ec88;
    }
    do {
      puVar1 = (uint *)puVar1[4];
      if (puVar1 == (uint *)0x0) goto LAB_0041ec0c;
    } while (uVar2 != *puVar1);
  }
  UNRECOVERED_JUMPTABLE = (code *)puVar1[1];
LAB_0041ec88:
                    /* WARNING: Could not recover jumptable at 0x0041eca4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*UNRECOVERED_JUMPTABLE)(p_dvbpsi,puVar1[2],p_section);
  return;
}



// ==== dvbpsi_AttachDemux @ 0041ecf0 ====

_Bool dvbpsi_AttachDemux(dvbpsi_t *p_dvbpsi,dvbpsi_demux_new_cb_t pf_new_cb,void *p_new_cb_data)

{
  dvbpsi_decoder_t *pdVar1;
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"demux.c",0x38,"dvbpsi_AttachDemux");
  }
  if (p_dvbpsi->p_decoder != (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder == ((void *)0)","demux.c",0x39,"dvbpsi_AttachDemux");
  }
  pdVar1 = dvbpsi_decoder_new(dvbpsi_Demux,0x1000,true,0x28);
  if (pdVar1 != (dvbpsi_decoder_t *)0x0) {
    pdVar1[1].i_magic[0] = 0;
    pdVar1[1].i_magic[1] = 0;
    pdVar1[1].i_magic[2] = 0;
    pdVar1[1].b_complete_header = false;
    *(dvbpsi_demux_new_cb_t *)&pdVar1[1].b_discontinuity = pf_new_cb;
    pdVar1[1].p_current_section = p_new_cb_data;
    p_dvbpsi->p_decoder = pdVar1;
  }
  return pdVar1 != (dvbpsi_decoder_t *)0x0;
}



// ==== dvbpsi_demuxGetSubDec @ 0041edc4 ====

dvbpsi_demux_subdec_t *
dvbpsi_demuxGetSubDec(dvbpsi_demux_t *p_demux,uint8_t i_table_id,uint16_t i_extension)

{
  dvbpsi_demux_subdec_t *pdVar1;
  uint32_t uVar2;
  
                    /* Unresolved local var: uint32_t i_id@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  pdVar1 = p_demux->p_first_subdec;
  if (pdVar1 != (dvbpsi_demux_subdec_t *)0x0) {
    uVar2 = pdVar1->i_id;
    while ((uVar2 != CONCAT12(i_table_id,i_extension) &&
           (pdVar1 = pdVar1->p_next, pdVar1 != (dvbpsi_demux_subdec_s *)0x0))) {
      uVar2 = pdVar1->i_id;
    }
  }
  return pdVar1;
}



// ==== dvbpsi_DetachDemux @ 0041ee10 ====

void dvbpsi_DetachDemux(dvbpsi_t *p_dvbpsi)

{
  dvbpsi_decoder_t *p_decoder;
  uint *__ptr;
  uint *puVar1;
  code *pcVar2;
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"demux.c",0x87,"dvbpsi_DetachDemux");
  }
  p_decoder = p_dvbpsi->p_decoder;
  if (p_decoder == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","demux.c",0x88,"dvbpsi_DetachDemux");
  }
  __ptr = *(uint **)(p_decoder + 1);
  if (__ptr != (uint *)0x0) {
                    /* Unresolved local var: dvbpsi_demux_subdec_t * p_subdec_temp@[???] */
    pcVar2 = (code *)__ptr[3];
    puVar1 = (uint *)__ptr[4];
    if (pcVar2 == (code *)0x0) goto LAB_0041ee84;
    while ((*pcVar2)(p_dvbpsi,(*__ptr & 0xffffff) >> 0x10,*__ptr & 0xffff), __ptr = puVar1,
          puVar1 != (uint *)0x0) {
      while( true ) {
        pcVar2 = (code *)__ptr[3];
        puVar1 = (uint *)__ptr[4];
        if (pcVar2 != (code *)0x0) break;
LAB_0041ee84:
        free(__ptr);
        __ptr = puVar1;
        if (puVar1 == (uint *)0x0) goto LAB_0041ee98;
      }
    }
LAB_0041ee98:
    p_decoder = p_dvbpsi->p_decoder;
  }
  dvbpsi_decoder_delete(p_decoder);
  p_dvbpsi->p_decoder = (dvbpsi_decoder_t *)0x0;
  return;
}



// ==== dvbpsi_NewDemuxSubDecoder @ 0041ef04 ====

dvbpsi_demux_subdec_t *
dvbpsi_NewDemuxSubDecoder
          (uint8_t i_table_id,uint16_t i_extension,dvbpsi_demux_detach_cb_t pf_detach,
          dvbpsi_demux_gather_cb_t pf_gather,dvbpsi_decoder_t *p_decoder)

{
  dvbpsi_demux_subdec_t *pdVar1;
  
                    /* Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???]
                       Unresolved local var: uint32_t i_id@[???] */
  if (pf_gather == (dvbpsi_demux_gather_cb_t)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("pf_gather","demux.c",0xa7,"dvbpsi_NewDemuxSubDecoder");
  }
  if (pf_detach == (dvbpsi_demux_detach_cb_t)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("pf_detach","demux.c",0xa8,"dvbpsi_NewDemuxSubDecoder");
  }
  pdVar1 = calloc(1,0x14);
  if (pdVar1 != (dvbpsi_demux_subdec_t *)0x0) {
    pdVar1->i_id = (uint)CONCAT12(i_table_id,i_extension);
    pdVar1->p_decoder = p_decoder;
    pdVar1->pf_gather = pf_gather;
    pdVar1->pf_detach = pf_detach;
  }
  return pdVar1;
}



// ==== dvbpsi_DeleteDemuxSubDecoder @ 0041efcc ====

void dvbpsi_DeleteDemuxSubDecoder(dvbpsi_demux_subdec_t *p_subdec)

{
  if (p_subdec == (dvbpsi_demux_subdec_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_subdec","demux.c",0xc0,"dvbpsi_DeleteDemuxSubDecoder");
  }
  dvbpsi_decoder_delete(p_subdec->p_decoder);
  free(p_subdec);
  return;
}



// ==== dvbpsi_AttachDemuxSubDecoder @ 0041f038 ====

void dvbpsi_AttachDemuxSubDecoder(dvbpsi_demux_t *p_demux,dvbpsi_demux_subdec_t *p_subdec)

{
  if (p_demux == (dvbpsi_demux_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_demux","demux.c",0xcf,"dvbpsi_AttachDemuxSubDecoder");
  }
  if (p_subdec == (dvbpsi_demux_subdec_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_subdec","demux.c",0xd0,"dvbpsi_AttachDemuxSubDecoder");
  }
  p_subdec->p_next = p_demux->p_first_subdec;
  p_demux->p_first_subdec = p_subdec;
  return;
}



// ==== dvbpsi_DetachDemuxSubDecoder @ 0041f0b4 ====

void dvbpsi_DetachDemuxSubDecoder(dvbpsi_demux_t *p_demux,dvbpsi_demux_subdec_t *p_subdec)

{
  dvbpsi_demux_subdec_t *pdVar1;
  dvbpsi_demux_subdec_s **ppdVar2;
  
                    /* Unresolved local var: dvbpsi_demux_subdec_t * * pp_prev_subdec@[???] */
  if (p_demux == (dvbpsi_demux_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_demux","demux.c",0xde,"dvbpsi_DetachDemuxSubDecoder");
  }
  pdVar1 = p_demux->p_first_subdec;
  if (pdVar1 == (dvbpsi_demux_subdec_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_demux->p_first_subdec","demux.c",0xdf,"dvbpsi_DetachDemuxSubDecoder");
  }
  if (p_subdec == (dvbpsi_demux_subdec_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_subdec","demux.c",0xe1,"dvbpsi_DetachDemuxSubDecoder");
  }
  ppdVar2 = &p_demux->p_first_subdec;
  for (; pdVar1 != p_subdec; pdVar1 = pdVar1->p_next) {
    ppdVar2 = &pdVar1->p_next;
  }
  *ppdVar2 = p_subdec->p_next;
  return;
}



