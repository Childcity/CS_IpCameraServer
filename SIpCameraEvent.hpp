//
// Created by childcity on 15.02.19.
//

#ifndef CS_IPCAMERASERVER_SIPCAMERAEVENT_HPP
#define CS_IPCAMERASERVER_SIPCAMERAEVENT_HPP
#pragma once

#include <string>

using string = std::string;

struct SIpCameraEvent {
    SIpCameraEvent() = default;
    SIpCameraEvent(const SIpCameraEvent &) = default;
    SIpCameraEvent &operator=(const SIpCameraEvent &) = default;

    string packetCounter;
    string datetime;
    string plateText;
    string plateCountry;
    string plateConfidence;
    string cameraId;
    string carState;
    string geotag_lat;
    string geotag_lon;
    string imageType;
    string plateImageType;
    string plateImageSize;
    string carMoveDirection;
    string timeProcessing;
    string plateCoordinates;
    string carID;
    string GEOtarget;
    string sensorProviderID;
    string rawJson;

    static const char *CREATE_TABLE_QUERY() {
        return "CREATE TABLE IF NOT EXISTS ip_camera_events(\n"
               "  packetCounter INTEGER,\n"
               "  `datetime` TEXT,\n"
               "  plateText BLOB,\n"
               "  plateCountry TEXT,\n"
               "  plateConfidence REAL,\n"
               "  cameraId TEXT,\n"
               "  carState TEXT,\n"
               "  geotag_lat TEXT,\n"
               "  geotag_lon TEXT,\n"
               "  imageType TEXT,\n"
               "  plateImageType TEXT,\n"
               "  plateImageSize TEXT,\n"
               "  carMoveDirection TEXT,\n"
               "  timeProcessing TEXT,\n"
               "  plateCoordinates TEXT,\n"
               "  carID TEXT,\n"
               "  GEOtarget TEXT,\n"
               "  sensorProviderID TEXT,\n"
               "  rawJson TEXT,\n"
               "  wroteToDbAt DATETIME DEFAULT (datetime('now', 'localtime'))\n"
               ");";
    }

    static string INSERT_EVENT_QUERY(const SIpCameraEvent &ipCamEvent) {
        static const string constPart("INSERT INTO ip_camera_events (packetCounter, `datetime`, plateText, plateCountry, plateConfidence, cameraId, carState, geotag_lat, geotag_lon, "
                                      "imageType, plateImageType, plateImageSize, carMoveDirection, timeProcessing, plateCoordinates, carID, GEOtarget, sensorProviderID, rawJson) ");

        return string(constPart + "VALUES "
                                  "('" + ipCamEvent.packetCounter		+ "', '" + ipCamEvent.datetime			+ "', '" + ipCamEvent.plateText
                      + "', '" + ipCamEvent.plateCountry		+ "', '" + ipCamEvent.plateConfidence
                      + "', '" + ipCamEvent.cameraId			+ "', '" + ipCamEvent.carState			+ "', '" + ipCamEvent.geotag_lat
                      + "', '" + ipCamEvent.geotag_lon		+ "', '" + ipCamEvent.imageType			+ "', '" + ipCamEvent.plateImageType
                      + "', '" + ipCamEvent.plateImageSize	+ "', '" + ipCamEvent.carMoveDirection	+ "', '" + ipCamEvent.timeProcessing
                      + "', '" + ipCamEvent.plateCoordinates	+ "', '" + ipCamEvent.carID				+ "', '" + ipCamEvent.GEOtarget
                      + "', '" + ipCamEvent.sensorProviderID	+ "', '" + ipCamEvent.rawJson
                      + "');"
        );
    }
};
#endif //CS_IPCAMERASERVER_SIPCAMERAEVENT_HPP
