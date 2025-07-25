/* Copyright (C)
* 2015 - John Melton, G0ORX/N6LYT
* 2025 - Christoph van Wüllen, DL1YCF
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*
*/

#include <gtk/gtk.h>
#include <SoapySDR/Device.h>

#include "receiver.h"
#include "transmitter.h"

SoapySDRDevice *get_soapy_device(void);

void soapy_protocol_create_receiver(RECEIVER *rx);
void soapy_protocol_create_dual_receiver(RECEIVER *rx1, RECEIVER *rx2);
void soapy_protocol_start_receiver(RECEIVER *rx);
void soapy_protocol_start_dual_receiver(RECEIVER *rx1, RECEIVER *rx2);

void soapy_protocol_init(gboolean hf);
void soapy_protocol_stop(void);
void soapy_protocol_stop_receiver(const RECEIVER *rx);
void soapy_protocol_set_rx_frequency(RECEIVER *rx, int v);
void soapy_protocol_set_rx_antenna(RECEIVER *rx, int ant);
void soapy_protocol_set_lna_gain(RECEIVER *rx, int gain);
void soapy_protocol_set_gain(RECEIVER *rx);
void soapy_protocol_attenuate(RECEIVER *rx);
void soapy_protocol_unattenuate(RECEIVER *rx);
void soapy_protocol_set_gain_element(const RECEIVER *rx, char *name, int gain);
int soapy_protocol_get_gain_element(RECEIVER *rx, char *name);
void soapy_protocol_change_sample_rate(RECEIVER *rx);
gboolean soapy_protocol_get_automatic_gain(RECEIVER *rx);
void soapy_protocol_set_automatic_gain(RECEIVER *rx, gboolean mode);
void soapy_protocol_create_transmitter(const TRANSMITTER *tx);
void soapy_protocol_start_transmitter(const TRANSMITTER *tx);
void soapy_protocol_stop_transmitter(const TRANSMITTER *tx);
void soapy_protocol_set_tx_frequency(const TRANSMITTER *tx);
void soapy_protocol_set_tx_antenna(const TRANSMITTER *tx, int ant);
void soapy_protocol_set_tx_gain(const TRANSMITTER *tx, int gain);
void soapy_protocol_set_tx_gain_element(TRANSMITTER *tx, char *name, int gain);
int soapy_protocol_get_tx_gain_element(TRANSMITTER *tx, char *name);
void soapy_protocol_iq_samples(float isample, float qsample);
void soapy_protocol_set_mic_sample_rate(int rate);
