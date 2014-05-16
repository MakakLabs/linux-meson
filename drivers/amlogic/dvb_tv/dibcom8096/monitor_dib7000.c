#include "component_monitor.h"

#define val(name) { printk("%-30s",name); }

#ifndef NO_DVBCHANNEL_BACKWARD_COMPAT
void dump_dvb_channel(struct dibDVBChannel *cd)
{
	switch (cd->nfft) {
		case 1:  printk("8K   "); break;
		case 2:  printk("4K   "); break;
		case 0:  printk("2K   "); break;
		default: printk("UNK  "); break;
	}

	printk("1/%-2i  ", 32 / (1 << cd->guard));

	switch (cd->nqam) {
		case 0:  printk("QPSK  "); break;
		case 1:  printk("16QAM "); break;
		case 2:  printk("64QAM "); break;
		default: printk("UNK   "); break;
	}

	printk("%i    ", cd->vit_hrch);
	printk("%i    ", cd->vit_alpha);
	printk("%i/%i ", cd->vit_code_rate_hp, cd->vit_code_rate_hp + 1);
	printk("%i/%i ", cd->vit_code_rate_lp, cd->vit_code_rate_lp + 1);
	printk("%s"    , cd->intlv_native ? "NATIVE" : "EXTENDED");
}
#endif

#ifdef CONFIG_STANDARD_DVBT
void dump_dvb_digital_channel(struct dibChannel *cd)
{
	switch (cd->u.dvbt.nfft) {
		case 1:  printk("8K   "); break;
		case 2:  printk("4K   "); break;
		case 0:  printk("2K   "); break;
		default: printk("UNK  "); break;
	}

	printk("1/%-2i  ", 32 / (1 << cd->u.dvbt.guard));

	switch (cd->u.dvbt.constellation) {
		case 0:  printk("QPSK  "); break;
		case 1:  printk("16QAM "); break;
		case 2:  printk("64QAM "); break;
		default: printk("UNK   "); break;
	}

	printk("%i    ", cd->u.dvbt.hrch);
	printk("%i    ", cd->u.dvbt.alpha);
	printk("%i/%i ", cd->u.dvbt.code_rate_hp, cd->u.dvbt.code_rate_hp + 1);
	printk("%i/%i ", cd->u.dvbt.code_rate_lp, cd->u.dvbt.code_rate_lp + 1);
	printk("%s"    , cd->u.dvbt.intlv_native ? "NATIVE" : "EXTENDED");
}

void dump_dvbsh_digital_channel(struct dibChannel *cd)
{
	switch (cd->u.dvbsh.dvb_common.nfft) {
		case 1:  printk("8K   "); break;
		case 2:  printk("4K   "); break;
		case 0:  printk("2K   "); break;
		case 3:  printk("1K   "); break;
		default: printk("UNK  "); break;
	}

	printk("1/%-2i  ", 32 / (1 << cd->u.dvbsh.dvb_common.guard));

	switch (cd->u.dvbsh.dvb_common.constellation) {
		case 0:  printk("QPSK  "); break;
		case 1:  printk("16QAM "); break;
		default: printk("UNK   "); break;
	}

	printk("%i    ", cd->u.dvbsh.dvb_common.hrch);
	printk("%i    ", cd->u.dvbsh.dvb_common.alpha);

    printk(" %s   ", DVBSH_CODERATE_TO_STRING(cd->u.dvbsh.dvb_common.code_rate_hp));
    printk("%s"    , DVBSH_CODERATE_TO_STRING(cd->u.dvbsh.dvb_common.code_rate_lp));

}
#endif

#ifdef CONFIG_STANDARD_ISDBT
void dump_isdbt_channel(struct dibChannel *cd)
{
    uint8_t i;

    switch (cd->u.isdbt.nfft) {
		case 1:  printk("8K     "); break;
		case 2:  printk("4K     "); break;
		case 0:  printk("2K     "); break;
		default: printk("UNK    "); break;
	}

	printk("1/%-2i      ", 32 / (1 << cd->u.isdbt.guard));
    printk("%i       ", cd->u.isdbt.sb_mode);
    printk("%i       ", cd->u.isdbt.partial_reception);
	for (i = 0; i<3; i++)
    {
        printk("%.2i    ", cd->u.isdbt.layer[i].nb_segments);

        switch (cd->u.isdbt.layer[i].constellation) {
		    case 0:  printk("QPSK   "); break;
		    case 1:  printk("16QAM  "); break;
		    case 2:  printk("64QAM  "); break;
            case 3 : printk("DQPSK  "); break;
		    default: printk("UNK    "); break;
        }
        printk("%i/%i    ", cd->u.isdbt.layer[i].code_rate, cd->u.isdbt.layer[i].code_rate + 1);
        printk("%i         ", cd->u.isdbt.layer[i].time_intlv);
	}
}
#endif

int channel_decoder_have_standard(struct dibChannelDecoderMonitor m[], int num, int st)
{
    do {
        num--;
        if (m[num].type == st)
            return 1;
    } while (num);
    return 0;
}

void channel_decoder_print_monitor(struct dibChannelDecoderMonitor m[], int num)
{
	return;
	
    int k=0;

    if (channel_decoder_have_standard(m, num, STANDARD_DVBSH)) {
        printk("\nSH info :          SH dem lock  per            iter        CRC header ok  tc_punct_id   Select_hp     nFFT    nqam     dvb_sh \n");
        printk("                   | SH lock    |       PS     |  overflow | pha_sh_frame |  pading_err | common_mult | hrch  | mux    |\n");
        for (k = 0; k < num; k++) {
            if (m[k].type == STANDARD_DVBSH)
                printk("Turbo decoder %d :  %d %d       %7.5f   %-5d  %-2d %d        %d %d            %-2d %-2d         %d %d           %d %d     %d 0x%04x %d\n", k, m[k].u.sh.misc, m[k].u.sh.sh_lock, m[k].u.sh.sh_per, m[k].u.sh.sh_PS, m[k].u.sh.sh_nb_iter, m[k].u.sh.sh_overflow, m[k].u.sh.sh_crc_header_ok, m[k].u.sh.sh_pha_sh_frame, m[k].u.sh.sh_tc_punct_id, m[k].u.sh.sh_padding_err, m[k].u.sh.sh_select_hp, m[k].u.sh.sh_common_mult , m[k].u.sh.sh_nfft , m[k].u.sh.sh_hrch,  m[k].u.sh.sh_Nqam, m[k].u.sh.mux, m[k].u.sh.dvb_sh);
        }
    }

    if (channel_decoder_have_standard(m, num, STANDARD_CMMB)) {
        printk("\nCMMB info :         slot num       per       iter/iter max     syndrome_first_iter    overflow_count   bloc_error      error flag     RS table count  RS table error count      RS per\n");
        for (k = 0; k < num; k++) {
            if (m[k].type == STANDARD_CMMB)
                printk("CMMB decoder %d :       %2d        %7.5f       %2d/%2d                     %d               %5d              %d                %d           %d                   %d               %f\n", k, m[k].u.cmmb.sl_num, m[k].u.cmmb.per, m[k].u.cmmb.nb_iteration, m[k].u.cmmb.nb_max_iteration, m[k].u.cmmb.syndrome_first_iter, m[k].u.cmmb.overflow_cnt, m[k].u.cmmb.bloc_error, m[k].u.cmmb.err_flag, m[k].u.cmmb.rs_table_count, m[k].u.cmmb.rs_table_error_count,  m[k].u.cmmb.rs_table_count ? (double)m[k].u.cmmb.rs_table_error_count / (double)m[k].u.cmmb.rs_table_count : 0);
        }
    }

    if (channel_decoder_have_standard(m, num, STANDARD_DVBT)) {
        printk("\n");
        printk("\n");
        val("Viterbi syndrome");
        for (k = 0; k < num; k++)
            printk("%-15i", m[k].u.dvbt.viterbi_syndrome);
        printk("\n");

        val("packet error count");
        for (k = 0; k < num; k++)
            printk("%-15i", m[k].u.dvbt.PacketErrors);
        printk("\n");

        val("packet error cumul");
        for (k = 0; k < num; k++)
            printk("%-15i", m[k].u.dvbt.PacketErrorCount);
        printk("\n");

        val("Bit Error Rate");
        for (k = 0; k < num; k++)
             printk("%-15g", m[k].u.dvbt.ber);
        printk("\n");

        val("Quasi Error Free");
        for (k = 0; k < num; k++)
            printk("%-15s", m[k].u.dvbt.ber < 2e-4 && m[k].u.dvbt.PacketErrors == 0 ? "OK" : "*** NOT OK ***");
        printk("\n");

        printk("\nchannel decoder locks:     fec_frm \n");
        printk("                       vit | fec_mpeg\n");
        for (k = 0; k < num; k++) {
            printk("channel decoder %2d:     %d  %d  %d\n",k,
                    m[k].u.dvbt.locks.vit, m[k].u.dvbt.locks.fec_frm, m[k].u.dvbt.locks.fec_mpeg/*, m[k].u.dvbt.viterbi_syndrome, m[k].u.dvbt.PacketErrors, m[k].u.dvbt.PacketErrorCount, m[k].u.dvbt.ber*/);
            printk("\n");
        }
    }

    if (channel_decoder_have_standard(m, num, STANDARD_ISDBT)) {
        printk("\n");
        printk("\n");
        val("Viterbi syndrome layer a");
        for (k = 0; k < num; k++)
            printk("%-15i", m[k].u.isdbt.viterbi_syndrome);
        printk("\n");
        val("Viterbi syndrome layer b");
        for (k = 0; k < num; k++)
            printk("%-15i", m[k].u.isdbt.viterbi_syndrome_b);
        printk("\n");
        val("Viterbi syndrome layer c");
        for (k = 0; k < num; k++)
            printk("%-15i", m[k].u.isdbt.viterbi_syndrome_c);
        printk("\n");

        val("packet error count A");
        for (k = 0; k < num; k++)
            printk("%-15i", m[k].u.isdbt.PacketErrors_A);
        printk("\n");
        val("packet error count B");
        for (k = 0; k < num; k++)
            printk("%-15i", m[k].u.isdbt.PacketErrors_B);
        printk("\n");
        val("packet error count C");
        for (k = 0; k < num; k++)
            printk("%-15i", m[k].u.isdbt.PacketErrors_C);
        printk("\n");


        val("packet error cumul");
        for (k = 0; k < num; k++)
        printk("%-15i", m[k].u.isdbt.PacketErrorCount);
        printk("\n");

        val("Bit Error Rate A");
        for (k = 0; k < num; k++)
            printk("%-15g", m[k].u.isdbt.berA);
        printk("\n");
        val("Bit Error Rate B");
        for (k = 0; k < num; k++)
            printk("%-15g", m[k].u.isdbt.berB);
        printk("\n");
        val("Bit Error Rate C");
        for (k = 0; k < num; k++)
            printk("%-15g", m[k].u.isdbt.berC);
        printk("\n");

        /*val("Quasi Error Free");
        for (k = 0; k < num; k++)
            printk("%-15s", m[k].u.isdbt.ber < 2e-4 && m[k].u.isdbt.PacketErrors == 0 ? "OK" : "*** NOT OK ***");
        printk("\n");*/

        printk("\nchannel decoder locks:            fec_mpeg(a b c) \n");
        printk("                       vit(a b c) | \n");
        for (k = 0; k < num; k++) {
            printk("channel decoder %2d:    %d %d %d       %d %d %d\n",k,
                    m[k].u.dvbt.locks.vit, m[k].u.isdbt.locks.vit_b, m[k].u.isdbt.locks.vit_c, m[k].u.isdbt.locks.fec_mpeg, m[k].u.isdbt.locks.fec_mpeg_b, m[k].u.isdbt.locks.fec_mpeg_c);
            printk("\n");
        }
    }

    if (channel_decoder_have_standard(m, num, STANDARD_DAB)) {

        printk("Viterbi syndrome : GLOBAL | FIC | MSC | SubId -> syndrome | ber \n");

        for (k = 0; k < num; k++) {
            if (m[k].type == STANDARD_DAB) {
                printk("                   %4d   %4d  %4d    %2d    -> %4d       %f\n", m[k].u.dab.syn,m[k].u.dab.syn_fic,m[k].u.dab.syn_msc, m[k].u.dab.subc_id, m[k].u.dab.syn_subc, m[k].u.dab.ber/1e8);
            }
        }
        printk("\nSubcId | Start addr | Size | Prev addr | Prev Size | Form | Table Index | L1punct | L2 punct |  PI1  |  PI2  | I  \n");

        for (k = 0; k < num; k++) {
            if (m[k].type == STANDARD_DAB) {
                printk("    %2d | %3d        | %3d  | %3d       | %3d       | %1d    | %2d          | %5d   |    %4d  |%4d   |%4d   | %4d  \n", m[k].u.dab.subc_id, m[k].u.dab.subc_staddr, m[k].u.dab.subc_sz, m[k].u.dab.subc_staddr_prev, m[k].u.dab.subc_sz_prev, m[k].u.dab.subc_form, m[k].u.dab.subc_index, m[k].u.dab.subc_L1punct, m[k].u.dab.subc_L2punct, m[k].u.dab.subc_PI1, m[k].u.dab.subc_PI2, m[k].u.dab.subc_I);
            }
        }
        printk("\n");
        printk("MPEG_lock     DMB_Lock_frm  Fec_Lock_frm     TimeDtlv_overflow  IsMSC          Recfg      Fec_mpfail\n");
        printk("| Fec_BER_RS  | Fec_mpcor   | Fec_ber_rscnt  | cif_counter      | Fec_rsnperr  | subc new | Fec_mpfail_cnt\n");


        for (k = 0; k < num; k++) {
            if (m[k].type == STANDARD_DAB) {
                printk("%1d %7d     %1d %2d          %1d %3d            %1d %3d              %1d %7d      %1d %1d        %1d %7d\n", m[k].u.dab.fec_lock_mpeg, m[k].u.dab.fec_ber_rs, m[k].u.dab.dmb_lock_frm, m[k].u.dab.fec_mpcor, m[k].u.dab.fec_lock_frm, m[k].u.dab.fec_ber_rscnt, m[k].u.dab.TimeDtlv_overflow, m[k].u.dab.cifcounter, m[k].u.dab.isMSC,  m[k].u.dab.fec_ber_rsnperr, m[k].u.dab.recfg, m[k].u.dab.subc_new, m[k].u.dab.fec_mpfail, m[k].u.dab.fec_mpfail_cnt);
            }
        }
    }
}

void mac_print_monitor(struct dibMacMonitor m_mac[], struct dibDemodMonitor m_demod[], int num)
{
    int k;

        printk("\nMAC info :  ADsample lock\n");
        printk("            | \n");
        for (k = 0; k < num; k++) {
            printk("Mac %d :     %d\n", k, m_mac[k].ad_sample_lock);
        }
}

int demods_have_standard(struct dibDemodMonitor m[], int num, int st)
{
    do {
        num--;
        if (m[num].cur_digital_channel.type == st)
            return 1;
    } while (num);
    return 0;
}



void dib7000_print_monitor(struct dibDemodMonitor m[], struct dibDemodStatistic s[], int stat_count, int num)
{
	return;
	
    int k,i;

#ifdef DIBCOM_EXTENDED_MONITORING
    if(!demods_have_standard(m, num, STANDARD_DAB)) {
        val("I adc power dBVrms");
        for (k = 0; k < num; k++)
            printk("%-15.2f", m[k].I_adc_power);
        printk("\n");

        val("Q adc power dBVrms");
        for (k = 0; k < num; k++)
            printk("%-15.2f", m[k].Q_adc_power);
        printk("\n");
    }

    val("DC offset on I");
    for (k = 0; k < num; k++)
        printk("%-15.2f", m[k].iqc_dcoff_i);
    printk("\n");

    val("DC offset on Q");
    for (k = 0; k < num; k++)
	printk("%-15.2f", m[k].iqc_dcoff_q);
    printk("\n");

    val("IQ gain mismatch (dB)");
    for (k = 0; k < num; k++)
        printk("%-15.2g", m[k].iq_gain_mismatch);
    printk("\n");

    val("IQ phase mismatch (degree)");
    for (k = 0; k < num; k++)
        printk("%-15.2g", m[k].iq_phase_mismatch);
    printk("\n");

    if (m[0].n_adc_mode <= 1 && !m[0].adc_monitor_only) {
        val("rf power dBm");
        for (k = 0; k < num; k++)
            printk("%-15.2g", m[k].rf_power_dbm);
        printk("\n");

        val("WBD:");
        for (k = 0; k < num; k++)
            printk("%-15.3g", m[k].agc_wbd);
        printk("\n");

        val("AGCglobal (norm)");
        for (k = 0; k < num; k++)
            printk("%-15.3g", m[k].rf_power/65536.0);
        printk("\n");

        val("AGC1 (norm)");
        for (k = 0; k < num; k++)
            printk("%-15.3g", m[k].agc1);
        printk("\n");

        val("AGC2 (norm)");
        for (k = 0; k < num; k++)
            printk("%-15.3g", m[k].agc2);
        printk("\n");
        val("AGC Split");
        for (k = 0; k < num; k++)
            printk("%-15d", m[k].agc_split);
        printk("\n");

        printk("\n");
    }
#ifdef CONFIG_STANDARD_DAB
    if(demods_have_standard(m, num, STANDARD_DAB)) {
        val("AGC power");
        for (k = 0; k < num; k++)
            printk("%-15.2f", m[k].agc_db);
        printk("\n");

        val("Power estimation dB");
        printk("%-15.2g", m[0].equal_signal_dB);
        printk("\n");

        val("Demod clock Hz");
        for (k = 0; k < num; k++)
            printk("%d", m[k].internal_clk);
        printk("\n");

        val("small/large freq offset");
        for (k = 0; k < num; k++)
            printk(" %-4.3g | %3d", m[k].dab.small_freqoff,m[k].dab.large_freqoff);
        printk("\n");
    }
#endif
        val("Carrier offset in Khz");
        for (k = 0; k < num; k++)
            printk("%-15.3g", m[k].carrier_offset);
        printk("\n");

    if (m[0].adc_monitor_only) {
        val("ADC LNPower");
        for (k = 0; k < num; k++)
            printk("%-15.3g", m[k].adc_lnpower);
        printk("\n");
    }

    if (m[0].n_adc_mode == 0) {
        val("Timing offset in ppm");
        for (k = 0; k < num; k++)
            printk("%-15.4g", m[k].timing_offset_ppm);
        printk("\n");

#ifdef CONFIG_STANDARD_ISDBT
        if (demods_have_standard(m, num, STANDARD_ISDBT)) {
            if (!m[0].do_not_display_chandec) {
                val("Viterbi syndrome layer a");
                for (k = 0; k < num; k++)
                    printk("%-15i", m[k].viterbi_syndrome);
                printk("\n");
                val("Viterbi syndrome layer b");
                for (k = 0; k < num; k++)
                    printk("%-15i", m[k].viterbi_syndrome_b);
                printk("\n");
                val("Viterbi syndrome layer c");
                for (k = 0; k < num; k++)
                    printk("%-15i", m[k].viterbi_syndrome_c);
                printk("\n");
            }

        }
#endif
#ifdef CONFIG_STANDARD_DVBT
        if (demods_have_standard(m, num, STANDARD_DVBT)) {
            if (!m[0].do_not_display_chandec) {
                val("Viterbi syndrome");
                for (k = 0; k < num; k++)
                    printk("%-15i", m[k].viterbi_syndrome);
                printk("\n");
            }
        }
#endif
#ifdef CONFIG_STANDARD_DAB
        if (demods_have_standard(m, num, STANDARD_DAB)) {
            if (!m[0].do_not_display_chandec) {
                val("\nTransmission Mode : ");
                switch (m[0].dab.tmode) {
                    case 0 : val("I"); break;
                    case 1 : val("II"); break;
                    case 2 : val("III"); break;
                    case 3 : val("IV"); break;
                }
                printk("\n");

                val("Viterbi syndrome GLOBAL | FIC | MSC : ");
                printk("  %d | %d | %d ", m[0].dab.syn,m[0].dab.syn_fic,m[0].dab.syn_msc);
                printk("\n");

                val("Viterbi syndrome SubId");
                for (k = 0; k < num; k++)
                    printk("%d syndrome = %d", m[0].dab.syn_subid, m[0].dab.syn_subc);
                printk("\n");
            }
        }
#endif
        val("C/N (dB)");
        for (k = 0; k < num; k++)
            printk("%-15.4g", m[k].CoN);
        printk("\n");

        if ( demods_have_standard(m, num, STANDARD_DVBT) || demods_have_standard(m, num, STANDARD_ISDBT) || demods_have_standard(m, num, STANDARD_DAB) ) {

#ifdef CONFIG_STANDARD_ISDBT
            if ((demods_have_standard(m, num, STANDARD_ISDBT)) && (m[0].can_display_ber_several_layers == 1))
            {
                if (!m[0].do_not_display_chandec) {
                    val("packet error count A");
                    for (k = 0; k < num; k++)
                        printk("%-15i", m[k].PacketErrors_A);
                    printk("\n");
                    val("packet error count B");
                    for (k = 0; k < num; k++)
                        printk("%-15i", m[k].PacketErrors_B);
                    printk("\n");
                    val("packet error count C");
                    for (k = 0; k < num; k++)
                        printk("%-15i", m[k].PacketErrors_C);
                    printk("\n");
                }
            }
            else
#endif
            if (!m[0].do_not_display_chandec) {

                val("packet error count");
                for (k = 0; k < num; k++)
                    printk("%-15i", m[k].PacketErrors);
                printk("\n");
            }

            if (!(demods_have_standard(m, num, STANDARD_ISDBT)) || (m[0].can_display_ber_several_layers != 1))
            {
                if (!m[0].do_not_display_chandec) {
                    val("packet error cumul");
                    for (k = 0; k < num; k++)
                    printk("%-15i", m[k].PacketErrorCount);
                    printk("\n");
                }
            }

#ifdef CONFIG_STANDARD_ISDBT
            if ((demods_have_standard(m, num, STANDARD_ISDBT)) && (m[0].can_display_ber_several_layers == 1) && (!m[0].do_not_display_chandec))
            {
                val("Bit Error Rate A");
                for (k = 0; k < num; k++)
                    printk("%-15g", m[k].berA);
                printk("\n");
                val("Bit Error Rate B");
                for (k = 0; k < num; k++)
                    printk("%-15g", m[k].berB);
                printk("\n");
                val("Bit Error Rate C");
                for (k = 0; k < num; k++)
                    printk("%-15g", m[k].berC);
                printk("\n");
            }
            else
#endif
            if (!m[0].do_not_display_chandec) {

                val("Bit Error Rate");
                for (k = 0; k < num; k++)
                    printk("%-15g", m[k].ber);
                printk("\n");
            }
        }

        if ( (demods_have_standard(m, num, STANDARD_DVBT)) || (demods_have_standard(m, num, STANDARD_ISDBT))) {
            if (!m[0].do_not_display_chandec) {
                val("MER");
                for (k = 0; k < num; k++)
                    printk("%-15g", m[k].mer);
                printk("\n");

                val("Quasi Error Free");
                for (k = 0; k < num; k++)
                    printk("%-15s", m[k].ber < 2e-4 && m[k].PacketErrors == 0 ? "OK" : "*** NOT OK ***");
                printk("\n");
            }
        } else if (demods_have_standard(m, num, STANDARD_DAB)) {
            val("Quasi Error Free : ber<1e-4");
            printk("%-15s", m[0].ber < 1e-4 && m[0].PacketErrors == 0 ? "OK" : "*** NOT OK ***");
            printk("\n");
        }
#ifdef CONFIG_STANDARD_ISDBT
            if (demods_have_standard(m, num, STANDARD_ISDBT)) {
                printk("                                            |            layer a          |           layer b           |          layer c            | \n");
                printk("demod channel: nfft  guard  sb_mode  p_rcpt   nb_seg  modu  crate t_intlv   nb_seg  modu  crate t_intlv   nb_seg  modu  crate t_intlv\n");
                for (k=0; k < num; k++) {
                    printk("demod %2d     : ",k);
                    dump_isdbt_channel(&m[k].cur_digital_channel);
                    printk("\n");
                }
            }
#endif
#ifdef CONFIG_STANDARD_DVBT
            if (demods_have_standard(m, num, STANDARD_DVBT)) {
                printk("demod channel: nfft guard nqam  hrch alpha crhp crlp intlv\n");
                for (k=0; k < num; k++) {
                    printk("demod %2d:      ",k);
                    dump_dvb_digital_channel(&m[k].cur_digital_channel);
                    printk("\n");
                }
            } else if (demods_have_standard(m, num, STANDARD_DVBSH)) {
                printk("demod channel: nfft guard nqam  hrch alpha crhp      crlp\n");
                for (k=0; k < num; k++) {
                    printk("demod %2d:      ",k);
                    dump_dvbsh_digital_channel(&m[k].cur_digital_channel);
                    printk("\n");
                }
            }
#endif
        printk("\n");
#ifdef CONFIG_STANDARD_ISDBT
        if (demods_have_standard(m, num, STANDARD_ISDBT)) {
            if (!m[0].do_not_display_chandec) {
                printk("demod locks:  agc    coff        lmod4  equal      tmcc_sync   dvsy\n");
                printk("              | corm | coff_cpil | pha3 | tmcc_dec | tmcc_data |  vit(a,b,c)  fec_mpeg(a,b,c) \n");
                for (k = 0; k < num; k++)
                    printk("demod %2d:     %d %d    %d %d         %d %d    %d %d        %d %d         %d  %d%d%d         %d%d%d\n",k,
                            m[k].locks.agc, m[k].locks.corm, m[k].locks.coff, m[k].locks.coff_cpil, m[k].locks.lmod4, m[k].locks.pha3, m[k].locks.equal,m[k].locks.tmcc_dec, m[k].locks.tmcc_sync, m[k].locks.tmcc_data, m[k].locks.dvsy, m[k].locks.vit, m[k].locks.vit_b,m[k].locks.vit_c, m[k].locks.fec_mpeg,m[k].locks.fec_mpeg_b,m[k].locks.fec_mpeg_c);
                printk("\n");
            }
        }
#endif
#ifdef CONFIG_STANDARD_DVBT
        if (demods_have_standard(m, num, STANDARD_DVBT)) {
            for (k = 0; k < num; k++) {
                if (!m[k].do_not_display_chandec) {
                    printk("demod locks:  agc    coff        lmod4  equal fec_frm    tps_dec    tps_data     dvsy\n");
                    printk("              | corm | coff_cpil | pha3 | vit | fec_mpeg | tps_sync | tps_cellid |\n");
                    break;
                }
            }
            for (k = 0; k < num; k++) {
                if (!m[k].do_not_display_chandec) {
                    printk("demod %2d:     %d %d    %d %d         %d %d    %d %d   %d %d        %d %d        %d %d          %d\n",k,
                            m[k].locks.agc, m[k].locks.corm, m[k].locks.coff, m[k].locks.coff_cpil, m[k].locks.lmod4, m[k].locks.pha3, m[k].locks.equal, m[k].locks.vit, m[k].locks.fec_frm, m[k].locks.fec_mpeg, m[k].locks.tps_dec, m[k].locks.tps_sync, m[k].locks.tps_data, m[k].locks.tps_cellid, m[k].locks.dvsy);
                    printk("\n");
                }
            }
        }
        else if (demods_have_standard(m, num, STANDARD_DVBSH)) {
            printk("demod locks:  agc    coff        lmod4  equal tps_dec    tps_data     dvsy\n");
            printk("              | corm | coff_cpil | pha3 |     | tps_sync | tps_cellid |\n");
            for (k = 0; k < num; k++)
                printk("demod %2d:     %d %d    %d %d         %d %d    %d     %d %d        %d %d          %d\n",k,
                        m[k].locks.agc, m[k].locks.corm, m[k].locks.coff, m[k].locks.coff_cpil, m[k].locks.lmod4, m[k].locks.pha3, m[k].locks.equal,  m[k].locks.tps_dec, m[k].locks.tps_sync, m[k].locks.tps_data, m[k].locks.tps_cellid, m[k].locks.dvsy);
            printk("\n");
        }
#endif
#ifdef CONFIG_STANDARD_DAB
         if (demods_have_standard(m, num, STANDARD_DAB)) {
            if (!m[0].do_not_display_chandec) {
                val("Nonlin quant | reverse quant | full internal : ");
                for (k = 0; k < num; k++)
                    printk("   %dbit | %dbit | %d", m[k].dab.nb_bit_quant, m[k].dab.nb_bit_reverse_quant,m[k].dab.tdint_full_internal);
                printk("\n");

                printk("DAB locks:   dmb_lock_frm  ndec_tmode    fec_mpeg   dabcoff_lock\n");
                printk("             | corm_dmb    | ndec_frame  | fec_frm  |\n");
                for (k = 0; k < num; k++)
                    printk("demod %2d:    %d %d           %d %d           %d %d        %d\n",k,m[k].dab.dmb_lock_frm, m[k].locks.corm_lock_dmb, m[k].locks.ndec_tmode_lock, m[k].locks.ndec_frame_lock ,m[k].locks.fec_mpeg, m[k].locks.fec_frm, m[k].locks.dabcoff_lock);

                printk("SubChan :     subc_id  recfg      cifcounter  subc_sz         sz_prev   index      L2punct    PI2\n");
                printk("              |  isMSC | subc_new |  staddr   |   staddr_prev |   form  | L1punct  |   PI1    | I\n");

                for (k = 0; k < num; k++)
                printk("demod %2d:    %2d  %d     %d %d       %3d %d       %d  %d          %d  %d     %d %d   %d  %d      %d %d\n",k,m[k].dab.subc_id, m[k].dab.isMSC, m[k].dab.recfg, m[k].dab.subc_new, m[k].dab.cifcounter, m[k].dab.subc_staddr, m[k].dab.subc_sz, m[k].dab.subc_staddr_prev, m[k].dab.subc_sz_prev, m[k].dab.subc_form, m[k].dab.subc_index, m[k].dab.subc_L1punct, m[k].dab.subc_L2punct, m[k].dab.subc_PI1, m[k].dab.subc_PI2, m[k].dab.subc_I);

                printk("\nFEC id    ");
                for (i = 0 ; i < 12 ; i++)
                    printk("%2d  ",i);
                printk("\n");

                for (k = 0; k < num; k++) {
                    printk("%d STATE:  ",k);
                    for (i = 0 ; i < 12 ; i++)
                        printk("%2d  ",(m[k].dab.fec_state >> i)&0x1);
                    printk("\n");

                    printk("%d SubId:  ",k);
                    for (i = 0 ; i < 12 ; i++)
                        printk("%2d  ",m[k].dab.fec_subid[i]);
                    printk("\n\n");
                }
            } else {
                printk("DAB locks:   signal_detect\n");
                for (k = 0; k < num; k++)
                    printk("demod %2d:    %d\n",k, m[k].locks.coff);
            }

        }
#endif
        if (m[0].n_2dpa_monitoring) {
            val("mu_int/num_fir");
            for (k = 0; k < num; k++)
                printk("%d/%d%15s", m[k].n_2dpa_mu_int, m[k].n_2dpa_num_fir, " ");
            printk("\n");

            for (i = 0; i < 4; i++) {
                printk("cti_def%d%20s",i," ");
                for (k = 0; k < num; k++)
                    printk("%.2f (%5d)   ", m[k].cti_def[i], m[k].cti_def_raw[i]);
                printk("\n");
            }
            printk("\n");
        }

        val("Divin FIFO overflow");
        for (k = 0; k < num; k++)
            printk("%-15d",m[k].divin_fifo_overflow);
        printk("\n");

        val("Divin error symbols");
        for (k = 0; k < num; k++)
            printk("%-15d",m[k].divin_nberr_sym);
        printk("\n");

        val("Dvsy first arrived");
        for (k = 0; k < num; k++)
            printk("%-15d",m[k].dvsy_first_arrived);
        printk("\n");

        val("Dvsy delay");
        for (k = 0; k < num; k++)
            printk("%-15d",m[k].dvsy_delay);
        printk("\n");

        val("Divin OK (Way 0, 1)");
        for (k = 0; k < num; k++)
            printk("%d,%d%12s",m[k].way0_ok,m[k].way1_ok,"");
        printk("\n");

        val("Inversion"); //, DDS, (msb,lsb)");
        for (k = 0; k < num; k++)
            printk("%-15d",m[k].invspec);
        printk("\n");

        val("p DDS Freq");
        for (k = 0; k < num; k++)
            printk("%-15g",m[k].viewable_dds_freq);
        printk("\n");

        if (stat_count > 0) {
            printk("\nDiB7000 statistics each time after %d monitor iterations\n",stat_count);
            val("C/N (linear mean)");
            for (k = 0; k < num; k++)
                printk("%-15g", s[k].CoN_lin_mean);
            printk("\n");

            val("C/N (logarithmic mean)");
            for (k = 0; k < num; k++)
                printk("%-15g", s[k].CoN_log_mean);
            printk("\n");

            val("C/N (min)");
            for (k = 0; k < num; k++)
                printk("%-15g", s[k].CoN_min);
            printk("\n");

            val("C/N (max)");
            for (k = 0; k < num; k++)
                printk("%-15g", s[k].CoN_max);
            printk("\n\n");
        }
    }
#else
    if(!demods_have_standard(m, num, STANDARD_DAB)) {
        val("I adc power dBVrms");
        for (k = 0; k < num; k++)
            printk("0x%-13x", m[k].adc_power_i);
        printk("\n");

        val("Q adc power dBVrms");
        for (k = 0; k < num; k++)
            printk("0x%-13x", m[k].adc_power_q);
        printk("\n");
    }

    val("DC offset on I");
    for (k = 0; k < num; k++)
        printk("0x%-13x", 0);
    printk("\n");

    val("DC offset on Q");
    for (k = 0; k < num; k++)
	printk("0x%-13x", 0);
    printk("\n");

    val("IQ gain mismatch (dB)");
    for (k = 0; k < num; k++)
        printk("0x%-13x", m[k].iq_misgain);
    printk("\n");

    val("IQ phase mismatch (degree)");
    for (k = 0; k < num; k++)
        printk("0x%-13x", m[k].iq_misphi);
    printk("\n");

    if (m[0].n_adc_mode <= 1 && !m[0].adc_monitor_only) {
        val("rf power dBm");
        for (k = 0; k < num; k++)
            printk("0x%-13x", m[k].rf_power);
        printk("\n");

        val("WBD:");
        for (k = 0; k < num; k++)
            printk("0x%-13x", m[k].agc_wbd_raw);
        printk("\n");

        val("AGCglobal (norm)");
        for (k = 0; k < num; k++)
            printk("0x%-13x", m[k].rf_power);
        printk("\n");

        val("AGC1 (norm)");
        for (k = 0; k < num; k++)
            printk("0x%-13x", m[k].agc1_raw);
        printk("\n");

        val("AGC2 (norm)");
        for (k = 0; k < num; k++)
            printk("0x%-13x", m[k].agc2_raw);
        printk("\n");
        val("AGC Split");
        for (k = 0; k < num; k++)
            printk("%-15d", m[k].agc_split);
        printk("\n");

        printk("\n");
    }

#ifdef CONFIG_STANDARD_DAB
    if(demods_have_standard(m, num, STANDARD_DAB)) {
        val("AGC power");
        for (k = 0; k < num; k++)
            printk("0x%-13x", m[k].rf_power);
        printk("\n");

        val("Power estimation dB");
        printk("0x%-4x*2^0x%-4x", m[0].equal_signal_mant, m[0].equal_signal_exp);
        printk("\n");

        val("Demod clock Hz");
        for (k = 0; k < num; k++)
            printk("%d", m[k].internal_clk);
        printk("\n");

        val("small/large freq offset");
        for (k = 0; k < num; k++)
            printk(" 0 | %3d", m[k].dab.large_freqoff);
        printk("\n");
    }
#endif
    val("Carrier offset");
    for (k = 0; k < num; k++)
        printk("0x%-13x", ABS(m[k].dds_freq-m[k].p_dds_freq));
    printk("\n");

    if (m[0].adc_monitor_only) {
        val("ADC LNPower");
        for (k = 0; k < num; k++)
            printk("0x%-13x", m[k].adc_lnpower_raw);
        printk("\n");
    }

    if (m[0].n_adc_mode == 0) {
        val("timf");
        for (k = 0; k < num; k++)
            printk("0x%-13x", m[k].timf_current);
        printk("\n");

#ifdef CONFIG_STANDARD_ISDBT
        if (demods_have_standard(m, num, STANDARD_ISDBT)) {
            if (!m[0].do_not_display_chandec) {
                val("Viterbi syndrome layer a");
                for (k = 0; k < num; k++)
                    printk("%-15i", m[k].viterbi_syndrome);
                printk("\n");
                val("Viterbi syndrome layer b");
                for (k = 0; k < num; k++)
                    printk("%-15i", m[k].viterbi_syndrome_b);
                printk("\n");
                val("Viterbi syndrome layer c");
                for (k = 0; k < num; k++)
                    printk("%-15i", m[k].viterbi_syndrome_c);
                printk("\n");
            }

        }
#endif
#ifdef CONFIG_STANDARD_DVBT
        if (demods_have_standard(m, num, STANDARD_DVBT)) {
            if (!m[0].do_not_display_chandec) {
                val("Viterbi syndrome");
                for (k = 0; k < num; k++)
                    printk("%-15i", m[k].viterbi_syndrome);
                printk("\n");
            }
        }
#endif
#ifdef CONFIG_STANDARD_DAB
        if (demods_have_standard(m, num, STANDARD_DAB)) {
            if (!m[0].do_not_display_chandec) {
                val("\nTransmission Mode : ");
                switch (m[0].dab.tmode) {
                    case 0 : val("I"); break;
                    case 1 : val("II"); break;
                    case 2 : val("III"); break;
                    case 3 : val("IV"); break;
                }
                printk("\n");

                val("Viterbi syndrome GLOBAL | FIC | MSC : ");
                printk("  %d | %d | %d ", m[0].dab.syn,m[0].dab.syn_fic,m[0].dab.syn_msc);
                printk("\n");

                val("Viterbi syndrome SubId");
                for (k = 0; k < num; k++)
                    printk("%d syndrome = %d", m[0].dab.syn_subid, m[0].dab.syn_subc);
                printk("\n");
            }
        }
#endif
        val("signal");
        for (k = 0; k < num; k++)
            printk("0x%-4x*2^0x%-4x", m[0].equal_signal_mant, m[0].equal_signal_exp);
        printk("\n");
        val("noise");
        for (k = 0; k < num; k++)
            printk("0x%-4x*2^0x%-4x", m[0].equal_noise_mant, m[0].equal_noise_exp);
        printk("\n");

        if ( demods_have_standard(m, num, STANDARD_DVBT) || demods_have_standard(m, num, STANDARD_ISDBT) || demods_have_standard(m, num, STANDARD_DAB) ) {

#ifdef CONFIG_STANDARD_ISDBT
            if ((demods_have_standard(m, num, STANDARD_ISDBT)) && (m[0].can_display_ber_several_layers == 1))
            {
                if (!m[0].do_not_display_chandec) {
                    val("packet error count A");
                    for (k = 0; k < num; k++)
                        printk("%-15i", m[k].PacketErrors_A);
                    printk("\n");
                    val("packet error count B");
                    for (k = 0; k < num; k++)
                        printk("%-15i", m[k].PacketErrors_B);
                    printk("\n");
                    val("packet error count C");
                    for (k = 0; k < num; k++)
                        printk("%-15i", m[k].PacketErrors_C);
                    printk("\n");
                }
            }
            else
#endif
            if (!m[0].do_not_display_chandec) {

                val("packet error count");
                for (k = 0; k < num; k++)
                    printk("%-15i", m[k].PacketErrors);
                printk("\n");
            }

            if (!(demods_have_standard(m, num, STANDARD_ISDBT)) || (m[0].can_display_ber_several_layers != 1))
            {
                if (!m[0].do_not_display_chandec) {
                    val("packet error cumul");
                    for (k = 0; k < num; k++)
                    printk("%-15i", m[k].PacketErrorCount);
                    printk("\n");
                }
            }

#ifdef CONFIG_STANDARD_ISDBT
            if ((demods_have_standard(m, num, STANDARD_ISDBT)) && (m[0].can_display_ber_several_layers == 1) && (!m[0].do_not_display_chandec))
            {
                val("Bit Error Rate A");
                for (k = 0; k < num; k++)
                    printk("0x%-13x", m[k].ber_raw_A);
                printk("\n");
                val("Bit Error Rate B");
                for (k = 0; k < num; k++)
                    printk("0x%-13x", m[k].ber_raw_B);
                printk("\n");
                val("Bit Error Rate C");
                for (k = 0; k < num; k++)
                    printk("0x%-13x", m[k].ber_raw_C);
                printk("\n");
            }
            else
#endif
            if (!m[0].do_not_display_chandec) {

                val("Bit Error Rate");
                for (k = 0; k < num; k++)
                    printk("0x%-13x", m[k].ber_raw);
                printk("\n");
            }
        }

        if ( (demods_have_standard(m, num, STANDARD_DVBT)) || (demods_have_standard(m, num, STANDARD_ISDBT))) {
            if (!m[0].do_not_display_chandec) {
                val("MER");
                for (k = 0; k < num; k++)
                    printk("0x%-4x*2^0x%-4x", m[k].mer_mant, m[k].mer_exp);
                printk("\n");

                val("Quasi Error Free");
                for (k = 0; k < num; k++)
                    printk("%-15s", m[k].ber < 2e-4 && m[k].PacketErrors == 0 ? "OK" : "*** NOT OK ***");
                printk("\n");
            }
        } else if (demods_have_standard(m, num, STANDARD_DAB)) {
            val("Quasi Error Free : ber<1e-4");
            printk("%-15s", m[0].ber_raw < 1e4 && m[0].PacketErrors == 0 ? "OK" : "*** NOT OK ***");
            printk("\n");
        }
#ifdef CONFIG_STANDARD_ISDBT
            if (demods_have_standard(m, num, STANDARD_ISDBT)) {
                printk("                                            |            layer a          |           layer b           |          layer c            | \n");
                printk("demod channel: nfft  guard  sb_mode  p_rcpt   nb_seg  modu  crate t_intlv   nb_seg  modu  crate t_intlv   nb_seg  modu  crate t_intlv\n");
                for (k=0; k < num; k++) {
                    printk("demod %2d     : ",k);
                    dump_isdbt_channel(&m[k].cur_digital_channel);
                    printk("\n");
                }
            }
#endif
#ifdef CONFIG_STANDARD_DVBT
            if (demods_have_standard(m, num, STANDARD_DVBT)) {
                printk("demod channel: nfft guard nqam  hrch alpha crhp crlp intlv\n");
                for (k=0; k < num; k++) {
                    printk("demod %2d:      ",k);
                    dump_dvb_digital_channel(&m[k].cur_digital_channel);
                    printk("\n");
                }
            } else if (demods_have_standard(m, num, STANDARD_DVBSH)) {
                printk("demod channel: nfft guard nqam  hrch alpha crhp      crlp\n");
                for (k=0; k < num; k++) {
                    printk("demod %2d:      ",k);
                    dump_dvbsh_digital_channel(&m[k].cur_digital_channel);
                    printk("\n");
                }
            }
#endif
        printk("\n");
#ifdef CONFIG_STANDARD_ISDBT
        if (demods_have_standard(m, num, STANDARD_ISDBT)) {
            if (!m[0].do_not_display_chandec) {
                printk("demod locks:  agc    coff        lmod4  equal      tmcc_sync   dvsy\n");
                printk("              | corm | coff_cpil | pha3 | tmcc_dec | tmcc_data |  vit(a,b,c)  fec_mpeg(a,b,c) \n");
                for (k = 0; k < num; k++)
                    printk("demod %2d:     %d %d    %d %d         %d %d    %d %d        %d %d         %d  %d%d%d         %d%d%d\n",k,
                            m[k].locks.agc, m[k].locks.corm, m[k].locks.coff, m[k].locks.coff_cpil, m[k].locks.lmod4, m[k].locks.pha3, m[k].locks.equal,m[k].locks.tmcc_dec, m[k].locks.tmcc_sync, m[k].locks.tmcc_data, m[k].locks.dvsy, m[k].locks.vit, m[k].locks.vit_b,m[k].locks.vit_c, m[k].locks.fec_mpeg,m[k].locks.fec_mpeg_b,m[k].locks.fec_mpeg_c);
                printk("\n");
            }
        }
#endif
#ifdef CONFIG_STANDARD_DVBT
        if (demods_have_standard(m, num, STANDARD_DVBT)) {
            for (k = 0; k < num; k++) {
                if (!m[k].do_not_display_chandec) {
                    printk("demod locks:  agc    coff        lmod4  equal fec_frm    tps_dec    tps_data     dvsy\n");
                    printk("              | corm | coff_cpil | pha3 | vit | fec_mpeg | tps_sync | tps_cellid |\n");
                    break;
                }
            }
            for (k = 0; k < num; k++) {
                if (!m[k].do_not_display_chandec) {
                    printk("demod %2d:     %d %d    %d %d         %d %d    %d %d   %d %d        %d %d        %d %d          %d\n",k,
                            m[k].locks.agc, m[k].locks.corm, m[k].locks.coff, m[k].locks.coff_cpil, m[k].locks.lmod4, m[k].locks.pha3, m[k].locks.equal, m[k].locks.vit, m[k].locks.fec_frm, m[k].locks.fec_mpeg, m[k].locks.tps_dec, m[k].locks.tps_sync, m[k].locks.tps_data, m[k].locks.tps_cellid, m[k].locks.dvsy);
                    printk("\n");
                }
            }
        }
        else if (demods_have_standard(m, num, STANDARD_DVBSH)) {
            printk("demod locks:  agc    coff        lmod4  equal tps_dec    tps_data     dvsy\n");
            printk("              | corm | coff_cpil | pha3 |     | tps_sync | tps_cellid |\n");
            for (k = 0; k < num; k++)
                printk("demod %2d:     %d %d    %d %d         %d %d    %d     %d %d        %d %d          %d\n",k,
                        m[k].locks.agc, m[k].locks.corm, m[k].locks.coff, m[k].locks.coff_cpil, m[k].locks.lmod4, m[k].locks.pha3, m[k].locks.equal,  m[k].locks.tps_dec, m[k].locks.tps_sync, m[k].locks.tps_data, m[k].locks.tps_cellid, m[k].locks.dvsy);
            printk("\n");
        }
#endif
#ifdef CONFIG_STANDARD_DAB
         if (demods_have_standard(m, num, STANDARD_DAB)) {
            if (!m[0].do_not_display_chandec) {
                val("Nonlin quant | reverse quant | full internal : ");
                for (k = 0; k < num; k++)
                    printk("   %dbit | %dbit | %d", m[k].dab.nb_bit_quant, m[k].dab.nb_bit_reverse_quant,m[k].dab.tdint_full_internal);
                printk("\n");

                printk("DAB locks:   dmb_lock_frm  ndec_tmode    fec_mpeg   dabcoff_lock\n");
                printk("             | corm_dmb    | ndec_frame  | fec_frm  |\n");
                for (k = 0; k < num; k++)
                    printk("demod %2d:    %d %d           %d %d           %d %d        %d\n",k,m[k].dab.dmb_lock_frm, m[k].locks.corm_lock_dmb, m[k].locks.ndec_tmode_lock, m[k].locks.ndec_frame_lock ,m[k].locks.fec_mpeg, m[k].locks.fec_frm, m[k].locks.dabcoff_lock);

                printk("SubChan :     subc_id  recfg      cifcounter  subc_sz         sz_prev   index      L2punct    PI2\n");
                printk("              |  isMSC | subc_new |  staddr   |   staddr_prev |   form  | L1punct  |   PI1    | I\n");

                for (k = 0; k < num; k++)
                printk("demod %2d:    %2d  %d     %d %d       %3d %d       %d  %d          %d  %d     %d %d   %d  %d      %d %d\n",k,m[k].dab.subc_id, m[k].dab.isMSC, m[k].dab.recfg, m[k].dab.subc_new, m[k].dab.cifcounter, m[k].dab.subc_staddr, m[k].dab.subc_sz, m[k].dab.subc_staddr_prev, m[k].dab.subc_sz_prev, m[k].dab.subc_form, m[k].dab.subc_index, m[k].dab.subc_L1punct, m[k].dab.subc_L2punct, m[k].dab.subc_PI1, m[k].dab.subc_PI2, m[k].dab.subc_I);

                printk("\nFEC id    ");
                for (i = 0 ; i < 12 ; i++)
                    printk("%2d  ",i);
                printk("\n");

                for (k = 0; k < num; k++) {
                    printk("%d STATE:  ",k);
                    for (i = 0 ; i < 12 ; i++)
                        printk("%2d  ",(m[k].dab.fec_state >> i)&0x1);
                    printk("\n");

                    printk("%d SubId:  ",k);
                    for (i = 0 ; i < 12 ; i++)
                        printk("%2d  ",m[k].dab.fec_subid[i]);
                    printk("\n\n");
                }
            } else {
                printk("DAB locks:   signal_detect\n");
                for (k = 0; k < num; k++)
                    printk("demod %2d:    %d\n",k, m[k].locks.coff);
            }

        }
#endif
        if (m[0].n_2dpa_monitoring) {
            val("mu_int/num_fir");
            for (k = 0; k < num; k++)
                printk("%d/%d%15s", m[k].n_2dpa_mu_int, m[k].n_2dpa_num_fir, " ");
            printk("\n");

            for (i = 0; i < 4; i++) {
                printk("cti_def%d%20s",i," ");
                for (k = 0; k < num; k++)
                    printk("     (%5d)   ", m[k].cti_def_raw[i]);
                printk("\n");
            }
            printk("\n");
        }

        val("Divin FIFO overflow");
        for (k = 0; k < num; k++)
            printk("%-15d",m[k].divin_fifo_overflow);
        printk("\n");

        val("Divin error symbols");
        for (k = 0; k < num; k++)
            printk("%-15d",m[k].divin_nberr_sym);
        printk("\n");

        val("Dvsy first arrived");
        for (k = 0; k < num; k++)
            printk("%-15d",m[k].dvsy_first_arrived);
        printk("\n");

        val("Dvsy delay");
        for (k = 0; k < num; k++)
            printk("%-15d",m[k].dvsy_delay);
        printk("\n");

        val("Divin OK (Way 0, 1)");
        for (k = 0; k < num; k++)
            printk("%d,%d%12s",m[k].way0_ok,m[k].way1_ok,"");
        printk("\n");

        val("Inversion"); //, DDS, (msb,lsb)");
        for (k = 0; k < num; k++)
            printk("%-15d",m[k].invspec);
        printk("\n");

        val("p DDS Freq");
        for (k = 0; k < num; k++)
            printk("0x%-13x",m[k].p_dds_freq);
        printk("\n");

    }
#endif
}
