/*!
 * \file glonass_l1_ca_dll_pll_tracking_cc.h
 * \brief  Implementation of a code DLL + carrier PLL tracking block
 * \author Gabriel Araujo, 2017. gabriel.araujo.5000(at)gmail.com
 * \author Luis Esteve, 2017. luis(at)epsilon-formacion.com
 * \author Damian Miralles, 2017. dmiralles2009(at)gmail.com
 *
 *
 * Code DLL + carrier PLL according to the algorithms described in:
 * K.Borre, D.M.Akos, N.Bertelsen, P.Rinder, and S.H.Jensen,
 * A Software-Defined GPS and Galileo Receiver. A Single-Frequency
 * Approach, Birkha user, 2007
 *
 * -----------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2020  (see AUTHORS file for a list of contributors)
 *
 * GNSS-SDR is a software defined Global Navigation
 *          Satellite Systems receiver
 *
 * This file is part of GNSS-SDR.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * -----------------------------------------------------------------------------
 */

#ifndef GNSS_SDR_GLONASS_L1_CA_DLL_PLL_TRACKING_CC_H
#define GNSS_SDR_GLONASS_L1_CA_DLL_PLL_TRACKING_CC_H

#include "cpu_multicorrelator.h"
#include "gnss_block_interface.h"
#include "gnss_synchro.h"
#include "tracking_2nd_DLL_filter.h"
#include "tracking_2nd_PLL_filter.h"
#include <gnuradio/block.h>
#include <volk_gnsssdr/volk_gnsssdr_alloc.h>  // for volk_gnsssdr::vector
#include <fstream>
#include <map>
#include <string>


/** \addtogroup Tracking
 * \{ */
/** \addtogroup Tracking_gnuradio_blocks
 * \{ */


class Glonass_L1_Ca_Dll_Pll_Tracking_cc;

using glonass_l1_ca_dll_pll_tracking_cc_sptr = gnss_shared_ptr<Glonass_L1_Ca_Dll_Pll_Tracking_cc>;

glonass_l1_ca_dll_pll_tracking_cc_sptr
glonass_l1_ca_dll_pll_make_tracking_cc(
    int64_t fs_in, uint32_t vector_length,
    bool dump,
    const std::string& dump_filename,
    float pll_bw_hz,
    float dll_bw_hz,
    float early_late_space_chips);


/*!
 * \brief This class implements a DLL + PLL tracking loop block
 */
class Glonass_L1_Ca_Dll_Pll_Tracking_cc : public gr::block
{
public:
    ~Glonass_L1_Ca_Dll_Pll_Tracking_cc();

    void set_channel(uint32_t channel);
    void set_gnss_synchro(Gnss_Synchro* p_gnss_synchro);
    void start_tracking();

    int general_work(int noutput_items, gr_vector_int& ninput_items,
        gr_vector_const_void_star& input_items, gr_vector_void_star& output_items);

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

private:
    friend glonass_l1_ca_dll_pll_tracking_cc_sptr
    glonass_l1_ca_dll_pll_make_tracking_cc(
        int64_t fs_in, uint32_t vector_length,
        bool dump,
        const std::string& dump_filename,
        float pll_bw_hz,
        float dll_bw_hz,
        float early_late_space_chips);

    Glonass_L1_Ca_Dll_Pll_Tracking_cc(
        int64_t fs_in, uint32_t vector_length,
        bool dump,
        const std::string& dump_filename,
        float pll_bw_hz,
        float dll_bw_hz,
        float early_late_space_chips);

    void check_carrier_phase_coherent_initialization();

    int32_t save_matfile() const;

    volk_gnsssdr::vector<gr_complex> d_ca_code;
    volk_gnsssdr::vector<float> d_local_code_shift_chips;
    volk_gnsssdr::vector<gr_complex> d_correlator_outs;
    volk_gnsssdr::vector<gr_complex> d_Prompt_buffer;

    Cpu_Multicorrelator multicorrelator_cpu;

    // PLL and DLL filter library
    Tracking_2nd_DLL_filter d_code_loop_filter;
    Tracking_2nd_PLL_filter d_carrier_loop_filter;

    Gnss_Synchro* d_acquisition_gnss_synchro;

    // file dump
    std::string d_dump_filename;
    std::ofstream d_dump_file;

    std::map<std::string, std::string> systemName;
    std::string sys;

    // tracking configuration vars
    int64_t d_fs_in;
    int64_t d_glonass_freq_ch;
    double d_early_late_spc_chips;
    uint32_t d_vector_length;
    uint32_t d_channel;

    // remaining code phase and carrier phase between tracking loops
    double d_rem_code_phase_samples;
    double d_rem_code_phase_chips;
    float d_rem_carr_phase_rad;

    // acquisition
    double d_acq_code_phase_samples;
    double d_acq_carrier_doppler_hz;

    // tracking vars
    double d_code_freq_chips;
    double d_code_phase_step_chips;
    double d_carrier_doppler_hz;
    double d_carrier_doppler_phase_step_rad;
    double d_carrier_frequency_hz;
    double d_carrier_phase_step_rad;
    double d_acc_carrier_phase_rad;
    double d_code_phase_samples;

    // correlator
    int32_t d_n_correlator_taps;

    // PRN period in samples
    int32_t d_current_prn_length_samples;

    // processing samples counters
    uint64_t d_sample_counter;
    uint64_t d_acq_sample_stamp;

    // CN0 estimation and lock detector
    double d_carrier_lock_test;
    double d_CN0_SNV_dB_Hz;
    double d_carrier_lock_threshold;
    int32_t d_carrier_lock_fail_counter;
    int32_t d_cn0_estimation_counter;

    // control vars
    bool d_enable_tracking;
    bool d_pull_in;
    bool d_acc_carrier_phase_initialized;

    bool d_dump;
};


/** \} */
/** \} */
#endif  // GNSS_SDR_GLONASS_L1_CA_DLL_PLL_TRACKING_CC_H
