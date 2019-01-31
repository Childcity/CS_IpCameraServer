#ifndef CS_CEVENTS_H
#define CS_CEVENTS_H
#pragma once

#include "main.h"
#include "glog/logging.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

struct SIpCameraEvent {
	SIpCameraEvent() = default;
    SIpCameraEvent(const SIpCameraEvent &) = default;
	SIpCameraEvent &operator=(const SIpCameraEvent &) = default;

	string packetCounter;
    string datetime;
    string plateText;
    string plateTextANSI;
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
			"  plateTextANSI TEXT,\n"
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
			"  wroteToDbAt DATETIME DEFAULT CURRENT_TIMESTAMP\n"
        ");";
	}
	
	static const char *INSERT_EVENT_QUERY(const SIpCameraEvent &ipCamEvent) {
		static const string constPart("INSERT INTO ip_camera_events (packetCounter, `datetime`, plateText, plateTextANSI, plateCountry, plateConfidence, cameraId, carState, geotag_lat, geotag_lon, "
		"imageType, plateImageType, plateImageSize, carMoveDirection, timeProcessing, plateCoordinates, carID, GEOtarget, sensorProviderID, rawJson) ");
		
		return string(constPart + "VALUES "
					"('" + ipCamEvent.packetCounter		+ "', '" + ipCamEvent.datetime			+ "', '" + ipCamEvent.plateText 
				+ "', '" + ipCamEvent.plateTextANSI		+ "', '" + ipCamEvent.plateCountry		+ "', '" + ipCamEvent.plateConfidence
				+ "', '" + ipCamEvent.cameraId			+ "', '" + ipCamEvent.carState			+ "', '" + ipCamEvent.geotag_lat		
				+ "', '" + ipCamEvent.geotag_lon		+ "', '" + ipCamEvent.imageType			+ "', '" + ipCamEvent.plateImageType
				+ "', '" + ipCamEvent.plateImageSize	+ "', '" + ipCamEvent.carMoveDirection	+ "', '" + ipCamEvent.timeProcessing
				+ "', '" + ipCamEvent.plateCoordinates	+ "', '" + ipCamEvent.carID				+ "', '" + ipCamEvent.GEOtarget
				+ "', '" + ipCamEvent.sensorProviderID	+ "', '" + ipCamEvent.rawJson 
				+ "');"
			).c_str();
	}
};

class CJsonParser {
    using string = std::string;
public:

	SIpCameraEvent parseIpCameraEvent(const std::string &jsonEvent){
		SIpCameraEvent ipCamEvent;

        try {
            std::stringstream strstream(jsonEvent);

            // Create an empty property tree object
			pt::ptree tree;

            // Parse the json into the property tree.
            pt::json_parser::read_json(strstream, tree);

            ipCamEvent.packetCounter = tree.get("packetCounter", string());
            ipCamEvent.datetime = tree.get("datetime", string());
			ipCamEvent.plateText = tree.get("plateText", string());
			ipCamEvent.plateTextANSI = tree.get("plateTextANSI", string());
			ipCamEvent.plateCountry = tree.get("plateCountry", string());
			ipCamEvent.plateConfidence = tree.get("plateConfidence", string());
			ipCamEvent.cameraId = tree.get("cameraId", string());
			ipCamEvent.carState = tree.get("carState", string());
			ipCamEvent.geotag_lat = tree.get("geotag.lat", string());
			ipCamEvent.geotag_lon = tree.get("geotag.lon", string());
			ipCamEvent.imageType = tree.get("imageType", string());
			ipCamEvent.plateImageType = tree.get("plateImageType", string());
			ipCamEvent.plateImageSize = tree.get("plateImageSize", string());
			ipCamEvent.carMoveDirection = tree.get("carMoveDirection", string());
			ipCamEvent.timeProcessing = tree.get("timeProcessing", string());
			ipCamEvent.plateCoordinates = tree.get("plateCoordinates", string());
			ipCamEvent.carID = tree.get("carID", string());
			ipCamEvent.GEOtarget = tree.get("GEOtarget", string());
			ipCamEvent.sensorProviderID = tree.get("sensorProviderID", string());
			ipCamEvent.rawJson = jsonEvent;

        } catch (std::exception &ex){
            LOG(WARNING) <<"ERROR (JsonParser): " <<ex.what();
        }

        return ipCamEvent;
    }


};

/* Example of event:
{
    "packetCounter":"67781",
    "datetime":"20190123 154156000",
    "plateText":"\u004f\u0042\u0032\u0035\u0035\u0038\u0041\u004b",
    "plateCountry":"BGR",
    "plateConfidence":"0.727815",
    "cameraId":"48EA633E7912",
    "carState":"new",
    "geotag":{
        "lat":50.418114,
        "lon":30.476213
    },
    "imageType":"plate",
    "plateImageType":"png",
    "plateImageSize":"0",
    "carMoveDirection":"in",
    "timeProcessing":"0",
    "plateCoordinates":[
        420,
        180,
        356,
        66
    ],
    "carID":"102",
    "GEOtarget":"Camera",
    "sensorProviderID":"Terminal_1"

}
*/

#endif //CS_CEVENTS_H
