# CS_IpCameraServer

#### This is cross platform TCP/IP server on boost::asio, that listen events from Ip camera, write it to sqlite db


- ### для того чтобы построить нужные библиотеки из boost и установить их в папку с проэктом
    - сделайте cd ./boost_1_67_0
    - затем выполните следующие 2 команды, предварительно изменив в путях '/home/childcity/CLionProjects' на папку, где лежит CS_IpCameraServer

        - ```./bootstrap.sh --with-libraries=system,thread,regex #--show-libraries```

        - ```./b2 -j2 --libdir=/home/childcity/CLionProjects/CS_IpCameraServer/libs/boost --includedir=/home/childcity/CLionProjects/CS_IpCameraServer/include --with-thread --with-regex --with-date_time --with-system variant=release link=static threading=multi runtime-link=shared install```

    - #### glog 3.5 должен быть установлен в систему

- ### Чтобы скомпилировать сервер выполните
```
    cd ./CS_IpCameraServer
    mkdir build
    cd build
    cmake ..
    cmake --build . --config Release --target CS_IpCameraServer -- -j 4
```

- ### Пример события от камеры:
    ```
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
    ```

- ### Команды серверу
    - #### Структура и пример команды серверу:
        ```
        {
            "command": "get_last_event"
        }
        {
            "command": "login",
            "params": {
                "message": "user_first"
            }
        }
        {
            "command": "fibo",
            "params": {
                "message": "25"
            }
        }
        {
            "command": "exit"
        }
        {
            "command": "who" // return list of all current clients
        }
        ```
    - #### Структура и пример ответа от сервера:
        - #### If positive:
            ```
            {
                "command": "get_last_event",
                "params": {
                        "status": "ok"
                        "event": {} // event from ip camera
                    }
                "timestamp": 123455234, // in milliseconds!
                "server_datatime": "YYYY-MM-DD HH:MM:SS"
            }
            ```

        - #### If negative (error occur):
            ```
            {
                "command": "get_last_event",
                "params": {
                        "status": "error",
                        "message": "ERROR: "
                    }
                "timestamp": 123455234,
                "server_datetime": ""
            }
            ```
