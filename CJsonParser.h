#ifndef CS_IPCAMERASERVER_CJSONPARSER_H
#define CS_IPCAMERASERVER_CJSONPARSER_H
#pragma once

#include "main.h"
#include "glog/logging.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace boost::property_tree;

class CJsonParser {
    using string = std::string ;
public:
    struct IpCameraEvent {
        IpCameraEvent() = default;
        IpCameraEvent(const IpCameraEvent &) = default;
        IpCameraEvent &operator=(const IpCameraEvent &) = default;

        long packetCounter;
        string datetime;
        string plateText;
        string plateTextANSI;
        string plateCountry;
        double plateConfidence;
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
        string rowEvent;
    };

    IpCameraEvent parseIpCameraEvent(const std::string &jsonEvent){
        IpCameraEvent ipCamEvent;

        try {
            std::stringstream strstream(jsonEvent);

            // Create an empty property tree object
            ptree tree;

            // Parse the json into the property tree.
            json_parser::read_json(strstream, tree);

            ipCamEvent.packetCounter = tree.get("packetCounter", -1l);
            ipCamEvent.datetime = tree.get("datetime", std::string());

        }catch (std::exception &ex){
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

#endif //CS_IPCAMERASERVER_CJSONPARSER_H
