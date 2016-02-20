
//#define DSD

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/tokenizer.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/foreach.hpp>


#include <iostream>
#include <cassert>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ncurses.h>
#include <menu.h>
#include <time.h>




#include "p25_decoder.h"




#include <osmosdr/source.h>

#include <gnuradio/uhd/usrp_source.h>
#include <gnuradio/msg_queue.h>
#include <gnuradio/message.h>
#include <gnuradio/blocks/file_sink.h>
#include <gnuradio/blocks/file_source.h>
#include <gnuradio/blocks/throttle.h>
#include <gnuradio/gr_complex.h>
#include <gnuradio/top_block.h>


using namespace std;
namespace logging = boost::log;



string system_type;
gr::top_block_sptr tb;

gr::msg_queue::sptr queue;

volatile sig_atomic_t exit_flag = 0;



void exit_interupt(int sig) { // can be called asynchronously
    exit_flag = 1; // set flag
}




int main(int argc,      // Number of strings in array argv
          char *argv[])
{
    gr::blocks::file_source::sptr source;
    gr::blocks::throttle::sptr throttle;

    p25_decoder_sptr decoder = make_p25_decoder(argv[2]);
    BOOST_STATIC_ASSERT(true) __attribute__((unused));
    signal(SIGINT, exit_interupt);
    logging::core::get()->set_filter
    (
     logging::trivial::severity >= logging::trivial::info
     );

    tb = gr::make_top_block("Trunking");
   source = gr::blocks::file_source::make(sizeof(float),argv[1], false);
    throttle =gr::blocks::throttle::make(sizeof(float),48000);



    tb->connect(source,0, throttle,0);
    tb->connect(throttle,0, decoder, 0);

    //tb->run();
        tb->start();
       
        //------------------------------------------------------------------
        //-- stop flow graph execution
        //------------------------------------------------------------------
        BOOST_LOG_TRIVIAL(info) << "stopping flow graph";

        tb->wait();
        




    return 0;

}
