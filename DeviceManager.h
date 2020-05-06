#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H
#define ERROR_CODE -1
// 设备号
typedef enum {PRINTER,NETWORK_CARD,HARDDISK,USB} Device_id;

// 初始化设备管理器
void initDeviceManagr();

// 模块资源释放
void deInitDeviceManager();

typedef struct PCB PCB;
// 进程pcb占用设备device_name, 时间为time_cost
int toUseDevice(PCB* pcb,const char* device_name, int time_cost);

// 设备使用情况
const char* getDeviceUseInfo();
#endif // DEVICEMANAGER_H
