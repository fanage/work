#include <thread>
#include <vector>
#include "sai_test_common.hpp"

int main()
{
#if defined(ENABLE_HTTP2) && !defined(ENABLE_DRIVER_DATA)
   std::vector<std::thread> v_th;
   for(size_t i = 0; i < 10; i++) v_th.push_back(std::thread(terminate_system));
   using namespace sai::test::handlers;
   int ret = init_system("sn", 0.35, "sai_config", asr_data_cb, ivw_cb, ivw_data_cb, voip_cb,
                                                   one_shot_cb, energy_cb, net_event_cb, net_status_cb, beat_data_cb, NULL, NULL, NULL);
   for(auto && v: v_th) v.join();
#endif
}

