#include <io.h> 
#include "iostream"
#include <fstream>    
#include <string>    
#include <vector>
#include <stdio.h>  
#include <dirent.h>
#include <unistd.h>
#include "sai_sdk.h"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include "sai_test_common.hpp"

int _wakeup_status = 0;
static int getFileBuffer(const char *fn, char **data) {
    FILE *fpBinary = fopen(fn, "rb");
    if (!fpBinary)
        return -1;
    fseek(fpBinary, 0L, SEEK_END);
    long size = ftell(fpBinary);
    char *buffer = (char *) malloc(size);
    rewind(fpBinary);
    if (fread(buffer, 1, size, fpBinary) != size) {
        free(buffer);
        return -1;
    }
    data[0] = buffer;
    fclose(fpBinary);
    return size;
}

static void testGetWavPcmBuffer(const char *file, char **buffer, int *len) {
    int size = 0;
    size = getFileBuffer(file, buffer);
    *len = size;
}

static void testFreePCMBuffer(char *buffer, int len) {
    free(buffer);
}

/*
void net_event(const char * id, int type, int rc, const char * msg)
{
    string s_id = id;
    string s_msg = msg;
    boost::filesystem::path old_cpath = boost::filesystem::current_path();
    boost::filesystem::path out_path = old_cpath.parent_path() / "data/out.txt";
    ofstream out(out_path.c_str(), ios::app);
    out << id << " " << msg  << "  ";
}
void net_status(networkStatusCode code, const char * msg) {}

void beat_peroid_data(const char * id, size_t len) {}

void energy_val_info(float val) {}

void voip(void * usr_data_voip, const char * id, const char *buffer, size_t size) {}

void ivw_data_cb_function(const char * id, const char *buffer, size_t size) {}

void asr_cb_function(void * usr_data_asr, const char * id, const char *buffer, size_t size)
{
    string s_id = id;
    string s_buffer = buffer;

    boost::filesystem::path old_cpath = boost::filesystem::current_path();
    boost::filesystem::path out_path = old_cpath.parent_path() / "data/out.txt";
    ofstream out(out_path.c_str(), ios::app);
    out << s_id << "  " << s_buffer << " ";
}

int wk_cb_function(void * usr_data_wk, const char * id, const char * key_word, 
                                    float score, int wakeup_result, float angle, int usr_set)
{
    _wakeup_status = 1;
    printf("%s, wake up key word: %d, angle value: %f\n", id, wakeup_result, angle);
    int usr_data = *((int*)(usr_data_wk));
    printf("usr data: %d \n", usr_data);
    return 0;
}

void one_shot_flag(int flag)
{
    printf("one shot flag: %d\n", flag);
}
*/

int main(int argc, char * argv[])  
{  
    int count=0;
    std::string line,data_name,angle;
 /* 
    boost::filesystem::path old_cpath = boost::filesystem::current_path();
    boost::filesystem::path parent_path = old_cpath.parent_path();  
    boost::filesystem::path data_path = old_cpath.parent_path() / "data/data.txt";
    boost::filesystem::path out_path = old_cpath.parent_path() / "data/out.txt";  */
    
    using namespace sai::test::handlers;
    int ret = init_system("sn",          0.35,     "sai_config", 
                           asr_data_cb,  ivw_cb,    ivw_data_cb,  
                           voip_cb,      one_shot_cb, energy_cb, 
                           net_event_cb, 
                           net_status_cb, 
                           beat_data_cb,
                           driver_cb,
                           NULL, 
                           NULL, NULL, NULL);
    printf("init_system ret: %d\n", ret);
    
    
    //ifstream datain(data_path.c_str());
    
    std::ifstream datain("data/data.txt");
    if(datain)  
    {
        int len;
        size_t lenth;
        char data[1024*8*2];
        while (getline (datain, line))  
        {
            std::vector<std::string> SplitLine;
            boost::split(SplitLine, line, boost::is_any_of(" "), boost::token_compress_on);
            data_name = *SplitLine.begin();
            angle = *(SplitLine.begin()+1);
            //ofstream out(out_path.c_str(), ios::app);
            std::ofstream out("data/out.txt", std::ios::app);
            time_t timep;
            time(&timep);
            out << std::endl << data_name.c_str() << "  angle:" << angle.c_str() << " time:" 
                << asctime(gmtime(&timep)) << "  ";
			
            char *test_buffer = NULL;
            int test_buf_len = 0;
	    testGetWavPcmBuffer(data_name.c_str(), &test_buffer, &test_buf_len);
	    
	    feed_driver_data(data, lenth);
	    lenth = test_buf_len;	
            testFreePCMBuffer(test_buffer, test_buf_len);

            out << std::endl;
        }
    }
    else
    {  
        std::cout <<"no such data file" << std::endl;  
    }
    
    
    ret = stop_service();
    if(ret != 0)
        printf("Stop service failed: %d\n", ret);

}

