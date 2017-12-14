{
    "master_settings": {
        "CAMERA_LOCATE_ID": {
            "url": "camera/LL",
            "keys": [
                "ON",
                "OFF"
            ],
            "values": [
                1,
                0
            ]
        },
        "CAMERA_VIDEO_RESOLUTION_ID": {
            "url": "camera/VV",
            "keys": [
                "4K 17:9",
                "4K",
                "2.7K 17:9",
                "2.7K",
                "1440",
                "1080 Super View",
                "1080",
                "960",
                "720 Super View",
                "720",
                "WVGA"
            ],
            "values": [
                8,
                6,
                7,
                5,
                4,
                9,
                3,
                2,
                10,
                1,
                0
            ]
        },
        "CAMERA_FRAME_RATE_ID": {
            "url": "camera/FS",
            "keys": [
                "12.5 FPS",
                "240 FPS",
                "120 FPS",
                "100 FPS",
                "60 FPS",
                "50 FPS",
                "48 FPS",
                "30 FPS",
                "25 FPS",
                "24 FPS",
                "15 FPS",
                "12 FPS"
            ],
            "values": [
                11,
                10,
                9,
                8,
                7,
                6,
                5,
                4,
                3,
                2,
                1,
                0
            ]
        },
        "CAMERA_LOW_LIGHT_ID": {
            "url": "camera/LW",
            "keys": [
                "ON",
                "OFF"
            ],
            "values": [
                1,
                0
            ]
        },
        "CAMERA_FIELD_OF_VIEW_ID": {
            "url": "camera/FV",
            "keys": [
                "WIDE",
                "MEDIUM",
                "NARROW"
            ],
            "values": [
                0,
                1,
                2
            ]
        },
        "CAMERA_PHOTO_RESOLUTION_ID": {
            "url": "camera/PR",
            "keys": [
                "12MP WIDE",
                "7MP WIDE",
                "7MP MED",
                "5MP MED"
            ],
            "values": [
                5,
                4,
                6,
                3
            ]
        },
        "CAMERA_CONTINUOUS_SHOT_ID": {
            "url": "camera/CS",
            "keys": [
                "SINGLE",
                "3 SPS",
                "5 SPS",
                "10 SPS"
            ],
            "values": [
                0,
                3,
                5,
                10
            ]
        },
        "CAMERA_BURST_RATE_ID": {
            "url": "camera/BU",
            "keys": [
                "3/1 SEC",
                "5/1 SEC",
                "10/1 SEC",
                "10/2 SEC",
                "30/1 SEC",
                "30/2 SEC",
                "30/3 SEC"
            ],
            "values": [
                0,
                1,
                2,
                3,
                4,
                5,
                6
            ]
        },
        "CAMERA_TIME_INTERVAL_ID": {
            "url": "camera/TI",
            "keys": [
                "0.5 SEC",
                "1 SEC",
                "2 SEC",
                "5 SEC",
                "10 SEC",
                "30 SEC",
                "60 SEC"
            ],
            "values": [
                0,
                1,
                2,
                5,
                10,
                30,
                60
            ]
        },
        "CAMERA_UPSIDE_DOWN_IMAGE_CAPTURE_ID": {
            "url": "camera/UP",
            "keys": [
                "ON",
                "OFF"
            ],
            "values": [
                1,
                0
            ]
        },
        "CAMERA_SPOT_METER_ID": {
            "url": "camera/EX",
            "keys": [
                "ON",
                "OFF"
            ],
            "values": [
                1,
                0
            ]
        },
        "CAMERA_VIDEO_PLUS_PHOTO_ID": {
            "url": "camera/PN",
            "keys": [
                "OFF",
                "5 SEC",
                "10 SEC",
                "30 SEC",
                "60 SEC"
            ],
            "values": [
                0,
                1,
                2,
                3,
                4
            ]
        },
        "CAMERA_LOOPING_ID": {
            "url": "camera/LO",
            "keys": [
                "OFF",
                "MAX",
                "5 MIN",
                "20 MIN",
                "60 MIN",
                "120 MIN"
            ],
            "values": [
                0,
                5,
                1,
                2,
                3,
                4
            ]
        },
        "CAMERA_PROTUNE_ID": {
            "url": "camera/PT",
            "keys": [
                "ON",
                "OFF"
            ],
            "values": [
                1,
                0
            ]
        },
        "CAMERA_WHITE_BALANCE_ID": {
            "url": "camera/WB",
            "keys": [
                "AUTO",
                "3000K",
                "5500K",
                "6500K",
                "CAM RAW"
            ],
            "values": [
                0,
                1,
                2,
                3,
                4
            ]
        },
        "CAMERA_DEFAULT_POWER_ON_ID": {
            "url": "camera/DM",
            "keys": [
                "VIDEO",
                "PHOTO",
                "BURST",
                "TIME LAPSE"
            ],
            "values": [
                0,
                1,
                2,
                3
            ]
        },
        "CAMERA_VIDEO_STANDARD_MODE_ID": {
            "url": "camera/VM",
            "keys": [
                "NTSC",
                "PAL"
            ],
            "values": [
                0,
                1
            ]
        },
        "CAMERA_LED_BLINKING_ID": {
            "url": "camera/LB",
            "keys": [
                "4",
                "2",
                "OFF"
            ],
            "values": [
                2,
                1,
                0
            ]
        },
        "CAMERA_BEEPING_SOUND_ID": {
            "url": "camera/BS",
            "keys": [
                "100%",
                "70%",
                "OFF"
            ],
            "values": [
                2,
                1,
                0
            ]
        }
    },
    "dynamic_settings": [
        {
            "state": {
                "camera/PT": 1
            },
            "urls": {
                "camera/VV": [
                    0
                ]
            }
        },
        {
            "state": {
                "camera/VV": 10,
                "camera/VM": 0,
                "camera/PT": 0
            },
            "urls": {
                "camera/FS": [
                    0,
                    1,
                    2,
                    3,
                    4,
                    6,
                    9,
                    10,
                    11
                ]
            }
        },
        {
            "state": {
                "camera/VV": 10,
                "camera/VM": 0,
                "camera/PT": 1
            },
            "urls": {
                "camera/FS": [
                    0,
                    1,
                    2,
                    3,
                    4,
                    5,
                    6,
                    9,
                    10,
                    11
                ]
            }
        },
        {
            "state": {
                "camera/VV": 9,
                "camera/VM": 0
            },
            "urls": {
                "camera/FS": [
                    0,
                    1,
                    3,
                    6,
                    7,
                    8,
                    9,
                    10,
                    11
                ]
            }
        },
        {
            "state": {
                "camera/VV": 8
            },
            "urls": {
                "camera/FS": [
                    1,
                    2,
                    3,
                    4,
                    5,
                    6,
                    7,
                    8,
                    9,
                    10,
                    11
                ]
            }
        },
        {
            "state": {
                "camera/VV": 7
            },
            "urls": {
                "camera/FS": [
                    0,
                    1,
                    3,
                    4,
                    5,
                    6,
                    7,
                    8,
                    9,
                    10,
                    11
                ]
            }
        },
        {
            "state": {
                "camera/VV": 6,
                "camera/VM": 0
            },
            "urls": {
                "camera/FS": [
                    0,
                    2,
                    3,
                    4,
                    5,
                    6,
                    7,
                    8,
                    9,
                    10,
                    11
                ]
            }
        },
        {
            "state": {
                "camera/VV": 5,
                "camera/VM": 0
            },
            "urls": {
                "camera/FS": [
                    0,
                    1,
                    2,
                    3,
                    5,
                    6,
                    7,
                    8,
                    9,
                    10,
                    11
                ]
            }
        },
        {
            "state": {
                "camera/VV": 4,
                "camera/VM": 0
            },
            "urls": {
                "camera/FS": [
                    0,
                    1,
                    3,
                    6,
                    7,
                    8,
                    9,
                    10,
                    11
                ]
            }
        },
        {
            "state": {
                "camera/VV": 3,
                "camera/VM": 0
            },
            "urls": {
                "camera/FS": [
                    0,
                    1,
                    3,
                    6,
                    8,
                    9,
                    10,
                    11
                ]
            }
        },
        {
            "state": {
                "camera/VV": 2,
                "camera/VM": 0,
                "camera/PT": 0
            },
            "urls": {
                "camera/FS": [
                    0,
                    1,
                    2,
                    3,
                    4,
                    6,
                    9,
                    10,
                    11
                ]
            }
        },
        {
            "state": {
                "camera/VV": 2,
                "camera/VM": 0,
                "camera/PT": 1
            },
            "urls": {
                "camera/FS": [
                    0,
                    1,
                    2,
                    3,
                    4,
                    5,
                    6,
                    9,
                    10,
                    11
                ]
            }
        },
        {
            "state": {
                "camera/VV": 1,
                "camera/VM": 0
            },
            "urls": {
                "camera/FS": [
                    0,
                    1,
                    2,
                    3,
                    4,
                    5,
                    6,
                    8,
                    10,
                    11
                ]
            }
        },
        {
            "state": {
                "camera/VV": 0
            },
            "urls": {
                "camera/FS": [
                    0,
                    1,
                    2,
                    3,
                    4,
                    5,
                    6,
                    7,
                    8,
                    9,
                    11
                ]
            }
        },
        {
            "state": {
                "camera/VV": 10,
                "camera/VM": 1,
                "camera/PT": 0
            },
            "urls": {
                "camera/FS": [
                    0,
                    1,
                    2,
                    3,
                    4,
                    7,
                    9,
                    10,
                    11
                ]
            }
        },
        {
            "state": {
                "camera/VV": 10,
                "camera/VM": 1,
                "camera/PT": 1
            },
            "urls": {
                "camera/FS": [
                    0,
                    1,
                    2,
                    3,
                    4,
                    5,
                    7,
                    9,
                    10,
                    11
                ]
            }
        },
        {
            "state": {
                "camera/VV": 9,
                "camera/VM": 1
            },
            "urls": {
                "camera/FS": [
                    0,
                    1,
                    4,
                    6,
                    7,
                    8,
                    9,
                    10,
                    11
                ]
            }
        },
        {
            "state": {
                "camera/VV": 6,
                "camera/VM": 1
            },
            "urls": {
                "camera/FS": [
                    0,
                    1,
                    2,
                    3,
                    4,
                    5,
                    6,
                    7,
                    8,
                    9,
                    10
                ]
            }
        },
        {
            "state": {
                "camera/VV": 5,
                "camera/VM": 1
            },
            "urls": {
                "camera/FS": [
                    0,
                    1,
                    2,
                    4,
                    5,
                    6,
                    7,
                    8,
                    9,
                    10,
                    11
                ]
            }
        },
        {
            "state": {
                "camera/VV": 4,
                "camera/VM": 1
            },
            "urls": {
                "camera/FS": [
                    0,
                    1,
                    4,
                    6,
                    7,
                    8,
                    9,
                    10,
                    11
                ]
            }
        },
        {
            "state": {
                "camera/VV": 3,
                "camera/VM": 1
            },
            "urls": {
                "camera/FS": [
                    0,
                    1,
                    4,
                    7,
                    8,
                    9,
                    10,
                    11
                ]
            }
        },
        {
            "state": {
                "camera/VV": 2,
                "camera/VM": 1,
                "camera/PT": 0
            },
            "urls": {
                "camera/FS": [
                    0,
                    1,
                    2,
                    3,
                    4,
                    7,
                    9,
                    10,
                    11
                ]
            }
        },
        {
            "state": {
                "camera/VV": 2,
                "camera/VM": 1,
                "camera/PT": 1
            },
            "urls": {
                "camera/FS": [
                    0,
                    1,
                    2,
                    3,
                    4,
                    5,
                    7,
                    9,
                    10,
                    11
                ]
            }
        },
        {
            "state": {
                "camera/VV": 1,
                "camera/VM": 1
            },
            "urls": {
                "camera/FS": [
                    0,
                    1,
                    2,
                    3,
                    4,
                    5,
                    7,
                    9,
                    10,
                    11
                ]
            }
        },
        {
            "state": {
                "camera/VV": 10
            },
            "urls": {
                "camera/FV": [
                    1,
                    2
                ]
            }
        },
        {
            "state": {
                "camera/VV": 9
            },
            "urls": {
                "camera/FV": [
                    1,
                    2
                ]
            }
        },
        {
            "state": {
                "camera/VV": 8
            },
            "urls": {
                "camera/FV": [
                    1,
                    2
                ]
            }
        },
        {
            "state": {
                "camera/VV": 7
            },
            "urls": {
                "camera/FV": [
                    2
                ]
            }
        },
        {
            "state": {
                "camera/VV": 6
            },
            "urls": {
                "camera/FV": [
                    1,
                    2
                ]
            }
        },
        {
            "state": {
                "camera/VV": 7
            },
            "urls": {
                "camera/FV": [
                    2
                ]
            }
        },
        {
            "state": {
                "camera/VV": 5
            },
            "urls": {
                "camera/FV": [
                    2
                ]
            }
        },
        {
            "state": {
                "camera/VV": 4
            },
            "urls": {
                "camera/FV": [
                    1,
                    2
                ]
            }
        },
        {
            "state": {
                "camera/VV": 2
            },
            "urls": {
                "camera/FV": [
                    1,
                    2
                ]
            }
        },
        {
            "state": {
                "camera/VV": 1,
                "camera/FS": 8
            },
            "urls": {
                "camera/FV": [
                    1
                ]
            }
        },
        {
            "state": {
                "camera/VV": 1,
                "camera/FS": 9
            },
            "urls": {
                "camera/FV": [
                    1
                ]
            }
        },
        {
            "state": {
                "camera/VV": 0
            },
            "urls": {
                "camera/FV": [
                    1,
                    2
                ]
            }
        },
        {
            "state": {
                "camera/PT": 0
            },
            "urls": {
                "camera/WB": [
                    0,
                    1,
                    2,
                    3,
                    4
                ]
            }
        },
        {
            "state": {
                "camera/PT": 1
            },
            "urls": {
                "camera/LO": [
                    0,
                    1,
                    2,
                    3,
                    4,
                    5
                ]
            }
        },
        {
            "state": {
                "camera/PT": 1
            },
            "urls": {
                "camera/PN": [
                    0,
                    1,
                    2,
                    3,
                    4
                ]
            }
        },
        {
            "state": {
                "camera/VV": 10
            },
            "urls": {
                "camera/PN": [
                    0,
                    1,
                    2,
                    3,
                    4
                ]
            }
        },
        {
            "state": {
                "camera/VV": 9
            },
            "urls": {
                "camera/PN": [
                    0,
                    1,
                    2,
                    3,
                    4
                ]
            }
        },
        {
            "state": {
                "camera/VV": 8
            },
            "urls": {
                "camera/PN": [
                    0,
                    1,
                    2,
                    3,
                    4
                ]
            }
        },
        {
            "state": {
                "camera/VV": 7
            },
            "urls": {
                "camera/PN": [
                    0,
                    1,
                    2,
                    3,
                    4
                ]
            }
        },
        {
            "state": {
                "camera/VV": 6
            },
            "urls": {
                "camera/PN": [
                    0,
                    1,
                    2,
                    3,
                    4
                ]
            }
        },
        {
            "state": {
                "camera/VV": 5
            },
            "urls": {
                "camera/PN": [
                    0,
                    1,
                    2,
                    3,
                    4
                ]
            }
        },
        {
            "state": {
                "camera/VV": 4,
                "camera/FS": 5
            },
            "urls": {
                "camera/PN": [
                    0,
                    1,
                    2,
                    3,
                    4
                ]
            }
        },
        {
            "state": {
                "camera/VV": 4,
                "camera/FS": 4
            },
            "urls": {
                "camera/PN": [
                    0,
                    1,
                    2,
                    3,
                    4
                ]
            }
        },
        {
            "state": {
                "camera/VV": 4,
                "camera/FS": 3
            },
            "urls": {
                "camera/PN": [
                    0,
                    1,
                    2,
                    3,
                    4
                ]
            }
        },
        {
            "state": {
                "camera/VV": 3,
                "camera/FS": 7
            },
            "urls": {
                "camera/PN": [
                    0,
                    1,
                    2,
                    3,
                    4
                ]
            }
        },
        {
            "state": {
                "camera/VV": 3,
                "camera/FS": 5
            },
            "urls": {
                "camera/PN": [
                    0,
                    1,
                    2,
                    3,
                    4
                ]
            }
        },
        {
            "state": {
                "camera/VV": 3,
                "camera/FS": 6
            },
            "urls": {
                "camera/PN": [
                    0,
                    1,
                    2,
                    3,
                    4
                ]
            }
        },
        {
            "state": {
                "camera/VV": 2
            },
            "urls": {
                "camera/PN": [
                    0,
                    1,
                    2,
                    3,
                    4
                ]
            }
        },
        {
            "state": {
                "camera/VV": 1,
                "camera/FS": 9
            },
            "urls": {
                "camera/PN": [
                    0,
                    1,
                    2,
                    3,
                    4
                ]
            }
        },
        {
            "state": {
                "camera/VV": 1,
                "camera/FS": 8
            },
            "urls": {
                "camera/PN": [
                    0,
                    1,
                    2,
                    3,
                    4
                ]
            }
        },
        {
            "state": {
                "camera/VV": 0
            },
            "urls": {
                "camera/PN": [
                    0,
                    1,
                    2,
                    3,
                    4
                ]
            }
        },
        {
            "state": {
                "camera/FS": 0
            },
            "urls": {
                "camera/LW": [
                    0,
                    1
                ]
            }
        },
        {
            "state": {
                "camera/FS": 1
            },
            "urls": {
                "camera/LW": [
                    0,
                    1
                ]
            }
        },
        {
            "state": {
                "camera/FS": 2
            },
            "urls": {
                "camera/LW": [
                    0,
                    1
                ]
            }
        },
        {
            "state": {
                "camera/FS": 3
            },
            "urls": {
                "camera/LW": [
                    0,
                    1
                ]
            }
        },
        {
            "state": {
                "camera/FS": 4
            },
            "urls": {
                "camera/LW": [
                    0,
                    1
                ]
            }
        },
        {
            "state": {
                "camera/FS": 8
            },
            "urls": {
                "camera/LW": [
                    0,
                    1
                ]
            }
        },
        {
            "state": {
                "camera/FS": 9
            },
            "urls": {
                "camera/LW": [
                    0,
                    1
                ]
            }
        },
        {
            "state": {
                "camera/FS": 10
            },
            "urls": {
                "camera/LW": [
                    0,
                    1
                ]
            }
        },
        {
            "state": {
                "camera/FS": 11
            },
            "urls": {
                "camera/LW": [
                    0,
                    1
                ]
            }
        }
    ]
}