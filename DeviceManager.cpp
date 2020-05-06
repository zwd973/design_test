#include "DeviceManager.h"
#include "SystemTimer.h"
#include "ProcessManager.h"
#include<list>
#include<unordered_map>
#include<string>
#include<vector>
#include<algorithm>


// 名字到id的转换
static std::unordered_map<std::string,Device_id> name_to_id;
// id到名字
static std::unordered_map<Device_id,std::string> id_to_name;
// 设备队列
std::vector<std::list<std::pair<int,PCB*>>*> devices_wait_queue;

// 初始化设备管理器
void initDeviceManagr()
{
    name_to_id["printer"] = PRINTER;
    name_to_id["network_card"] = NETWORK_CARD;
    name_to_id["harddisk"] = HARDDISK;
    name_to_id["usb"] = USB;

    for(auto item : name_to_id){
        id_to_name[item.second] = item.first;
    }

    for(int i=0;i<name_to_id.size();i++){
        devices_wait_queue.push_back(new std::list<std::pair<int,PCB*>>());
    }
}

// 从名称获取设备ID
static int getIDFromName(const char* name)
{
    if(name_to_id.count(name))
        return (int)name_to_id[name];
    return ERROR_CODE;
}

// 从名称获取设备ID
static const char* getNameFromId(Device_id id)
{
    if(id_to_name.count(id))
        return id_to_name[id].c_str();
    return "unknow device";
}


// 模块资源释放
void deInitDeviceManager()
{
    for(int i=0;i<name_to_id.size();i++){
        delete devices_wait_queue[i];
    }
}

// 设备使用完成中断程序
void finishDeviceUse(void * args)
{
    long device_id = (long)args;
    PCB* pcb = devices_wait_queue[device_id]->front().second;
    devices_wait_queue[device_id]->pop_front();
    pushBackPCB(pcb);
    if(devices_wait_queue[device_id]->size()>0){
        startTimer(devices_wait_queue[device_id]->front().first,finishDeviceUse,(void*)device_id);
    }
}

// 进程pcb占用设备device_name, 时间为time_cost
int toUseDevice(PCB* pcb,const char* device_name, int time_cost)
{
    int id = getIDFromName(device_name);
    if(id==ERROR_CODE)
        return ERROR_CODE;
    std::pair<int,PCB*> item(time_cost,pcb);
    devices_wait_queue[(int)id]->push_back(item);
    if(devices_wait_queue[(int)id]->size()==1){
        startTimer(time_cost,finishDeviceUse,(void*)id);
    }
    return 0;
}

// 设备使用情况
const char* getDeviceUseInfo()
{
    static std::string  process_ui_info;
    std::vector<std::pair<int,std::string>> processes_info;
    char buff[1024];
    process_ui_info.clear();


    for(int i=0;i<devices_wait_queue.size();i++){
        auto q = devices_wait_queue[i];
        auto itr = q->begin();
        for (int j=0;j<q->size();j++) {
            PCB* p = itr->second;
            if(j==0)
                sprintf(buff, "%d\t%-4d\tusing %4s\t%d\n", p->pid, 0,getNameFromId((Device_id)i),p->total_run_time);
            else
                sprintf(buff, "%d\t%-4d\twaiting for %4s\t%d\n", p->pid, 0,getNameFromId((Device_id)i),p->total_run_time);
            std::pair<int,std::string> item(p->pid,buff);
            processes_info.push_back(item);
            itr++;
        }
    }

    std::sort(processes_info.begin(),processes_info.end());
    for(auto item:processes_info)
    {
        process_ui_info+=item.second;
    }
    return process_ui_info.c_str();
}
