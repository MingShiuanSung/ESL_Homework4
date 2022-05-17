#ifndef GAUSS_FILTER_H_
#define GAUSS_FILTER_H_
#include <systemc>
#include <cmath>
#include <iomanip>
using namespace sc_core;

#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "filter_def.h"

struct GaussFilter : public sc_module {
  tlm_utils::simple_target_socket<GaussFilter> tsock;

  sc_fifo<unsigned char> i_r;
  sc_fifo<unsigned char> i_g;
  sc_fifo<unsigned char> i_b;
  sc_fifo<unsigned char> o_result_r;
  sc_fifo<unsigned char> o_result_g;
  sc_fifo<unsigned char> o_result_b;

  SC_HAS_PROCESS(GaussFilter);

  GaussFilter(sc_module_name n): 
    sc_module(n), 
    tsock("t_skt"), 
    base_offset(0) 
  {
    tsock.register_b_transport(this, &GaussFilter::blocking_transport);
    SC_THREAD(do_filter);
  }

  ~GaussFilter() {
	}

  unsigned int base_offset;

  void do_filter(){
    { wait(CLOCK_PERIOD, SC_NS); }
    while (true) {
      double red = 0.0, green = 0.0, blue = 0.0;
      for (unsigned int v = 0; v < MASK_Y; ++v) {
        for (unsigned int u = 0; u < MASK_X; ++u) {
          red += i_r.read() * mask[u][v];
          green += i_g.read() * mask[u][v];
          blue += i_b.read() * mask[u][v];
          wait(CLOCK_PERIOD, SC_NS);
        }
      }
      unsigned char result_r = (unsigned char) std::min(std::max(int(factor * red + bias), 0), 255);
      unsigned char result_g = (unsigned char) std::min(std::max(int(factor * green + bias), 0), 255);
      unsigned char result_b = (unsigned char) std::min(std::max(int(factor * blue + bias), 0), 255);
      o_result_r.write(result_r);
      o_result_g.write(result_g);
      o_result_b.write(result_b);
      wait(CLOCK_PERIOD, SC_NS);
    }
  }

  void blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay){
    wait(delay);
    // unsigned char *mask_ptr = payload.get_byte_enable_ptr();
    // auto len = payload.get_data_length();
    tlm::tlm_command cmd = payload.get_command();
    sc_dt::uint64 addr = payload.get_address();
    unsigned char *data_ptr = payload.get_data_ptr();

    addr -= base_offset;


    // cout << (int)data_ptr[0] << endl;
    // cout << (int)data_ptr[1] << endl;
    // cout << (int)data_ptr[2] << endl;
    word buffer;

    switch (cmd) {
      case tlm::TLM_READ_COMMAND:
        // cout << "READ" << endl;
        switch (addr) {
          case GAUSS_FILTER_RESULT_ADDR:
            buffer.uc[0] = o_result_r.read();
            buffer.uc[1] = o_result_g.read();
            buffer.uc[2] = o_result_b.read();
            buffer.uc[3] = 0;
            break;
          default:
            std::cerr << "READ Error! GaussFilter::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
          }
        data_ptr[0] = buffer.uc[0];
        data_ptr[1] = buffer.uc[1];
        data_ptr[2] = buffer.uc[2];
        data_ptr[3] = buffer.uc[3];
        break;
      case tlm::TLM_WRITE_COMMAND:
        // cout << "WRITE" << endl;
        switch (addr) {
          case GAUSS_FILTER_R_ADDR:
            i_r.write(data_ptr[0]);
            i_g.write(data_ptr[1]);
            i_b.write(data_ptr[2]);
            break;
          default:
            std::cerr << "WRITE Error! GaussFilter::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
        }
        break;
      case tlm::TLM_IGNORE_COMMAND:
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
      default:
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
      }
      payload.set_response_status(tlm::TLM_OK_RESPONSE); // Always OK
  }
};
#endif
