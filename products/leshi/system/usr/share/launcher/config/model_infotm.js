[
		{
		    "type" : "group",
		    "id" : "video",
		    "items" : [
		             {
		                "type" : "select",
		                "id" : "video.resolution",
		                "itms":[{"chose":"1920x1920"},{"chose":"1088x1088"},{"chose":"768x768"}],
		                "itmsvalue":[{"chose":"1920x1920"},{"chose":"1088x1088"},{"chose":"768x768"}]
		            },{
		                "type" : "select",
		                "id" : "video.recordmode",
		                "itms":[{"chose":"普通"},{"chose":"高速摄影"},{"chose":"缩时录像"}],
		                "itmsvalue":[{"chose":"Normal"},{"chose":"Video High Speed"},{"chose":"Video Timelapsed Image"}]
		            },{
		                "type" : "select",
		                "id" : "video.preview",
		                "itms":[{"chose":"360全景圆形"},{"chose":"360全景展开"},{"chose":"二分割"},{"chose":"三分割"},{"chose":"四分割"},{"chose":"圆环"},{"chose":"VR"}],
		                "itmsvalue":[{"chose":"360 Panorama Round"},{"chose":"360 Panorama Round"},{"chose":"Two Segment"},{"chose":"Three Segment"},{"chose":"Four Segment"},{"chose":"Round"},{"chose":"VR"}]
		            },{
		                "type" : "select",
		                "id" : "video.looprecording",
		                "itms":[{"chose":"关"},{"chose":"1 分钟"},{"chose":"3 分钟"},{"chose":"5 分钟"}],
		                "itmsvalue":[{"chose":"Off"},{"chose":"1 Minute"},{"chose":"3 Minutes"},{"chose":"5 Minutes"}]
		            },{
		                "type" : "select",
		                "id" : "video.timelapsedimage",
		                "itms":[{"chose":"关"},{"chose":"1 秒"},{"chose":"2 秒"},{"chose":"3 秒"},{"chose":"5 秒"},{"chose":"10 秒"}],
		                "itmsvalue":[{"chose":"Off"},{"chose":"1s"},{"chose":"2s"},{"chose":"3s"},{"chose":"5s"},{"chose":"10s"}]
		            },{
		                "type" : "toggle",
		                "id" : "video.highspeed",
		                "itms":[{"chose":"开"},{"chose":"关"}],
		                "itmsvalue":[{"chose":"On"},{"chose":"Off"}]
		            },{
		                "type" : "toggle",
		                "id" : "video.wdr",
		                "itms":[{"chose":"开"},{"chose":"关"}],
		                "itmsvalue":[{"chose":"On"},{"chose":"Off"}]
		            },{
		                "type" : "select",
		                "id" : "video.ev",
		                "itms":[{"chose":"+2.0"},{"chose":"+1.5"},{"chose":"+1.0"},{"chose":"+0.5"},{"chose":"0"},{"chose":"-0.5"},{"chose":"-1.0"},{"chose":"-1.5"},{"chose":"-2.0"}],
		                "itmsvalue":[{"chose":"+2.0"},{"chose":"+1.5"},{"chose":"+1.0"},{"chose":"+0.5"},{"chose":"0"},{"chose":"-0.5"},{"chose":"-1.0"},{"chose":"-1.5"},{"chose":"-2.0"}]
		            },{
		                "type" : "select",
		                "id" : "video.gsensor",
		                "itms":[{"chose":"关"},{"chose":"低"},{"chose":"中"},{"chose":"高"}],
		                "itmsvalue":[{"chose":"Off"},{"chose":"Low"},{"chose":"Medium"},{"chose":"High"}]
		            },{
		                "type" : "toggle",
		                "id" : "video.datestamp",
		                "itms":[{"chose":"开"},{"chose":"关"}],
		                "itmsvalue":[{"chose":"On"},{"chose":"Off"}]
		            },{
		                "type" : "toggle",
		                "id" : "video.recordaudio",
		                "itms":[{"chose":"开"},{"chose":"关"}],
		                "itmsvalue":[{"chose":"On"},{"chose":"Off"}]
		            }
		    ]
		},{
		    "type" : "group",
		    "id" : "photo",
		    "items" : [
		            {
		                "type" : "select",
		                "id" : "photo.resolution",
		                "itms":[{"chose":"16M(4096x4096)"},{"chose":"10M(3264x3264)"},{"chose":"8M(2880x2880)"},{"chose":"4M(2448x2448)"},{"chose":"1088x1088"}],
		                "itmsvalue":[{"chose":"16M(4096x4096)"},{"chose":"10M(3264x3264)"},{"chose":"8M(2880x2880)"},{"chose":"4M(2448x2448)"},{"chose":"1088x1088"}]
		            },{
		                "type" : "select",
		                "id" : "photo.capturemode",
		                "itms":[{"chose":"普通"},{"chose":"定时拍照"},{"chose":"自动拍照"},{"chose":"连续拍照"}],
		                "itmsvalue":[{"chose":"Normal"},{"chose":"Capture By Timer"},{"chose":"Loop Capture"},{"chose":"Capture Sequence"}]
		            },{
		                "type" : "select",
		                "id" : "photo.preview",
		                "itms":[{"chose":"360全景圆形"},{"chose":"360全景展开"},{"chose":"二分割"},{"chose":"三分割"},{"chose":"四分割"},{"chose":"圆环"},{"chose":"VR"}],
		                "itmsvalue":[{"chose":"360 Panorama Round"},{"chose":"360 Panorama Round"},{"chose":"Two Segment"},{"chose":"Three Segment"},{"chose":"Four Segment"},{"chose":"Round"},{"chose":"VR"}]
		            },{
		                "type" : "select",
		                "id" : "photo.modetimer",
		                "itms":[{"chose":"关"},{"chose":"2秒"},{"chose":"5秒"},{"chose":"10秒"}],
		                "itmsvalue":[{"chose":"Off"},{"chose":"2s"},{"chose":"5s"},{"chose":"10s"}]
		            },{
		                "type" : "select",
		                "id" : "photo.modeauto",
		                "itms":[{"chose":"关"},{"chose":"3秒"},{"chose":"10秒"},{"chose":"15秒"},{"chose":"20秒"},{"chose":"30秒"}],
		                "itmsvalue":[{"chose":"Off"},{"chose":"3s"},{"chose":"10s"},{"chose":"15s"},{"chose":"20s"},{"chose":"30s"}]
		            },{
		                "type" : "select",
		                "id" : "photo.modesequence",
		                "itms":[{"chose":"关"},{"chose":"3张/秒"},{"chose":"5张/秒"},{"chose":"7张/秒"},{"chose":"10张/秒"}],
		                "itmsvalue":[{"chose":"Off"},{"chose":"3P/S"},{"chose":"5P/S"},{"chose":"7P/S"},{"chose":"10P/S"}]
		            }, {
		                "type" : "select",
		                "id" : "photo.quality",
		                "itms":[{"chose":"好"},{"chose":"一般"},{"chose":"经济"}],
		                "itmsvalue":[{"chose":"Fine"},{"chose":"Normal"},{"chose":"Economy"}]
		            }, {
		                "type" : "select",
		                "id" : "photo.sharpness",
		                "itms":[{"chose":"强烈"},{"chose":"一般"},{"chose":"柔和"}],
		                "itmsvalue":[{"chose":"Strong"},{"chose":"Normal"},{"chose":"Soft"}]
		            }, {
		                "type" : "select",
		                "id" : "photo.whitebalance",
		                "itms":[{"chose":"自动"},{"chose":"日光"},{"chose":"阴天"},{"chose":"钨丝灯"},{"chose":"荧光灯"}],
		                "itmsvalue":[{"chose":"Auto"},{"chose":"Daylight"},{"chose":"Cloudy"},{"chose":"Tungsten"},{"chose":"Fluorescent"}]
		            }, {
		                "type" : "select",
		                "id" : "photo.color",
		                "itms":[{"chose":"色彩"},{"chose":"黑白"},{"chose":"棕褐色"}],
		                "itmsvalue":[{"chose":"Color"},{"chose":"Black & White"},{"chose":"Sepia"}]
		            }, {
		                "type" : "select",
		                "id" : "photo.isolimit",
		                "itms":[{"chose":"自动"},{"chose":"800"},{"chose":"400"},{"chose":"200"},{"chose":"100"}],
		                "itmsvalue":[{"chose":"Auto"},{"chose":"800"},{"chose":"400"},{"chose":"200"},{"chose":"100"}]
		            }, {
		                "type" : "select",
		                "id" : "photo.ev",
		                "itms":[{"chose":"+2.0"},{"chose":"+1.5"},{"chose":"+1.0"},{"chose":"+0.5"},{"chose":"0"},{"chose":"-0.5"},{"chose":"-1.0"},{"chose":"-1.5"},{"chose":"-2.0"}],
		                "itmsvalue":[{"chose":"+2.0"},{"chose":"+1.5"},{"chose":"+1.0"},{"chose":"+0.5"},{"chose":"0"},{"chose":"-0.5"},{"chose":"-1.0"},{"chose":"-1.5"},{"chose":"-2.0"}]
		            },  {
		                "type" : "toggle",
		                "id" : "photo.antishaking",
		                "itms":[{"chose":"开"},{"chose":"关"}],
		                "itmsvalue":[{"chose":"On"},{"chose":"Off"}]
		            }, {
		                "type" : "toggle",
		                "id" : "photo.datestamp",
		                "itms":[{"chose":"开"},{"chose":"关"}],
		                "itmsvalue":[{"chose":"On"},{"chose":"Off"}]
		            }
		    ]
		},{
		    "type" : "group",
		    "id" : "setup",
		    "items" : [
		            {
		                "type" : "select",
		                "id" : "setup.language",
		                "itms":[{"chose":"英"},{"chose":"简"},{"chose":"繁"},{"chose":"法"},{"chose":"西"},{"chose":"意"},{"chose":"葡"},{"chose":"德"},{"chose":"俄"},{"chose":"韩"},{"chose":"日"}],
		                "itmsvalue":[{"chose":"EN"},{"chose":"ZH"},{"chose":"TW"},{"chose":"FR"},{"chose":"ES"},{"chose":"IT"},{"chose":"PT"},{"chose":"DE"},{"chose":"RU"},{"chose":"KR"},{"chose":"JP"}]
		            } ,{
		                "type" : "select",
		                "id" : "setup.wifimode",
		                "itms":[{"chose":"关"},{"chose":"直连"},{"chose":"连接路由器"}],
		                "itmsvalue":[{"chose":"Off"},{"chose":"Wifi Direct"},{"chose":"Wifi Router"}]
		            },{
		                "type" : "select",
		                "id" : "setup.ledfreq",
		                "itms":[{"chose":"关"},{"chose":"50HZ"},{"chose":"60HZ"}],
		                "itmsvalue":[{"chose":"Off"},{"chose":"50HZ"},{"chose":"60HZ"}]
		            }, {
		                "type" : "toggle",
		                "id" : "setup.led",
		                "itms":[{"chose":"关"},{"chose":"开"}],
		                "itmsvalue":[{"chose":"Off"},{"chose":"On"}]
		            }, {
		                "type" : "toggle",
		                "id" : "setup.cardvmode",
		                "itms":[{"chose":"关"},{"chose":"开"}],
		                "itmsvalue":[{"chose":"Off"},{"chose":"On"}]
		            }, {
		                "type" : "toggle",
		                "id" : "setup.parkingguard",
		                "itms":[{"chose":"开"},{"chose":"关"}],
		                "itmsvalue":[{"chose":"On"},{"chose":"Off"}]
		            }, {
		                "type" : "toggle",
		                "id" : "setup.imagerotation",
		                "itms":[{"chose":"开"},{"chose":"关"}],
		                "itmsvalue":[{"chose":"On"},{"chose":"Off"}]
		            }, {
		                "type" : "select",
		                "id" : "setup.volume",
		                "itms":[{"chose":"10%"},{"chose":"20%"},{"chose":"30%"},{"chose":"40%"},{"chose":"50%"},{"chose":"60%"},{"chose":"70%"},{"chose":"80%"},{"chose":"90%"},{"chose":"100%"}],
		                "itmsvalue":[{"chose":"10%"},{"chose":"20%"},{"chose":"30%"},{"chose":"40%"},{"chose":"50%"},{"chose":"60%"},{"chose":"70%"},{"chose":"80%"},{"chose":"90%"},{"chose":"100%"}]
		            } ,{
		                "type" : "toggle",
		                "id" : "setup.keytone",
		                "itms":[{"chose":"关"},{"chose":"开"}],
		                "itmsvalue":[{"chose":"Off"},{"chose":"On"}]
		            }, {
		                "type" : "select",
		                "id" : "setup.brightness",
		                "itms":[{"chose":"高"},{"chose":"中"},{"chose":"低"}],
		                "itmsvalue":[{"chose":"High"},{"chose":"Medium"},{"chose":"Low"}]
		            }, {
		                "type" : "select",
		                "id" : "setup.autosleep",
		                "itms":[{"chose":"关"},{"chose":"10秒"},{"chose":"20秒"},{"chose":"30秒"}],
		                "itmsvalue":[{"chose":"Off"},{"chose":"10s"},{"chose":"20s"},{"chose":"30s"}]
		            }, {
		                "type" : "select",
		                "id" : "setup.autopoweroff",
		                "itms":[{"chose":"关"},{"chose":"1分钟"},{"chose":"3分钟"},{"chose":"5分钟"}],
		                "itmsvalue":[{"chose":"Off"},{"chose":"1 Minute"},{"chose":"3 Minutes"},{"chose":"5 Minutes"}]
		            }, {
		                "type" : "select",
		                "id" : "setup.delayedshutdown",
		                "itms":[{"chose":"关"},{"chose":"10秒"},{"chose":"20秒"},{"chose":"30秒"}],
		                "itmsvalue":[{"chose":"Off"},{"chose":"10s"},{"chose":"20s"},{"chose":"30s"}]
		            },{
		                "type" : "button",
		                "id" : "setup.format",
		                "itms":[{"chose":"开"},{"chose":"关"}],
		                "itmsvalue":[{"chose":"On"},{"chose":"Off"}]
		            }
		            ,{
		                "type" : "button",
		                "id" : "setup.reset",
		                "itms":[{"chose":"开"},{"chose":"关"}],
		                "itmsvalue":[{"chose":"On"},{"chose":"Off"}]
		            }
		    ]
		}
]
