//
// Created by childcity on 07.04.19.
//

#include "CKnownIpCameraList.h"

void CKnownIpCameraList::setActive(const CSQLiteDB::ptr &dbPtr) {
    IResult *res = dbPtr->ExecuteSelect(SKnownIpCamera::SELECT_ALL_ACTIVE_QUERY());

    if (nullptr == res) {
        LOG(WARNING) << "KnownIpCamera: (WARNING): query is NULL";
        return;
    }

    if(res->GetColumnCount() != 1){
        LOG(WARNING) << "KnownIpCamera: returned columns != 1 as expected";
        return;
    }

    activeCameras_.clear();

    while (res->Next()) {
        const char *tmpSensorProviderId = res->ColomnData(0);

        if(nullptr != tmpSensorProviderId){
            SKnownIpCamera knownIpCamera;
            knownIpCamera.sensorProviderID = std::move(string(tmpSensorProviderId));
            knownIpCamera.isActive = true;
            activeCameras_.emplace_back(std::move(knownIpCamera));
        }
    }

    //release Result Data
    res->ReleaseStatement();
}

bool CKnownIpCameraList::isActive(const std::string &sensorProviderID) const {
    return activeCameras_.end() != std::find_if(activeCameras_.begin(), activeCameras_.end(), [&sensorProviderID](const SKnownIpCamera &cam){
        return cam.sensorProviderID == sensorProviderID;
    });
}
