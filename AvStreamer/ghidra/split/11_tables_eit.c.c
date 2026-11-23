// ==== NewEITSection @ 0041bbe0 ====

dvbpsi_psi_section_t * NewEITSection(dvbpsi_eit_t *p_eit,int i_table_id,int i_section_number)

{
  _Bool _Var1;
  uint16_t uVar2;
  dvbpsi_psi_section_t *pdVar3;
  
                    /* Unresolved local var: dvbpsi_psi_section_t * p_result@[DW_OP_reg2(v0)] */
  pdVar3 = dvbpsi_NewPSISection(0xffe);
  pdVar3->i_table_id = (uint8_t)i_table_id;
  uVar2 = p_eit->i_extension;
  pdVar3->b_syntax_indicator = true;
  pdVar3->b_private_indicator = true;
  pdVar3->i_length = 0xf;
  pdVar3->i_extension = uVar2;
  pdVar3->i_version = p_eit->i_version;
  _Var1 = p_eit->b_current_next;
  pdVar3->i_number = (uint8_t)i_section_number;
  uVar2 = p_eit->i_ts_id;
  pdVar3->b_current_next = _Var1;
  pdVar3->p_payload_end = pdVar3->p_payload_end + 0xe;
  pdVar3->p_payload_start = pdVar3->p_data + 8;
  pdVar3->p_data[8] = (uint8_t)(uVar2 >> 8);
  pdVar3->p_data[9] = (uint8_t)p_eit->i_ts_id;
  pdVar3->p_data[10] = (uint8_t)(p_eit->i_network_id >> 8);
  pdVar3->p_data[0xb] = (uint8_t)p_eit->i_network_id;
  pdVar3->p_data[0xd] = p_eit->i_last_table_id;
  return pdVar3;
}



// ==== dvbpsi_eit_attach @ 0041bcb4 ====

_Bool dvbpsi_eit_attach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension,
                       dvbpsi_eit_callback pf_callback,void *p_cb_data)

{
  dvbpsi_demux_subdec_t *pdVar1;
  dvbpsi_decoder_t *p_decoder;
  dvbpsi_demux_t *p_demux;
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_eit_decoder_t * p_eit_decoder@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/eit.c",0x3c,"dvbpsi_eit_attach");
  }
  p_demux = (dvbpsi_demux_t *)p_dvbpsi->p_decoder;
  if (p_demux == (dvbpsi_demux_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/eit.c",0x3d,"dvbpsi_eit_attach");
  }
  pdVar1 = dvbpsi_demuxGetSubDec(p_demux,i_table_id,i_extension);
  if (pdVar1 == (dvbpsi_demux_subdec_t *)0x0) {
    p_decoder = dvbpsi_decoder_new((dvbpsi_callback_gather_t)0x0,0,true,0x3c);
    if (p_decoder != (dvbpsi_decoder_t *)0x0) {
      pdVar1 = dvbpsi_NewDemuxSubDecoder
                         (i_table_id,i_extension,dvbpsi_eit_detach,dvbpsi_eit_sections_gather,
                          p_decoder);
      if (pdVar1 != (dvbpsi_demux_subdec_t *)0x0) {
        dvbpsi_AttachDemuxSubDecoder(p_demux,pdVar1);
        *(dvbpsi_eit_callback *)(p_decoder + 1) = pf_callback;
        *(void **)&p_decoder[1].b_discontinuity = p_cb_data;
        p_decoder[1].i_need = 0;
        return true;
      }
      dvbpsi_decoder_delete(p_decoder);
    }
  }
  else {
    dvbpsi_error(p_dvbpsi,"EIT decoder",
                 "Already a decoder for (table_id == 0x%02x,extension == 0x%02x)",i_table_id,
                 i_extension);
  }
  return false;
}



// ==== dvbpsi_eit_init @ 0041be64 ====

void dvbpsi_eit_init(dvbpsi_eit_t *p_eit,uint8_t i_table_id,uint16_t i_extension,uint8_t i_version,
                    _Bool b_current_next,uint16_t i_ts_id,uint16_t i_network_id,
                    uint8_t i_segment_last_section_number,uint8_t i_last_table_id)

{
  p_eit->i_table_id = i_table_id;
  p_eit->i_extension = i_extension;
  p_eit->i_version = i_version;
  p_eit->b_current_next = b_current_next;
  p_eit->i_ts_id = i_ts_id;
  p_eit->i_network_id = i_network_id;
  p_eit->i_segment_last_section_number = i_segment_last_section_number;
  p_eit->i_last_table_id = i_last_table_id;
  p_eit->p_first_event = (dvbpsi_eit_event_t *)0x0;
  return;
}



// ==== dvbpsi_eit_new @ 0041bea0 ====

dvbpsi_eit_t *
dvbpsi_eit_new(uint8_t i_table_id,uint16_t i_extension,uint8_t i_version,_Bool b_current_next,
              uint16_t i_ts_id,uint16_t i_network_id,uint8_t i_segment_last_section_number,
              uint8_t i_last_table_id)

{
  dvbpsi_eit_t *pdVar1;
  
                    /* Unresolved local var: dvbpsi_eit_t * p_eit@[DW_OP_reg2(v0)] */
  pdVar1 = malloc(0x10);
  if (pdVar1 != (dvbpsi_eit_t *)0x0) {
    pdVar1->i_table_id = i_table_id;
    pdVar1->i_extension = i_extension;
    pdVar1->i_version = i_version;
    pdVar1->b_current_next = b_current_next;
    pdVar1->i_ts_id = i_ts_id;
    pdVar1->i_network_id = i_network_id;
    pdVar1->i_segment_last_section_number = i_segment_last_section_number;
    pdVar1->i_last_table_id = i_last_table_id;
    pdVar1->p_first_event = (dvbpsi_eit_event_t *)0x0;
  }
  return pdVar1;
}



// ==== dvbpsi_eit_empty @ 0041bf58 ====

void dvbpsi_eit_empty(dvbpsi_eit_t *p_eit)

{
  dvbpsi_eit_event_t *__ptr;
  dvbpsi_eit_event_s *pdVar1;
  
                    /* Unresolved local var: dvbpsi_eit_event_t * p_event@[???] */
  __ptr = p_eit->p_first_event;
  while (__ptr != (dvbpsi_eit_event_t *)0x0) {
                    /* Unresolved local var: dvbpsi_eit_event_t * p_tmp@[???] */
    pdVar1 = __ptr->p_next;
    dvbpsi_DeleteDescriptors(__ptr->p_first_descriptor);
    free(__ptr);
    __ptr = pdVar1;
  }
  p_eit->p_first_event = (dvbpsi_eit_event_t *)0x0;
  return;
}



// ==== dvbpsi_eit_delete @ 0041bfcc ====

void dvbpsi_eit_delete(dvbpsi_eit_t *p_eit)

{
  if (p_eit != (dvbpsi_eit_t *)0x0) {
    dvbpsi_eit_empty(p_eit);
  }
  free(p_eit);
  return;
}



// ==== dvbpsi_eit_detach @ 0041c010 ====

void dvbpsi_eit_detach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension)

{
  dvbpsi_demux_subdec_t *p_subdec;
  dvbpsi_eit_t *p_eit;
  dvbpsi_demux_t *p_demux;
  dvbpsi_decoder_t *pdVar1;
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???]
                       Unresolved local var: dvbpsi_eit_decoder_t * p_eit_decoder@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/eit.c",0x6d,"dvbpsi_eit_detach");
  }
  p_demux = (dvbpsi_demux_t *)p_dvbpsi->p_decoder;
  if (p_demux == (dvbpsi_demux_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/eit.c",0x6e,"dvbpsi_eit_detach");
  }
  p_subdec = dvbpsi_demuxGetSubDec(p_demux,i_table_id,i_extension);
  if (p_subdec == (dvbpsi_demux_subdec_t *)0x0) {
    dvbpsi_error(p_dvbpsi,"EIT Decoder",
                 "No such EIT decoder (table_id == 0x%02x,extension == 0x%02x)",i_table_id,
                 i_extension);
    return;
  }
  pdVar1 = p_subdec->p_decoder;
  p_eit = (dvbpsi_eit_t *)pdVar1[1].i_need;
  if (p_eit != (dvbpsi_eit_t *)0x0) {
    dvbpsi_eit_delete(p_eit);
  }
  pdVar1[1].i_need = 0;
  dvbpsi_DetachDemuxSubDecoder(p_demux,p_subdec);
  dvbpsi_DeleteDemuxSubDecoder(p_subdec);
  return;
}



// ==== dvbpsi_ReInitEIT @ 0041c158 ====

void dvbpsi_ReInitEIT(dvbpsi_eit_decoder_t *p_decoder,_Bool b_force)

{
  if (p_decoder == (dvbpsi_eit_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_decoder","tables/eit.c",0x119,"dvbpsi_ReInitEIT");
  }
  dvbpsi_decoder_reset((dvbpsi_decoder_t *)p_decoder,true);
  if (p_decoder->p_building_eit != (dvbpsi_eit_t *)0x0) {
    dvbpsi_eit_delete(p_decoder->p_building_eit);
  }
  p_decoder->p_building_eit = (dvbpsi_eit_t *)0x0;
  return;
}



// ==== dvbpsi_eit_event_add @ 0041c1d0 ====

dvbpsi_eit_event_t *
dvbpsi_eit_event_add
          (dvbpsi_eit_t *p_eit,uint16_t i_event_id,uint64_t i_start_time,uint32_t i_duration,
          uint8_t i_running_status,_Bool b_free_ca,uint16_t i_event_descriptor_length)

{
  dvbpsi_eit_event_t *pdVar1;
  dvbpsi_eit_event_t *pdVar2;
  uint local_20;
  
                    /* Unresolved local var: dvbpsi_eit_event_t * p_event@[DW_OP_reg2(v0)] */
  pdVar1 = calloc(1,0x28);
  local_20 = (uint)i_start_time;
  if (pdVar1 != (dvbpsi_eit_event_t *)0x0) {
    pdVar2 = p_eit->p_first_event;
    pdVar1->i_event_id = i_event_id;
    pdVar1->i_start_time = i_start_time;
    pdVar1->i_duration = i_duration;
    pdVar1->i_running_status = i_running_status;
    pdVar1->b_free_ca = b_free_ca;
    pdVar1->b_nvod = (local_20 & 0xfffff000) == 0xfffff000 && i_running_status == 0;
    pdVar1->p_next = (dvbpsi_eit_event_s *)0x0;
    pdVar1->i_descriptors_length = i_event_descriptor_length;
    pdVar1->p_first_descriptor = (dvbpsi_descriptor_t *)0x0;
    if (pdVar2 == (dvbpsi_eit_event_t *)0x0) {
      p_eit->p_first_event = pdVar1;
    }
    else {
      while (pdVar2->p_next != (dvbpsi_eit_event_s *)0x0) {
                    /* Unresolved local var: dvbpsi_eit_event_t * p_last_event@[???] */
        pdVar2 = pdVar2->p_next;
      }
      pdVar2->p_next = pdVar1;
    }
  }
  return pdVar1;
}



// ==== dvbpsi_eit_nvod_event_add @ 0041c2c0 ====

dvbpsi_eit_event_t *
dvbpsi_eit_nvod_event_add
          (dvbpsi_eit_t *p_eit,uint16_t i_event_id,uint32_t i_duration,_Bool b_free_ca,
          uint16_t i_event_descriptor_length)

{
  dvbpsi_eit_event_t *pdVar1;
  
  pdVar1 = dvbpsi_eit_event_add
                     (p_eit,i_event_id,0xfffff000,i_duration,0,b_free_ca,i_event_descriptor_length);
  return pdVar1;
}



// ==== dvbpsi_eit_event_descriptor_add @ 0041c2fc ====

dvbpsi_descriptor_t *
dvbpsi_eit_event_descriptor_add
          (dvbpsi_eit_event_t *p_event,uint8_t i_tag,uint8_t i_length,uint8_t *p_data)

{
  dvbpsi_descriptor_t *p_descriptor;
  dvbpsi_descriptor_t *pdVar1;
  
                    /* Unresolved local var: dvbpsi_descriptor_t * p_descriptor@[???] */
  p_descriptor = dvbpsi_NewDescriptor(i_tag,i_length,p_data);
  if (p_descriptor != (dvbpsi_descriptor_t *)0x0) {
    pdVar1 = dvbpsi_AddDescriptor(p_event->p_first_descriptor,p_descriptor);
    p_event->p_first_descriptor = pdVar1;
    if (pdVar1 == (dvbpsi_descriptor_t *)0x0) {
                    /* WARNING: Subroutine does not return */
      __assert_fail("p_event->p_first_descriptor","tables/eit.c",0x10f,
                    "dvbpsi_eit_event_descriptor_add");
    }
  }
  return p_descriptor;
}



// ==== dvbpsi_eit_sections_decode @ 0041c394 ====

void dvbpsi_eit_sections_decode(dvbpsi_eit_t *p_eit,dvbpsi_psi_section_t *p_section)

{
  byte bVar1;
  dvbpsi_eit_event_t *p_event;
  uint8_t *puVar2;
  uint8_t *puVar3;
  uint uVar4;
  uint8_t *puVar5;
  
  do {
    if (p_section == (dvbpsi_psi_section_t *)0x0) {
      return;
    }
    puVar5 = p_section->p_payload_end;
    puVar3 = p_section->p_payload_start + 6;
    while (puVar3 < puVar5) {
      while( true ) {
                    /* Unresolved local var: uint16_t i_event_id@[???]
                       Unresolved local var: uint64_t i_start_time@[???]
                       Unresolved local var: uint32_t i_duration@[???]
                       Unresolved local var: uint8_t i_running_status@[???]
                       Unresolved local var: _Bool b_free_ca@[???]
                       Unresolved local var: uint16_t i_ev_length@[???]
                       Unresolved local var: dvbpsi_eit_event_t * p_event@[???]
                       Unresolved local var: uint8_t * p_ev_end@[???] */
        bVar1 = puVar3[10];
        uVar4 = (bVar1 & 0xf) << 8 | (uint)puVar3[0xb];
        p_event = dvbpsi_eit_event_add
                            (p_eit,CONCAT11(*puVar3,puVar3[1]),
                             (ulonglong)
                             CONCAT14(puVar3[2],
                                      (uint)puVar3[3] << 0x18 | (uint)puVar3[6] |
                                      (uint)puVar3[4] << 0x10 | (uint)puVar3[5] << 8),
                             (uint)puVar3[7] << 0x10 | (uint)puVar3[8] << 8 | (uint)puVar3[9],
                             bVar1 >> 5,(bVar1 & 8) == 0,(uint16_t)uVar4);
        if (p_event == (dvbpsi_eit_event_t *)0x0) goto LAB_0041c4cc;
        puVar3 = puVar3 + 0xc;
        puVar2 = puVar3 + uVar4;
        if (p_section->p_payload_end <= puVar3 + uVar4) {
          puVar2 = p_section->p_payload_end;
        }
        if (puVar2 <= puVar3) break;
        do {
                    /* Unresolved local var: uint8_t i_tag@[???]
                       Unresolved local var: uint8_t i_length@[???] */
          bVar1 = puVar3[1];
          if ((int)(bVar1 + 1) < (int)puVar2 - (int)puVar3) {
            dvbpsi_eit_event_descriptor_add(p_event,*puVar3,bVar1,puVar3 + 2);
          }
          puVar3 = puVar3 + bVar1 + 2;
        } while (puVar3 < puVar2);
        if (puVar5 <= puVar3) goto LAB_0041c4cc;
      }
    }
LAB_0041c4cc:
    p_section = p_section->p_next;
  } while( true );
}



// ==== dvbpsi_eit_sections_gather @ 0041c518 ====

void dvbpsi_eit_sections_gather
               (dvbpsi_t *p_dvbpsi,dvbpsi_decoder_t *p_private_decoder,
               dvbpsi_psi_section_t *p_section)

{
  byte bVar1;
  short sVar2;
  _Bool _Var3;
  undefined3 extraout_var;
  int iVar4;
  undefined3 extraout_var_00;
  uint uVar5;
  dvbpsi_psi_section_t *pdVar6;
  uint8_t *puVar7;
  dvbpsi_callback_gather_t p_Var8;
  dvbpsi_eit_t *pdVar9;
  undefined4 uVar10;
  undefined1 b_force;
  dvbpsi_psi_section_t *pdVar11;
  uint8_t table_id;
  dvbpsi_psi_section_t *pdVar12;
  dvbpsi_decoder_t *pdVar13;
  
                    /* Unresolved local var: uint8_t i_table_id@[DW_OP_reg2(v0)]
                       Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_eit_decoder_t * p_eit_decoder@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/eit.c",0x1ab,"dvbpsi_eit_sections_gather");
  }
  if (p_dvbpsi->p_decoder == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/eit.c",0x1ac,"dvbpsi_eit_sections_gather");
  }
  table_id = 0x4e;
  if ((byte)(p_section->i_table_id + 0xb2) < 0x22) {
    table_id = p_section->i_table_id;
  }
  pdVar11 = p_section;
  _Var3 = dvbpsi_CheckPSISection(p_dvbpsi,p_section,table_id,"EIT decoder");
  if (CONCAT31(extraout_var,_Var3) == 0) goto LAB_0041c7e8;
  pdVar13 = p_dvbpsi->p_decoder;
  if (pdVar13->b_discontinuity == false) {
    iVar4 = p_private_decoder[1].i_need;
    if (iVar4 == 0) {
      if ((p_private_decoder->b_current_valid != false) &&
         (sVar2._0_1_ = p_section->i_version, sVar2._1_1_ = p_section->b_current_next,
         *(short *)&p_private_decoder[1].p_sections == sVar2)) {
        dvbpsi_debug(p_dvbpsi,"EIT decoder","ignoring already decoded section %d",
                     (uint)p_section->i_number);
        goto LAB_0041c7e8;
      }
    }
    else {
                    /* Unresolved local var: _Bool b_reinit@[???]
                       Unresolved local var: char[16] __PRETTY_FUNCTION__@[???] */
      if (*(uint16_t *)(iVar4 + 2) == p_section->i_extension) {
        if (*(uint8_t *)(iVar4 + 4) == p_section->i_version) {
          if (p_private_decoder->i_last_section_number == p_section->i_last_number)
          goto LAB_0041c5d8;
          b_force = 0x20;
          dvbpsi_error(p_dvbpsi,"EIT decoder",
                       "\'last_section_number\' differs whereas no discontinuity has occured");
        }
        else {
          b_force = 0x20;
          dvbpsi_error(p_dvbpsi,"EIT decoder",
                       "\'version_number\' differs whereas no discontinuity has occurred");
        }
      }
      else {
        b_force = 0x20;
        dvbpsi_error(p_dvbpsi,"EIT decoder",
                     "\'service_id\' differs whereas no TS discontinuity has occurred");
      }
      dvbpsi_ReInitEIT((dvbpsi_eit_decoder_t *)p_private_decoder,(_Bool)b_force);
    }
  }
  else {
    dvbpsi_ReInitEIT((dvbpsi_eit_decoder_t *)p_private_decoder,SUB41(pdVar11,0));
    p_private_decoder->b_discontinuity = false;
    pdVar13->b_discontinuity = false;
  }
LAB_0041c5d8:
  if (p_private_decoder == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_eit_decoder","tables/eit.c",0x181,"dvbpsi_AddSectionEIT");
  }
  if (p_private_decoder[1].i_need == 0) {
    puVar7 = p_section->p_payload_start;
    pdVar9 = dvbpsi_eit_new(p_section->i_table_id,p_section->i_extension,p_section->i_version,
                            p_section->b_current_next,CONCAT11(*puVar7,puVar7[1]),
                            CONCAT11(puVar7[2],puVar7[3]),puVar7[4],puVar7[5]);
    p_private_decoder[1].i_need = (int)pdVar9;
    p_private_decoder->i_last_section_number = p_section->i_last_number;
    p_private_decoder[2].i_magic[0] = p_section->i_number;
    if (pdVar9 == (dvbpsi_eit_t *)0x0) {
      dvbpsi_error(p_dvbpsi,"EIT decoder","failed decoding section %d",(uint)p_section->i_number);
LAB_0041c7e8:
      dvbpsi_DeletePSISections(p_section);
      return;
    }
    p_private_decoder->i_last_section_number = p_section->i_last_number;
  }
  _Var3 = dvbpsi_decoder_psi_section_add(p_private_decoder,p_section);
  if (CONCAT31(extraout_var_00,_Var3) != 0) {
    dvbpsi_debug(p_dvbpsi,"EIT decoder","overwrite section number %d",(uint)p_section->i_number);
  }
                    /* Unresolved local var: _Bool b_complete@[???]
                       Unresolved local var: char[21] __PRETTY_FUNCTION__@[???] */
  uVar5 = (uint)p_private_decoder[2].i_magic[0];
  if (uVar5 == 0) {
    if (p_section->i_number != p_private_decoder->i_last_section_number) {
      return;
    }
  }
  else if ((uVar5 != p_section->i_number) && ((uint)p_section->i_number != uVar5 - 1)) {
    return;
  }
                    /* Unresolved local var: dvbpsi_psi_section_t * p@[???] */
  pdVar11 = p_private_decoder->p_sections;
  if (pdVar11 != (dvbpsi_psi_section_t *)0x0) {
    bVar1 = pdVar11->i_number;
    pdVar6 = pdVar11;
    while( true ) {
      while( true ) {
        if (bVar1 == p_private_decoder->i_last_section_number) {
          iVar4._0_1_ = p_private_decoder[1].i_magic[0];
          iVar4._1_1_ = p_private_decoder[1].i_magic[1];
          iVar4._2_1_ = p_private_decoder[1].i_magic[2];
          iVar4._3_1_ = p_private_decoder[1].b_complete_header;
          if (iVar4 == 0) {
                    /* WARNING: Subroutine does not return */
            __assert_fail("p_eit_decoder->pf_eit_callback","tables/eit.c",0x1eb,
                          "dvbpsi_eit_sections_gather");
          }
          pdVar9 = (dvbpsi_eit_t *)p_private_decoder[1].i_need;
          pdVar12 = *(dvbpsi_psi_section_t **)pdVar9;
          pdVar6 = *(dvbpsi_psi_section_t **)&pdVar9->i_version;
          p_Var8 = *(dvbpsi_callback_gather_t *)&pdVar9->i_network_id;
          p_private_decoder[1].i_section_max_size = (int)pdVar9->p_first_event;
          p_private_decoder[1].p_current_section = pdVar12;
          p_private_decoder[1].p_sections = pdVar6;
          p_private_decoder[1].pf_gather = p_Var8;
          p_private_decoder->b_current_valid = true;
          dvbpsi_eit_sections_decode(pdVar9,pdVar11);
          dvbpsi_DeletePSISections(p_private_decoder->p_sections);
          uVar10._0_1_ = p_private_decoder[1].b_discontinuity;
          uVar10._1_1_ = p_private_decoder[1].b_current_valid;
          uVar10._2_1_ = p_private_decoder[1].i_continuity_counter;
          uVar10._3_1_ = p_private_decoder[1].i_last_section_number;
          iVar4 = p_private_decoder[1].i_need;
          p_private_decoder->p_sections = (dvbpsi_psi_section_t *)0x0;
          (**(code **)(p_private_decoder + 1))(uVar10,iVar4);
          p_private_decoder[1].i_need = 0;
          return;
        }
        if (pdVar6->p_payload_start[4] != bVar1) break;
        do {
          pdVar6 = pdVar6->p_next;
          if (pdVar6 == (dvbpsi_psi_section_s *)0x0) {
            return;
          }
          bVar1 = pdVar6->i_number;
        } while (bVar1 < p_private_decoder->i_last_section_number);
      }
      pdVar6 = pdVar6->p_next;
      if (pdVar6 == (dvbpsi_psi_section_s *)0x0) break;
      bVar1 = pdVar6->i_number;
    }
  }
  return;
}



// ==== dvbpsi_eit_sections_generate @ 0041c960 ====

dvbpsi_psi_section_t *
dvbpsi_eit_sections_generate(dvbpsi_t *p_dvbpsi,dvbpsi_eit_t *p_eit,uint8_t i_table_id)

{
  dvbpsi_psi_section_s **ppdVar1;
  int iVar2;
  dvbpsi_psi_section_t *pdVar3;
  dvbpsi_descriptor_t *pdVar4;
  uint8_t *puVar5;
  dvbpsi_psi_section_t *pdVar6;
  uint uVar7;
  uint8_t *puVar8;
  dvbpsi_psi_section_t *pdVar9;
  dvbpsi_eit_event_t *pdVar10;
  uint8_t *puVar11;
  uint i_section_number;
  byte *pbVar12;
  
                    /* Unresolved local var: dvbpsi_psi_section_t * p_result@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_current@[???]
                       Unresolved local var: uint8_t i_last_section_number@[???]
                       Unresolved local var: dvbpsi_eit_event_t * p_event@[???] */
  pdVar3 = NewEITSection(p_eit,(uint)i_table_id,0);
  if (pdVar3 != (dvbpsi_psi_section_t *)0x0) {
    pdVar10 = p_eit->p_first_event;
    i_section_number = 0;
    if (pdVar10 != (dvbpsi_eit_event_t *)0x0) {
                    /* Unresolved local var: uint8_t * p_event_start@[???]
                       Unresolved local var: uint16_t i_event_length@[???]
                       Unresolved local var: dvbpsi_descriptor_t * p_descriptor@[???] */
      pdVar9 = pdVar3;
LAB_0041c9d8:
      do {
        pdVar4 = pdVar10->p_first_descriptor;
        puVar11 = pdVar9->p_payload_end;
        if (pdVar4 != (dvbpsi_descriptor_t *)0x0) {
          uVar7 = pdVar4->i_length + 0xe;
          iVar2 = uVar7 - (int)pdVar9->p_data;
          while ((int)(puVar11 + iVar2) < 0xffb) {
            pdVar4 = pdVar4->p_next;
            if (pdVar4 == (dvbpsi_descriptor_s *)0x0) goto LAB_0041ca38;
            uVar7 = uVar7 + pdVar4->i_length + 2 & 0xffff;
            iVar2 = uVar7 - (int)pdVar9->p_data;
          }
          i_section_number = i_section_number + 1 & 0xff;
          pdVar6 = NewEITSection(p_eit,(uint)i_table_id,i_section_number);
          puVar11 = pdVar6->p_payload_end;
          pdVar9->p_next = pdVar6;
          pdVar9 = pdVar6;
        }
LAB_0041ca38:
        pbVar12 = puVar11 + 10;
        *puVar11 = (uint8_t)(pdVar10->i_event_id >> 8);
        puVar11[1] = (uint8_t)pdVar10->i_event_id;
        puVar11[2] = (uint8_t)*(undefined4 *)((int)&pdVar10->i_start_time + 4);
        puVar11[3] = *(uint8_t *)((int)&pdVar10->i_start_time + 3);
        puVar11[4] = (uint8_t)*(undefined2 *)((int)&pdVar10->i_start_time + 2);
        puVar11[5] = (uint8_t)(pdVar10->i_start_time >> 8);
        puVar11[6] = (uint8_t)pdVar10->i_start_time;
        puVar11[7] = (uint8_t)*(undefined2 *)((int)&pdVar10->i_duration + 2);
        puVar11[8] = (uint8_t)(pdVar10->i_duration >> 8);
        puVar11[9] = (uint8_t)pdVar10->i_duration;
        puVar11[10] = (pdVar10->b_free_ca & 1U) << 4 | pdVar10->i_running_status << 5;
        pdVar4 = pdVar10->p_first_descriptor;
        puVar5 = pdVar9->p_payload_end + 0xc;
        pdVar9->p_payload_end = puVar5;
        pdVar9->i_length = pdVar9->i_length + 0xc;
        if (pdVar4 != (dvbpsi_descriptor_t *)0x0) {
          puVar8 = puVar5 + ((uint)pdVar4->i_length - (int)pdVar9->p_data);
          while ((int)puVar8 < 0xffb) {
            *puVar5 = pdVar4->i_tag;
            pdVar9->p_payload_end[1] = pdVar4->i_length;
            memcpy(pdVar9->p_payload_end + 2,pdVar4->p_data,(size_t)pdVar4->i_length);
            puVar5 = pdVar9->p_payload_end + pdVar4->i_length + 2;
            pdVar9->p_payload_end = puVar5;
            puVar8 = &pdVar4->i_length;
            pdVar4 = pdVar4->p_next;
            pdVar9->i_length = pdVar9->i_length + 2 + (ushort)*puVar8;
            if (pdVar4 == (dvbpsi_descriptor_s *)0x0) goto LAB_0041cb74;
            puVar8 = puVar5 + ((uint)pdVar4->i_length - (int)pdVar9->p_data);
          }
          dvbpsi_error(p_dvbpsi,"EIT generator",
                       "too many descriptors in event, unable to carry all the descriptors");
          puVar5 = pdVar9->p_payload_end;
          *pbVar12 = (byte)((uint)(puVar5 + (-0xc - (int)puVar11)) >> 8) & 0xf | *pbVar12;
          puVar11[0xb] = (uint8_t)(puVar5 + (-0xc - (int)puVar11));
          pdVar10 = pdVar10->p_next;
          if (pdVar10 == (dvbpsi_eit_event_s *)0x0) {
            pdVar9 = pdVar3;
            goto LAB_0041cba4;
          }
          goto LAB_0041c9d8;
        }
LAB_0041cb74:
        *pbVar12 = (byte)((uint)(puVar5 + (-0xc - (int)puVar11)) >> 8) & 0xf | *pbVar12;
        puVar11[0xb] = (uint8_t)(puVar5 + (-0xc - (int)puVar11));
        pdVar10 = pdVar10->p_next;
      } while (pdVar10 != (dvbpsi_eit_event_s *)0x0);
    }
    pdVar9 = pdVar3;
LAB_0041cba4:
    do {
      pdVar9->p_data[0xc] = (uint8_t)i_section_number;
      pdVar9->i_last_number = (uint8_t)i_section_number;
      dvbpsi_BuildPSISection(p_dvbpsi,pdVar9);
      ppdVar1 = &pdVar9->p_next;
      pdVar9 = *ppdVar1;
    } while (*ppdVar1 != (dvbpsi_psi_section_s *)0x0);
  }
  return pdVar3;
}



