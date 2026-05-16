#ifndef JSONREAD
#define JSONREAD
#include "json.hpp"
using json = nlohmann::json; 

class jsonRead{
    
    private:
    json loadData();
    static const std::string STORAGE_FILENAME;
   
    public:
    void saveData(const json& consolidatedData);

    void handlePacket(const std::string& json_string);
    
};
#endif