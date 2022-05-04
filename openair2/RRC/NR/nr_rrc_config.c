/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

/*! \file nr_rrc_config.c
 * \brief rrc config for gNB
 * \author Raymond Knopp, WEI-TAI CHEN
 * \date 2018
 * \version 1.0
 * \company Eurecom, NTUST
 * \email: raymond.knopp@eurecom.fr, kroempa@gmail.com
 */

#include "nr_rrc_config.h"
#include "common/utils/nr/nr_common.h"

const uint8_t slotsperframe[5] = {10, 20, 40, 80, 160};


void set_csirs_periodicity(NR_NZP_CSI_RS_Resource_t *nzpcsi0, int uid, int nb_slots_per_period) {

  nzpcsi0->periodicityAndOffset = calloc(1,sizeof(*nzpcsi0->periodicityAndOffset));
  int ideal_period = nb_slots_per_period*MAX_MOBILES_PER_GNB;

  if (ideal_period<5) {
    nzpcsi0->periodicityAndOffset->present = NR_CSI_ResourcePeriodicityAndOffset_PR_slots4;
    nzpcsi0->periodicityAndOffset->choice.slots4 = nb_slots_per_period*uid;
  }
  else if (ideal_period<6) {
    nzpcsi0->periodicityAndOffset->present = NR_CSI_ResourcePeriodicityAndOffset_PR_slots5;
    nzpcsi0->periodicityAndOffset->choice.slots5 = nb_slots_per_period*uid;
  }
  else if (ideal_period<9) {
    nzpcsi0->periodicityAndOffset->present = NR_CSI_ResourcePeriodicityAndOffset_PR_slots8;
    nzpcsi0->periodicityAndOffset->choice.slots8 = nb_slots_per_period*uid;
  }
  else if (ideal_period<11) {
    nzpcsi0->periodicityAndOffset->present = NR_CSI_ResourcePeriodicityAndOffset_PR_slots10;
    nzpcsi0->periodicityAndOffset->choice.slots10 = nb_slots_per_period*uid;
  }
  else if (ideal_period<17) {
    nzpcsi0->periodicityAndOffset->present = NR_CSI_ResourcePeriodicityAndOffset_PR_slots16;
    nzpcsi0->periodicityAndOffset->choice.slots16 = nb_slots_per_period*uid;
  }
  else if (ideal_period<21) {
    nzpcsi0->periodicityAndOffset->present = NR_CSI_ResourcePeriodicityAndOffset_PR_slots20;
    nzpcsi0->periodicityAndOffset->choice.slots20 = nb_slots_per_period*uid;
  }
  else if (ideal_period<41) {
    nzpcsi0->periodicityAndOffset->present = NR_CSI_ResourcePeriodicityAndOffset_PR_slots40;
    nzpcsi0->periodicityAndOffset->choice.slots40 = nb_slots_per_period*uid;
  }
  else if (ideal_period<81) {
    nzpcsi0->periodicityAndOffset->present = NR_CSI_ResourcePeriodicityAndOffset_PR_slots80;
    nzpcsi0->periodicityAndOffset->choice.slots80 = nb_slots_per_period*uid;
  }
  else if (ideal_period<161) {
    nzpcsi0->periodicityAndOffset->present = NR_CSI_ResourcePeriodicityAndOffset_PR_slots160;
    nzpcsi0->periodicityAndOffset->choice.slots160 = nb_slots_per_period*uid;
  }
  else {
    nzpcsi0->periodicityAndOffset->present = NR_CSI_ResourcePeriodicityAndOffset_PR_slots320;
    nzpcsi0->periodicityAndOffset->choice.slots320 = (nb_slots_per_period*uid)%320 + (nb_slots_per_period*uid)/320;
  }
}

void config_csirs(NR_ServingCellConfigCommon_t *servingcellconfigcommon,
                  NR_CSI_MeasConfig_t *csi_MeasConfig,
                  int uid,
                  int num_dl_antenna_ports,
                  int curr_bwp,
                  int do_csirs) {

  if (do_csirs) {

    csi_MeasConfig->nzp_CSI_RS_ResourceSetToAddModList  = calloc(1,sizeof(*csi_MeasConfig->nzp_CSI_RS_ResourceSetToAddModList));
    NR_NZP_CSI_RS_ResourceSet_t *nzpcsirs0 = calloc(1,sizeof(*nzpcsirs0));
    nzpcsirs0->nzp_CSI_ResourceSetId = 0;
    NR_NZP_CSI_RS_ResourceId_t *nzpid0 = calloc(1,sizeof(*nzpid0));
    *nzpid0 = 0;
    ASN_SEQUENCE_ADD(&nzpcsirs0->nzp_CSI_RS_Resources,nzpid0);
    nzpcsirs0->repetition = NULL;
    nzpcsirs0->aperiodicTriggeringOffset = NULL;
    nzpcsirs0->trs_Info = NULL;
    ASN_SEQUENCE_ADD(&csi_MeasConfig->nzp_CSI_RS_ResourceSetToAddModList->list,nzpcsirs0);

    const NR_TDD_UL_DL_Pattern_t *tdd = servingcellconfigcommon->tdd_UL_DL_ConfigurationCommon ?
                                        &servingcellconfigcommon->tdd_UL_DL_ConfigurationCommon->pattern1 : NULL;

    const int n_slots_frame = slotsperframe[*servingcellconfigcommon->ssbSubcarrierSpacing];

    int nb_slots_per_period = n_slots_frame;
    if (tdd)
      nb_slots_per_period = n_slots_frame/get_nb_periods_per_frame(tdd->dl_UL_TransmissionPeriodicity);

    csi_MeasConfig->nzp_CSI_RS_ResourceToAddModList = calloc(1,sizeof(*csi_MeasConfig->nzp_CSI_RS_ResourceToAddModList));
    NR_NZP_CSI_RS_Resource_t *nzpcsi0 = calloc(1,sizeof(*nzpcsi0));
    nzpcsi0->nzp_CSI_RS_ResourceId = 0;
    NR_CSI_RS_ResourceMapping_t resourceMapping;
    switch (num_dl_antenna_ports) {
      case 1:
        resourceMapping.frequencyDomainAllocation.present = NR_CSI_RS_ResourceMapping__frequencyDomainAllocation_PR_row2;
        resourceMapping.frequencyDomainAllocation.choice.row2.buf = calloc(2, sizeof(uint8_t));
        resourceMapping.frequencyDomainAllocation.choice.row2.size = 2;
        resourceMapping.frequencyDomainAllocation.choice.row2.bits_unused = 4;
        resourceMapping.frequencyDomainAllocation.choice.row2.buf[0] = 0;
        resourceMapping.frequencyDomainAllocation.choice.row2.buf[1] = 16;
        resourceMapping.nrofPorts = NR_CSI_RS_ResourceMapping__nrofPorts_p1;
        resourceMapping.cdm_Type = NR_CSI_RS_ResourceMapping__cdm_Type_noCDM;
        break;
      case 2:
        resourceMapping.frequencyDomainAllocation.present = NR_CSI_RS_ResourceMapping__frequencyDomainAllocation_PR_other;
        resourceMapping.frequencyDomainAllocation.choice.other.buf = calloc(2, sizeof(uint8_t));
        resourceMapping.frequencyDomainAllocation.choice.other.size = 1;
        resourceMapping.frequencyDomainAllocation.choice.other.bits_unused = 2;
        resourceMapping.frequencyDomainAllocation.choice.other.buf[0] = 4;
        resourceMapping.nrofPorts = NR_CSI_RS_ResourceMapping__nrofPorts_p2;
        resourceMapping.cdm_Type = NR_CSI_RS_ResourceMapping__cdm_Type_fd_CDM2;
        break;
      case 4:
        resourceMapping.frequencyDomainAllocation.present = NR_CSI_RS_ResourceMapping__frequencyDomainAllocation_PR_row4;
        resourceMapping.frequencyDomainAllocation.choice.row4.buf = calloc(2, sizeof(uint8_t));
        resourceMapping.frequencyDomainAllocation.choice.row4.size = 1;
        resourceMapping.frequencyDomainAllocation.choice.row4.bits_unused = 5;
        resourceMapping.frequencyDomainAllocation.choice.row4.buf[0] = 32;
        resourceMapping.nrofPorts = NR_CSI_RS_ResourceMapping__nrofPorts_p4;
        resourceMapping.cdm_Type = NR_CSI_RS_ResourceMapping__cdm_Type_fd_CDM2;
        break;
      default:
        AssertFatal(1==0,"Number of ports not yet supported\n");
    }
    resourceMapping.firstOFDMSymbolInTimeDomain = 13;  // last symbol of slot
    resourceMapping.firstOFDMSymbolInTimeDomain2 = NULL;
    resourceMapping.density.present = NR_CSI_RS_ResourceMapping__density_PR_one;
    resourceMapping.density.choice.one = (NULL_t)0;
    resourceMapping.freqBand.startingRB = 0;
    resourceMapping.freqBand.nrofRBs = ((curr_bwp>>2)+(curr_bwp%4>0))<<2;
    nzpcsi0->resourceMapping = resourceMapping;
    nzpcsi0->powerControlOffset = 0;
    nzpcsi0->powerControlOffsetSS=calloc(1,sizeof(*nzpcsi0->powerControlOffsetSS));
    *nzpcsi0->powerControlOffsetSS = NR_NZP_CSI_RS_Resource__powerControlOffsetSS_db0;
    nzpcsi0->scramblingID = *servingcellconfigcommon->physCellId;
    set_csirs_periodicity(nzpcsi0, uid, nb_slots_per_period);
    nzpcsi0->qcl_InfoPeriodicCSI_RS = NULL;
    ASN_SEQUENCE_ADD(&csi_MeasConfig->nzp_CSI_RS_ResourceToAddModList->list,nzpcsi0);
  }
  else {
    csi_MeasConfig->nzp_CSI_RS_ResourceToAddModList = NULL;
    csi_MeasConfig->nzp_CSI_RS_ResourceSetToAddModList  = NULL;
  }
  csi_MeasConfig->nzp_CSI_RS_ResourceSetToReleaseList = NULL;
  csi_MeasConfig->nzp_CSI_RS_ResourceToReleaseList = NULL;
}

// TODO: Implement to b_SRS = 1 and b_SRS = 2
long rrc_get_max_nr_csrs(uint8_t max_rbs, long b_SRS) {

  if(b_SRS>0) {
    LOG_E(NR_RRC,"rrc_get_max_nr_csrs(): Not implemented yet for b_SRS>0\n");
    return 0; // This c_srs is always valid
  }

  const uint16_t m_SRS[64] = { 4, 8, 12, 16, 16, 20, 24, 24, 28, 32, 36, 40, 48, 48, 52, 56, 60, 64, 72, 72, 76, 80, 88,
                               96, 96, 104, 112, 120, 120, 120, 128, 128, 128, 132, 136, 144, 144, 144, 144, 152, 160,
                               160, 160, 168, 176, 184, 192, 192, 192, 192, 208, 216, 224, 240, 240, 240, 240, 256, 256,
                               256, 264, 272, 272, 272 };

  long c_srs = 0;
  uint16_t m = 4;
  for(int c = 1; c<64; c++) {
    if(m_SRS[c]>m && m_SRS[c]<max_rbs) {
      c_srs = c;
      m = m_SRS[c];
    }
  }

  return c_srs;
}

void config_srs(NR_SetupRelease_SRS_Config_t *setup_release_srs_Config,
                NR_ServingCellConfigCommon_t *servingcellconfigcommon,
                int uid,
                int do_srs) {

  setup_release_srs_Config->present = NR_SetupRelease_SRS_Config_PR_setup;

  NR_SRS_Config_t *srs_Config;
  if (setup_release_srs_Config->choice.setup) {
    srs_Config = setup_release_srs_Config->choice.setup;
    if (srs_Config->srs_ResourceSetToReleaseList) {
      free(srs_Config->srs_ResourceSetToReleaseList);
    }
    if (srs_Config->srs_ResourceSetToAddModList) {
      free(srs_Config->srs_ResourceSetToAddModList);
    }
    if (srs_Config->srs_ResourceToReleaseList) {
      free(srs_Config->srs_ResourceToReleaseList);
    }
    if (srs_Config->srs_ResourceToAddModList) {
      free(srs_Config->srs_ResourceToAddModList);
    }
    free(srs_Config);
  }

  setup_release_srs_Config->choice.setup = calloc(1,sizeof(*setup_release_srs_Config->choice.setup));
  srs_Config = setup_release_srs_Config->choice.setup;

  srs_Config->srs_ResourceSetToReleaseList = NULL;

  srs_Config->srs_ResourceSetToAddModList = calloc(1,sizeof(*srs_Config->srs_ResourceSetToAddModList));
  NR_SRS_ResourceSet_t *srs_resset0 = calloc(1,sizeof(*srs_resset0));
  srs_resset0->srs_ResourceSetId = 0;
  srs_resset0->srs_ResourceIdList = calloc(1,sizeof(*srs_resset0->srs_ResourceIdList));
  NR_SRS_ResourceId_t *srs_resset0_id = calloc(1,sizeof(*srs_resset0_id));
  *srs_resset0_id = 0;
  ASN_SEQUENCE_ADD(&srs_resset0->srs_ResourceIdList->list, srs_resset0_id);
  srs_Config->srs_ResourceToReleaseList=NULL;
  if (do_srs) {
    srs_resset0->resourceType.present =  NR_SRS_ResourceSet__resourceType_PR_periodic;
    srs_resset0->resourceType.choice.periodic = calloc(1,sizeof(*srs_resset0->resourceType.choice.periodic));
    srs_resset0->resourceType.choice.periodic->associatedCSI_RS = NULL;
  } else {
    srs_resset0->resourceType.present =  NR_SRS_ResourceSet__resourceType_PR_aperiodic;
    srs_resset0->resourceType.choice.aperiodic = calloc(1,sizeof(*srs_resset0->resourceType.choice.aperiodic));
    srs_resset0->resourceType.choice.aperiodic->aperiodicSRS_ResourceTrigger=1;
    srs_resset0->resourceType.choice.aperiodic->csi_RS=NULL;
    srs_resset0->resourceType.choice.aperiodic->slotOffset = calloc(1,sizeof(*srs_resset0->resourceType.choice.aperiodic->slotOffset));
    *srs_resset0->resourceType.choice.aperiodic->slotOffset = 2;
    srs_resset0->resourceType.choice.aperiodic->ext1 = NULL;
  }
  srs_resset0->usage=NR_SRS_ResourceSet__usage_codebook;
  srs_resset0->alpha = calloc(1,sizeof(*srs_resset0->alpha));
  *srs_resset0->alpha = NR_Alpha_alpha1;
  srs_resset0->p0 = calloc(1,sizeof(*srs_resset0->p0));
  *srs_resset0->p0 =-80;
  srs_resset0->pathlossReferenceRS = NULL;
  srs_resset0->srs_PowerControlAdjustmentStates = NULL;
  ASN_SEQUENCE_ADD(&srs_Config->srs_ResourceSetToAddModList->list,srs_resset0);

  srs_Config->srs_ResourceToReleaseList = NULL;

  srs_Config->srs_ResourceToAddModList = calloc(1,sizeof(*srs_Config->srs_ResourceToAddModList));
  NR_SRS_Resource_t *srs_res0=calloc(1,sizeof(*srs_res0));
  srs_res0->srs_ResourceId = 0;
  srs_res0->nrofSRS_Ports = NR_SRS_Resource__nrofSRS_Ports_port1;
  srs_res0->ptrs_PortIndex = NULL;
  srs_res0->transmissionComb.present = NR_SRS_Resource__transmissionComb_PR_n2;
  srs_res0->transmissionComb.choice.n2 = calloc(1,sizeof(*srs_res0->transmissionComb.choice.n2));
  srs_res0->transmissionComb.choice.n2->combOffset_n2 = 0;
  srs_res0->transmissionComb.choice.n2->cyclicShift_n2 = 0;
  srs_res0->resourceMapping.startPosition = 2 + uid%2;
  srs_res0->resourceMapping.nrofSymbols = NR_SRS_Resource__resourceMapping__nrofSymbols_n1;
  srs_res0->resourceMapping.repetitionFactor = NR_SRS_Resource__resourceMapping__repetitionFactor_n1;
  srs_res0->freqDomainPosition = 0;
  srs_res0->freqDomainShift = 0;
  srs_res0->freqHopping.b_SRS = 0;
  srs_res0->freqHopping.b_hop = 0;
  srs_res0->freqHopping.c_SRS = servingcellconfigcommon ?
                                rrc_get_max_nr_csrs(
                                    NRRIV2BW(servingcellconfigcommon->uplinkConfigCommon->initialUplinkBWP->genericParameters.locationAndBandwidth, 275),
                                    srs_res0->freqHopping.b_SRS) : 0;
  srs_res0->groupOrSequenceHopping = NR_SRS_Resource__groupOrSequenceHopping_neither;
  if (do_srs) {
    srs_res0->resourceType.present = NR_SRS_Resource__resourceType_PR_periodic;
    srs_res0->resourceType.choice.periodic = calloc(1,sizeof(*srs_res0->resourceType.choice.periodic));
    srs_res0->resourceType.choice.periodic->periodicityAndOffset_p.present = NR_SRS_PeriodicityAndOffset_PR_sl160;
    srs_res0->resourceType.choice.periodic->periodicityAndOffset_p.choice.sl160 = 17 + (uid>1)*10; // 17/17/.../147/157 are mixed slots
  } else {
    srs_res0->resourceType.present = NR_SRS_Resource__resourceType_PR_aperiodic;
    srs_res0->resourceType.choice.aperiodic = calloc(1,sizeof(*srs_res0->resourceType.choice.aperiodic));
  }
  srs_res0->sequenceId = 40;
  srs_res0->spatialRelationInfo = calloc(1,sizeof(*srs_res0->spatialRelationInfo));
  srs_res0->spatialRelationInfo->servingCellId = NULL;
  srs_res0->spatialRelationInfo->referenceSignal.present = NR_SRS_SpatialRelationInfo__referenceSignal_PR_csi_RS_Index;
  srs_res0->spatialRelationInfo->referenceSignal.choice.csi_RS_Index = 0;
  ASN_SEQUENCE_ADD(&srs_Config->srs_ResourceToAddModList->list,srs_res0);
}

void prepare_sim_uecap(NR_UE_NR_Capability_t *cap,
                       NR_ServingCellConfigCommon_t *scc,
                       int numerology,
                       int rbsize,
                       int mcs_table) {

  int band = *scc->downlinkConfigCommon->frequencyInfoDL->frequencyBandList.list.array[0];
  NR_BandNR_t *nr_bandnr = CALLOC(1,sizeof(NR_BandNR_t));
  nr_bandnr->bandNR = band;
  ASN_SEQUENCE_ADD(&cap->rf_Parameters.supportedBandListNR.list,
                   nr_bandnr);
  if (mcs_table == 1) {
    int bw = get_supported_band_index(numerology, band, rbsize);
    if (band>256) {
      NR_BandNR_t *bandNRinfo = cap->rf_Parameters.supportedBandListNR.list.array[0];
      bandNRinfo->pdsch_256QAM_FR2 = CALLOC(1,sizeof(*bandNRinfo->pdsch_256QAM_FR2));
      *bandNRinfo->pdsch_256QAM_FR2 = NR_BandNR__pdsch_256QAM_FR2_supported;
    }
    else{
      cap->phy_Parameters.phy_ParametersFR1 = CALLOC(1,sizeof(*cap->phy_Parameters.phy_ParametersFR1));
      NR_Phy_ParametersFR1_t *phy_fr1 = cap->phy_Parameters.phy_ParametersFR1;
      phy_fr1->pdsch_256QAM_FR1 = CALLOC(1,sizeof(*phy_fr1->pdsch_256QAM_FR1));
      *phy_fr1->pdsch_256QAM_FR1 = NR_Phy_ParametersFR1__pdsch_256QAM_FR1_supported;
    }
    cap->featureSets = CALLOC(1,sizeof(*cap->featureSets));
    NR_FeatureSets_t *fs=cap->featureSets;
    fs->featureSetsDownlinkPerCC = CALLOC(1,sizeof(*fs->featureSetsDownlinkPerCC));
    NR_FeatureSetDownlinkPerCC_t *fs_cc = CALLOC(1,sizeof(NR_FeatureSetDownlinkPerCC_t));
    fs_cc->supportedSubcarrierSpacingDL = numerology;
    if(band>256) {
      fs_cc->supportedBandwidthDL.present = NR_SupportedBandwidth_PR_fr2;
      fs_cc->supportedBandwidthDL.choice.fr2 = bw;
    }
    else{
      fs_cc->supportedBandwidthDL.present = NR_SupportedBandwidth_PR_fr1;
      fs_cc->supportedBandwidthDL.choice.fr1 = bw;
    }
    fs_cc->supportedModulationOrderDL = CALLOC(1,sizeof(*fs_cc->supportedModulationOrderDL));
    *fs_cc->supportedModulationOrderDL = NR_ModulationOrder_qam256;
    ASN_SEQUENCE_ADD(&fs->featureSetsDownlinkPerCC->list,
                     fs_cc);
  }
}

void nr_rrc_config_dl_tda(NR_ServingCellConfigCommon_t *scc){

  frame_type_t frame_type = get_frame_type(*scc->downlinkConfigCommon->frequencyInfoDL->frequencyBandList.list.array[0], *scc->ssbSubcarrierSpacing);
  int curr_bwp = scc->downlinkConfigCommon->frequencyInfoDL->scs_SpecificCarrierList.list.array[0]->carrierBandwidth;
  // coreset duration setting to be improved in the framework of RRC harmonization, potentially using a common function
  int len_coreset = 1;
  if (curr_bwp < 48)
    len_coreset = 2;
  // setting default TDA for DL with
  struct NR_PDSCH_TimeDomainResourceAllocation *timedomainresourceallocation = CALLOC(1,sizeof(NR_PDSCH_TimeDomainResourceAllocation_t));
  timedomainresourceallocation->mappingType = NR_PDSCH_TimeDomainResourceAllocation__mappingType_typeA;
  timedomainresourceallocation->startSymbolAndLength = get_SLIV(len_coreset,14-len_coreset); // basic slot configuration starting in symbol 1 til the end of the slot
  ASN_SEQUENCE_ADD(&scc->downlinkConfigCommon->initialDownlinkBWP->pdsch_ConfigCommon->choice.setup->pdsch_TimeDomainAllocationList->list,
                   timedomainresourceallocation);
  if(frame_type==TDD) {
    // TDD
    if(scc->tdd_UL_DL_ConfigurationCommon) {
      int dl_symb = scc->tdd_UL_DL_ConfigurationCommon->pattern1.nrofDownlinkSymbols;
      if(dl_symb > 1) {
        timedomainresourceallocation = CALLOC(1,sizeof(NR_PDSCH_TimeDomainResourceAllocation_t));
        timedomainresourceallocation->mappingType = NR_PDSCH_TimeDomainResourceAllocation__mappingType_typeA;
        timedomainresourceallocation->startSymbolAndLength = get_SLIV(len_coreset,dl_symb-len_coreset); // mixed slot configuration starting in symbol 1 til the end of the dl allocation
        ASN_SEQUENCE_ADD(&scc->downlinkConfigCommon->initialDownlinkBWP->pdsch_ConfigCommon->choice.setup->pdsch_TimeDomainAllocationList->list,
                         timedomainresourceallocation);
      }
    }
  }
}


void nr_rrc_config_ul_tda(NR_ServingCellConfigCommon_t *scc, int min_fb_delay){

  //TODO change to accomodate for SRS

  int temp_min_delay = 6; // k2 = 2 or 3 won'r work as well as higher values
  int k2 = (min_fb_delay<temp_min_delay)?temp_min_delay:min_fb_delay;

  uint8_t DELTA[4]= {2,3,4,6}; // Delta parameter for Msg3
  int mu = scc->uplinkConfigCommon->initialUplinkBWP->genericParameters.subcarrierSpacing;
  frame_type_t frame_type = get_frame_type(*scc->downlinkConfigCommon->frequencyInfoDL->frequencyBandList.list.array[0], *scc->ssbSubcarrierSpacing);

  struct NR_PUSCH_TimeDomainResourceAllocation *pusch_timedomainresourceallocation = CALLOC(1,sizeof(struct NR_PUSCH_TimeDomainResourceAllocation));
  pusch_timedomainresourceallocation->k2  = CALLOC(1,sizeof(long));
  *pusch_timedomainresourceallocation->k2 = k2;
  pusch_timedomainresourceallocation->mappingType = NR_PUSCH_TimeDomainResourceAllocation__mappingType_typeB;
  pusch_timedomainresourceallocation->startSymbolAndLength = get_SLIV(0,13); // basic slot configuration starting in symbol 0 til the last but one symbol
  ASN_SEQUENCE_ADD(&scc->uplinkConfigCommon->initialUplinkBWP->pusch_ConfigCommon->choice.setup->pusch_TimeDomainAllocationList->list,pusch_timedomainresourceallocation); 

  if(frame_type==TDD) {
    // TDD
    if(scc->tdd_UL_DL_ConfigurationCommon) {
      int ul_symb = scc->tdd_UL_DL_ConfigurationCommon->pattern1.nrofUplinkSymbols;
      pusch_timedomainresourceallocation = CALLOC(1,sizeof(struct NR_PUSCH_TimeDomainResourceAllocation));
      pusch_timedomainresourceallocation->k2  = CALLOC(1,sizeof(long));
      *pusch_timedomainresourceallocation->k2 = k2;
      pusch_timedomainresourceallocation->mappingType = NR_PUSCH_TimeDomainResourceAllocation__mappingType_typeB;
      pusch_timedomainresourceallocation->startSymbolAndLength = get_SLIV(14-ul_symb,ul_symb-1); // starting in fist ul symbol til the last but one
      ASN_SEQUENCE_ADD(&scc->uplinkConfigCommon->initialUplinkBWP->pusch_ConfigCommon->choice.setup->pusch_TimeDomainAllocationList->list,pusch_timedomainresourceallocation);

      // for msg3 in the mixed slot
      int nb_periods_per_frame = get_nb_periods_per_frame(scc->tdd_UL_DL_ConfigurationCommon->pattern1.dl_UL_TransmissionPeriodicity);
      int nb_slots_per_period = ((1<<mu) * 10)/nb_periods_per_frame;
      struct NR_PUSCH_TimeDomainResourceAllocation *pusch_timedomainresourceallocation_msg3 = CALLOC(1,sizeof(struct NR_PUSCH_TimeDomainResourceAllocation));
      pusch_timedomainresourceallocation_msg3->k2  = CALLOC(1,sizeof(long));
      *pusch_timedomainresourceallocation_msg3->k2 = nb_slots_per_period - DELTA[mu];
      if(*pusch_timedomainresourceallocation_msg3->k2 < min_fb_delay)
        *pusch_timedomainresourceallocation_msg3->k2 += nb_slots_per_period;
      AssertFatal(*pusch_timedomainresourceallocation_msg3->k2<33,"Computed k2 for msg3 %ld is larger than the range allowed by RRC (0..32)\n",
                  *pusch_timedomainresourceallocation_msg3->k2);
      pusch_timedomainresourceallocation_msg3->mappingType = NR_PUSCH_TimeDomainResourceAllocation__mappingType_typeB;
      pusch_timedomainresourceallocation_msg3->startSymbolAndLength = get_SLIV(14-ul_symb,ul_symb-1); // starting in fist ul symbol til the last but one
      ASN_SEQUENCE_ADD(&scc->uplinkConfigCommon->initialUplinkBWP->pusch_ConfigCommon->choice.setup->pusch_TimeDomainAllocationList->list,pusch_timedomainresourceallocation_msg3);
    }
  }
}


void set_dl_mcs_table(int scs, NR_UE_NR_Capability_t *cap,
                      NR_BWP_DownlinkDedicated_t *bwp_Dedicated,
                      NR_ServingCellConfigCommon_t *scc) {

  if (cap == NULL){
    bwp_Dedicated->pdsch_Config->choice.setup->mcs_Table = NULL;
    return;
  }

  int band = *scc->downlinkConfigCommon->frequencyInfoDL->frequencyBandList.list.array[0];
  struct NR_FrequencyInfoDL__scs_SpecificCarrierList scs_list = scc->downlinkConfigCommon->frequencyInfoDL->scs_SpecificCarrierList;
  int bw_rb = -1;
  for(int i=0; i<scs_list.list.count; i++){
    if(scs == scs_list.list.array[i]->subcarrierSpacing){
      bw_rb = scs_list.list.array[i]->carrierBandwidth;
      break;
    }
  }
  AssertFatal(bw_rb>0,"Could not find scs-SpecificCarrierList element for scs %d",scs);
  int bw = get_supported_band_index(scs, band, bw_rb);
  AssertFatal(bw>0,"Supported band corresponding to %d RBs not found\n", bw_rb);

  bool supported = false;
  if (band>256) {
    for (int i=0;i<cap->rf_Parameters.supportedBandListNR.list.count;i++) {
      NR_BandNR_t *bandNRinfo = cap->rf_Parameters.supportedBandListNR.list.array[i];
      if(bandNRinfo->bandNR == band && bandNRinfo->pdsch_256QAM_FR2) {
        supported = true;
        break;
      }
    }
  }
  else if (cap->phy_Parameters.phy_ParametersFR1 && cap->phy_Parameters.phy_ParametersFR1->pdsch_256QAM_FR1)
    supported = true;

  if (supported) {
    if(bwp_Dedicated->pdsch_Config->choice.setup->mcs_Table == NULL)
      bwp_Dedicated->pdsch_Config->choice.setup->mcs_Table = calloc(1, sizeof(*bwp_Dedicated->pdsch_Config->choice.setup->mcs_Table));
    *bwp_Dedicated->pdsch_Config->choice.setup->mcs_Table = NR_PDSCH_Config__mcs_Table_qam256;
  }
  else
    bwp_Dedicated->pdsch_Config->choice.setup->mcs_Table = NULL;
}



