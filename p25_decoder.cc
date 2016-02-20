
#include "p25_decoder.h"
#include <boost/log/trivial.hpp>


p25_decoder_sptr make_p25_decoder(char *filename)
{
	return gnuradio::get_initial_sptr(new p25_decoder(filename));
}





p25_decoder::p25_decoder(char *filename)
	: gr::hier_block2 ("p25_decoder",
	                   gr::io_signature::make  (1, 1, sizeof(float)),
	                   gr::io_signature::make  (0, 0, sizeof(float)))
{





	float symbol_rate = 4800;
	double samples_per_symbol = 10;
	float symbol_deviation = 600.0;

	std::vector<float> sym_taps;
	const double pi = M_PI; //boost::math::constants::pi<double>();

	timestamp = time(NULL);
	starttime = time(NULL);


        float if_rate = 48000;
        float gain_mu = 0.025;
        float costas_alpha = 0.04;
        double sps = 0.0;
        float bb_gain = 1.0;

    


  
        

        // convert from radians such that signal is in -3/-1/+1/+3
        rescale = gr::blocks::multiply_const_ff::make( (1 / (pi / 4)) );





	double symbol_decim = 1;

	tune_queue = gr::msg_queue::make(2);
	traffic_queue = gr::msg_queue::make(2);
	rx_queue = gr::msg_queue::make(100);
	const float l[] = { -2.0, 0.0, 2.0, 4.0 };
	std::vector<float> levels( l,l + sizeof( l ) / sizeof( l[0] ) );
	slicer = gr::op25_repeater::fsk4_slicer_fb::make(levels);

	int udp_port = 0;
	int verbosity = 10;
	const char * wireshark_host="127.0.0.1";
	bool do_imbe = 1;
	bool do_output = 1;
	bool do_msgq = 0;
	bool do_audio_output = 1;
	bool do_tdma = 0;
	op25_frame_assembler = gr::op25_repeater::p25_frame_assembler::make(wireshark_host,udp_port,verbosity,do_imbe, do_output, do_msgq, rx_queue, do_audio_output, do_tdma);
	
	converter = gr::blocks::short_to_float::make(1,32768.0);// 4096.0);//8192.0);//16384);//32768.0);//8192.0);
	  
        float convert_num = float(1.0)/float(256.0);
	//float convert_num = float(1.0)/float(8192.0);
	//float convert_num = float(1.0)/float(32768.0);
        
	multiplier = gr::blocks::multiply_const_ff::make(convert_num);
/*	tm *ltm = localtime(&starttime);

	std::stringstream path_stream;
	path_stream << boost::filesystem::current_path().string() <<  "/" << 1900 + ltm->tm_year << "/" << 1 + ltm->tm_mon << "/" << ltm->tm_mday;

	boost::filesystem::create_directories(path_stream.str());
	sprintf(filename, "%s/%ld-%ld_%g.wav", path_stream.str().c_str(),talkgroup,timestamp,freq);*/
	wav_sink = gr::blocks::nonstop_wavfile_sink::make(filename,1,8000,16);

    null_sink = gr::blocks::null_sink::make(sizeof(int16_t)); //sizeof(gr_complex));



		connect(self(),0,  rescale, 0);
		connect(rescale, 0, slicer, 0);

        
	 connect(slicer,0, op25_frame_assembler,0);
        
        
		connect(op25_frame_assembler, 0,  converter,0);
        
		connect(converter, 0, wav_sink,0); 
	
}


p25_decoder::~p25_decoder() {

wav_sink->close();
}


