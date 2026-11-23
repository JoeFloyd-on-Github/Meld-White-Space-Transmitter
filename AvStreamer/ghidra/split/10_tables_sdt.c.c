// ==== dvbpsi_sdt_attach @ 0041ac60 ====

_Bool dvbpsi_sdt_attach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension,
                       dvbpsi_sdt_callback pf_callback,void *p_cb_data)

{
  dvbpsi_demux_subdec_t *pdVar1;
  dvbpsi_decoder_t *p_decoder;
  dvbpsi_demux_t *p_demux;
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_sdt_decoder_t * p_sdt_decoder@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/sdt.c",0x3b,"dvbpsi_sdt_attach");
  }
  p_demux = (dvbpsi_demux_t *)p_dvbpsi->p_decoder;
  if (p_demux == (dvbpsi_demux_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/sdt.c",0x3c,"dvbpsi_sdt_attach");
  }
  pdVar1 = dvbpsi_demuxGetSubDec(p_demux,i_table_id,i_extension);
  if (pdVar1 == (dvbpsi_demux_subdec_t *)0x0) {
    p_decoder = dvbpsi_decoder_new((dvbpsi_callback_gather_t)0x0,0,true,0x34);
    if (p_decoder != (dvbpsi_decoder_t *)0x0) {
      pdVar1 = dvbpsi_NewDemuxSubDecoder
                         (i_table_id,i_extension,dvbpsi_sdt_detach,dvbpsi_sdt_sections_gather,
                          p_decoder);
      if (pdVar1 != (dvbpsi_demux_subdec_t *)0x0) {
        dvbpsi_AttachDemuxSubDecoder(p_demux,pdVar1);
        *(dvbpsi_sdt_callback *)(p_decoder + 1) = pf_callback;
        *(void **)&p_decoder[1].b_discontinuity = p_cb_data;
        p_decoder[1].i_section_max_size = 0;
        return true;
      }
      dvbpsi_decoder_delete(p_decoder);
    }
  }
  else {
    dvbpsi_error(p_dvbpsi,"SDT decoder",
                 "Already a decoder for (table_id == 0x%02x,extension == 0x%02x)",i_table_id,
                 i_extension);
  }
  return false;
}



// ==== dvbpsi_sdt_init @ 0041ae10 ====

void dvbpsi_sdt_init(dvbpsi_sdt_t *p_sdt,uint8_t i_table_id,uint16_t i_extension,uint8_t i_version,
                    _Bool b_current_next,uint16_t i_network_id)

{
  if (p_sdt == (dvbpsi_sdt_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_sdt","tables/sdt.c",0x90,"dvbpsi_sdt_init");
  }
  p_sdt->i_table_id = i_table_id;
  p_sdt->i_version = i_version;
  p_sdt->i_extension = i_extension;
  p_sdt->b_current_next = b_current_next;
  p_sdt->i_network_id = i_network_id;
  p_sdt->p_first_service = (dvbpsi_sdt_service_t *)0x0;
  return;
}



// ==== dvbpsi_sdt_new @ 0041ae84 ====

dvbpsi_sdt_t *
dvbpsi_sdt_new(uint8_t i_table_id,uint16_t i_extension,uint8_t i_version,_Bool b_current_next,
              uint16_t i_network_id)

{
  dvbpsi_sdt_t *pdVar1;
  
                    /* Unresolved local var: dvbpsi_sdt_t * p_sdt@[DW_OP_reg2(v0)] */
  pdVar1 = malloc(0xc);
  if (pdVar1 != (dvbpsi_sdt_t *)0x0) {
    pdVar1->i_table_id = i_table_id;
    pdVar1->i_extension = i_extension;
    pdVar1->i_version = i_version;
    pdVar1->b_current_next = b_current_next;
    pdVar1->i_network_id = i_network_id;
    pdVar1->p_first_service = (dvbpsi_sdt_service_t *)0x0;
  }
  return pdVar1;
}



// ==== dvbpsi_sdt_empty @ 0041af0c ====

void dvbpsi_sdt_empty(dvbpsi_sdt_t *p_sdt)

{
  dvbpsi_sdt_service_t *__ptr;
  dvbpsi_sdt_service_s *pdVar1;
  
                    /* Unresolved local var: dvbpsi_sdt_service_t * p_service@[???] */
  __ptr = p_sdt->p_first_service;
  while (__ptr != (dvbpsi_sdt_service_t *)0x0) {
                    /* Unresolved local var: dvbpsi_sdt_service_t * p_tmp@[???] */
    pdVar1 = __ptr->p_next;
    dvbpsi_DeleteDescriptors(__ptr->p_first_descriptor);
    free(__ptr);
    __ptr = pdVar1;
  }
  p_sdt->p_first_service = (dvbpsi_sdt_service_t *)0x0;
  return;
}



// ==== dvbpsi_sdt_delete @ 0041af80 ====

void dvbpsi_sdt_delete(dvbpsi_sdt_t *p_sdt)

{
  if (p_sdt != (dvbpsi_sdt_t *)0x0) {
    dvbpsi_sdt_empty(p_sdt);
  }
  free(p_sdt);
  return;
}



// ==== dvbpsi_ReInitSDT @ 0041afc4 ====

void dvbpsi_ReInitSDT(dvbpsi_sdt_decoder_t *p_decoder,_Bool b_force)

{
  undefined3 in_register_00000015;
  
  if (p_decoder == (dvbpsi_sdt_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_decoder","tables/sdt.c",0x10a,"dvbpsi_ReInitSDT");
  }
  dvbpsi_decoder_reset((dvbpsi_decoder_t *)p_decoder,b_force);
  if ((CONCAT31(in_register_00000015,b_force) != 0) &&
     (p_decoder->p_building_sdt != (dvbpsi_sdt_t *)0x0)) {
    dvbpsi_sdt_delete(p_decoder->p_building_sdt);
  }
  p_decoder->p_building_sdt = (dvbpsi_sdt_t *)0x0;
  return;
}



// ==== dvbpsi_sdt_detach @ 0041b050 ====

void dvbpsi_sdt_detach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension)

{
  dvbpsi_demux_subdec_t *p_subdec;
  dvbpsi_demux_t *p_demux;
  dvbpsi_decoder_t *pdVar1;
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???]
                       Unresolved local var: dvbpsi_sdt_decoder_t * p_sdt_decoder@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/sdt.c",0x6b,"dvbpsi_sdt_detach");
  }
  p_demux = (dvbpsi_demux_t *)p_dvbpsi->p_decoder;
  if (p_demux == (dvbpsi_demux_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/sdt.c",0x6c,"dvbpsi_sdt_detach");
  }
  p_subdec = dvbpsi_demuxGetSubDec(p_demux,i_table_id,i_extension);
  if (p_subdec == (dvbpsi_demux_subdec_t *)0x0) {
    dvbpsi_error(p_dvbpsi,"SDT Decoder",
                 "No such SDT decoder (table_id == 0x%02x,extension == 0x%02x)",i_table_id,
                 i_extension);
    return;
  }
  pdVar1 = p_subdec->p_decoder;
  if (pdVar1 == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_subdec->p_decoder","tables/sdt.c",0x7b,"dvbpsi_sdt_detach");
  }
  if ((dvbpsi_sdt_t *)pdVar1[1].i_section_max_size != (dvbpsi_sdt_t *)0x0) {
    dvbpsi_sdt_delete((dvbpsi_sdt_t *)pdVar1[1].i_section_max_size);
  }
  pdVar1[1].i_section_max_size = 0;
  dvbpsi_DetachDemuxSubDecoder(p_demux,p_subdec);
  dvbpsi_DeleteDemuxSubDecoder(p_subdec);
  return;
}



// ==== dvbpsi_sdt_service_add @ 0041b1c0 ====

dvbpsi_sdt_service_t *
dvbpsi_sdt_service_add
          (dvbpsi_sdt_t *p_sdt,uint16_t i_service_id,_Bool b_eit_schedule,_Bool b_eit_present,
          uint8_t i_running_status,_Bool b_free_ca)

{
  dvbpsi_sdt_service_t *pdVar1;
  dvbpsi_sdt_service_t *pdVar2;
  
                    /* Unresolved local var: dvbpsi_sdt_service_t * p_service@[DW_OP_reg2(v0)] */
  pdVar1 = calloc(1,0x10);
  if (pdVar1 != (dvbpsi_sdt_service_t *)0x0) {
    pdVar2 = p_sdt->p_first_service;
    pdVar1->i_service_id = i_service_id;
    pdVar1->b_eit_schedule = b_eit_schedule;
    pdVar1->b_eit_present = b_eit_present;
    pdVar1->i_running_status = i_running_status;
    pdVar1->b_free_ca = b_free_ca;
    pdVar1->p_next = (dvbpsi_sdt_service_s *)0x0;
    pdVar1->p_first_descriptor = (dvbpsi_descriptor_t *)0x0;
    if (pdVar2 == (dvbpsi_sdt_service_t *)0x0) {
      p_sdt->p_first_service = pdVar1;
    }
    else {
      while (pdVar2->p_next != (dvbpsi_sdt_service_s *)0x0) {
                    /* Unresolved local var: dvbpsi_sdt_service_t * p_last_service@[???] */
        pdVar2 = pdVar2->p_next;
      }
      pdVar2->p_next = pdVar1;
    }
  }
  return pdVar1;
}



// ==== dvbpsi_sdt_service_descriptor_add @ 0041b288 ====

dvbpsi_descriptor_t *
dvbpsi_sdt_service_descriptor_add
          (dvbpsi_sdt_service_t *p_service,uint8_t i_tag,uint8_t i_length,uint8_t *p_data)

{
  dvbpsi_descriptor_t *p_descriptor;
  dvbpsi_descriptor_t *pdVar1;
  
                    /* Unresolved local var: dvbpsi_descriptor_t * p_descriptor@[???] */
  p_descriptor = dvbpsi_NewDescriptor(i_tag,i_length,p_data);
  if (p_descriptor != (dvbpsi_descriptor_t *)0x0) {
    pdVar1 = dvbpsi_AddDescriptor(p_service->p_first_descriptor,p_descriptor);
    p_service->p_first_descriptor = pdVar1;
    if (pdVar1 == (dvbpsi_descriptor_t *)0x0) {
                    /* WARNING: Subroutine does not return */
      __assert_fail("p_service->p_first_descriptor","tables/sdt.c",0x100,
                    "dvbpsi_sdt_service_descriptor_add");
    }
  }
  return p_descriptor;
}



// ==== dvbpsi_sdt_sections_decode @ 0041b320 ====

void dvbpsi_sdt_sections_decode(dvbpsi_sdt_t *p_sdt,dvbpsi_psi_section_t *p_section)

{
  byte bVar1;
  byte bVar2;
  uint8_t *puVar3;
  dvbpsi_sdt_service_t *p_service;
  uint8_t *puVar4;
  uint8_t *p_data;
  uint8_t *puVar5;
  uint8_t *puVar6;
  uint8_t *puVar7;
  
  do {
    if (p_section == (dvbpsi_psi_section_t *)0x0) {
      return;
    }
    puVar4 = p_section->p_payload_end;
    puVar5 = p_section->p_payload_start + 3;
    puVar3 = p_section->p_payload_start + 7;
    while (puVar6 = puVar5, puVar3 < puVar4) {
      while( true ) {
                    /* Unresolved local var: uint16_t i_service_id@[???]
                       Unresolved local var: _Bool b_eit_schedule@[???]
                       Unresolved local var: _Bool b_eit_present@[???]
                       Unresolved local var: uint8_t i_running_status@[???]
                       Unresolved local var: _Bool b_free_ca@[???]
                       Unresolved local var: uint16_t i_srv_length@[???]
                       Unresolved local var: dvbpsi_sdt_service_t * p_service@[???] */
        bVar1 = puVar6[3];
        bVar2 = puVar6[4];
        p_service = dvbpsi_sdt_service_add
                              (p_sdt,CONCAT11(*puVar6,puVar6[1]),SUB41((puVar6[2] & 3) >> 1,0),
                               (_Bool)(puVar6[2] & 1),bVar1 >> 5,SUB41((bVar1 & 0x1f) >> 4,0));
        puVar4 = p_section->p_payload_end;
        puVar5 = puVar6 + 5;
        puVar7 = puVar5 + ((bVar1 & 0xf) << 8 | (uint)bVar2);
        if (puVar4 < puVar7) goto LAB_0041b418;
        p_data = puVar6 + 7;
        puVar3 = puVar6 + 9;
        if (puVar7 < p_data) break;
        do {
                    /* Unresolved local var: uint8_t i_tag@[???]
                       Unresolved local var: uint8_t i_length@[???] */
          bVar1 = puVar5[1];
          if ((int)(bVar1 + 1) < (int)puVar7 - (int)puVar5) {
            dvbpsi_sdt_service_descriptor_add(p_service,*puVar5,bVar1,p_data);
          }
          puVar5 = puVar5 + bVar1 + 2;
          p_data = puVar5 + 2;
        } while (p_data <= puVar7);
        puVar6 = puVar5;
        if (p_section->p_payload_end <= puVar5 + 4) goto LAB_0041b418;
      }
    }
LAB_0041b418:
    p_section = p_section->p_next;
  } while( true );
}



// ==== dvbpsi_sdt_sections_gather @ 0041b45c ====

void dvbpsi_sdt_sections_gather
               (dvbpsi_t *p_dvbpsi,dvbpsi_decoder_t *p_private_decoder,
               dvbpsi_psi_section_t *p_section)

{
  short sVar1;
  _Bool _Var2;
  undefined3 extraout_var;
  int iVar3;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  dvbpsi_psi_section_t *pdVar4;
  dvbpsi_sdt_t *pdVar5;
  undefined4 uVar6;
  dvbpsi_psi_section_t *p_section_00;
  dvbpsi_psi_section_t *pdVar7;
  dvbpsi_decoder_t *pdVar8;
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_sdt_decoder_t * p_sdt_decoder@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/sdt.c",0x161,"dvbpsi_sdt_sections_gather");
  }
  if (p_dvbpsi->p_decoder == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/sdt.c",0x162,"dvbpsi_sdt_sections_gather");
  }
  _Var2 = dvbpsi_CheckPSISection(p_dvbpsi,p_section,0x42,"SDT decoder");
  if (CONCAT31(extraout_var,_Var2) == 0) goto LAB_0041b694;
  pdVar8 = p_dvbpsi->p_decoder;
  if (pdVar8->b_discontinuity == false) {
    iVar3 = p_private_decoder[1].i_section_max_size;
    if (iVar3 == 0) {
      if ((p_private_decoder->b_current_valid != false) &&
         (sVar1._0_1_ = p_section->i_version, sVar1._1_1_ = p_section->b_current_next,
         *(short *)&p_private_decoder[1].p_sections == sVar1)) {
        dvbpsi_debug(p_dvbpsi,"SDT decoder","ignoring already decoded section %d",
                     (uint)p_section->i_number);
        goto LAB_0041b694;
      }
    }
    else {
                    /* Unresolved local var: _Bool b_reinit@[???]
                       Unresolved local var: char[16] __PRETTY_FUNCTION__@[???] */
      if (*(uint16_t *)(iVar3 + 2) == p_section->i_extension) {
        if (*(uint8_t *)(iVar3 + 4) == p_section->i_version) {
          if (p_private_decoder->i_last_section_number == p_section->i_last_number)
          goto LAB_0041b50c;
          dvbpsi_error(p_dvbpsi,"SDT decoder",
                       "\'last_section_number\' differs whereas no discontinuity has occured");
        }
        else {
          dvbpsi_error(p_dvbpsi,"SDT decoder",
                       "\'version_number\' differs whereas no discontinuity has occured");
        }
      }
      else {
        dvbpsi_error(p_dvbpsi,"SDT decoder",
                     "\'transport_stream_id\' differs whereas no TS discontinuity has occured");
      }
      dvbpsi_ReInitSDT((dvbpsi_sdt_decoder_t *)p_private_decoder,true);
    }
  }
  else {
    dvbpsi_ReInitSDT((dvbpsi_sdt_decoder_t *)p_private_decoder,true);
    p_private_decoder->b_discontinuity = false;
    pdVar8->b_discontinuity = false;
  }
LAB_0041b50c:
  if (p_private_decoder == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_sdt_decoder","tables/sdt.c",0x13f,"dvbpsi_AddSectionSDT");
  }
  if (p_section == (dvbpsi_psi_section_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_section","tables/sdt.c",0x140,"dvbpsi_AddSectionSDT");
  }
  if (p_private_decoder[1].i_section_max_size == 0) {
    pdVar5 = dvbpsi_sdt_new(p_section->i_table_id,p_section->i_extension,p_section->i_version,
                            p_section->b_current_next,
                            CONCAT11(*p_section->p_payload_start,p_section->p_payload_start[1]));
    p_private_decoder[1].i_section_max_size = (int)pdVar5;
    if (pdVar5 == (dvbpsi_sdt_t *)0x0) {
      dvbpsi_error(p_dvbpsi,"SDT decoder","failed decoding section %d",(uint)p_section->i_number);
LAB_0041b694:
      dvbpsi_DeletePSISections(p_section);
      return;
    }
    p_private_decoder->i_last_section_number = p_section->i_last_number;
  }
  _Var2 = dvbpsi_decoder_psi_section_add(p_private_decoder,p_section);
  if (CONCAT31(extraout_var_00,_Var2) != 0) {
    dvbpsi_debug(p_dvbpsi,"SDT decoder","overwrite section number %d",(uint)p_section->i_number);
  }
  _Var2 = dvbpsi_decoder_psi_sections_completed(p_private_decoder);
  if (CONCAT31(extraout_var_01,_Var2) == 0) {
    return;
  }
  iVar3._0_1_ = p_private_decoder[1].i_magic[0];
  iVar3._1_1_ = p_private_decoder[1].i_magic[1];
  iVar3._2_1_ = p_private_decoder[1].i_magic[2];
  iVar3._3_1_ = p_private_decoder[1].b_complete_header;
  if (iVar3 == 0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_sdt_decoder->pf_sdt_callback","tables/sdt.c",0x19a,"dvbpsi_sdt_sections_gather"
                 );
  }
  pdVar5 = (dvbpsi_sdt_t *)p_private_decoder[1].i_section_max_size;
  p_section_00 = p_private_decoder->p_sections;
  pdVar7 = *(dvbpsi_psi_section_t **)pdVar5;
  pdVar4 = *(dvbpsi_psi_section_t **)&pdVar5->i_version;
  p_private_decoder[1].pf_gather = (dvbpsi_callback_gather_t)pdVar5->p_first_service;
  p_private_decoder[1].p_current_section = pdVar7;
  p_private_decoder[1].p_sections = pdVar4;
  p_private_decoder->b_current_valid = true;
  dvbpsi_sdt_sections_decode(pdVar5,p_section_00);
  dvbpsi_DeletePSISections(p_private_decoder->p_sections);
  uVar6._0_1_ = p_private_decoder[1].b_discontinuity;
  uVar6._1_1_ = p_private_decoder[1].b_current_valid;
  uVar6._2_1_ = p_private_decoder[1].i_continuity_counter;
  uVar6._3_1_ = p_private_decoder[1].i_last_section_number;
  iVar3 = p_private_decoder[1].i_section_max_size;
  p_private_decoder->p_sections = (dvbpsi_psi_section_t *)0x0;
  (**(code **)(p_private_decoder + 1))(uVar6,iVar3);
  dvbpsi_ReInitSDT((dvbpsi_sdt_decoder_t *)p_private_decoder,false);
  return;
}



// ==== dvbpsi_sdt_sections_generate @ 0041b7f4 ====

dvbpsi_psi_section_t * dvbpsi_sdt_sections_generate(dvbpsi_t *p_dvbpsi,dvbpsi_sdt_t *p_sdt)

{
  dvbpsi_psi_section_s **ppdVar1;
  _Bool _Var2;
  uint16_t uVar3;
  uint8_t uVar4;
  dvbpsi_psi_section_t *pdVar5;
  dvbpsi_descriptor_t *pdVar6;
  uint8_t *puVar7;
  dvbpsi_psi_section_t *pdVar8;
  uint uVar9;
  uint8_t *puVar10;
  byte bVar11;
  int iVar12;
  dvbpsi_psi_section_t *p_section;
  dvbpsi_psi_section_t *pdVar13;
  dvbpsi_sdt_service_t *pdVar14;
  uint8_t *puVar15;
  byte *pbVar16;
  
                    /* Unresolved local var: dvbpsi_psi_section_t * p_result@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_current@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_prev@[???]
                       Unresolved local var: dvbpsi_sdt_service_t * p_service@[???] */
  pdVar5 = dvbpsi_NewPSISection(0x400);
  pdVar14 = p_sdt->p_first_service;
  pdVar5->i_table_id = 0x42;
  uVar3 = p_sdt->i_extension;
  pdVar5->b_syntax_indicator = true;
  pdVar5->b_private_indicator = true;
  pdVar5->i_length = 0xc;
  pdVar5->i_extension = uVar3;
  pdVar5->i_version = p_sdt->i_version;
  _Var2 = p_sdt->b_current_next;
  pdVar5->i_number = 0;
  uVar3 = p_sdt->i_network_id;
  pdVar5->b_current_next = _Var2;
  pdVar5->p_payload_end = pdVar5->p_payload_end + 0xb;
  pdVar5->p_payload_start = pdVar5->p_data + 8;
  pdVar5->p_data[8] = (uint8_t)(uVar3 >> 8);
  pdVar5->p_data[9] = (uint8_t)p_sdt->i_network_id;
  pdVar5->p_data[10] = 0xff;
  pdVar13 = pdVar5;
joined_r0x0041b8c0:
  if (pdVar14 == (dvbpsi_sdt_service_t *)0x0) {
    p_section = pdVar5;
    pdVar8 = pdVar13;
    do {
      uVar4 = pdVar8->i_number;
LAB_0041ba64:
      p_section->i_last_number = uVar4;
      dvbpsi_BuildPSISection(p_dvbpsi,p_section);
      ppdVar1 = &p_section->p_next;
      p_section = *ppdVar1;
    } while (*ppdVar1 != (dvbpsi_psi_section_t *)0x0);
    return pdVar5;
  }
  do {
    pdVar6 = pdVar14->p_first_descriptor;
    puVar15 = pdVar13->p_payload_end;
    pdVar8 = pdVar13;
    if (pdVar6 != (dvbpsi_descriptor_t *)0x0) {
      iVar12 = (int)puVar15 - (int)pdVar13->p_data;
      uVar9 = 5;
      if (iVar12 < 0x3f8) {
        bVar11 = pdVar6->i_length;
        while( true ) {
          pdVar6 = pdVar6->p_next;
          uVar9 = uVar9 + bVar11 + 2 & 0xffff;
          if (pdVar6 == (dvbpsi_descriptor_s *)0x0) break;
          if (0x3fc < (int)(uVar9 + iVar12)) {
            if ((iVar12 != 0xb) && (uVar9 < 0x3f2)) goto LAB_0041bb20;
            break;
          }
          bVar11 = pdVar6->i_length;
        }
      }
      else {
LAB_0041bb20:
        dvbpsi_debug(p_dvbpsi,"SDT generator",
                     "create a new section to carry more Service descriptors");
        pdVar8 = dvbpsi_NewPSISection(0x400);
        pdVar8->i_table_id = 0x42;
        uVar3 = p_sdt->i_extension;
        pdVar8->b_syntax_indicator = true;
        pdVar8->b_private_indicator = true;
        pdVar13->p_next = pdVar8;
        pdVar8->i_extension = uVar3;
        pdVar8->i_length = 0xc;
        uVar4 = pdVar13->i_number;
        pdVar8->i_version = p_sdt->i_version;
        _Var2 = p_sdt->b_current_next;
        pdVar8->i_number = uVar4 + 1;
        uVar3 = p_sdt->i_network_id;
        pdVar8->p_payload_end = pdVar8->p_payload_end + 0xb;
        pdVar8->b_current_next = _Var2;
        pdVar8->p_payload_start = pdVar8->p_data + 8;
        pdVar8->p_data[8] = (uint8_t)(uVar3 >> 8);
        pdVar8->p_data[9] = (uint8_t)p_sdt->i_network_id;
        pdVar8->p_data[10] = 0xff;
        puVar15 = pdVar8->p_payload_end;
      }
    }
    bVar11 = 0xfc;
    pbVar16 = puVar15 + 3;
    *puVar15 = (uint8_t)(pdVar14->i_service_id >> 8);
    puVar15[1] = (uint8_t)pdVar14->i_service_id;
    if (pdVar14->b_eit_schedule != false) {
      bVar11 = 0xfe;
    }
    puVar15[2] = bVar11 | pdVar14->b_eit_present;
    puVar15[3] = (pdVar14->b_free_ca & 1U) << 4 | pdVar14->i_running_status << 5;
    pdVar6 = pdVar14->p_first_descriptor;
    puVar7 = pdVar8->p_payload_end + 5;
    pdVar8->p_payload_end = puVar7;
    pdVar8->i_length = pdVar8->i_length + 5;
    pdVar13 = pdVar8;
    if (pdVar6 == (dvbpsi_descriptor_t *)0x0) break;
    puVar10 = puVar7 + ((uint)pdVar6->i_length - (int)pdVar8->p_data);
    while ((int)puVar10 < 0x3fb) {
      *puVar7 = pdVar6->i_tag;
      pdVar8->p_payload_end[1] = pdVar6->i_length;
      memcpy(pdVar8->p_payload_end + 2,pdVar6->p_data,(size_t)pdVar6->i_length);
      puVar7 = pdVar8->p_payload_end + pdVar6->i_length + 2;
      pdVar8->p_payload_end = puVar7;
      puVar10 = &pdVar6->i_length;
      pdVar6 = pdVar6->p_next;
      pdVar8->i_length = pdVar8->i_length + 2 + (ushort)*puVar10;
      if (pdVar6 == (dvbpsi_descriptor_s *)0x0) goto LAB_0041ba30;
      puVar10 = puVar7 + ((uint)pdVar6->i_length - (int)pdVar8->p_data);
    }
    dvbpsi_error(p_dvbpsi,"SDT generator","unable to carry all the descriptors");
    puVar7 = pdVar8->p_payload_end;
    *pbVar16 = (byte)((uint)(puVar7 + (-5 - (int)puVar15)) >> 8) & 0xf | *pbVar16;
    puVar15[4] = (uint8_t)(puVar7 + (-5 - (int)puVar15));
    pdVar14 = pdVar14->p_next;
    if (pdVar14 == (dvbpsi_sdt_service_s *)0x0) {
      uVar4 = pdVar8->i_number;
      p_section = pdVar5;
      goto LAB_0041ba64;
    }
  } while( true );
LAB_0041ba30:
  *pbVar16 = (byte)((uint)(puVar7 + (-5 - (int)puVar15)) >> 8) & 0xf | *pbVar16;
  puVar15[4] = (uint8_t)(puVar7 + (-5 - (int)puVar15));
  pdVar14 = pdVar14->p_next;
  goto joined_r0x0041b8c0;
}



