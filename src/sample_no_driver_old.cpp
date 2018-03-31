#include <io.h> 
#include "iostream"
#include <fstream>    
#include <string>    
#include <vector>   
#include<boost/filesystem.hpp> 
#include <stdio.h>  
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include "sai_sdk.h"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

using namespace std;

int _wakeup_status = 0;

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
                               float score, int wakeup_result, float angle, int usr_set) {}

void one_shot_flag(int flag)
{
    printf("one shot flag: %d\n", flag);
}

int main(int argc, char * argv[])  
{
    int count=0;
    string line,data_name,angle;/*
    boost::filesystem::path old_cpath = boost::filesystem::current_path(); 
    boost::filesystem::path data_path = old_cpath / "data/data.txt";
    boost::filesystem::path out_path = old_cpath / "data/out.txt";*/
    int id = 1234569;
    size_t run_time = 5000;
    string file_tmp,file_in,file_out;    
    
    int ret = init_system("xxxxxxxxxx",     0.3,             "./sai_config",
                          asr_cb_function,  wk_cb_function,  ivw_data_cb_function,
                          voip,             one_shot_flag,   energy_val_info,
                          net_event,
                          net_status,
                          beat_peroid_data,
                          NULL,
                          nullptr, nullptr, nullptr, nullptr);
    if(ret != 0)
        cout << "init_system failed" << endl;
    
    //ifstream datain(data_path.c_str());
    ifstream datain("data/data.txt");
    if(datain)  
    {
        int len;
        size_t lenth;
        char data[1024*8*2];
        while (getline (datain, line))  
        {
            vector<string> SplitLine;
            boost::split(SplitLine, line, boost::is_any_of(" "), boost::token_compress_on);
            data_name = *SplitLine.begin();
            angle = *(SplitLine.begin()+1);
            //ofstream out(out_path.c_str(), ios::app);
            ofstream out("data/out.txt", ios::app);
            time_t timep;
            time(&timep);
            out << endl << data_name.c_str() << "  " << angle.c_str() << " " 
                << asctime(gmtime(&timep)) << "  ";

            FILE* file_in = fopen(data_name.c_str(), "rb");
            while((len = fread(data, 1024*8*2, 1, file_in)) > 0)
            {
                set_wakeup_status();
                lenth = len;
		feed_driver_data(data, lenth);
		usleep(1000*64);
                set_unwakeup_status();
            }
            fclose(file_in);

            out << endl;
        }
    }
    else
    {  
        cout <<"no such data file" << endl;  
    } 

    ret = stop_service();
    if(ret != 0)
        printf("Stop service failed: %d\n", ret); 
    

}
