//
// Created by childcity on 03.10.18.
//

#include "CBusinessLogic.h"

CBusinessLogic::CBusinessLogic()
{/*static int instCount = 0; instCount++;VLOG(1) <<instCount;*/}

CBusinessLogic::~CBusinessLogic() {VLOG(1) <<"DEBUG: ~CBusinessLogic";}

void CBusinessLogic::setLastIpCamEvent(const SIpCameraEvent &ipCamEvent) {
    //TODO: db check
    //exclusive access to data!
    boost::unique_lock<boost::shared_mutex> lock(business_logic_mtx_);
    for(auto &ev : lastIpCamEvents_){
        if(ev.sensorProviderID == ipCamEvent.sensorProviderID){
            ev = ipCamEvent;
            return;
        }
    }
    // if here -> this is first event
    lastIpCamEvents_.emplace_back(ipCamEvent);
}

SIpCameraEvent CBusinessLogic::getLastIpCamEvent(const string &sensorProviderID) const {
    //TODO: db check
    //NOT exclusive access to data! Allows only read, not write!
    boost::shared_lock<boost::shared_mutex> lock(business_logic_mtx_);
    for(const auto &ev : lastIpCamEvents_){
        VLOG(1) <<ev.sensorProviderID <<lastIpCamEvents_.size();
    }
    for(const auto &ev : lastIpCamEvents_){
        if(ev.sensorProviderID == sensorProviderID){
            return ev;
        }
    }
    // if here -> no event with this sensorProviderID
    return SIpCameraEvent();
}

void CBusinessLogic::CreateOrUseDb(const std::string &dbPath) {

    const auto tmpDb = CSQLiteDB::new_(dbPath);

    // check if tmp db exists
    if(! tmpDb->OpenConnection(SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_READWRITE)){
        LOG(INFO) <<"Can't connect to " <<dbPath <<": " <<tmpDb->GetLastError();
        LOG(INFO) <<"Trying to create new " <<dbPath;

        //create new db. If can't create, return;
        if(! tmpDb->OpenConnection(SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE)){
            LOG(WARNING) <<"BUSINESS_LOGIC: can't create or connect to " <<dbPath <<": " <<tmpDb->GetLastError();
            throw BusinessLogicError("Database can't be opened or created. Check permissions and free place on disk");
        }
    }

    //create table for temporary query strings
    int res = tmpDb->Execute(SIpCameraEvent::CREATE_TABLE_QUERY());

    if(res < 0){
        string errMsg("Can't create table for ip_camera events");
        LOG(WARNING) <<"BUSINESS_LOGIC: " <<errMsg;
        throw BusinessLogicError(errMsg);
    }

}


