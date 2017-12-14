[
		{
		    "type" : "group",
		    "id" : "video",
		    "items" : [
		             {
		                "type" : "select",
		                "id" : "video.resolution",
		                "itms":[{"chose":"1080FHD"},{"chose":"720P 30FPS"},{"chose":"WVGA"},{"chose":"VGA"}],
		                "itmsvalue":[{"chose":"1080FHD"},{"chose":"720P 30FPS"},{"chose":"WVGA"},{"chose":"VGA"}]
		            }, {
		                "type" : "select",
		                "id" : "video.looprecording",
		                "itms":[{"chose":"Off"},{"chose":"1 Minute"},{"chose":"3 Minute"},{"chose":"5 Minute"}],
		                "itmsvalue":[{"chose":"Off"},{"chose":"1 Minute"},{"chose":"3 Minutes"},{"chose":"5 Minutes"}]
		            }, {
		                "type" : "toggle",
		                "id" : "video.rearcamera",
		                "itms":[{"chose":"On"},{"chose":"Off"}],
		                "itmsvalue":[{"chose":"On"},{"chose":"Off"}]
		            }, {
		                "type" : "toggle",
		                "id" : "video.pip",
		                "itms":[{"chose":"On"},{"chose":"Off"}],
		                "itmsvalue":[{"chose":"On"},{"chose":"Off"}]
		            },{
		                "type" : "toggle",
		                "id" : "video.frontbig",
		                "itms":[{"chose":"On"},{"chose":"Off"}],
		                "itmsvalue":[{"chose":"On"},{"chose":"Off"}]
		            },{
		                "type" : "toggle",
		                "id" : "video.wdr",
		                "itms":[{"chose":"On"},{"chose":"Off"}],
		                "itmsvalue":[{"chose":"On"},{"chose":"Off"}]
		            },{
		                "type" : "select",
		                "id" : "video.ev",
		                "itms":[{"chose":"+2.0"},{"chose":"+1.5"},{"chose":"+1.0"},{"chose":"+0.5"},{"chose":"0"},{"chose":"-0.5"},{"chose":"-1.0"},{"chose":"-1.5"},{"chose":"-2.0"}],
		                "itmsvalue":[{"chose":"+2.0"},{"chose":"+1.5"},{"chose":"+1.0"},{"chose":"+0.5"},{"chose":"0"},{"chose":"-0.5"},{"chose":"-1.0"},{"chose":"-1.5"},{"chose":"-2.0"}]
		            },{
		                "type" : "toggle",
		                "id" : "video.recordaudio",
		                "itms":[{"chose":"On"},{"chose":"Off"}],
		                "itmsvalue":[{"chose":"On"},{"chose":"Off"}]
		            },{
		                "type" : "toggle",
		                "id" : "video.datestamp",
		                "itms":[{"chose":"On"},{"chose":"Off"}],
		                "itmsvalue":[{"chose":"On"},{"chose":"Off"}]
		            },{
		                "type" : "select",
		                "id" : "video.gsensor",
		                "itms":[{"chose":"Off"},{"chose":"Low"},{"chose":"Medium"},{"chose":"High"}],
		                "itmsvalue":[{"chose":"Off"},{"chose":"Low"},{"chose":"Medium"},{"chose":"High"}]
		            },{
		                "type" : "toggle",
		                "id" : "video.platestamp",
		                "itms":[{"chose":"On"},{"chose":"Off"}],
		                "itmsvalue":[{"chose":"On"},{"chose":"Off"}]
		            }
		    ]
		}, {
		    "type" : "group",
		    "id" : "photo",
		    "items" : [
		            {
		                "type" : "select",
		                "id" : "photo.capturemode",
		                "itms":[{"chose":"Single"},{"chose":"2S Timer"},{"chose":"5S Timer"},{"chose":"10S Timer"}],
		                "itmsvalue":[{"chose":"Single"},{"chose":"2S Timer"},{"chose":"5S Timer"},{"chose":"10S Timer"}]
		            }, {
		                "type" : "select",
		                "id" : "photo.resolution",
		                "itms":[{"chose":"2MHD"},{"chose":"VGA"},{"chose":"1.3M"}],
		                "itmsvalue":[{"chose":"2MHD"},{"chose":"VGA"},{"chose":"1.3M"}]
		            }, {
		                "type" : "toggle",
		                "id" : "photo.sequence",
		                "itms":[{"chose":"On"},{"chose":"Off"}],
		                "itmsvalue":[{"chose":"On"},{"chose":"Off"}]
		            }, {
		                "type" : "select",
		                "id" : "photo.quality",
		                "itms":[{"chose":"Fine"},{"chose":"Normal"},{"chose":"Economy"}],
		                "itmsvalue":[{"chose":"Fine"},{"chose":"Normal"},{"chose":"Economy"}]
		            }, {
		                "type" : "select",
		                "id" : "photo.sharpness",
		                "itms":[{"chose":"Strong"},{"chose":"Normal"},{"chose":"Soft"}],
		                "itmsvalue":[{"chose":"Strong"},{"chose":"Normal"},{"chose":"Soft"}]
		            }, {
		                "type" : "select",
		                "id" : "photo.whitebalance",
		                "itms":[{"chose":"Auto"},{"chose":"Daylight"},{"chose":"Cloudy"},{"chose":"Tungsten"},{"chose":"Fluorescent"}],
		                "itmsvalue":[{"chose":"Auto"},{"chose":"Daylight"},{"chose":"Cloudy"},{"chose":"Tungsten"},{"chose":"Fluorescent"}]
		            }, {
		                "type" : "select",
		                "id" : "photo.color",
		                "itms":[{"chose":"Color"},{"chose":"Black&White"},{"chose":"Sepia"}],
		                "itmsvalue":[{"chose":"Color"},{"chose":"Black&White"},{"chose":"Sepia"}]
		            }, {
		                "type" : "select",
		                "id" : "photo.isolimit",
		                "itms":[{"chose":"Auto"},{"chose":"800"},{"chose":"400"},{"chose":"200"},{"chose":"100"}],
		                "itmsvalue":[{"chose":"Auto"},{"chose":"800"},{"chose":"400"},{"chose":"200"},{"chose":"100"}]
		            }, {
		                "type" : "select",
		                "id" : "photo.ev",
		                "itms":[{"chose":"+2.0"},{"chose":"+1.5"},{"chose":"+1.0"},{"chose":"+0.5"},{"chose":"0"},{"chose":"-0.5"},{"chose":"-1.0"},{"chose":"-1.5"},{"chose":"-2.0"}],
		                "itmsvalue":[{"chose":"+2.0"},{"chose":"+1.5"},{"chose":"+1.0"},{"chose":"+0.5"},{"chose":"0"},{"chose":"-0.5"},{"chose":"-1.0"},{"chose":"-1.5"},{"chose":"-2.0"}]
		            },  {
		                "type" : "toggle",
		                "id" : "photo.antishaking",
		                "itms":[{"chose":"On"},{"chose":"Off"}],
		                "itmsvalue":[{"chose":"On"},{"chose":"Off"}]
		            }, {
		                "type" : "toggle",
		                "id" : "photo.datestamp",
		                "itms":[{"chose":"On"},{"chose":"Off"}],
		                "itmsvalue":[{"chose":"On"},{"chose":"Off"}]
		            }
		    ]
		}, {
		    "type" : "group",
		    "id" : "setup",
		    "items" : [
		            {
		                "type" : "button",
		                "id" : "setup.wifi",
		                "itms":[{"chose":"Off"},{"chose":"On"}],
		                "itmsvalue":[{"chose":"Off"},{"chose":"On"}]
		            }, {
		                "type" : "toggle",
		                "id" : "setup.time.syncgps",
		                "itms":[{"chose":"Off"},{"chose":"On"}],
		                "itmsvalue":[{"chose":"Off"},{"chose":"On"}]
		            }, {
		                "type" : "select",
		                "id" : "setup.beepsound",
		                "itms":[{"chose":"10%"},{"chose":"20%"},{"chose":"30%"},{"chose":"40%"},{"chose":"50%"},{"chose":"60"},{"chose":"70%"},{"chose":"80%"},{"chose":"90%"},{"chose":"100%"}],
		                "itmsvalue":[{"chose":"10%"},{"chose":"20%"},{"chose":"30%"},{"chose":"40%"},{"chose":"50%"},{"chose":"60"},{"chose":"70%"},{"chose":"80%"},{"chose":"90%"},{"chose":"100%"}]
		            } ,{
		                "type" : "toggle",
		                "id" : "setup.keytone",
		                "itms":[{"chose":"Off"},{"chose":"On"}],
		                "itmsvalue":[{"chose":"Off"},{"chose":"On"}]
		            }, {
		                "type" : "select",
		                "id" : "setup.display.sleep",
		                "itms":[{"chose":"Off"},{"chose":"3 Minute"},{"chose":"5 Minute"},{"chose":"10 Minute"},{"chose":"Never"}],
		                "itmsvalue":[{"chose":"Off"},{"chose":"3 Minute"},{"chose":"5 Minutes"},{"chose":"10 Minutes"},{"chose":"Never"}]
		            }, {
		                "type" : "select",
		                "id" : "setup.display.brightness",
		                "itms":[{"chose":"High"},{"chose":"Medium"},{"chose":"Low"}],
		                "itmsvalue":[{"chose":"High"},{"chose":"Medium"},{"chose":"Low"}]
		            }, {
		                "type" : "select",
		                "id" : "setup.tvmode",
		                "itms":[{"chose":"NTSC"},{"chose":"PAL"}],
		                "itmsvalue":[{"chose":"NTSC"},{"chose":"PAL"}]
		            }, {
		                "type" : "toggle",
		                "id" : "setup.irled",
		                "itms":[{"chose":"On"},{"chose":"Off"}],
		                "itmsvalue":[{"chose":"On"},{"chose":"Off"}]
		            }, {
		                "type" : "toggle",
		                "id" : "setup.imagerotation",
		                "itms":[{"chose":"On"},{"chose":"Off"}],
		                "itmsvalue":[{"chose":"On"},{"chose":"Off"}]
		            }, {
		                "type" : "select",
		                "id" : "setup.platenumber",
		                "itms":[{"chose":"京A00001"}],
		                "itmsvalue":[{"chose":"京A00001"}]
		            }
		    ]
		}
]
