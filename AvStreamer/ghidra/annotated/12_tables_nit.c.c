/*
 * 12_tables_nit.c.c  --  annotated decompilation of libdvbpsi tables/nit.c
 * =========================================================================
 *
 * Original source file: tables/nit.c (VideoLAN libdvbpsi, LGPL, statically
 * linked into av_streamer).
 *
 * Upstream API generation: libdvbpsi 1.x (1.0.x-1.3.x era), evidenced by the
 * `dvbpsi_t *` handle parameter, lowercase `dvbpsi_nit_*` public API
 * (`dvbpsi_nit_attach`, `dvbpsi_nit_init`, `dvbpsi_nit_sections_generate`,
 * `dvbpsi_nit_descriptor_add`, `dvbpsi_nit_ts_add`,
 * `dvbpsi_nit_ts_descriptor_add`), and `dvbpsi_demux_t` sub-decoder model.
 * Internal helper `dvbpsi_ReInitNIT` retains the older CamelCase naming.
 *
 * Purpose in av_streamer:
 *   Provides DVB Network Information Table (NIT, ETSI EN 300 468, PID 0x0010,
 *   table_id 0x40 actual network / 0x41 other network) construction, section
 *   serialization, and section parsing/decoding.
 *   In av_streamer, ONLY the table-building / section-generation side is
 *   called at runtime — specifically from `genNIT()` in
 *   01_libxcode4encoder.c.c (`dvbpsi_nit_init`, `dvbpsi_nit_descriptor_add`,
 *   `dvbpsi_nit_ts_add`, `dvbpsi_nit_ts_descriptor_add`,
 *   `dvbpsi_nit_sections_generate`, `dvbpsi_nit_empty`), which emits an NIT on
 *   PID 0x0010 (table_id=0x40, network_id=0x2114, version=0x17) with a
 *   network_name_descriptor (0x40, "MELD") and one TS entry (ts_id=0,
 *   orig_network_id=0x2114) carrying terrestrial_delivery_system (0x5a),
 *   cell_list (0x6c, lat/lon), and service_list (0x41) descriptors.
 *   The decoder/demux functions (`dvbpsi_nit_attach`, `dvbpsi_nit_detach`,
 *   `dvbpsi_nit_new`, `dvbpsi_nit_delete`, `dvbpsi_ReInitNIT`,
 *   `dvbpsi_nit_sections_decode`, `dvbpsi_nit_sections_gather`) are dead code
 *   in av_streamer.
 *
 * Main data structures (offsets from binary DWARF):
 *   dvbpsi_nit_t (16 bytes):
 *     +0x00 uint8_t  i_table_id          (0x40 = NIT actual, 0x41 = NIT other)
 *     +0x02 uint16_t i_extension         (sub-decoder extension)
 *     +0x04 uint16_t i_network_id        (written to PSI section_syntax_id / i_extension)
 *     +0x06 uint8_t  i_version           (5-bit version_number)
 *     +0x07 _Bool    b_current_next
 *     +0x08 dvbpsi_descriptor_t *p_first_descriptor  (network descriptors)
 *     +0x0c dvbpsi_nit_ts_t     *p_first_ts          (transport stream loop)
 *   dvbpsi_nit_ts_t (12 bytes):
 *     +0x00 uint16_t i_ts_id
 *     +0x02 uint16_t i_orig_network_id
 *     +0x04 dvbpsi_descriptor_t *p_first_descriptor  (TS descriptors)
 *     +0x08 dvbpsi_nit_ts_s     *p_next
 *   dvbpsi_nit_decoder_t (60 / 0x3c bytes):
 *     +0x00..+0x1b dvbpsi_decoder_t      (28 bytes)
 *     +0x1c dvbpsi_nit_callback pf_nit_callback      (Ghidra: p_decoder + 1)
 *     +0x20 void    *p_cb_data                       (Ghidra: &p_decoder[1].b_discontinuity)
 *     +0x24 dvbpsi_nit_t current_nit                 (+0x2a: {i_version, b_current_next})
 *     +0x34 dvbpsi_nit_t *p_building_nit             (Ghidra: p_decoder[1].i_need)
 *     +0x38 uint16_t i_network_id                    (Ghidra: p_decoder[2].i_magic[0..1])
 *
 * Globals owned: none.
 */

/* dvbpsi_nit_attach @ 0041cc80
 * Role (upstream libdvbpsi 1.x): Attaches an NIT sub-decoder for (i_table_id,
 *   i_extension) to p_dvbpsi's demuxer. Stores i_extension as expected
 *   i_network_id at +0x38.
 * Parameters: p_dvbpsi, i_table_id (0x40 or 0x41), i_extension (network_id),
 *   pf_callback, p_cb_data.
 * Returns: true on success, false if already attached or allocation fails.
 * Callers: dead code in av_streamer. */
_Bool dvbpsi_nit_attach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension,
                       dvbpsi_nit_callback pf_callback,void *p_cb_data)

{
  dvbpsi_demux_subdec_t *pdVar1;     // existing or newly created demux sub-decoder
  dvbpsi_decoder_t *p_decoder;       // allocated dvbpsi_nit_decoder_t (0x3c bytes)
  dvbpsi_demux_t *p_demux;           // demuxer from p_dvbpsi->p_decoder
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_nit_decoder_t * p_nit_decoder@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/nit.c",0x3e,"dvbpsi_nit_attach");
  }
  p_demux = (dvbpsi_demux_t *)p_dvbpsi->p_decoder;
  if (p_demux == (dvbpsi_demux_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/nit.c",0x3f,"dvbpsi_nit_attach");
  }
  pdVar1 = dvbpsi_demuxGetSubDec(p_demux,i_table_id,i_extension);
  if (pdVar1 == (dvbpsi_demux_subdec_t *)0x0) {
    // Allocate 0x3c (60) bytes for dvbpsi_nit_decoder_t
    p_decoder = dvbpsi_decoder_new((dvbpsi_callback_gather_t)0x0,0,true,0x3c);
    if (p_decoder != (dvbpsi_decoder_t *)0x0) {
      pdVar1 = dvbpsi_NewDemuxSubDecoder
                         (i_table_id,i_extension,dvbpsi_nit_detach,dvbpsi_nit_sections_gather,
                          p_decoder);
      if (pdVar1 != (dvbpsi_demux_subdec_t *)0x0) {
        dvbpsi_AttachDemuxSubDecoder(p_demux,pdVar1);
        *(uint16_t *)p_decoder[2].i_magic = i_extension;       // +0x38: i_network_id
        *(dvbpsi_nit_callback *)(p_decoder + 1) = pf_callback; // +0x1c: pf_nit_callback
        *(void **)&p_decoder[1].b_discontinuity = p_cb_data;   // +0x20: p_cb_data
        p_decoder[1].i_need = 0;                               // +0x34: p_building_nit = NULL
        return true;
      }
      dvbpsi_decoder_delete(p_decoder);
    }
  }
  else {
    dvbpsi_error(p_dvbpsi,"NIT decoder",
                 "Already a decoder for (table_id == 0x%02x,extension == 0x%02x)",i_table_id,
                 i_extension);
  }
  return false;
}



/* dvbpsi_nit_init @ 0041ce34
 * Role (upstream libdvbpsi 1.x): Initializes a caller-allocated dvbpsi_nit_t.
 * Parameters: p_nit, i_table_id (0x40), i_extension, i_network_id, i_version,
 *   b_current_next.
 * Callers: genNIT() in 01_libxcode4encoder.c.c (0x40, ext=3, net_id=0x2114,
 *   ver=0x17, b_current_next=true). */
void dvbpsi_nit_init(dvbpsi_nit_t *p_nit,uint8_t i_table_id,uint16_t i_extension,
                    uint16_t i_network_id,uint8_t i_version,_Bool b_current_next)

{
  p_nit->i_table_id = i_table_id;
  p_nit->i_extension = i_extension;
  p_nit->i_network_id = i_network_id;
  p_nit->i_version = i_version;
  p_nit->b_current_next = b_current_next;
  p_nit->p_first_descriptor = (dvbpsi_descriptor_t *)0x0;
  p_nit->p_first_ts = (dvbpsi_nit_ts_t *)0x0;
  return;
}



/* dvbpsi_nit_new @ 0041ce5c
 * Role (upstream libdvbpsi 1.x): Heap-allocates (16 bytes) and initializes a
 *   dvbpsi_nit_t structure.
 * Returns: newly allocated dvbpsi_nit_t *, or NULL on malloc failure.
 * Callers: dvbpsi_nit_sections_gather (via inlined dvbpsi_AddSectionNIT). */
dvbpsi_nit_t *
dvbpsi_nit_new(uint8_t i_table_id,uint16_t i_extension,uint16_t i_network_id,uint8_t i_version,
              _Bool b_current_next)

{
  dvbpsi_nit_t *pdVar1;              // newly allocated dvbpsi_nit_t *
  
                    /* Unresolved local var: dvbpsi_nit_t * p_nit@[DW_OP_reg2(v0)] */
  pdVar1 = malloc(0x10);             // sizeof(dvbpsi_nit_t) == 16
  if (pdVar1 != (dvbpsi_nit_t *)0x0) {
    pdVar1->i_table_id = i_table_id;
    pdVar1->i_extension = i_extension;
    pdVar1->i_network_id = i_network_id;
    pdVar1->i_version = i_version;
    pdVar1->b_current_next = b_current_next;
    pdVar1->p_first_descriptor = (dvbpsi_descriptor_t *)0x0;
    pdVar1->p_first_ts = (dvbpsi_nit_ts_t *)0x0;
  }
  return pdVar1;
}



/* dvbpsi_nit_empty @ 0041cee8
 * Role (upstream libdvbpsi 1.x): Frees network-level descriptors, all TS
 *   entries in p_nit->p_first_ts, and each TS entry's descriptor list.
 * Callers: genNIT() in 01_libxcode4encoder.c.c, and dvbpsi_nit_delete(). */
void dvbpsi_nit_empty(dvbpsi_nit_t *p_nit)

{
  dvbpsi_nit_ts_t *__ptr;            // current TS node being freed (p_ts)
  dvbpsi_nit_ts_s *pdVar1;           // saved next pointer (p_tmp)
  
                    /* Unresolved local var: dvbpsi_nit_ts_t * p_ts@[???] */
  __ptr = p_nit->p_first_ts;
  dvbpsi_DeleteDescriptors(p_nit->p_first_descriptor);
  while (__ptr != (dvbpsi_nit_ts_t *)0x0) {
                    /* Unresolved local var: dvbpsi_nit_ts_t * p_tmp@[???] */
    pdVar1 = __ptr->p_next;
    dvbpsi_DeleteDescriptors(__ptr->p_first_descriptor);
    free(__ptr);
    __ptr = pdVar1;
  }
  p_nit->p_first_descriptor = (dvbpsi_descriptor_t *)0x0;
  p_nit->p_first_ts = (dvbpsi_nit_ts_t *)0x0;
  return;
}



/* dvbpsi_nit_delete @ 0041cf70
 * Role (upstream libdvbpsi 1.x): Empties and frees a heap-allocated dvbpsi_nit_t.
 * Callers: dvbpsi_ReInitNIT, dvbpsi_nit_detach. */
void dvbpsi_nit_delete(dvbpsi_nit_t *p_nit)

{
  if (p_nit != (dvbpsi_nit_t *)0x0) {
    dvbpsi_nit_empty(p_nit);
  }
  free(p_nit);
  return;
}



/* dvbpsi_ReInitNIT @ 0041cfb4
 * Role (upstream libdvbpsi 1.x internal): Resets the NIT decoder state.
 * Parameters: p_decoder (asserted non-NULL), b_force (if true, also deletes
 *   p_building_nit).
 * Callers: dvbpsi_nit_sections_gather. */
void dvbpsi_ReInitNIT(dvbpsi_nit_decoder_t *p_decoder,_Bool b_force)

{
  undefined3 in_register_00000015;   // upper 24 bits of MIPS $a1 register (b_force)
  
  if (p_decoder == (dvbpsi_nit_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_decoder","tables/nit.c",0x11d,"dvbpsi_ReInitNIT");
  }
  dvbpsi_decoder_reset((dvbpsi_decoder_t *)p_decoder,b_force);
  if ((CONCAT31(in_register_00000015,b_force) != 0) &&
     (p_decoder->p_building_nit != (dvbpsi_nit_t *)0x0)) {
    dvbpsi_nit_delete(p_decoder->p_building_nit);
  }
  p_decoder->p_building_nit = (dvbpsi_nit_t *)0x0;
  return;
}



/* dvbpsi_nit_detach @ 0041d040
 * Role (upstream libdvbpsi 1.x): Detaches and frees the NIT sub-decoder for
 *   (i_table_id, i_extension) from p_dvbpsi's demuxer.
 * Quirk: omits NULL-pointer asserts on p_dvbpsi and p_dvbpsi->p_decoder that
 *   dvbpsi_sdt_detach and dvbpsi_eit_detach include.
 * Callers: dead code in av_streamer. */
void dvbpsi_nit_detach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension)

{
  dvbpsi_demux_subdec_t *p_subdec;   // located sub-decoder entry
  dvbpsi_nit_t *p_nit;               // p_nit_decoder->p_building_nit (+0x34)
  dvbpsi_demux_t *p_demux;           // demuxer from p_dvbpsi->p_decoder
  dvbpsi_decoder_t *pdVar1;          // p_subdec->p_decoder (dvbpsi_nit_decoder_t *)
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???]
                       Unresolved local var: dvbpsi_nit_decoder_t * p_nit_decoder@[???] */
  p_demux = (dvbpsi_demux_t *)p_dvbpsi->p_decoder;
  p_subdec = dvbpsi_demuxGetSubDec(p_demux,i_table_id,i_extension);
  if (p_subdec == (dvbpsi_demux_subdec_t *)0x0) {
    dvbpsi_error(p_dvbpsi,"NIT Decoder",
                 "No such NIT decoder (table_id == 0x%02x,extension == 0x%02x)",i_table_id,
                 i_extension);
    return;
  }
  pdVar1 = p_subdec->p_decoder;
  p_nit = (dvbpsi_nit_t *)pdVar1[1].i_need; // +0x34: p_building_nit
  if (p_nit != (dvbpsi_nit_t *)0x0) {
    dvbpsi_nit_delete(p_nit);
  }
  pdVar1[1].i_need = 0;
  dvbpsi_DetachDemuxSubDecoder(p_demux,p_subdec);
  dvbpsi_DeleteDemuxSubDecoder(p_subdec);
  return;
}



/* dvbpsi_nit_descriptor_add @ 0041d138
 * Role (upstream libdvbpsi 1.x): Creates a network-level descriptor and appends
 *   it to p_nit->p_first_descriptor (inline list append rather than calling
 *   dvbpsi_AddDescriptor).
 * Callers: genNIT() in 01_libxcode4encoder.c.c (tag 0x40 network_name "MELD"),
 *   and dvbpsi_nit_sections_decode(). */
dvbpsi_descriptor_t *
dvbpsi_nit_descriptor_add(dvbpsi_nit_t *p_nit,uint8_t i_tag,uint8_t i_length,uint8_t *p_data)

{
  dvbpsi_descriptor_t *pdVar1;       // newly allocated descriptor (p_descriptor)
  dvbpsi_descriptor_t *pdVar2;       // list traversal cursor (p_last_descriptor)
  
                    /* Unresolved local var: dvbpsi_descriptor_t * p_descriptor@[DW_OP_reg2(v0)] */
  pdVar1 = dvbpsi_NewDescriptor(i_tag,i_length,p_data);
  if (pdVar1 != (dvbpsi_descriptor_t *)0x0) {
    pdVar2 = p_nit->p_first_descriptor;
    if (p_nit->p_first_descriptor == (dvbpsi_descriptor_t *)0x0) {
      p_nit->p_first_descriptor = pdVar1;
    }
    else {
      while (pdVar2->p_next != (dvbpsi_descriptor_s *)0x0) {
                    /* Unresolved local var: dvbpsi_descriptor_t * p_last_descriptor@[???] */
        pdVar2 = pdVar2->p_next;
      }
      pdVar2->p_next = pdVar1;
    }
  }
  return pdVar1;
}



/* dvbpsi_nit_ts_add @ 0041d1b0
 * Role (upstream libdvbpsi 1.x): Allocates a dvbpsi_nit_ts_t (12 bytes) and
 *   appends it to p_nit->p_first_ts.
 * Parameters: p_nit, i_ts_id, i_orig_network_id.
 * Returns: newly allocated TS entry, or NULL on malloc failure.
 * Callers: genNIT() in 01_libxcode4encoder.c.c (ts_id=0, orig_net_id=0x2114),
 *   and dvbpsi_nit_sections_decode(). */
dvbpsi_nit_ts_t * dvbpsi_nit_ts_add(dvbpsi_nit_t *p_nit,uint16_t i_ts_id,uint16_t i_orig_network_id)

{
  dvbpsi_nit_ts_t *pdVar1;           // newly allocated TS entry (p_ts)
  dvbpsi_nit_ts_t *pdVar2;           // list traversal cursor (p_last_ts)
  
                    /* Unresolved local var: dvbpsi_nit_ts_t * p_ts@[DW_OP_reg2(v0)] */
  pdVar1 = malloc(0xc);              // sizeof(dvbpsi_nit_ts_t) == 12
  if (pdVar1 != (dvbpsi_nit_ts_t *)0x0) {
    pdVar2 = p_nit->p_first_ts;
    pdVar1->i_ts_id = i_ts_id;
    pdVar1->i_orig_network_id = i_orig_network_id;
    pdVar1->p_first_descriptor = (dvbpsi_descriptor_t *)0x0;
    pdVar1->p_next = (dvbpsi_nit_ts_s *)0x0;
    if (pdVar2 == (dvbpsi_nit_ts_t *)0x0) {
      p_nit->p_first_ts = pdVar1;
    }
    else {
      while (pdVar2->p_next != (dvbpsi_nit_ts_s *)0x0) {
                    /* Unresolved local var: dvbpsi_nit_ts_t * p_last_ts@[???] */
        pdVar2 = pdVar2->p_next;
      }
      pdVar2->p_next = pdVar1;
    }
  }
  return pdVar1;
}



/* dvbpsi_nit_ts_descriptor_add @ 0041d244
 * Role (upstream libdvbpsi 1.x): Creates a TS-level descriptor and appends it
 *   to p_ts->p_first_descriptor via dvbpsi_AddDescriptor.
 * Callers: genNIT() in 01_libxcode4encoder.c.c (tags 0x5a, 0x6c, 0x41), and
 *   dvbpsi_nit_sections_decode(). */
dvbpsi_descriptor_t *
dvbpsi_nit_ts_descriptor_add(dvbpsi_nit_ts_t *p_ts,uint8_t i_tag,uint8_t i_length,uint8_t *p_data)

{
  dvbpsi_descriptor_t *p_descriptor; // newly allocated descriptor
  dvbpsi_descriptor_t *pdVar1;       // updated head of descriptor list
  
                    /* Unresolved local var: dvbpsi_descriptor_t * p_descriptor@[???] */
  p_descriptor = dvbpsi_NewDescriptor(i_tag,i_length,p_data);
  if (p_descriptor != (dvbpsi_descriptor_t *)0x0) {
    pdVar1 = dvbpsi_AddDescriptor(p_ts->p_first_descriptor,p_descriptor);
    p_ts->p_first_descriptor = pdVar1;
    if (pdVar1 == (dvbpsi_descriptor_t *)0x0) {
                    /* WARNING: Subroutine does not return */
      __assert_fail("p_ts->p_first_descriptor","tables/nit.c",0x113,"dvbpsi_nit_ts_descriptor_add");
    }
  }
  return p_descriptor;
}



/* dvbpsi_nit_sections_decode @ 0041d2dc
 * Role (upstream libdvbpsi 1.x): Parses all linked PSI sections in p_section
 *   and populates p_nit with network descriptors and TS entries (+ their
 *   descriptors).
 * NIT section payload layout (after 8-byte PSI header):
 *   +0..+1: reserved_future_use[15:12], network_descriptors_length[11:0]
 *   +2..:   network descriptor loop
 *   next 2B: reserved_future_use[15:12], transport_stream_loop_length[11:0]
 *   loop of TS entries, each 6 bytes + transport_descriptors_length:
 *     +0..+1: transport_stream_id
 *     +2..+3: original_network_id
 *     +4..+5: reserved_future_use[15:12], transport_descriptors_length[11:0]
 * Callers: dvbpsi_nit_sections_gather (dead code in av_streamer). */
void dvbpsi_nit_sections_decode(dvbpsi_nit_t *p_nit,dvbpsi_psi_section_t *p_section)

{
  byte bVar1;                        // scratch byte (length hi / descriptor length)
  uint8_t i_tag;                     // descriptor tag
  byte bVar2;                        // low byte of transport_descriptors_length
  dvbpsi_nit_ts_t *p_ts;             // newly added TS entry
  uint8_t *p_data;                   // descriptor payload pointer
  byte *pbVar3;                      // current byte cursor / lookahead pointer
  uint uVar4;                        // descriptor length
  byte *pbVar5;                      // end of current descriptor loop / TS loop
  byte *pbVar6;                      // descriptor payload or TS entry cursor
  byte *pbVar7;                      // next descriptor payload pointer
  byte *pbVar8;                      // scratch: &length byte or end of TS descriptors
  
  do {
    if (p_section == (dvbpsi_psi_section_t *)0x0) {
      return;
    }
    pbVar6 = p_section->p_payload_start;
    pbVar3 = pbVar6 + 2;
    pbVar5 = pbVar3 + ((*pbVar6 & 0xf) << 8 | (uint)pbVar6[1]); // end of network descriptors
    pbVar6 = pbVar6 + 4;
    if (pbVar5 < pbVar6) {
      bVar1 = *pbVar3;
    }
    else {
      do {
        while( true ) {
          pbVar8 = pbVar3 + 1;
          uVar4 = (uint)*pbVar8;
          i_tag = *pbVar3;
          if ((int)pbVar5 - (int)pbVar3 <= (int)(uVar4 + 1)) break;
          pbVar3 = pbVar3 + uVar4 + 2;
          pbVar7 = pbVar3 + 2;
          dvbpsi_nit_descriptor_add(p_nit,i_tag,*pbVar8,pbVar6);
          pbVar6 = pbVar7;
          if (pbVar5 < pbVar7) goto LAB_0041d3a0;
        }
                    /* Unresolved local var: uint8_t i_tag@[???]
                       Unresolved local var: uint8_t i_length@[???] */
        pbVar3 = pbVar3 + uVar4 + 2;
        pbVar6 = pbVar3 + 2;
      } while (pbVar6 <= pbVar5);
LAB_0041d3a0:
      bVar1 = *pbVar3;               // high byte of transport_stream_loop_length
    }
    pbVar5 = pbVar3 + ((bVar1 & 0xf) << 8 | (uint)pbVar3[1]) + 2;
    if (p_section->p_payload_end <= pbVar3 + ((bVar1 & 0xf) << 8 | (uint)pbVar3[1]) + 2) {
      pbVar5 = p_section->p_payload_end;
    }
    while (pbVar3 = pbVar6 + 6, pbVar3 <= pbVar5) {
                    /* Unresolved local var: uint8_t * p_end2@[???]
                       Unresolved local var: uint16_t i_ts_id@[???]
                       Unresolved local var: uint16_t i_orig_network_id@[???]
                       Unresolved local var: uint16_t i_ts_length@[???]
                       Unresolved local var: dvbpsi_nit_ts_t * p_ts@[???] */
      bVar1 = pbVar6[4];
      bVar2 = pbVar6[5];
      p_ts = dvbpsi_nit_ts_add(p_nit,CONCAT11(*pbVar6,pbVar6[1]),CONCAT11(pbVar6[2],pbVar6[3]));
      if (p_ts == (dvbpsi_nit_ts_t *)0x0) break;
      pbVar8 = pbVar3 + ((bVar1 & 0xf) << 8 | (uint)bVar2);
      if (p_section->p_payload_end <= pbVar3 + ((bVar1 & 0xf) << 8 | (uint)bVar2)) {
        pbVar8 = p_section->p_payload_end;
      }
      p_data = pbVar6 + 8;
      pbVar6 = pbVar3;
      while (p_data <= pbVar8) {
                    /* Unresolved local var: uint8_t i_tag@[???]
                       Unresolved local var: uint8_t i_length@[???] */
        bVar1 = pbVar6[1];
        if ((int)(bVar1 + 1) < (int)pbVar8 - (int)pbVar6) {
          dvbpsi_nit_ts_descriptor_add(p_ts,*pbVar6,bVar1,p_data);
        }
        pbVar6 = pbVar6 + bVar1 + 2;
        p_data = pbVar6 + 2;
      }
    }
    p_section = p_section->p_next;
  } while( true );
}



/* dvbpsi_nit_sections_gather @ 0041d4d4
 * Role (upstream libdvbpsi 1.x): Demux gather callback (pf_gather) for NIT
 *   sections. Accepts table_id 0x40 (actual) or 0x41 (other), verifies
 *   section->i_extension == p_nit_decoder->i_network_id, checks discontinuity /
 *   version / last_section_number (inlined dvbpsi_CheckNIT), buffers sections
 *   (inlined dvbpsi_AddSectionNIT), decodes when complete, and invokes
 *   pf_nit_callback.
 * Callers: registered in dvbpsi_nit_attach (dead code in av_streamer). */
void dvbpsi_nit_sections_gather
               (dvbpsi_t *p_dvbpsi,dvbpsi_decoder_t *p_private_decoder,
               dvbpsi_psi_section_t *p_section)

{
  short sVar1;                       // packed {i_version, b_current_next} for comparison
  _Bool _Var2;                       // boolean return from PSI helpers
  undefined3 extraout_var;           // MIPS ABI upper 3 bytes of _Bool return
  undefined3 extraout_var_00;        // MIPS ABI upper 3 bytes
  undefined3 extraout_var_01;        // MIPS ABI upper 3 bytes
  dvbpsi_descriptor_t *pdVar3;       // copy of current_nit.p_first_descriptor
  dvbpsi_nit_t *pdVar4;              // p_building_nit (dvbpsi_nit_t *)
  undefined4 uVar5;                  // p_nit_decoder->p_cb_data (+0x20)
  uint16_t i_extension;              // expected i_network_id (+0x38) / p_section->i_extension
  dvbpsi_psi_section_t *p_section_00; // p_private_decoder->p_sections
  int iVar6;                         // scratch: pf_nit_callback / p_building_nit
  uint8_t table_id;                  // expected table_id (0x40 or 0x41)
  uint16_t i_network_id;             // p_section->i_extension / expected i_network_id
  dvbpsi_psi_section_t *pdVar7;      // copy of current_nit bytes +0x04..+0x07
  dvbpsi_psi_section_t *pdVar8;      // copy of current_nit bytes +0x00..+0x03
  
                    /* Unresolved local var: uint8_t i_table_id@[DW_OP_reg2(v0)]
                       Unresolved local var: dvbpsi_nit_decoder_t * p_nit_decoder@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/nit.c",0x16b,"dvbpsi_nit_sections_gather");
  }
  // Accept table_id 0x40 (NIT actual) or 0x41 (NIT other)
  table_id = 0x40;
  if ((byte)(p_section->i_table_id - 0x40) < 2) {
    table_id = p_section->i_table_id;
  }
  _Var2 = dvbpsi_CheckPSISection(p_dvbpsi,p_section,table_id,"NIT decoder");
  if (CONCAT31(extraout_var,_Var2) == 0) goto LAB_0041d6b0;
  i_extension = *(uint16_t *)p_private_decoder[2].i_magic; // +0x38: i_network_id
  i_network_id = p_section->i_extension;
  if (i_network_id != i_extension) {
    dvbpsi_error(p_dvbpsi,"NIT decoder","\'network_id\' don\'t match");
    dvbpsi_DeletePSISections(p_section);
    return;
  }
  if (p_private_decoder->b_discontinuity == false) {
    if (p_private_decoder[1].i_need != 0) { // +0x34: p_building_nit
                    /* Unresolved local var: _Bool b_reinit@[???]
                       Unresolved local var: char[16] __PRETTY_FUNCTION__@[???] */
      if (*(uint8_t *)(p_private_decoder[1].i_need + 6) == p_section->i_version) {
        if (p_private_decoder->i_last_section_number == p_section->i_last_number) goto LAB_0041d5a0;
        dvbpsi_error(p_dvbpsi,"NIT decoder",
                     "\'last_section_number\' differs whereas no discontinuity has occured");
      }
      else {
        dvbpsi_error(p_dvbpsi,"NIT decoder",
                     "\'version_number\' differs whereas no discontinuity has occured");
      }
      dvbpsi_ReInitNIT((dvbpsi_nit_decoder_t *)p_private_decoder,true);
      goto LAB_0041d594;
    }
    // Check {i_version, b_current_next} at current_nit+0x06 (+0x2a in decoder)
    if ((p_private_decoder->b_current_valid != false) &&
       (sVar1._0_1_ = p_section->i_version, sVar1._1_1_ = p_section->b_current_next,
       *(short *)((int)&p_private_decoder[1].p_sections + 2) == sVar1)) {
      dvbpsi_debug(p_dvbpsi,"NIT decoder","ignoring already decoded section %d",
                   (uint)p_section->i_number);
      goto LAB_0041d6b0;
    }
  }
  else {
    dvbpsi_ReInitNIT((dvbpsi_nit_decoder_t *)p_private_decoder,true);
    p_private_decoder->b_discontinuity = false;
LAB_0041d594:
                    /* Unresolved local var: char[21] __PRETTY_FUNCTION__@[???] */
    if (p_private_decoder[1].i_need != 0) goto LAB_0041d5a0;
    i_extension = p_section->i_extension;
    i_network_id = *(uint16_t *)p_private_decoder[2].i_magic;
  }
  pdVar4 = dvbpsi_nit_new(p_section->i_table_id,i_extension,i_network_id,p_section->i_version,
                          p_section->b_current_next);
  p_private_decoder[1].i_need = (int)pdVar4;
  if (pdVar4 == (dvbpsi_nit_t *)0x0) {
    dvbpsi_error(p_dvbpsi,"NIT decoder","failed decoding section %d",(uint)p_section->i_number);
LAB_0041d6b0:
    dvbpsi_DeletePSISections(p_section);
    return;
  }
  p_private_decoder->i_last_section_number = p_section->i_last_number;
LAB_0041d5a0:
  _Var2 = dvbpsi_decoder_psi_section_add(p_private_decoder,p_section);
  if (CONCAT31(extraout_var_00,_Var2) != 0) {
    dvbpsi_debug(p_dvbpsi,"NIT decoder","overwrite section number %d",(uint)p_section->i_number);
  }
  _Var2 = dvbpsi_decoder_psi_sections_completed(p_private_decoder);
  if (CONCAT31(extraout_var_01,_Var2) == 0) {
    return;
  }
  iVar6._0_1_ = p_private_decoder[1].i_magic[0];
  iVar6._1_1_ = p_private_decoder[1].i_magic[1];
  iVar6._2_1_ = p_private_decoder[1].i_magic[2];
  iVar6._3_1_ = p_private_decoder[1].b_complete_header;
  if (iVar6 == 0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_nit_decoder->pf_nit_callback","tables/nit.c",0x1ae,"dvbpsi_nit_sections_gather"
                 );
  }
  // Copy *p_building_nit into current_nit (+0x24..+0x33)
  pdVar4 = (dvbpsi_nit_t *)p_private_decoder[1].i_need;
  p_section_00 = p_private_decoder->p_sections;
  pdVar8 = *(dvbpsi_psi_section_t **)pdVar4;
  pdVar7 = *(dvbpsi_psi_section_t **)&pdVar4->i_network_id;
  pdVar3 = pdVar4->p_first_descriptor;
  p_private_decoder[1].i_section_max_size = (int)pdVar4->p_first_ts;
  p_private_decoder[1].p_current_section = pdVar8;
  p_private_decoder[1].p_sections = pdVar7;
  p_private_decoder[1].pf_gather = (dvbpsi_callback_gather_t)pdVar3;
  p_private_decoder->b_current_valid = true;
  dvbpsi_nit_sections_decode(pdVar4,p_section_00);
  dvbpsi_DeletePSISections(p_private_decoder->p_sections);
  uVar5._0_1_ = p_private_decoder[1].b_discontinuity;
  uVar5._1_1_ = p_private_decoder[1].b_current_valid;
  uVar5._2_1_ = p_private_decoder[1].i_continuity_counter;
  uVar5._3_1_ = p_private_decoder[1].i_last_section_number;
  iVar6 = p_private_decoder[1].i_need;
  p_private_decoder->p_sections = (dvbpsi_psi_section_t *)0x0;
  (**(code **)(p_private_decoder + 1))(uVar5,iVar6); // pf_nit_callback(p_cb_data, p_building_nit)
  dvbpsi_ReInitNIT((dvbpsi_nit_decoder_t *)p_private_decoder,false);
  return;
}



/* dvbpsi_nit_sections_generate @ 0041d808
 * Role (upstream libdvbpsi 1.x): Serializes a dvbpsi_nit_t (network-level
 *   descriptors + transport_stream loop + TS descriptors) into a linked list
 *   of 1024-byte (0x400) PSI sections and finalizes CRC32 via
 *   dvbpsi_BuildPSISection().
 * Parameters: p_dvbpsi, p_nit, i_table_id (0x40 or 0x41).
 * Returns: head of linked dvbpsi_psi_section_t * list.
 * Note: sets b_private_indicator = false (unlike SDT/EIT which set true), and
 *   writes p_nit->i_network_id into section i_extension.
 * Callers: genNIT() in 01_libxcode4encoder.c.c (active path in av_streamer). */
dvbpsi_psi_section_t *
dvbpsi_nit_sections_generate(dvbpsi_t *p_dvbpsi,dvbpsi_nit_t *p_nit,uint8_t i_table_id)

{
  uint8_t uVar1;                     // previous section i_number
  _Bool _Var2;                       // scratch p_nit->b_current_next
  byte bVar3;                        // scratch descriptor length
  uint16_t uVar4;                    // scratch p_nit->i_network_id
  dvbpsi_psi_section_t *pdVar5;      // head of section list (p_result)
  uint8_t *puVar6;                   // scratch: p_data / p_payload_start / TS entry start
  dvbpsi_psi_section_t *pdVar7;      // current section being populated (p_current)
  uint8_t *puVar8;                   // write cursor / p_transport_stream_loop_length
  uint uVar9;                        // accumulated byte size / loop length
  uint8_t *puVar10;                  // projected end offset for overflow check (< 0x3fb)
  uint8_t *puVar11;                  // write cursor inside TS descriptor loop
  int iVar12;                        // current byte count in section (p_payload_end - p_data)
  dvbpsi_descriptor_t *pdVar13;      // descriptor list cursor (p_descriptor)
  dvbpsi_psi_section_t *pdVar14;     // previous section pointer (p_prev) / finalization cursor
  dvbpsi_nit_ts_s *pdVar15;          // TS list cursor (p_ts)
  
                    /* Unresolved local var: dvbpsi_psi_section_t * p_result@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_current@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_prev@[???]
                       Unresolved local var: dvbpsi_descriptor_t * p_descriptor@[???]
                       Unresolved local var: dvbpsi_nit_ts_t * p_ts@[???]
                       Unresolved local var: uint16_t i_network_descriptors_length@[???]
                       Unresolved local var: uint16_t i_transport_stream_loop_length@[???]
                       Unresolved local var: uint8_t * p_transport_stream_loop_length@[???] */
  pdVar5 = dvbpsi_NewPSISection(0x400);
  pdVar13 = p_nit->p_first_descriptor;
  pdVar15 = p_nit->p_first_ts;
  pdVar5->i_table_id = i_table_id;
  uVar4 = p_nit->i_network_id;
  pdVar5->b_syntax_indicator = true;
  pdVar5->b_private_indicator = false;
  pdVar5->i_length = 0xd;            // 5B PSI ext + 2B net_desc_len + 2B ts_loop_len + 4B CRC = 13
  pdVar5->i_extension = uVar4;
  puVar6 = pdVar5->p_data;
  pdVar5->i_version = p_nit->i_version;
  puVar8 = pdVar5->p_payload_end + 10; // 8B PSI hdr + 2B network_descriptors_length = 10
  pdVar5->b_current_next = p_nit->b_current_next;
  pdVar5->i_number = 0;
  pdVar5->p_payload_end = puVar8;
  pdVar5->p_payload_start = puVar6 + 8;
  pdVar7 = pdVar5;
  pdVar14 = pdVar5;
  // Phase 1: serialize network-level descriptors
  if (pdVar13 != (dvbpsi_descriptor_t *)0x0) {
    while( true ) {
      pdVar7 = pdVar14;
      if (0x3fa < (int)(puVar8 + ((uint)pdVar13->i_length - (int)puVar6))) {
        puVar11 = pdVar14->p_payload_start;
        puVar6[8] = (byte)(((uint)(puVar8 + (-2 - (int)puVar11)) & 0xffff) >> 8) | 0xf0;
        pdVar14->p_data[9] = (uint8_t)((uint)(puVar8 + (-2 - (int)puVar11)) & 0xffff);
        *pdVar14->p_payload_end = 0; // empty transport_stream_loop_length in overflowed section
        pdVar14->p_payload_end[1] = 0;
        pdVar14->p_payload_end = pdVar14->p_payload_end + 2;
        pdVar7 = dvbpsi_NewPSISection(0x400);
        uVar1 = pdVar14->i_number;
        pdVar7->i_table_id = i_table_id;
        uVar4 = p_nit->i_network_id;
        pdVar14->p_next = pdVar7;
        pdVar7->b_syntax_indicator = true;
        pdVar7->b_private_indicator = false;
        pdVar7->i_extension = uVar4;
        pdVar7->i_length = 0xd;
        pdVar7->i_version = p_nit->i_version;
        _Var2 = p_nit->b_current_next;
        puVar8 = pdVar7->p_payload_end + 10;
        pdVar7->p_payload_end = puVar8;
        pdVar7->i_number = uVar1 + 1;
        pdVar7->b_current_next = _Var2;
        pdVar7->p_payload_start = pdVar7->p_data + 8;
      }
      *puVar8 = pdVar13->i_tag;
      pdVar7->p_payload_end[1] = pdVar13->i_length;
      memcpy(pdVar7->p_payload_end + 2,pdVar13->p_data,(size_t)pdVar13->i_length);
      puVar8 = pdVar7->p_payload_end + pdVar13->i_length + 2;
      pdVar7->p_payload_end = puVar8;
      puVar6 = &pdVar13->i_length;
      pdVar13 = pdVar13->p_next;
      pdVar7->i_length = pdVar7->i_length + 2 + (ushort)*puVar6;
      if (pdVar13 == (dvbpsi_descriptor_s *)0x0) break;
      puVar6 = pdVar7->p_data;
      pdVar14 = pdVar7;
    }
  }
  // Backfill network_descriptors_length (12 bits, reserved bits [15:12] = 0xf)
  puVar6 = pdVar7->p_payload_start;
  pdVar7->p_data[8] = (byte)(((uint)(puVar8 + (-2 - (int)puVar6)) & 0xffff) >> 8) | 0xf0;
  pdVar7->p_data[9] = (uint8_t)((uint)(puVar8 + (-2 - (int)puVar6)) & 0xffff);
  puVar8 = pdVar7->p_payload_end;    // p_transport_stream_loop_length (2 bytes)
  puVar6 = puVar8 + 2;
  pdVar7->p_payload_end = puVar6;
  pdVar14 = pdVar7;
  // Phase 2: serialize transport stream loop
  if (pdVar15 == (dvbpsi_nit_ts_t *)0x0) {
LAB_0041dbf4:
    uVar9 = (uint)(pdVar7->p_payload_end + (-2 - (int)puVar8)) & 0xffff;
    *puVar8 = (byte)(uVar9 >> 8) | 0xf0;
    puVar8[1] = (uint8_t)uVar9;
    pdVar14 = pdVar5;
    do {
      pdVar14->i_last_number = pdVar7->i_number;
      dvbpsi_BuildPSISection(p_dvbpsi,pdVar14);
      pdVar14 = pdVar14->p_next;
    } while (pdVar14 != (dvbpsi_psi_section_s *)0x0);
    return pdVar5;
  }
  do {
    pdVar13 = pdVar15->p_first_descriptor;
    pdVar7 = pdVar14;
    if (pdVar13 != (dvbpsi_descriptor_t *)0x0) {
      iVar12 = (int)puVar6 - (int)pdVar14->p_data;
      uVar9 = 5;
      if (iVar12 < 0x3f8) {
        bVar3 = pdVar13->i_length;
        while( true ) {
          pdVar13 = pdVar13->p_next;
          uVar9 = uVar9 + bVar3 + 2 & 0xffff;
          if (pdVar13 == (dvbpsi_descriptor_s *)0x0) break;
          if (0x3fc < (int)(uVar9 + iVar12)) {
            if ((iVar12 != 0xc) && (uVar9 < 0x3f1)) goto LAB_0041dc84;
            break;
          }
          bVar3 = pdVar13->i_length;
        }
      }
      else {
LAB_0041dc84:
        // Finalize transport_stream_loop_length on current section and open a new section
        *puVar8 = (byte)(((uint)(puVar6 + (-2 - (int)puVar8)) & 0xffff) >> 8) | 0xf0;
        puVar8[1] = (uint8_t)((uint)(puVar6 + (-2 - (int)puVar8)) & 0xffff);
        dvbpsi_debug(p_dvbpsi,"NIT generator","create a new section to carry more TS descriptors");
        pdVar7 = dvbpsi_NewPSISection(0x400);
        pdVar7->i_table_id = i_table_id;
        uVar4 = p_nit->i_network_id;
        pdVar14->p_next = pdVar7;
        pdVar7->b_syntax_indicator = true;
        pdVar7->b_private_indicator = false;
        pdVar7->i_length = 0xd;
        pdVar7->i_extension = uVar4;
        uVar1 = pdVar14->i_number;
        pdVar7->i_version = p_nit->i_version;
        _Var2 = p_nit->b_current_next;
        pdVar7->p_payload_end = pdVar7->p_payload_end + 10;
        pdVar7->b_current_next = _Var2;
        pdVar7->i_number = uVar1 + 1;
        pdVar7->p_payload_start = pdVar7->p_data + 8;
        pdVar7->p_data[8] = 0xf0;    // network_descriptors_length = 0 in continuation section
        pdVar7->p_data[9] = 0;
        puVar8 = pdVar7->p_payload_end;
        puVar6 = puVar8 + 2;
        pdVar7->p_payload_end = puVar6;
      }
    }
    *puVar6 = (uint8_t)(pdVar15->i_ts_id >> 8);
    puVar6[1] = (uint8_t)pdVar15->i_ts_id;
    puVar6[2] = (uint8_t)(pdVar15->i_orig_network_id >> 8);
    puVar6[3] = (uint8_t)pdVar15->i_orig_network_id;
    pdVar13 = pdVar15->p_first_descriptor;
    puVar11 = pdVar7->p_payload_end + 6;
    pdVar7->p_payload_end = puVar11;
    pdVar7->i_length = pdVar7->i_length + 6;
    if (pdVar13 == (dvbpsi_descriptor_t *)0x0) {
LAB_0041db7c:
      puVar6[4] = (byte)(((uint)(puVar11 + (-6 - (int)puVar6)) & 0xffff) >> 8) | 0xf0;
      puVar6[5] = (uint8_t)((uint)(puVar11 + (-6 - (int)puVar6)) & 0xffff);
      pdVar15 = pdVar15->p_next;
    }
    else {
      puVar10 = puVar11 + ((uint)pdVar13->i_length - (int)pdVar7->p_data);
      while ((int)puVar10 < 0x3fb) { // ensure room for descriptor + 4B CRC within 1024B
        *puVar11 = pdVar13->i_tag;
        pdVar7->p_payload_end[1] = pdVar13->i_length;
        memcpy(pdVar7->p_payload_end + 2,pdVar13->p_data,(size_t)pdVar13->i_length);
        puVar11 = pdVar7->p_payload_end + pdVar13->i_length + 2;
        pdVar7->p_payload_end = puVar11;
        puVar10 = &pdVar13->i_length;
        pdVar13 = pdVar13->p_next;
        pdVar7->i_length = pdVar7->i_length + 2 + (ushort)*puVar10;
        if (pdVar13 == (dvbpsi_descriptor_s *)0x0) goto LAB_0041db7c;
        puVar10 = puVar11 + ((uint)pdVar13->i_length - (int)pdVar7->p_data);
      }
      dvbpsi_error(p_dvbpsi,"NIT generator","unable to carry all the TS descriptors");
      uVar9 = (uint)(pdVar7->p_payload_end + (-6 - (int)puVar6)) & 0xffff;
      puVar6[4] = (byte)(uVar9 >> 8) | 0xf0;
      puVar6[5] = (uint8_t)uVar9;
      pdVar15 = pdVar15->p_next;
    }
    if (pdVar15 == (dvbpsi_nit_ts_s *)0x0) goto LAB_0041dbf4;
    puVar6 = pdVar7->p_payload_end;
    pdVar14 = pdVar7;
  } while( true );
}
