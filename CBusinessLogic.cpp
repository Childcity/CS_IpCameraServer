//
// Created by childcity on 03.10.18.
//

#include "CBusinessLogic.h"

CBusinessLogic::CBusinessLogic()
{/*static int instCount = 0; instCount++;VLOG(1) <<instCount;*/}

CBusinessLogic::~CBusinessLogic() {VLOG(1) <<"DEBUG: ~CBusinessLogic";}

//void CBusinessLogic::CreateOrUseOldTmpDb() {
//
//    const auto tmpDb = CSQLiteDB::new_(getTmpDbPath());
//
//    // check if tmp db exists
//    if(! tmpDb->OpenConnection(SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_READWRITE)){
//        LOG(INFO) <<"Can't connect to " <<getTmpDbPath() <<": " <<tmpDb->GetLastError();
//        LOG(INFO) <<"Trying to create new " <<getTmpDbPath();
//
//        //create new db. If can't create, return;
//        if(! tmpDb->OpenConnection(SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE)){
//            LOG(WARNING) <<"BUSINESS_LOGIC: can't create or connect to " <<getTmpDbPath() <<": " <<tmpDb->GetLastError();
//            throw BusinessLogicError("Temporary database can't be opened or created. Check permissions and free place on disk");
//        }
//
//        //create table for temporary query strings
//        int res = tmpDb->Execute("CREATE TABLE tmp_querys(\n"
//                                 "   query          TEXT    NOT NULL,\n"
//                                 "   timestamp DATETIME DEFAULT CURRENT_TIMESTAMP\n"
//                                 ");");
//
//        if(res < 0){
//            string errMsg("Can't create table for temporary database");
//            LOG(WARNING) <<"BUSINESS_LOGIC: " <<errMsg;
//            throw BusinessLogicError(errMsg);
//        }
//
//        //created new db. Created table
//    }
//
//}
//
//int CBusinessLogic::SaveQueryToTmpDb(const string &query) {
//
//    const auto tmpDb = CSQLiteDB::new_(getTmpDbPath());
//
//    if(! tmpDb->OpenConnection()) {
//        string errMsg("can't connect to " + getTmpDbPath() + ": " + tmpDb->GetLastError());
//        LOG(WARNING) <<"BUSINESS_LOGIC: " <<errMsg;
//        throw BusinessLogicError(errMsg);
//    }
//
//    const string insertQuery("INSERT INTO `tmp_querys` (query) VALUES ('" + boost::replace_all_copy(query, "'", "''") + "');"); //replace ' to ''
//    return tmpDb->Execute(insertQuery.c_str());
//}

