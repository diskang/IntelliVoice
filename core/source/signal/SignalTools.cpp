// SignalTools.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/03/04
// Copyright (C) 2007-14 University of Augsburg, Lab for Human Centered Multimedia
//
// *************************************************************************************************
//
// This file is part of Social Signal Interpretation (SSI) developed at the 
// Lab for Human Centered Multimedia of the University of Augsburg
//
// This library is free software; you can redistribute itand/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or any laterversion.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FORA PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along withthis library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
//*************************************************************************************************

#include "signal/SignalTools.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

void SignalTools::Transform (ssi_stream_t &from,
	ssi_stream_t &to,
	ITransformer &transformer,
	ssi_size_t frame_size,
	ssi_size_t delta_size,
	bool call_enter,
	bool call_flush) {

	if (frame_size <= 0) {

		ssi_time_t sample_rate_in = from.sr;
		ssi_size_t sample_number_in = from.num;
		ssi_size_t sample_dimension_in = from.dim;
		ssi_size_t sample_bytes_in = from.byte;
		ssi_type_t sample_type_in = from.type;
		ssi_size_t sample_number_out = sample_number_in == 0 ? 0 : transformer.getSampleNumberOut (sample_number_in);
		ssi_size_t sample_dimension_out = transformer.getSampleDimensionOut (sample_dimension_in);
		ssi_size_t sample_bytes_out = transformer.getSampleBytesOut (sample_bytes_in);
		ssi_type_t sample_type_out = transformer.getSampleTypeOut (sample_type_in);
		ssi_time_t sample_rate_out = sample_number_in == 0 ? 0 : (ssi_cast (ssi_time_t, sample_number_out) / ssi_cast (ssi_time_t, sample_number_in)) * sample_rate_in;

		ssi_stream_init (to, 0, sample_dimension_out, sample_bytes_out, sample_type_out, sample_rate_out);

		if (sample_number_out > 0) {
			ssi_stream_adjust (to, sample_number_out);
			if (call_enter) {
				transformer.transform_enter (from, to, 0, 0);	
			}
			ITransformer::info tinfo;
			tinfo.delta_num = 0;
			tinfo.frame_num = from.num;
			tinfo.time = 0;
			transformer.transform (tinfo, from, to, 0, 0);
			if (call_flush) {
				transformer.transform_flush (from, to, 0, 0);
			}
		}

	} else {

		ssi_time_t sample_rate_in = from.sr;

		ssi_size_t from_num = from.num;
		ssi_size_t from_tot = from.tot;
		SSI_ASSERT (from_num > frame_size + delta_size);
		ssi_size_t max_shift = (from_num - delta_size) / frame_size;

		ssi_size_t sample_number_in = frame_size + delta_size;
		ssi_size_t sample_number_out = transformer.getSampleNumberOut (frame_size);	
		ssi_size_t sample_dimension_in = from.dim;
		ssi_size_t sample_dimension_out = transformer.getSampleDimensionOut (sample_dimension_in);
		ssi_size_t sample_bytes_in = from.byte;
		ssi_size_t sample_bytes_out = transformer.getSampleBytesOut (sample_bytes_in);
		ssi_type_t sample_type_in = from.type;
		ssi_type_t sample_type_out = transformer.getSampleTypeOut (sample_type_in);
		ssi_time_t sample_rate_out = (ssi_cast (ssi_time_t, sample_number_out) / ssi_cast (ssi_time_t, frame_size)) * sample_rate_in;

		ssi_size_t to_num = max_shift * sample_number_out;
		ssi_stream_init (to, 0, sample_dimension_out, sample_bytes_out, sample_type_out, sample_rate_out);
		ssi_stream_adjust (to, to_num);
		ssi_size_t to_tot = to.tot;

		ssi_byte_t *from_ptr = from.ptr;
		ssi_byte_t *to_ptr = to.ptr;
		from.num = sample_number_in;
		to.num = sample_number_out;
		ssi_size_t byte_shift_in = sample_bytes_in * sample_dimension_in * frame_size;
		from.tot = byte_shift_in;
		ssi_size_t byte_shift_out = sample_bytes_out * sample_dimension_out * sample_number_out;
		to.tot = byte_shift_out;

		if (call_enter) {
			transformer.transform_enter (from, to, 0, 0);	
		}
		ITransformer::info tinfo;
		tinfo.delta_num = delta_size;
		tinfo.frame_num = frame_size;
		tinfo.time = 0;
		for (ssi_size_t i = 0; i < max_shift; i++) {			
			transformer.transform (tinfo, from, to, 0, 0);					
			tinfo.time += frame_size / sample_rate_in;
			from.ptr += byte_shift_in;
			to.ptr += byte_shift_out;
		}
		if (call_flush) {
			transformer.transform_flush (from, to, 0, 0);
		}

		from.ptr = from_ptr;
		from.num = from_num;
		from.tot = from_tot;
		to.ptr = to_ptr;
		to.num = to_num;
		to.tot = to_tot;
	}
}

void SignalTools::Transform_Xtra (ssi_stream_t &from,
		ssi_stream_t &to,
		ITransformer &transformer,
		ssi_size_t frame_size,
		ssi_size_t delta_size,
		bool call_enter,
		bool call_flush,
		ssi_size_t xtra_stream_in_num,
		ssi_stream_t xtra_stream_in[]){

	if (frame_size <= 0) {

		ssi_time_t sample_rate_in = from.sr;
		ssi_size_t sample_number_in = from.num;
		ssi_size_t sample_dimension_in = from.dim;
		ssi_size_t sample_bytes_in = from.byte;
		ssi_type_t sample_type_in = from.type;
		ssi_size_t sample_number_out = sample_number_in == 0 ? 0 : transformer.getSampleNumberOut (sample_number_in);
		ssi_size_t sample_dimension_out = transformer.getSampleDimensionOut (sample_dimension_in);
		ssi_size_t sample_bytes_out = transformer.getSampleBytesOut (sample_bytes_in);
		ssi_type_t sample_type_out = transformer.getSampleTypeOut (sample_type_in);
		ssi_time_t sample_rate_out = sample_number_in == 0 ? 0 : (ssi_cast (ssi_time_t, sample_number_out) / ssi_cast (ssi_time_t, sample_number_in)) * sample_rate_in;

		ssi_stream_init (to, 0, sample_dimension_out, sample_bytes_out, sample_type_out, sample_rate_out);

		if (sample_number_out > 0) {
			ssi_stream_adjust (to, sample_number_out);
			if (call_enter) {
				transformer.transform_enter (from, to, xtra_stream_in_num, xtra_stream_in);	
			}
			ITransformer::info tinfo;
			tinfo.delta_num = 0;
			tinfo.frame_num = from.num;
			tinfo.time = 0;
			transformer.transform (tinfo, from, to, xtra_stream_in_num, xtra_stream_in);
			if (call_flush) {
				transformer.transform_flush (from, to, xtra_stream_in_num, xtra_stream_in);
			}
		}

	} else {

		ssi_time_t sample_rate_in = from.sr;

		ssi_size_t n_froms = n_froms = 1 + xtra_stream_in_num;;
		ssi_real_t *from_nums = new ssi_real_t[n_froms];
		ssi_size_t *from_tots = new ssi_size_t[n_froms];
		ssi_size_t from_num_min = 0;
		
		from_nums[0] = (ssi_real_t)from.num;
		from_tots[0] = from.tot;

		for(ssi_size_t i = 1; i < n_froms; i++){
			from_nums[i] = (ssi_real_t)xtra_stream_in[i-1].num;
			from_tots[i] = xtra_stream_in[i-1].tot;
		}

		if(n_froms > 1){

			ssi_real_t maxval = 0;
			ssi_real_t minval = 0;
			ssi_size_t maxpos = 0;
			ssi_size_t minpos = 0;
			ssi_minmax(n_froms, 1, from_nums, &minval, &minpos, &maxval, &maxpos);

			from_num_min = (ssi_size_t)minval;

		} else {

			from_num_min = (ssi_size_t)from_nums[0];

		}
		
		SSI_ASSERT (from_num_min > frame_size + delta_size);
		ssi_size_t max_shift = (from_num_min - delta_size) / frame_size;
				
		ssi_size_t sample_number_out = transformer.getSampleNumberOut (frame_size);
		ssi_size_t sample_dimension_in = from.dim;
		ssi_size_t sample_dimension_out = transformer.getSampleDimensionOut (sample_dimension_in);
		ssi_size_t sample_byte_in = from.byte;
		ssi_size_t sample_bytes_out = transformer.getSampleBytesOut (sample_byte_in);
		ssi_type_t sample_type_in = from.type;
		ssi_type_t sample_type_out = transformer.getSampleTypeOut (sample_type_in);
		ssi_time_t sample_rate_out = (ssi_cast (ssi_time_t, sample_number_out) / ssi_cast (ssi_time_t, frame_size)) * sample_rate_in;

		ssi_size_t to_num = max_shift * sample_number_out;
		ssi_stream_init (to, 0, sample_dimension_out, sample_bytes_out, sample_type_out, sample_rate_out);
		ssi_stream_adjust (to, to_num);
		ssi_size_t to_tot = to.tot;

		ssi_byte_t **from_ptrs = new ssi_byte_t*[n_froms];
		from_ptrs[0] = from.ptr;
		for(ssi_size_t i = 1; i < n_froms; i++){
			from_ptrs[i] = xtra_stream_in[i-1].ptr;
		}
		ssi_byte_t *to_ptr = to.ptr;
				
		ssi_size_t byte_shift_out = sample_bytes_out * sample_dimension_out * sample_number_out;
		to.tot = byte_shift_out;
		to.num = sample_number_out;//!

		ssi_size_t sample_number_in = frame_size + delta_size;
		from.num = sample_number_in;
		ssi_size_t byte_shift_in = sample_byte_in * sample_dimension_in * frame_size;
		from.tot = byte_shift_in;

		ssi_time_t from_secs = 0;
		ssi_time_t to_secs = 0;				

		if (call_enter) {
			
			for(ssi_size_t k = 1; k < n_froms; k++){
				xtra_stream_in[k-1].ptr = 0;
				xtra_stream_in[k-1].num = 0;
				xtra_stream_in[k-1].tot = 0;
			}

			transformer.transform_enter (from, to, xtra_stream_in_num, xtra_stream_in);	

		}

		for (ssi_size_t i = 0; i < max_shift; i++) {

			ITransformer::info tinfo;
			tinfo.delta_num = delta_size;
			tinfo.frame_num = frame_size;
			tinfo.time = 0;

			from_secs = (i * frame_size) / from.sr;
			to_secs = from_secs + (frame_size + delta_size)/from.sr;
						
			ssi_size_t from_num, n_num;
			for(ssi_size_t k = 1; k < n_froms; k++){

				from_num = (ssi_size_t)(from_secs * xtra_stream_in[k-1].sr + 0.5);
				n_num = (ssi_size_t)((to_secs - from_secs) * xtra_stream_in[k-1].sr + 0.5);

				xtra_stream_in[k-1].ptr = from_ptrs[k] + from_num * xtra_stream_in[k-1].dim * xtra_stream_in[k-1].byte;
				xtra_stream_in[k-1].num = n_num;
				xtra_stream_in[k-1].tot = n_num * xtra_stream_in[k-1].dim * xtra_stream_in[k-1].byte;

				SSI_ASSERT( (from_num + n_num) <= xtra_stream_in[k-1].num_real);
			}
			
			transformer.transform (tinfo, from, to, xtra_stream_in_num, xtra_stream_in);					
			tinfo.time += frame_size / sample_rate_in;
			
			from.ptr += byte_shift_in;
			to.ptr += byte_shift_out;

		}
		
		if (call_flush) {

			from_secs = (0 * frame_size) / from.sr;
			to_secs = from_secs + (frame_size + delta_size)/from.sr;
						
			for(ssi_size_t k = 1; k < n_froms; k++){
				xtra_stream_in[k-1].ptr = 0;
				xtra_stream_in[k-1].num = 0;
				xtra_stream_in[k-1].tot = 0;
			}

			transformer.transform_flush (from, to, xtra_stream_in_num, xtra_stream_in);

		}

		from.ptr = from_ptrs[0];
		from.num = (ssi_size_t)from_nums[0];
		from.tot = from_tots[0];

		for(ssi_size_t i = 1; i < n_froms; i++){
			xtra_stream_in[i-1].ptr = from_ptrs[i];
			xtra_stream_in[i-1].num = (ssi_size_t)from_nums[i];
			xtra_stream_in[i-1].tot = from_tots[i];
		}

		to.ptr = to_ptr;
		to.num = to_num;
		to.tot = to_tot;

		delete [] from_nums; from_nums = 0;
		delete [] from_tots; from_tots = 0;
		delete [] from_ptrs; from_ptrs = 0;

	}

}

void SignalTools::Consume (ssi_stream_t &from,		
	IConsumer &consumer,		
	ssi_size_t frame_size,
	ssi_size_t delta_size,
	bool call_enter,
	bool call_flush) {

	if (frame_size <= 0) {

		if (call_enter) {
			consumer.consume_enter (1, &from);	
		}
		IConsumer::info cinfo;
		cinfo.time = 0;
		cinfo.dur = from.num / from.sr;
		cinfo.status = IConsumer::NO_TRIGGER;
		cinfo.event = 0;
		consumer.consume (cinfo, 1, &from);
		if (call_flush) {
			consumer.consume_flush (1, &from);
		}		

	} else {

		ssi_time_t sample_rate_in = from.sr;

		ssi_size_t from_num = from.num;
		ssi_size_t from_tot = from.tot;
		SSI_ASSERT (from_num > frame_size + delta_size);
		ssi_size_t max_shift = (from_num - delta_size) / frame_size;

		ssi_size_t sample_number_in = frame_size + delta_size;		
		ssi_size_t sample_dimension_in = from.dim;
		ssi_size_t sample_bytes_in = from.byte;
		ssi_type_t sample_type_in = from.type;

		ssi_byte_t *from_ptr = from.ptr;
		from.num = sample_number_in;
		ssi_size_t byte_shift_in = sample_bytes_in * sample_dimension_in * frame_size;		
		from.tot = byte_shift_in;

		if (call_enter) {
			consumer.consume_enter (1, &from);	
		}
		IConsumer::info cinfo;		
		cinfo.time = 0;
		cinfo.dur = sample_number_in / sample_rate_in;
		cinfo.status = IConsumer::NO_TRIGGER;
		cinfo.event = 0;
		for (ssi_size_t i = 0; i < max_shift; i++) {			
			consumer.consume (cinfo, 1, &from);					
			cinfo.time += frame_size / sample_rate_in;
			from.ptr += byte_shift_in;			
		}
		if (call_flush) {
			consumer.consume_flush (1, &from);
		}

		from.ptr = from_ptr;
		from.num = from_num;
		from.tot = from_tot;
	}
}

void SignalTools::Series (ssi_stream_t &series,		
	ssi_time_t duration, 		
	ssi_real_t offset) {

	/* matlab code:

	t = 0:1/sr:dur-1/sr;

	*/

	ssi_real_t delta = ssi_cast (ssi_real_t, 1.0 / series.sr);
	ssi_size_t number = ssi_cast (ssi_size_t, duration / delta);
	SSI_ASSERT (number > 0);

	ssi_stream_adjust (series, number);
	ssi_real_t *out = ssi_pcast (ssi_real_t, series.ptr);
	ssi_real_t *outptr_new = out;
	ssi_real_t *outptr_old = out;
	for (ssi_size_t i = 0; i < series.dim; ++i) {
		*outptr_new++ = offset;
	}
	for (ssi_size_t i = series.dim; i < series.dim * series.num; ++i) {
		*outptr_new++ = *outptr_old++ + delta;	
	}
}

void SignalTools::Sine (ssi_stream_t &series,
	ssi_time_t *frequency, 
	ssi_real_t *amplitude) {

	/* matlab code:

	t = 0:1/sr:dur-1/sr;
	signal = A' * sin (2*pi*f*t);

	*/
	
	ssi_real_t *ptr = ssi_pcast (ssi_real_t, series.ptr);
	ssi_real_t *pipif = new ssi_real_t[series.dim];

	for (ssi_size_t j = 0; j < series.dim; ++j) {
		pipif[j] = static_cast<ssi_real_t> (2.0 * PI * frequency[j]);
	}
	for (ssi_size_t i = 0; i < series.num; ++i) {
		for (ssi_size_t j = 0; j < series.dim; ++j) {
			*ptr = amplitude[j] * sin (pipif[j] * *ptr);
			++ptr;
		}
	}	

	delete[] pipif;
}

void SignalTools::Cosine (ssi_stream_t &series,
	ssi_time_t *frequency, 
	ssi_real_t *amplitude) {

	/* matlab code:

	t = 0:1/sr:dur-1/sr;
	signal = A' * sin (2*pi*f*t);

	*/
	
	ssi_real_t *ptr = ssi_pcast (ssi_real_t, series.ptr);
	ssi_real_t *pipif = new ssi_real_t[series.dim];

	for (ssi_size_t j = 0; j < series.dim; ++j) {
		pipif[j] = static_cast<ssi_real_t> (2.0 * PI * frequency[j]);
	}
	for (ssi_size_t i = 0; i < series.num; ++i) {
		for (ssi_size_t j = 0; j < series.dim; ++j) {
			*ptr = amplitude[j] * cos (pipif[j] * *ptr);
			++ptr;
		}
	}	

	delete[] pipif;
}

void SignalTools::Sum (ssi_stream_t &series) {

	ssi_real_t *old_ptr = ssi_pcast (ssi_real_t, series.ptr);
	ssi_real_t *new_ptr = new ssi_real_t[series.num];

	ssi_real_t *srcptr = old_ptr;
	ssi_real_t *dstptr = new_ptr;
	for (ssi_size_t i = 0; i < series.num; i++) {
		*dstptr = 0;
		for (ssi_size_t j = 0; j < series.dim; j++) {
			*dstptr += *srcptr++;
		}
		dstptr++;
	}
	
	series.ptr = ssi_pcast (ssi_byte_t, new_ptr);
	series.dim = 1;
	delete[] old_ptr;	
}

void SignalTools::Random(ssi_stream_t &stream) {

	ssi_real_t *ptr = ssi_pcast(ssi_real_t, stream.ptr);
	for (ssi_size_t i = 0; i < stream.dim * stream.num; i++) {
		*ptr++ = ssi_cast (ssi_real_t, ssi_random());
	}
}

}
