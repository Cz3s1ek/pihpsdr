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

extern void old_protocol_stop(void);
extern void old_protocol_run(void);

extern void old_protocol_init(int rate);
extern void old_protocol_set_mic_sample_rate(int rate);

extern void old_protocol_audio_samples(short left_audio_sample, short right_audio_sample);
extern void old_protocol_iq_samples(int isample, int qsample, int side);
