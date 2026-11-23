/*
 * 08_dvbpsi.c.c  --  annotated decompilation of libdvbpsi-1.1.0/src/dvbpsi.c
 * =========================================================================
 *
 * Original source : /var/lib/tftpboot/Meld_AV_Script/2p08b/avstreamer/src/libdvbpsi-1.1.0/src/dvbpsi.c
 *                   (DWARF CU @ 0x419a00..0x41a76c, compiled with GNU C 4.5.3, MIPS32-LE)
 * Library / API   : VideoLAN libdvbpsi 1.1.0 (LGPLv2.1+), statically linked.
 *                   Matches the libdvbpsi 1.x handle-based API (dvbpsi_new /
 *                   dvbpsi_delete / dvbpsi_decoder_new / dvbpsi_packet_push),
 *                   as opposed to the legacy 0.x API without dvbpsi_t handles.
 *
 * Role in av_streamer:
 *   Core handle lifecycle, generic PSI section-reassembly state machine
 *   (dvbpsi_packet_push), and logging helpers (dvbpsi_error/warning/debug).
 *   In av_streamer, 01_libxcode4encoder.c.c calls dvbpsi_new(message,
 *   DVBPSI_MSG_DEBUG) and dvbpsi_delete() when generating DVB SI tables
 *   (genNIT, genSDT, genTDT, genEIT); the decoder/packet_push path is linked
 *   in from libdvbpsi.a (not called by the encoder-only SI generators).
 *
 * Main data structures (from DWARF):
 *   dvbpsi_t (struct dvbpsi_s, 16 bytes):
 *     +0x00  dvbpsi_decoder_t *p_decoder   (attached PSI/SI decoder, or NULL)
 *     +0x04  dvbpsi_message_cb pf_message  (logging callback)
 *     +0x08  dvbpsi_msg_level_t i_msg_level (-1=NONE, 0=ERROR, 1=WARN, 2=DEBUG)
 *     +0x0c  void *p_sys                   (user/system private pointer)
 *
 *   dvbpsi_decoder_t (struct dvbpsi_decoder_s, 28 bytes = 0x1c):
 *     +0x00  uint8_t i_magic[3]            ("psi" = 0x70, 0x73, 0x69)
 *     +0x03  _Bool b_complete_header       (true once 3-byte PSI header read)
 *     +0x04  _Bool b_discontinuity         (TS continuity discontinuity flag)
 *     +0x05  _Bool b_current_valid         (true once a valid table is current)
 *     +0x06  uint8_t i_continuity_counter  (last TS CC, 0xff = uninitialized)
 *     +0x07  uint8_t i_last_section_number (expected last section_number)
 *     +0x08  dvbpsi_psi_section_t *p_current_section (in-progress section)
 *     +0x0c  dvbpsi_psi_section_t *p_sections        (sorted completed list)
 *     +0x10  dvbpsi_callback_gather_t pf_gather      (section callback)
 *     +0x14  int i_section_max_size        (max section buffer size)
 *     +0x18  int i_need                    (bytes remaining for header/payload)
 *
 * Globals owned: none.
 * Upstream fidelity: unmodified libdvbpsi 1.1.0 dvbpsi.c.
 */

/* dvbpsi_new @ 0x00419a00
 * Purpose  : Allocate and initialize a top-level libdvbpsi handle.
 * Params   : callback - user logging callback (dvbpsi_message_cb), or NULL
 *            level    - verbosity threshold (dvbpsi_msg_level)
 * Returns  : Newly allocated dvbpsi_t * on success, or NULL on OOM.
 * Callers  : genNIT, genSDT, genTDT, genEIT in 01_libxcode4encoder.c.c
 * Callees  : calloc
 */
// ==== dvbpsi_new @ 00419a00 ====

dvbpsi_t * dvbpsi_new(dvbpsi_message_cb callback,dvbpsi_msg_level level)

{
  dvbpsi_t *pdVar1;          // allocated dvbpsi_t handle
  
                    /* Unresolved local var: dvbpsi_t * p_dvbpsi@[DW_OP_reg2(v0)] */
  pdVar1 = calloc(1,0x10);   // sizeof(dvbpsi_s) == 16 bytes
  if (pdVar1 != (dvbpsi_t *)0x0) {
    pdVar1->p_decoder = (dvbpsi_decoder_t *)0x0;
    pdVar1->pf_message = callback;
    pdVar1->i_msg_level = level;
  }
  return pdVar1;
}



/* dvbpsi_delete @ 0x00419a5c
 * Purpose  : Free a libdvbpsi handle after its decoder has been detached.
 *            Asserts p_dvbpsi->p_decoder == NULL before freeing.
 * Params   : p_dvbpsi - handle previously created by dvbpsi_new()
 * Returns  : void
 * Callers  : genNIT, genSDT, genTDT, genEIT in 01_libxcode4encoder.c.c
 * Callees  : __assert_fail, free
 */
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



/* dvbpsi_decoder_new @ 0x00419ab0
 * Purpose  : Allocate and initialize a PSI/SI decoder base struct (or a larger
 *            table-specific decoder struct whose first member is
 *            dvbpsi_decoder_t, e.g. dvbpsi_tot_t / dvbpsi_demux_t).
 * Params   : pf_gather          - callback invoked per completed PSI section
 *            i_section_max_size - max section size (e.g. 1024 or 4096)
 *            b_discontinuity      - initial discontinuity state
 *            psi_size           - total allocation size (>= sizeof(dvbpsi_decoder_t) = 0x1c)
 * Returns  : Pointer to zero-initialized decoder struct, or NULL on OOM.
 * Callers  : dvbpsi_tot_attach (13_tables_tot.c.c), dvbpsi_AttachDemux (14_demux.c.c), etc.
 * Callees  : __assert_fail, calloc
 */
// ==== dvbpsi_decoder_new @ 00419ab0 ====

void * dvbpsi_decoder_new(dvbpsi_callback_gather_t pf_gather,int i_section_max_size,
                         _Bool b_discontinuity,size_t psi_size)

{
  undefined2 *puVar1;        // raw pointer to allocated dvbpsi_decoder_t (viewed as uint16_t*)
  undefined2 *puVar2;        // return value (same as puVar1, or NULL on OOM)
  
                    /* Unresolved local var: dvbpsi_decoder_t * p_decoder@[???] */
  if (psi_size < 0x1c) {     // 0x1c == sizeof(dvbpsi_decoder_s)
                    /* WARNING: Subroutine does not return */
    __assert_fail("psi_size >= sizeof(dvbpsi_decoder_t)","dvbpsi.c",0x51,"dvbpsi_decoder_new");
  }
  puVar1 = calloc(1,psi_size);
  puVar2 = (undefined2 *)0x0;
  if (puVar1 != (undefined2 *)0x0) {
    *(dvbpsi_callback_gather_t *)(puVar1 + 8) = pf_gather;   // +0x10: pf_gather
    *puVar1 = 0x7370;                                        // +0x00: i_magic[0..1] = 'p','s' (LE)
    *(undefined1 *)(puVar1 + 1) = 0x69;                      // +0x02: i_magic[2] = 'i' ("psi")
    *(undefined1 *)(puVar1 + 3) = 0xff;                      // +0x06: i_continuity_counter = 0xff (unset)
    *(undefined4 *)(puVar1 + 4) = 0;                         // +0x08: p_current_section = NULL
    *(int *)(puVar1 + 10) = i_section_max_size;              // +0x14: i_section_max_size
    *(_Bool *)(puVar1 + 2) = b_discontinuity;                // +0x04: b_discontinuity
    *(undefined1 *)((int)puVar1 + 5) = 0;                    // +0x05: b_current_valid = false
    *(undefined1 *)((int)puVar1 + 7) = 0;                    // +0x07: i_last_section_number = 0
    *(undefined4 *)(puVar1 + 6) = 0;                         // +0x0c: p_sections = NULL
    *(undefined1 *)((int)puVar1 + 3) = 0;                    // +0x03: b_complete_header = false
    puVar2 = puVar1;
  }
  return puVar2;
}



/* dvbpsi_decoder_reset @ 0x00419b80
 * Purpose  : Discard all queued completed PSI sections on a decoder and
 *            optionally invalidate the current table state.
 * Params   : p_decoder - decoder instance (asserted non-NULL)
 *            b_force   - if true, clears b_current_valid
 * Returns  : void
 * Callers  : table gather callbacks (e.g. dvbpsi_tot_sections_gather, etc.)
 * Callees  : __assert_fail, dvbpsi_DeletePSISections
 */
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



/* dvbpsi_decoder_psi_sections_completed @ 0x00419bf0
 * Purpose  : Check whether the decoder's sorted p_sections list contains a
 *            contiguous sequence of sections 0 .. i_last_section_number.
 * Params   : p_decoder - decoder instance (asserted non-NULL)
 * Returns  : true if all sections 0..i_last_section_number are present, else false.
 * Callers  : table gather callbacks (SDT, EIT, NIT, TOT)
 * Callees  : __assert_fail
 */
// ==== dvbpsi_decoder_psi_sections_completed @ 00419bf0 ====

_Bool dvbpsi_decoder_psi_sections_completed(dvbpsi_decoder_t *p_decoder)

{
  _Bool _Var1;               // b_complete result
  dvbpsi_psi_section_t *pdVar2; // cursor walking p_decoder->p_sections
  uint uVar3;                // prev_nr: section number of current node
  uint uVar4;                // expected next section number (prev_nr + 1)
  
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
        _Var1 = true;        // reached expected last_section_number contiguously
      }
      if (pdVar2 == (dvbpsi_psi_section_s *)0x0) {
        return _Var1;
      }
      uVar4 = uVar3 + 1;
      if (uVar4 == 0x100) {  // section_number is 8-bit (0..255)
                    /* WARNING: Subroutine does not return */
        __assert_fail(s_prev_nr_<_256_00424318,"dvbpsi.c",0x84,
                      "dvbpsi_decoder_psi_sections_completed");
      }
      uVar3 = (uint)pdVar2->i_number;
    } while (uVar3 == uVar4);
  }
  return _Var1;
}



/* dvbpsi_decoder_psi_section_add @ 0x00419cac
 * Purpose  : Insert a newly gathered PSI section into p_decoder->p_sections
 *            sorted by ascending i_number. If a section with the same i_number
 *            already exists, it is replaced and freed.
 * Params   : p_decoder - decoder instance (asserted non-NULL)
 *            p_section - detached section (asserted non-NULL, p_next == NULL)
 * Returns  : true if an existing section with the same i_number was overwritten,
 *            false if inserted as a new section number.
 * Callers  : table gather callbacks
 * Callees  : __assert_fail, dvbpsi_DeletePSISections
 */
// ==== dvbpsi_decoder_psi_section_add @ 00419cac ====

_Bool dvbpsi_decoder_psi_section_add(dvbpsi_decoder_t *p_decoder,dvbpsi_psi_section_t *p_section)

{
  byte bVar1;                // p_section->i_number
  _Bool _Var2;               // b_overwrite return flag
  dvbpsi_psi_section_t *p_section_00; // p: current node during traversal
  dvbpsi_psi_section_t *p_section_01; // p_prev: previous node (initially head)
  
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
    // Empty list: insert at head
    p_decoder->p_sections = p_section;
    p_section->p_next = (dvbpsi_psi_section_s *)0x0;
    return false;
  }
  bVar1 = p_section->i_number;
  if (p_section_01->i_number == bVar1) {
    // Replace head node with matching section number
    p_section->p_next = p_section_01->p_next;
    p_section_01->p_next = (dvbpsi_psi_section_s *)0x0;
    dvbpsi_DeletePSISections(p_section_01);
    _Var2 = true;
    p_decoder->p_sections = p_section;
  }
  else if (bVar1 < p_section_01->i_number) {
    // Insert before head
    p_section->p_next = p_section_01;
    _Var2 = false;
    p_decoder->p_sections = p_section;
  }
  else {
    // Walk list to find insertion point or matching i_number
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
    // Append at tail
    _Var2 = false;
    if (p_section_01->i_number < bVar1) {
      p_section_01->p_next = p_section;
      p_section->p_next = (dvbpsi_psi_section_s *)0x0;
    }
  }
  return _Var2;
}



/* dvbpsi_decoder_delete @ 0x00419e54
 * Purpose  : Free all queued sections, any in-progress current section, and
 *            the decoder struct itself.
 * Params   : p_decoder - decoder instance (asserted non-NULL)
 * Returns  : void
 * Callers  : dvbpsi_tot_detach (13_tables_tot.c.c), dvbpsi_DetachDemux (14_demux.c.c), etc.
 * Callees  : __assert_fail, dvbpsi_DeletePSISections, free
 */
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



/* dvbpsi_decoder_present @ 0x00419edc
 * Purpose  : Check whether a dvbpsi_t handle is non-NULL and has a decoder attached.
 * Params   : p_dvbpsi - handle to inspect
 * Returns  : true if p_dvbpsi != NULL && p_dvbpsi->p_decoder != NULL
 */
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



/* dvbpsi_error @ 0x00419ef4
 * Purpose  : Format and emit a DVBPSI_MSG_ERROR message via dvbpsi->pf_message
 *            if dvbpsi->i_msg_level > DVBPSI_MSG_NONE (-1).
 * Params   : dvbpsi - handle, src - component label, fmt... - printf format & args
 * Returns  : void
 * Callees  : vasprintf, asprintf, free, (*dvbpsi->pf_message)
 */
// ==== dvbpsi_error @ 00419ef4 ====

void dvbpsi_error(dvbpsi_t *dvbpsi,char *src,char *fmt,...)

{
  int iVar1;                 // vasprintf return (length of formatted body)
  int iVar2;                 // asprintf return
  undefined4 in_a3;          // 4th MIPS arg register ($a3) spilled for va_list
  undefined4 local_resc;     // stack home for $a3 / start of va_list
  char *local_1c;            // tmp: formatted message body
  char *local_18 [2];        // msg[0]: "libdvbpsi (%s): %s"
  
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



/* dvbpsi_packet_push @ 0x00419fe4
 * Purpose  : Feed one 188-byte (0xbc) MPEG-2 TS packet into the attached PSI
 *            decoder, checking sync byte (0x47), continuity counter, adaptation
 *            field, and pointer_field (PUSI), reassembling PSI sections and
 *            validating CRC-32 before invoking p_decoder->pf_gather.
 * Params   : p_dvbpsi - handle with attached decoder (asserted non-NULL)
 *            p_data   - pointer to 188-byte TS packet
 * Returns  : true on normal consumption (or end of packet), false on error
 *            (bad sync, duplicate CC, no payload, waiting for PUSI, or OOM).
 * Quirks   : Upstream libdvbpsi 1.1.0 bug at lines 391-405: dvbpsi_has_CRC32()
 *            reads p_section->i_table_id and p_section->b_syntax_indicator
 *            BEFORE they are populated from p_section->p_data[0..1].
 * Callees  : __assert_fail, dvbpsi_error, dvbpsi_DeletePSISections,
 *            dvbpsi_NewPSISection, dvbpsi_ValidPSISection, memcpy, pf_gather
 */
// ==== dvbpsi_packet_push @ 00419fe4 ====

_Bool dvbpsi_packet_push(dvbpsi_t *p_dvbpsi,uint8_t *p_data)

{
  uint8_t uVar1;             // scratch for i_table_id / i_last_number
  bool bVar2;                // result of dvbpsi_has_CRC32(p_section) check
  _Bool _Var3;               // scratch bool (b_complete_header / b_syntax_indicator / ValidPSI)
  uint uVar4;                // i_expected_counter, then section_length
  int iVar5;                 // i_need / i_section_max_size scratch
  uint8_t *puVar6;           // p_section->p_data
  undefined3 extraout_var;   // MIPS ABI upper 24 bits of _Bool return
  undefined3 extraout_var_00;// MIPS ABI upper 24 bits of _Bool return
  uint8_t *puVar7;           // updated p_payload_end / p_section->p_data
  char cVar8;                // cached b_syntax_indicator
  uint uVar9;                // received TS continuity_counter (p_data[3] & 0xf)
  dvbpsi_psi_section_t *p_section; // active section being assembled
  dvbpsi_decoder_t *pdVar10; // p_decoder = p_dvbpsi->p_decoder
  uint8_t *__n;              // i_available: bytes remaining in 188-byte TS packet
  byte *__src;               // p_payload_pos: current read pointer inside p_data
  byte *pbVar11;             // p_new_pos: start of new section if PUSI is set
  byte *pbVar12;             // pending start of next section in same TS packet
  
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
  if (*p_data != 0x47) {     // MPEG-2 TS sync byte 0x47 ('G')
    dvbpsi_error(p_dvbpsi,"PSI decoder","not a TS packet");
    return false;
  }
  // Continuity counter (CC) check: p_data[3] bits 3..0
  if (pdVar10->i_continuity_counter == 0xff) {
    pdVar10->i_continuity_counter = p_data[3] & 0xf;
  }
  else {
    uVar4 = pdVar10->i_continuity_counter + 1 & 0xf;
    uVar9 = p_data[3] & 0xf;
    pdVar10->i_continuity_counter = (uint8_t)uVar9;
    if ((uVar4 == (uVar9 + 1 & 0xf)) && (pdVar10->b_discontinuity == false)) {
      // Same CC twice in a row -> duplicate packet; PID = ((p_data[1] & 0x1f) << 8) | p_data[2]
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
  // Adaptation field control (p_data[3] & 0x30): bit 4 = payload present, bit 5 = adaptation field
  if ((p_data[3] & 0x10) == 0) {
    return false;            // no payload in TS packet
  }
  __src = p_data + 4;
  if ((p_data[3] & 0x20) != 0) {
    __src = p_data + p_data[4] + 5; // skip adaptation_field_length + adaptation_field
  }
  // Payload Unit Start Indicator (PUSI = p_data[1] & 0x40): pointer_field follows
  pbVar11 = (byte *)0x0;
  if ((p_data[1] & 0x40) != 0) {
    pbVar11 = __src + *__src + 1; // start of new section after pointer_field bytes
    __src = __src + 1;
  }
  p_section = pdVar10->p_current_section;
  pbVar12 = pbVar11;
  if (p_section == (dvbpsi_psi_section_t *)0x0) {
    if (pbVar11 == (byte *)0x0) {
      return false;          // no section in progress and no PUSI -> wait for start
    }
    p_section = dvbpsi_NewPSISection(pdVar10->i_section_max_size);
    pdVar10->p_current_section = p_section;
    if (p_section == (dvbpsi_psi_section_t *)0x0) {
      return false;
    }
    pdVar10->i_need = 3;     // need 3-byte common PSI header first
    pdVar10->b_complete_header = false;
    pbVar12 = (byte *)0x0;
    __src = pbVar11;
  }
  __n = p_data + (0xbc - (int)__src); // bytes left in 188-byte (0xbc) TS packet
  if ((int)__n < 1) {
    return true;
  }
  do {
    while( true ) {
      if ((int)__n < pdVar10->i_need) {
        // Partial copy; more bytes needed from subsequent TS packet(s)
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
      // Full section body received. Check dvbpsi_has_CRC32():
      // true unless table_id in {0x70(TDT), 0x71(RST), 0x72(ST), 0x7e(DIT)},
      // or if b_syntax_indicator==false and table_id != 0x73(TOT).
      // NOTE: upstream bug in libdvbpsi 1.1.0 reads i_table_id/b_syntax_indicator
      // before parsing them from p_section->p_data below.
      uVar1 = p_section->i_table_id;
      bVar2 = false;
      if (((2 < (byte)(uVar1 + 0x90)) && (uVar1 != 0x7e)) &&
         (bVar2 = true, p_section->b_syntax_indicator == false)) {
        bVar2 = uVar1 == 0x73;
      }
      puVar6 = p_section->p_data;
      _Var3 = (_Bool)-((char)puVar6[1] >> 7); // bit 7 of byte 1: section_syntax_indicator
      p_section->b_syntax_indicator = _Var3;
      p_section->b_private_indicator = SUB41((puVar6[1] & 0x7f) >> 6,0); // bit 6: private_indicator
      if (_Var3 == false) {
        cVar8 = '\0';
        if (!bVar2) {
LAB_0041a39c:
          p_section->i_table_id = *puVar6;
          if (cVar8 != '\0') goto LAB_0041a3a8;
LAB_0041a424:
          // Short section (syntax_indicator == 0): 3-byte header only
          p_section->i_extension = 0;
          p_section->i_version = 0;
          p_section->b_current_next = true;
          p_section->i_number = 0;
          p_section->i_last_number = 0;
          p_section->p_payload_start = puVar6 + 3;
          goto LAB_0041a3ec;
        }
        p_section->p_payload_end = puVar7 + -4; // strip 4-byte CRC-32 from payload
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
        p_section->p_payload_end = puVar7 + -4; // strip 4-byte CRC-32 from payload
        if (bVar2) goto LAB_0041a314;
        p_section->i_table_id = *puVar6;
        if (_Var3 == false) goto LAB_0041a424;
LAB_0041a3a8:
        // Long section (syntax_indicator == 1): 8-byte extended PSI header
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
        if (*__src == 0xff) { // TS stuffing byte 0xff marks end of sections in packet
          return true;
        }
      }
LAB_0041a218:
      // Start next section in remaining bytes of this TS packet
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
    // Just finished 3-byte common PSI header: extract 12-bit section_length
    puVar7 = p_section->p_data;
    pdVar10->b_complete_header = true;
    iVar5 = pdVar10->i_section_max_size;
    uVar4 = (puVar7[1] & 0xf) << 8 | (uint)puVar7[2];
    p_section->i_length = (uint16_t)uVar4;
    pdVar10->i_need = uVar4;
    if (iVar5 + -2 <= (int)uVar4) { // section_length + 3 > i_section_max_size
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



/* dvbpsi_warning @ 0x0041a588
 * Purpose  : Format and emit a DVBPSI_MSG_WARN message via dvbpsi->pf_message
 *            if dvbpsi->i_msg_level > DVBPSI_MSG_ERROR (0).
 * Params   : dvbpsi - handle, src - component label, fmt... - printf format & args
 * Returns  : void
 * Callees  : vasprintf, asprintf, free, (*dvbpsi->pf_message)
 */
// ==== dvbpsi_warning @ 0041a588 ====

void dvbpsi_warning(dvbpsi_t *dvbpsi,char *src,char *fmt,...)

{
  int iVar1;                 // vasprintf return (length of formatted body)
  int iVar2;                 // asprintf return
  undefined4 in_a3;          // 4th MIPS arg register ($a3) spilled for va_list
  undefined4 local_resc;     // stack home for $a3 / start of va_list
  char *local_1c;            // tmp: formatted message body
  char *local_18 [2];        // msg[0]: "libdvbpsi (%s): %s"
  
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



/* dvbpsi_debug @ 0x0041a678
 * Purpose  : Format and emit a DVBPSI_MSG_DEBUG message via dvbpsi->pf_message
 *            if dvbpsi->i_msg_level > DVBPSI_MSG_WARN (1).
 * Params   : dvbpsi - handle, src - component label, fmt... - printf format & args
 * Returns  : void
 * Callees  : vasprintf, asprintf, free, (*dvbpsi->pf_message)
 */
// ==== dvbpsi_debug @ 0041a678 ====

void dvbpsi_debug(dvbpsi_t *dvbpsi,char *src,char *fmt,...)

{
  int iVar1;                 // vasprintf return (length of formatted body)
  int iVar2;                 // asprintf return
  undefined4 in_a3;          // 4th MIPS arg register ($a3) spilled for va_list
  undefined4 local_resc;     // stack home for $a3 / start of va_list
  char *local_1c;            // tmp: formatted message body
  char *local_18 [2];        // msg[0]: "libdvbpsi (%s): %s"
  
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


