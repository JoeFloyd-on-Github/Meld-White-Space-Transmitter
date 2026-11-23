/*
 * 14_demux.c.c  --  annotated decompilation of libdvbpsi-1.1.0/src/demux.c
 * ========================================================================
 *
 * Original source : /var/lib/tftpboot/Meld_AV_Script/2p08b/avstreamer/src/libdvbpsi-1.1.0/src/demux.c
 *                   (DWARF CU @ 0x41eb90..0x41f16c, compiled with GNU C 4.5.3, MIPS32-LE)
 * Library / API   : VideoLAN libdvbpsi 1.1.0 (LGPLv2.1+), statically linked.
 *                   Matches the libdvbpsi 1.x subtable demuxer API
 *                   (dvbpsi_AttachDemux / dvbpsi_DetachDemux / subdecoder helpers).
 *
 * Role in av_streamer:
 *   Dispatches PSI sections arriving on a shared PID (e.g. DVB SDT/BAT PID
 *   0x0011, EIT PID 0x0012, TDT/TOT PID 0x0014) to registered subdecoders
 *   keyed by (table_id, table_id_extension). In av_streamer, table attach/
 *   detach functions (SDT, EIT, TOT) reference dvbpsi_demuxGetSubDec,
 *   dvbpsi_NewDemuxSubDecoder, dvbpsi_AttachDemuxSubDecoder, etc., though
 *   av_streamer itself only generates SI tables and does not run the demuxer.
 *
 * Main data structures (from DWARF):
 *   dvbpsi_demux_t (struct dvbpsi_demux_s, 40 bytes = 0x28):
 *     +0x00..+0x1b  dvbpsi_decoder_t base (28 bytes = 1 * sizeof(dvbpsi_decoder_t))
 *     +0x1c         dvbpsi_demux_subdec_t *p_first_subdec (Ghidra: *(pdVar3 + 1))
 *     +0x20         dvbpsi_demux_new_cb_t pf_new_callback (Ghidra: &pdVar3[1].b_discontinuity)
 *     +0x24         void *p_new_cb_data                   (Ghidra: pdVar3[1].p_current_section)
 *
 *   dvbpsi_demux_subdec_t (struct dvbpsi_demux_subdec_s, 20 bytes = 0x14):
 *     +0x00  uint32_t i_id                     (((uint32_t)i_table_id << 16) | i_extension)
 *     +0x04  dvbpsi_demux_gather_cb_t pf_gather
 *     +0x08  dvbpsi_decoder_t *p_decoder
 *     +0x0c  dvbpsi_demux_detach_cb_t pf_detach
 *     +0x10  dvbpsi_demux_subdec_s *p_next
 *
 * Globals owned: none.
 * Upstream fidelity: unmodified libdvbpsi 1.1.0 demux.c.
 */

/* dvbpsi_Demux @ 0x0041eb90
 * Purpose  : Demuxer gather callback registered with dvbpsi_decoder_new().
 *            Looks up a subdecoder matching (p_section->i_table_id,
 *            p_section->i_extension). If none is found, invokes
 *            p_demux->pf_new_callback so the application can attach one, then
 *            retries the lookup; if still absent, frees p_section.
 * Params   : p_dvbpsi  - handle with attached dvbpsi_demux_t (asserted non-NULL)
 *            p_section - completed PSI section from dvbpsi_packet_push()
 * Returns  : void
 * Callers  : dvbpsi_packet_push (via p_decoder->pf_gather)
 * Callees  : __assert_fail, (*pf_new_callback), (*p_subdec->pf_gather),
 *            dvbpsi_DeletePSISections
 */
// ==== dvbpsi_Demux @ 0041eb90 ====

void dvbpsi_Demux(dvbpsi_t *p_dvbpsi,dvbpsi_psi_section_t *p_section)

{
  uint *puVar1;              // p_subdec (dvbpsi_demux_subdec_t* viewed as uint32_t*)
  uint uVar2;                // i_id = ((uint)i_table_id << 16) | i_extension
  dvbpsi_decoder_t *pdVar3;  // p_demux (dvbpsi_demux_t*, base at +0x00)
  code *UNRECOVERED_JUMPTABLE; // p_subdec->pf_gather
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"demux.c",0x68,"dvbpsi_Demux"); // "p_dvbpsi" (inferred)
  }
  pdVar3 = p_dvbpsi->p_decoder;
  if (pdVar3 == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","demux.c",0x69,"dvbpsi_Demux");
  }
                    /* Unresolved local var: uint32_t i_id@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  puVar1 = *(uint **)(pdVar3 + 1); // p_demux->p_first_subdec (+0x1c)
  if (puVar1 == (uint *)0x0) {
LAB_0041ec0c:
    // No matching subdecoder yet: notify user callback (+0x20 pf_new_callback, +0x24 p_new_cb_data)
    (**(code **)&pdVar3[1].b_discontinuity)
              (p_dvbpsi,p_section->i_table_id,p_section->i_extension,pdVar3[1].p_current_section);
                    /* Unresolved local var: uint32_t i_id@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
    puVar1 = *(uint **)(pdVar3 + 1); // re-read p_first_subdec after callback may have attached one
    if (puVar1 == (uint *)0x0) {
LAB_0041ec64:
      dvbpsi_DeletePSISections(p_section);
      return;
    }
    uVar2 = (uint)CONCAT12(p_section->i_table_id,p_section->i_extension);
    if (uVar2 == *puVar1) {
      UNRECOVERED_JUMPTABLE = (code *)puVar1[1]; // p_subdec->pf_gather (+0x04)
      goto LAB_0041ec88;
    }
    do {
      puVar1 = (uint *)puVar1[4]; // p_subdec->p_next (+0x10)
      if (puVar1 == (uint *)0x0) goto LAB_0041ec64;
    } while (uVar2 != *puVar1);
  }
  else {
    // Search p_first_subdec list for matching (table_id, extension)
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
  // Dispatch: p_subdec->pf_gather(p_dvbpsi, p_subdec->p_decoder, p_section)
  (*UNRECOVERED_JUMPTABLE)(p_dvbpsi,puVar1[2],p_section);
  return;
}



/* dvbpsi_AttachDemux @ 0x0041ecf0
 * Purpose  : Allocate a dvbpsi_demux_t (40 bytes, max section 4096 = 0x1000,
 *            discontinuity=true) and attach it to p_dvbpsi->p_decoder.
 * Params   : p_dvbpsi      - handle with no existing decoder (asserted)
 *            pf_new_cb     - callback invoked when an unknown subtable arrives
 *            p_new_cb_data - opaque user data passed to pf_new_cb
 * Returns  : true on success, false on OOM.
 * Callees  : __assert_fail, dvbpsi_decoder_new
 */
// ==== dvbpsi_AttachDemux @ 0041ecf0 ====

_Bool dvbpsi_AttachDemux(dvbpsi_t *p_dvbpsi,dvbpsi_demux_new_cb_t pf_new_cb,void *p_new_cb_data)

{
  dvbpsi_decoder_t *pdVar1;  // p_demux (dvbpsi_demux_t*)
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"demux.c",0x38,"dvbpsi_AttachDemux");
  }
  if (p_dvbpsi->p_decoder != (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder == ((void *)0)","demux.c",0x39,"dvbpsi_AttachDemux");
  }
  pdVar1 = dvbpsi_decoder_new(dvbpsi_Demux,0x1000,true,0x28); // 0x28 = sizeof(dvbpsi_demux_s)
  if (pdVar1 != (dvbpsi_decoder_t *)0x0) {
    // Zero p_demux->p_first_subdec (+0x1c, 4 bytes) via overlaid dvbpsi_decoder_t fields
    pdVar1[1].i_magic[0] = 0;
    pdVar1[1].i_magic[1] = 0;
    pdVar1[1].i_magic[2] = 0;
    pdVar1[1].b_complete_header = false;
    *(dvbpsi_demux_new_cb_t *)&pdVar1[1].b_discontinuity = pf_new_cb; // +0x20: pf_new_callback
    pdVar1[1].p_current_section = p_new_cb_data;                      // +0x24: p_new_cb_data
    p_dvbpsi->p_decoder = pdVar1;
  }
  return pdVar1 != (dvbpsi_decoder_t *)0x0;
}



/* dvbpsi_demuxGetSubDec @ 0x0041edc4
 * Purpose  : Linearly search p_demux->p_first_subdec for a subdecoder
 *            matching (i_table_id, i_extension).
 * Params   : p_demux     - demux instance
 *            i_table_id  - MPEG/DVB table_id
 *            i_extension - 16-bit table_id_extension
 * Returns  : Matching dvbpsi_demux_subdec_t *, or NULL if not found.
 * Callers  : table *_attach() functions (SDT, EIT, NIT, TOT)
 */
// ==== dvbpsi_demuxGetSubDec @ 0041edc4 ====

dvbpsi_demux_subdec_t *
dvbpsi_demuxGetSubDec(dvbpsi_demux_t *p_demux,uint8_t i_table_id,uint16_t i_extension)

{
  dvbpsi_demux_subdec_t *pdVar1; // p_subdec cursor
  uint32_t uVar2;                // current p_subdec->i_id
  
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



/* dvbpsi_DetachDemux @ 0x0041ee10
 * Purpose  : Detach all attached subdecoders (calling pf_detach if non-NULL,
 *            otherwise freeing the subdecoder node), then delete the demux
 *            decoder and clear p_dvbpsi->p_decoder.
 * Params   : p_dvbpsi - handle with attached demuxer (asserted non-NULL)
 * Returns  : void
 * Callees  : __assert_fail, (*p_subdec->pf_detach), free, dvbpsi_decoder_delete
 */
// ==== dvbpsi_DetachDemux @ 0041ee10 ====

void dvbpsi_DetachDemux(dvbpsi_t *p_dvbpsi)

{
  dvbpsi_decoder_t *p_decoder; // p_demux (dvbpsi_demux_t*)
  uint *__ptr;                 // p_subdec (dvbpsi_demux_subdec_t* as uint*)
  uint *puVar1;                // p_subdec_temp (saved p_next before detach/free)
  code *pcVar2;                // p_subdec->pf_detach (+0x0c)
  
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
  __ptr = *(uint **)(p_decoder + 1); // p_demux->p_first_subdec (+0x1c)
  if (__ptr != (uint *)0x0) {
                    /* Unresolved local var: dvbpsi_demux_subdec_t * p_subdec_temp@[???] */
    pcVar2 = (code *)__ptr[3]; // pf_detach
    puVar1 = (uint *)__ptr[4]; // p_next
    if (pcVar2 == (code *)0x0) goto LAB_0041ee84;
    // Call pf_detach(p_dvbpsi, i_table_id, i_extension) for each subdecoder
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



/* dvbpsi_NewDemuxSubDecoder @ 0x0041ef04
 * Purpose  : Allocate and populate a dvbpsi_demux_subdec_t (0x14 bytes) for
 *            a specific (i_table_id, i_extension) pair.
 * Params   : i_table_id  - table_id to match
 *            i_extension - table_id_extension to match
 *            pf_detach   - detach callback (asserted non-NULL)
 *            pf_gather   - section gather callback (asserted non-NULL)
 *            p_decoder   - table-specific decoder struct
 * Returns  : Newly allocated dvbpsi_demux_subdec_t *, or NULL on OOM.
 * Callers  : table *_attach() functions (SDT, EIT, NIT, TOT)
 * Callees  : __assert_fail, calloc
 */
// ==== dvbpsi_NewDemuxSubDecoder @ 0041ef04 ====

dvbpsi_demux_subdec_t *
dvbpsi_NewDemuxSubDecoder
          (uint8_t i_table_id,uint16_t i_extension,dvbpsi_demux_detach_cb_t pf_detach,
          dvbpsi_demux_gather_cb_t pf_gather,dvbpsi_decoder_t *p_decoder)

{
  dvbpsi_demux_subdec_t *pdVar1; // allocated subdec (0x14 = 20 bytes)
  
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



/* dvbpsi_DeleteDemuxSubDecoder @ 0x0041efcc
 * Purpose  : Delete a subdecoder's underlying table decoder and free the
 *            dvbpsi_demux_subdec_t struct.
 * Params   : p_subdec - subdecoder to free (asserted non-NULL)
 * Returns  : void
 * Callers  : table *_detach() functions (SDT, EIT, NIT, TOT)
 * Callees  : __assert_fail, dvbpsi_decoder_delete, free
 */
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



/* dvbpsi_AttachDemuxSubDecoder @ 0x0041f038
 * Purpose  : Prepend a subdecoder to p_demux->p_first_subdec.
 * Params   : p_demux  - demux instance (asserted non-NULL)
 *            p_subdec - subdecoder to attach (asserted non-NULL)
 * Returns  : void
 * Callers  : table *_attach() functions (SDT, EIT, NIT, TOT)
 * Callees  : __assert_fail
 */
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



/* dvbpsi_DetachDemuxSubDecoder @ 0x0041f0b4
 * Purpose  : Unlink p_subdec from p_demux->p_first_subdec (without freeing it).
 * Params   : p_demux  - demux instance (asserted non-NULL, list non-empty)
 *            p_subdec - subdecoder to unlink (asserted non-NULL)
 * Returns  : void
 * Callers  : table *_detach() functions (SDT, EIT, NIT, TOT)
 * Callees  : __assert_fail
 */
// ==== dvbpsi_DetachDemuxSubDecoder @ 0041f0b4 ====

void dvbpsi_DetachDemuxSubDecoder(dvbpsi_demux_t *p_demux,dvbpsi_demux_subdec_t *p_subdec)

{
  dvbpsi_demux_subdec_t *pdVar1;   // current node in list walk
  dvbpsi_demux_subdec_s **ppdVar2; // pp_prev_subdec: pointer to previous p_next (or head)
  
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


