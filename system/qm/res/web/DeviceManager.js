// File: DeviceManager.js
// Description: the device manager for single IPCamera or DVS.
// Author: huangsanyi

// Change __DEBUG to 1 when you want to run the script in debug mode. 
// Meanwhile you have to modify strServerIP, nPort, strUserName and strUserPwd.
// When __DEBUG = 0, the script will be run in release mode and no debug message will be given out.
var __DEBUG = 0;

var nSetScreenType = 2; //0默认比例，1默认比例，2适应窗
var nOriginalType = 0; //0适应窗，1原始尺寸
var nStreamType = 1; //0主码流lan，1子码流wan
var RecordTime = 600000; //录像文件时长(ms)

var DeviceManager = {
    // The version number of the coressponding OCX control.
    // It must be the same as the number you get using GetVersion() method of the OCX control.
    nOCXVersion: 1357747200, //1.13.01.10
    // OCX install packet.
    strOCXPacket: "TLNVOCX_setup.exe",

    // Server IP and command port.
    strServerIP: "172.20.120.179",
    nPort: 8200,
    strUserName: "admin",
    strUserPwd: "admin",
    // stream type

    //OCX ID
    strOCXId: "tlocx",
    ocxHandle: null,
    
    recTimer: null,

    objHtml: "",

    // online flag
    bConnected: false,
    // the total channels.
    nChannelCount: 0,
    // the channel open-close flags.
    bChannelOpened: [],
    // the channel record flags.
    bRecordOn: [],
    // the audio-talk flag.
    bTalkOn: false,
    // the voice flag.
    bVoiceOn: false,
    // the PTZ auto-scan flag.
    bAutoScan: false,
    // the wipper flag.
    bWipperOn: false,
    // the light flag.
    bLightOn: false,
    // the minimum value of pre-set point.
    nMinPrePointValue: 0,
    // the maximum value of pre-set point.
    nMaxPrePointValue: 255,
    // the selected language packet.
    lang: {},

    // the current function page, 0 realplay, 1 playback
    page: 0,

    // if Initialization
    bInit: false,

    bInitColorParam: false,
    //frequency
    nFrequency: 0,

    // Initialization entry.
    Init: function() {
        //Get browser
        GetBbowser();

        this.InitObjHtml();
        this.ocxHandle = document.getElementById(this.strOCXId);

        if (nBrowser != "ie") {
            document.body.appendChild(this.ocxHandle);
        }

        // set the language.
        this.SetLang(null);

        // Check the OCX
        this.CheckOCX();

        try {
            // Initialize the event handlers.
            var _this = this;

            // the handler of the "OnVideoWndSelec" event.
            if (nBrowser == "ie") {
                // the handler of the "OnVideoWndSelec" event.
                // IE11不支持attachEvent，attachEvent 是很旧的非标准方法。请使用 addEventListener。
                if (this.ocxHandle.attachEvent) {
                    this.ocxHandle.attachEvent("OnVideoWndSelect", function(idx) {
                        _this.OnVideoWndSelect(idx);
                    });
                } else if (this.ocxHandle.addEventListener) {
                    this.ocxHandle.addEventListener("OnVideoWndSelect", function(idx) {
                        _this.OnVideoWndSelect(idx);
                    });
                }
            }

            // Login button.
            $("#loginBtn").click(function() {

                // get the user-name and password the operator typed
                /*if(this.ocxHandle == null)
                {
                	alert("OCX is null!");
                	return;
                }*/
                var strInputUserName = $("#userName").val();
                var strInputUserPwd = $("#userPwd").val();
                if (strInputUserName.length == 0 || strInputUserPwd.length == 0) {
                    alert(_this.lang["MSG"]["blank_user_pwd"].text);
                    return;
                }
                if (!__DEBUG) {
                    _this.strUserName = strInputUserName;
                    _this.strUserPwd = strInputUserPwd;
                }
                // get the stream type corresponding the netwrok type the operator selected.
                var net_type = $("#idNType").val();
                if (net_type == "lan") {
                    nStreamType = 0;
                } else if (net_type == "wan") {
                    nStreamType = 1;
                }
                // launch the longin routine.
                if (_this.LogIn()) {
                    // Clear up the user-name and password the operator typed in the login page
                    // As it should not be kept.
                    $("#userPwd").val("");
                    setCookie("userName", strInputUserName); //cookie保留15天

                    // navigate to the realplay function page.
                    $("#LoginPage").css("display", "none");
                    $("#mainBox").css("display", "block");

                    // set default real play parameters
                    $("#StreamSelect").val(nStreamType);
                    $("#ScaleSelect").val(nSetScreenType);

                    if (nBrowser != "ie") {
                        $("#ocxDiv").append(_this.ocxHandle);
                        _this.ocxHandle.SetControlLanguage(_this.lang["LANG_ID"]);
                        // Get the channel count.
                        _this.nChannelCount = _this.ocxHandle.GetChannelCount();
                        _this.ocxHandle.SetShowWindows(_this.nChannelCount);
                        for (var i = 0; i < _this.nChannelCount; ++i) {
                            if (_this.OpenChannel(i)) {
                                _this.bChannelOpened[i] = true;
                                _this.bRecordOn[i] = false;
                            }
                        }
                    }

                    ResizeBody();
                    InitImageParams();
                }
            });

            $("#cancelloginBtn").click(function() {
                window.close();
            });
            //handler of ENTER key event in the password box.
            $("#userPwd").keypress(function(e) {
                e = e || window.event;
                if (e.keyCode == 13) //Enter
                {
                    $("#loginBtn").click(); //simulate a click to the login button.
                }
            });
            // select a language
            $("#language").change(function() {
                _this.SetLang(this.value);
            });

            // the realplay funciton button.
            $("#liveBtn").click(function() {
                _this.RealPlayPage();
            });
            // the playback function button.
            $("#playbackBtn").click(function() {
                _this.PlayBackPage();
            });
            // the setup button.
            $("#setupBtn").click(function() {
                _this.Setup();
            });
            // the logout button.
            $("#logoutBtn").click(function() {
                _this.LogOut();
            });

            //Snap
            $("#snapBtn").click(function() {
                if (_this.Snap()) {
                    window.status = _this.lang["MSG"]["snap_ok"].text;
                } else {
                    alert(_this.lang["MSG"]["snap_fail"].text);
                }
            });
            // start/stop recording
            $("#recordBtn").click(function() {
                var nCurrentChannel = _this.GetCurrentChannel();
                _this.RecordPro(nCurrentChannel, !_this.bRecordOn[nCurrentChannel]);
                if (_this.recTimer != null)
                {
                    clearTimeout(_this.recTimer);
                }
                _this.recTimer = setTimeout(function() { RecordRestart(); }, RecordTime);
                _this.UpdateChannelUI(nCurrentChannel);
            });
            // start/stop audio talk.
            $("#talkBtn").click(function() {
                _this.VoiceTalk();
                if (_this.bTalkOn) {
                    $("#talkBtn").toggleClass("off", false);
                    $("#talkBtn").toggleClass("on", true);
                } else {
                    $("#talkBtn").toggleClass("on", false);
                    $("#talkBtn").toggleClass("off", true);
                }
            });
            //zoom
            /*$("#zoomBtn").click(function() {
                _this.el_zoomBtn();
            });*/
            // the mute switch.
            $("#VoiceBtn").click(function() {
                _this.MuteSwitch();
                if (_this.bVoiceOn) {
                    $("#VoiceBtn").toggleClass("off", false);
                    $("#VoiceBtn").toggleClass("on", true);
                } else {
                    $("#VoiceBtn").toggleClass("on", false);
                    $("#VoiceBtn").toggleClass("off", true);
                }
            });


            // the PTZ pannel.
            //UpLeft
            $("#ptzupleft").mousedown(function() {
                _this.StartUpLeftWard();
            });
            $("#ptzupleft").mouseup(function() {
                _this.StopUpLeftWard();
            });
            //Up
            $("#ptzup").mousedown(function() {
                _this.StartUpWard();
            });
            $("#ptzup").mouseup(function() {
                _this.StopUpWard();
            });
            //UpRight
            $("#ptzupright").mousedown(function() {
                _this.StartUpRightWard();
            });
            $("#ptzupright").mouseup(function() {
                _this.StopUpRightWard();
            });

            //Left
            $("#ptzleft").mousedown(function() {
                _this.StartLeftWard();
            });
            $("#ptzleft").mouseup(function() {
                _this.StopLeftWard();
            });
            //Scan
            $("#ptzscan").click(function() {
                _this.AutoScanControl();
            });
            //Right
            $("#ptzright").mousedown(function() {
                _this.StartRightWard();
            });
            $("#ptzright").mouseup(function() {
                _this.StopRightWard();
            });

            //DownLeft
            $("#ptzdownleft").mousedown(function() {
                _this.StartDownLeftWard();
            });
            $("#ptzdownleft").mouseup(function() {
                _this.StopDownLeftWard();
            });
            //Down
            $("#ptzdown").mousedown(function() {
                _this.StartDownWard();
            });
            $("#ptzdown").mouseup(function() {
                _this.StopDownWard();
            });
            //DownRight
            $("#ptzdownRight").mousedown(function() {
                _this.StartDownRightWard();
            });
            $("#ptzdownRight").mouseup(function() {
                _this.StopDownRightWard();
            });

            //ZOOM
            $("#zoomFar").mousedown(function() {
                _this.StartZoomFar();
            });
            $("#zoomFar").mouseup(function() {
                _this.StopZoomFar();
            });
            $("#zoomNear").mousedown(function() {
                _this.StartZoomNear();
            });
            $("#zoomNear").mouseup(function() {
                _this.StopZoomNear();
            });
            //Focus
            $("#focusFar").mousedown(function() {
                _this.StartFocusFar();
            });
            $("#focusFar").mouseup(function() {
                _this.StopFocusFar();
            });
            $("#focusNear").mousedown(function() {
                _this.StartFocusNear();
            });
            $("#focusNear").mouseup(function() {
                _this.StopFocusNear();
            });

            // pre-set points.
            $("#setPrepointBtn").click(function() {
                var nPrePointValue = $("#prePoint").val();
                if (!_this.ValidatePrePoint(nPrePointValue)) {
                    var msg = _this.lang["MSG"]["invalid_prepoint"].text;
                    var re = /__min__/g;
                    msg = msg.replace(re, _this.nMinPrePointValue.toString());
                    re = /__max__/g;
                    msg = msg.replace(re, _this.nMaxPrePointValue.toString());
                    alert(msg);
                    return;
                }
                _this.SetPrePoint(nPrePointValue);
            });
            $("#callPrepointBtn").click(function() {
                var nPrePointValue = $("#prePoint").val();
                if (!_this.ValidatePrePoint(nPrePointValue)) {
                    var msg = _this.lang["MSG"]["invalid_prepoint"].text;
                    var re = /__min__/g;
                    msg = msg.replace(re, _this.nMinPrePointValue.toString());
                    re = /__max__/g;
                    msg = msg.replace(re, _this.nMaxPrePointValue.toString());
                    alert(msg);
                    return;
                }
                _this.CallPrePoint(nPrePointValue);
            });
            $("#delPrePointBtn").click(function() {});
            //cruisePath
            $("#playCruisePath").click(function() {});
            $("#stopCruisePath").click(function() {});

            //Mirror select
            $("#MirrorVID0").click(function() {
                OnChangeMirror();
            });

            //Mirror select
            $("#MirrorVID1").click(function() {
                OnChangeMirror();
            });

            //Mirror select
            $("#MirrorHID0").click(function() {
                OnChangeMirror();
            });

            //Mirror select
            $("#MirrorHID2").click(function() {
                OnChangeMirror();
            });

            //Frequency select
            $("#FrequencyID0").click(function() {
                OnChangeVideoStandard();
            });

            //Frequency select
            $("#FrequencyID1").click(function() {
                OnChangeVideoStandard();
            });

            //Scene select
            $("#SceneSelect").change(function() {
                OnChangeScene();
            });

            //Param refresh
            $("#imageParamRefresh").click(function() {
                OnRefreshParams();
            });

            //Param default
            $("#imageParamDefault").click(function() {
                OnDefaultParams();
            });

            //Param save
            $("#imageParamSave").click(function() {
                OnSaveParams();
            });

            //Stream select
            $("#StreamSelect").change(function() {
                OnChangeStreamType();
            });
            //Scale select
            $("#ScaleSelect").change(function() {
                OnChangeScale();
            });

            //alert("cookie svrName:" + getCookie("svrName"));
            //clearCookie("svrName");
            // Show the server name if it's provided.
            if ((typeof ServerName) != "undefined" && ServerName != undefined) {
                $("#svrName").html(ServerName);
                //setCookie("svrName", ServerName, 1000);
                //clearCookie("svrName");
            }

            // get the server IP and command port.
            if (!__DEBUG) {
                this.strServerIP = location.hostname;
                if (this.strServerIP == vDevIP) {
                    this.nPort = vPort; //from a.js
                } else {
                    this.nPort = vUPnPPort;
                }

            }

            // focus to the user-name box.
            if ($("#userName").val() == "") {
                $("#userName").focus();
            } else {
                $("#userPwd").focus();
            }
        } catch (e) {
            if (__DEBUG) {
                alert("Init Error: \n" + e.description);
            }
        }
    },

    //PageUI initialization//页面文字初始化,要在设置语言之后
    PageUIInit: function() {

        // Initialize the UI of some buttons corresponding to different verions of IE.
        //alert()
        if (this.bTalkOn) {
            $("#talkBtn").toggleClass("off", false);
            $("#talkBtn").toggleClass("on", true);
        } else {
            $("#talkBtn").toggleClass("on", false);
            $("#talkBtn").toggleClass("off", true);
        }

        if (this.bVoiceOn) {
            $("#VoiceBtn").toggleClass("off", false);
            $("#VoiceBtn").toggleClass("on", true);
        } else {
            $("#VoiceBtn").toggleClass("on", false);
            $("#VoiceBtn").toggleClass("off", true);
        }

        $("#idNType option[value=wan]").html(this.lang["MSG"]["net_type_items"]["wan"]);
        $("#idNType option[value=lan]").html(this.lang["MSG"]["net_type_items"]["lan"]);

        $("#StreamSelect option[value=0]").html(this.lang["MSG"]["net_type_items"]["lan"]);
        $("#StreamSelect option[value=1]").html(this.lang["MSG"]["net_type_items"]["wan"]);

        $("#SceneSelect option[value=0]").html(this.lang["MSG"]["Scene_type"]["0"]);
        $("#SceneSelect option[value=1]").html(this.lang["MSG"]["Scene_type"]["1"]);
        $("#SceneSelect option[value=2]").html(this.lang["MSG"]["Scene_type"]["2"]);
        $("#SceneSelect option[value=3]").html(this.lang["MSG"]["Scene_type"]["3"]);

        $("#InfraredSelect option[value=0]").html(this.lang["MSG"]["Infrared_Level"]["0"]);
        $("#InfraredSelect option[value=1]").html(this.lang["MSG"]["Infrared_Level"]["1"]);

        $("#ScaleSelect option[value=0]").html(this.lang["MSG"]["ImageScale_type"]["0"]);
        $("#ScaleSelect option[value=1]").html(this.lang["MSG"]["ImageScale_type"]["1"]);
        $("#ScaleSelect option[value=2]").html(this.lang["MSG"]["ImageScale_type"]["2"]);
    },


    //Un-initialization
    UnInit: function() {
        this.LogOut();
    },

    //Detect whether the OCX was installed, prompt to update the OCX if there is new versions available.
    CheckOCX: function() {
        if (nBrowser == "ie") {
            if (this.ocxHandle != null) {
                // Not installed.
                if ((typeof this.ocxHandle.OpenServer) == "undefined") {
                    if (confirm(this.lang["MSG"]["ocx_not_installed"].text)) {
                        location.assign(this.strOCXPacket);
                        }
                    }
            } else {
                throw new Error("Logic Error, this.ocxHandle = null");
            }
        }
    },

    //加载OCX
    InitObjHtml: function() {
        try {
            if (nBrowser != "ie") {
                this.objHtml = "<EMBED id='tlocx' type='application/x-itst-activex' clsid='{577493A1-A039-4A90-92D2-9087BECE615C}' width='0' height='0'></EMBED>";
            } else {
                this.objHtml = "<object id='tlocx' classid='clsid:577493A1-A039-4A90-92D2-9087BECE615C' width='0' height='0'></object>";
            }
            $("#ocxDiv").html(this.objHtml);
        } catch (e) {
            if (__DEBUG) {
                alert("Init Obj Html Error: \n" + e.description);
            }
        }
    },

    //Set the language
    SetLang: function(l) {
        // It will check the system language when null is provided.
        if (l == null) {
            if (nBrowser != "ie") {
                l = navigator.language.toLowerCase();
            } else {
                l = navigator.systemLanguage.toLowerCase();
            }
            $("#language option[value=" + l + "]").prop("selected", true);
        }

        var lang = g_lang[l];

        // If the required language pack is not provided, we will use the English instead.
        if (lang == undefined || (typeof lang) == undefined) {
            //alert("en-us");
            lang = g_lang["en-us"];
        }

        for (var ele_id in lang["UI"]) {
            try {
                var $ele = $("#" + ele_id);
                var ele_lang = lang["UI"][ele_id];
                if (ele_lang.text != undefined) {
                    $ele.html(ele_lang.text);
                }
                if (ele_lang.tip != undefined) {
                    $ele.attr("title", ele_lang.tip);
                }
            } catch (e) {
                if (__DEBUG) {
                    alert(ele_id + "not found.");
                }
            }
        }

        this.lang = lang;
        this.PageUIInit(); //页面文字初始化,要在设置语言之后
        try {
            if (nBrowser == "ie") {
                this.ocxHandle.SetControlLanguage(lang["LANG_ID"]);
            }
        } catch (e) {
            if (__DEBUG) {
                alert("Error this.ocxHandle.SetControlLanguage");
            }
        }
    },

    // update some buttons's UI by the channel number.
    UpdateChannelUI: function(idx) {
        var bOpened = this.bChannelOpened[idx];
        var bInRecord = this.bRecordOn[idx];
        //$("#ChannelBtn").toggleClass(bOpened ? "on" : "off", true);
        if (bInRecord) {
            $("#recordBtn").toggleClass("off", false);
            $("#recordBtn").toggleClass("on", true);
            $("#recordBtn").attr("title", this.lang["MSG"]["recordBtn"]["1"]);
        } else {
            $("#recordBtn").toggleClass("on", false);
            $("#recordBtn").toggleClass("off", true);
            $("#recordBtn").attr("title", this.lang["MSG"]["recordBtn"]["0"]);
        }
    },

    OnVideoWndSelect: function(idx) {
        this.UpdateChannelUI(idx);
    },

    //the Login routine.
    LogIn: function() {
        //var str;
        //var str2 = ",";
        //this.strServerIP = "192.168.1.251";
        //str = this.strServrIP + str2 + this.nPort + str2 + this.strUserName + str2 + this.strUserPwd;

        if (this.Connect(
                this.strServerIP,
                this.nPort,
                this.strUserName,
                this.strUserPwd,
                nStreamType
            )) {
            return true;
        } else {
            return false;
        }
    },
    // the Logout routine.
    LogOut: function() {
        this.StopAllRecord();
        this.CloseAllChannel();
        this.Close();

        // navigate back to the login page.
        $("#LoginPage").css("display", "block");
        $("#mainBox").css("display", "none");
        location.reload();
    },

    //Connect the DVR server.
    Connect: function(strServerIP, nPort, strUserName, strUserPwd, nStreamType) {
        /*if(this.ocxHandle == null)
        {
        	alert("OCX is null!");
        	return false;
        }*/
        try {
            this.bConnected = this.ocxHandle.OpenServerEx(strServerIP, nPort, strUserName, strUserPwd, nStreamType);
            if (!this.bConnected) {
                return false;
            }

            if (nBrowser == "ie") {
                // Get the channel count.
                this.nChannelCount = this.ocxHandle.GetChannelCount();
                // Initialize certain falgs.
                for (var i = 0; i < this.nChannelCount; ++i) {
                    // As the OCX will open the real video itself after a successful login
                    // We set channel open/close falgs to true.
                    this.bChannelOpened[i] = true;
                    this.bRecordOn[i] = false;
                }
            }

            this.bTalkOn = false;

            //this.bVoiceOn = true;
            //this.bVoiceOn = this.ocxHandle.Mute(); //opensever之后默认是关的，这里打开
            if (this.bVoiceOn) {
                $("#VoiceBtn").toggleClass("off", false);
                $("#VoiceBtn").toggleClass("on", true);
            } else {
                $("#VoiceBtn").toggleClass("on", false);
                $("#VoiceBtn").toggleClass("off", true);
            }

            this.bAutoScan = false;
            return true;
        } catch (e) {
            if (__DEBUG) {
                alert("Connect Error: \n" + e.description);
            }

            return false;
        }
    },

    // Close the connection.
    Close: function() {
        try {
            for (var i = 0; i < this.nChannelCount; ++i) {
                this.bChannelOpened[i] = false;
            }
            this.ocxHandle.CloseServer();
            this.nChannelCount = 0;
            this.bConnected = false;
        } catch (e) {
            if (__DEBUG) {
                alert("Close Error: \n" + e.description);
            }

        }
    },

    // Reserved.
    SetScreenNum: function() {
        try {
            this.ocxHandle.SetShowWindows((this.nChannelCount > 0 ? this.nChannelCount : 1));
        } catch (e) {
            if (__DEBUG) {
                alert("SetScreenNum Error: \n" + e.description);
            }

        }
    },

    // Detect whether the connection is broken.
    isConnected: function() {
        try {
            return this.ocxHandle.isConnect();
        } catch (e) {
            if (__DEBUG) {
                alert("isConnected Error: \n" + e.description);
            }

            return false;
        }
    },

    RealPlayPage: function() {
        if (this.page == 0) {
            return;
        }
        try {
            //if (!DeviceManager.bChannelOpened[DeviceManager.GetCurrentChannel()]) {
            //    return;
            //}
            //DeviceManager.CloseChannel(DeviceManager.GetCurrentChannel());
            DeviceManager.OpenChannel(DeviceManager.GetCurrentChannel());
            //$("#ChannelBtn").toggleClass("on", true);
            //$("#ChannelBtn").attr("title", this.lang["MSG"]["ChannelBtn"]["1"]);
        } catch (e) {
            if (__DEBUG) {
                alert("StreamTypeSwitch Error: \n" + e.description);
            }
        }

        try {
            $("#panelBox").css("display", "block");
            this.ocxHandle.RealPlay();
            this.page = 0;
            ResizeBody();
        } catch (e) {
            if (__DEBUG) {
                alert("RealPlayPage Error: \n" + e.description);
            }
        }
    },

    // Switch to the playback page.
    PlayBackPage: function() {
        if (this.page == 1) {
            return;
        }

        try {
            if (!DeviceManager.bChannelOpened[DeviceManager.GetCurrentChannel()]) {
                return;
            }
            DeviceManager.CloseChannel(DeviceManager.GetCurrentChannel());
        } catch (e) {
            if (__DEBUG) {
                alert("StreamTypeSwitch Error: \n" + e.description);
            }
        }

        try {
            nOriginalType = 0;
            $("#panelBox").css("display", "none");
            this.ocxHandle.PlayBack();
            this.page = 1;
            ResizeBody();
        } catch (e) {
            if (__DEBUG) {
                alert("PlayBackPage Error: \n" + e.description);
            }
        }
    },

    Setup: function() {
        try {
            if (!this.isConnected()) {
                return;
            }
            this.ocxHandle.ShowSetup(1, 0, 0);
        } catch (e) {
            if (__DEBUG) {
                alert("Setup Error: \n" + e.description);
            }
        }
    },

    // Get the current selected channel number.
    GetCurrentChannel: function() {
        try {
            return this.ocxHandle.GetSelectChannel();
        } catch (e) {
            if (__DEBUG) {
                alert("GetCurrentChannel Error: \n" + e.description);
            }

            return 0;
        }
    },

    // Open the channel(realvideo).
    OpenChannel: function(idx) {
        try {
            if (this.ocxHandle.OpenChannelEx(idx, 0, nStreamType, 1)) {
                this.bChannelOpened[idx] = true;
                if (nOriginalType == 1) {
                    //this.SetOriginal_1(); //获取尺寸并设置窗口
                }
                return true;
            }
            return false;
        } catch (e) {
            if (__DEBUG) {
                alert("OpenChannel Error: \n" + e.description);
            }

            return false;
        }
    },

    // Close the channel(real video)
    CloseChannel: function(idx) {
        try {
            // Stop the recording first.
            this.RecordPro(idx, 0);
            // Close it.
            this.ocxHandle.CloseChannel(idx);
            this.bChannelOpened[idx] = false;
            if (idx == this.GetCurrentChannel())
            {
                this.UpdateChannelUI(idx);
            }
        } catch (e) {
            if (__DEBUG) {
                alert("CloseChannel Error: \n" + e.description);
            }
        }
    },

    //SetSubStream
    SetSubStream: function() {
        try {
            if (!this.bChannelOpened[this.GetCurrentChannel()]) {
                return;
            }
            if (nStreamType == 0) {
                nStreamType = 1;
            }
            this.CloseChannel(this.GetCurrentChannel());
            this.OpenChannel(this.GetCurrentChannel());
            //$("#ChannelBtn").toggleClass("on", true);
            //$("#ChannelBtn").attr("title", this.lang["MSG"]["ChannelBtn"]["1"]);
        } catch (e) {
            if (__DEBUG) {
                alert("StreamTypeSwitch Error: \n" + e.description);
            }
        }
    },
    //SetMainStream
    SetMainStream: function() {
        try {
            if (!this.bChannelOpened[this.GetCurrentChannel()]) {
                return;
            }
            if (nStreamType == 1) {
                nStreamType = 0;
            }
            this.CloseChannel(this.GetCurrentChannel());
            this.OpenChannel(this.GetCurrentChannel());
            //$("#ChannelBtn").toggleClass("on", true);
            //$("#ChannelBtn").attr("title", this.lang["MSG"]["ChannelBtn"]["1"]);
        } catch (e) {
            if (__DEBUG) {
                alert("StreamTypeSwitch Error: \n" + e.description);
            }
        }
    },
    OpenAllChannel: function() {

        //alert("OpenAllChannel");
        for (var i = 0; i < this.nChannelCount; ++i) {
            if (!this.bChannelOpened[i]) {
                this.OpenChannel(i);
                this.bChannelOpened[i] = true;
            }
        }
    },
    CloseAllChannel: function() {
        for (var i = 0; i < this.nChannelCount; ++i) {
            if (this.bChannelOpened[i]) {
                this.CloseChannel(i);
                this.bChannelOpened[i] = false;
            }
        }
    },

    // Snap
    Snap: function() {
        try {
            if (!this.isConnected()) {
                return false;
            }

            var i = -1;
            if (arguments.length > 0) {
                i = parseInt(arguments[0]);
            }
            return this.ocxHandle.SnapShot(i);
        } catch (e) {
            if (__DEBUG) {
                alert("Snap Error: \n" + e.description);
            }

            return false;
        }
    },

    // Start / stop recording.
    RecordPro: function(idx, bOn) {
        try {
            //alert("recordpro");
            this.bRecordOn[idx] = this.ocxHandle.VideoRecordEx(idx, 0, bOn);
        } catch (e) {
            if (__DEBUG) {
                alert("RecordPro Error: \n" + e.description);
            }
        }
    },
    //Start recording in all channels. If the record is already started, it will skipped.
    StartAllRecord: function() {
        for (var i = 0; i < this.nChannelCount; ++i) {
            if (!this.bRecordOn[i]) {
                this.RecordPro(i, 1);
            }
        }
    },
    // Stop recording in all channels.
    StopAllRecord: function() {
        for (var i = 0; i < this.nChannelCount; ++i) {
            if (this.bRecordOn[i]) {
                this.RecordPro(i, 0);
            }
        }
    },

    //Audio talk
    VoiceTalk: function() {
        try {
            if (!this.isConnected()) {
                return;
            }
            this.bTalkOn = this.ocxHandle.TalkBack();
        } catch (e) {
            if (__DEBUG) {
                alert("VoiceTalk Error: \n" + e.description);
            }
        }
    },

    //el_zoom
    /*el_zoomBtn: function() {
        try {
            if (!this.isConnected()) {
                return false;
            }
            var i = -1;
            if (arguments.length > 0) {
                i = parseInt(arguments[0]);
            }
            return this.ocxHandle.ShowElectronicZoom(i);
        }
        catch (e) {
            if (__DEBUG) {
                alert("el_zoomBtn Error: \n" + e.description);
            }

            return false;
        }
    },*/

    // Mute switch for all channels.
    MuteSwitch: function() {
        try {
            if (!this.isConnected()) {
                return;
            }
            this.bVoiceOn = this.ocxHandle.Mute();
        } catch (e) {
            if (__DEBUG) {
                alert("MuteSwitch Error: \n" + e.description);
            }
        }
    },
    //Reserved
    FullScreen: function() {
        try {
            this.ocxHandle.FullScreen(1);
        } catch (e) {
            if (__DEBUG) {
                alert("FullScreen Error: \n" + e.description);
            }
        }
    },


    //========================================================================

    //Hold the cradle
    HoldCradle: function() {
        try {
            if (!this.isConnected()) {
                return;
            }
            this.ocxHandle.SendPTZCommand(11, 0, -1);
        } catch (e) {
            if (__DEBUG) {
                alert("HoldCradle Error: \n" + e.description);
            }
        }
    },

    //Upward, Leftward, Rightward, Downward and Auto-Scan in PTZ control

    //UpLeft
    StartUpLeftWard: function() {
        try {
            if (!this.isConnected()) {
                return;
            }
            this.ocxHandle.SendPTZCommand(23, 0, -1);
        } catch (e) {
            if (__DEBUG) {
                alert("StartUpLeftWard Error: \n" + e.description);
            }
        }
    },
    StopUpLeftWard: function() {
        this.HoldCradle();
    },
    //Up
    StartUpWard: function() {
        try {
            if (!this.isConnected()) {
                return;
            }
            this.ocxHandle.SendPTZCommand(1, 0, -1);
        } catch (e) {
            if (__DEBUG) {
                alert("StartUpWard Error: \n" + e.description);
            }
        }
    },
    StopUpWard: function() {
        this.HoldCradle();
    },
    //UpRight
    StartUpRightWard: function() {
        try {
            if (!this.isConnected()) {
                return;
            }
            this.ocxHandle.SendPTZCommand(24, 0, -1);
        } catch (e) {
            if (__DEBUG) {
                alert("StartUpRightWard Error: \n" + e.description);
            }
        }
    },
    StopUpRightWard: function() {
        this.HoldCradle();
    },

    //Left
    StartLeftWard: function() {
        try {
            if (!this.isConnected()) {
                return;
            }
            this.ocxHandle.SendPTZCommand(3, 0, -1);
        } catch (e) {
            if (__DEBUG) {
                alert("StartLefWard Error: \n" + e.description);
            }
        }
    },
    StopLeftWard: function() {
        this.HoldCradle();
    },
    //Right
    StartRightWard: function() {
        try {
            if (!this.isConnected()) {
                return;
            }
            this.ocxHandle.SendPTZCommand(4, 0, -1);
        } catch (e) {
            if (__DEBUG) {
                alert("StartRightWard Error: \n" + e.description);
            }
        }
    },
    StopRightWard: function() {
        this.HoldCradle();
    },

    //DownLeft
    StartDownLeftWard: function() {
        try {
            if (!this.isConnected()) {
                return;
            }
            this.ocxHandle.SendPTZCommand(25, 0, -1);
        } catch (e) {
            if (__DEBUG) {
                alert("StartDownLeftWard Error: \n" + e.description);
            }
        }
    },
    StopDownLeftWard: function() {
        this.HoldCradle();
    },
    //Down
    StartDownWard: function() {
        try {
            if (!this.isConnected()) {
                return;
            }
            this.ocxHandle.SendPTZCommand(2, 0, -1);
        } catch (e) {
            if (__DEBUG) {
                alert("StartDownWard Error: \n" + e.description);
            }
        }
    },
    StopDownWard: function() {
        this.HoldCradle();
    },
    //DownRight
    StartDownRightWard: function() {
        try {
            if (!this.isConnected()) {
                return;
            }
            this.ocxHandle.SendPTZCommand(26, 0, -1);
        } catch (e) {
            if (__DEBUG) {
                alert("StartDownRightWard Error: \n" + e.description);
            }
        }
    },
    StopDownRightWard: function() {
        this.HoldCradle();
    },

    //scan
    AutoScanControl: function() {
        try {
            if (!this.isConnected()) {
                return;
            }

            this.bAutoScan = !this.bAutoScan;
            if (this.bAutoScan) {
                this.ocxHandle.SendPTZCommand(14, 0, -1);
            } else {
                this.ocxHandle.SendPTZCommand(15, 0, -1);
            }
        } catch (e) {
            if (__DEBUG) {
                alert("AutoScanControl Error: \n" + e.description);
            }
        }
    },

    //========================================================================

    //ZOOM
    StartZoomFar: function() {
        try {
            if (!this.isConnected()) {
                return;
            }
            this.ocxHandle.SendPTZCommand(8, 0, -1);
        } catch (e) {
            if (__DEBUG) {
                alert("StartZoomFar Error: \n" + e.description);
            }
        }
    },
    StopZoomFar: function() {
        this.HoldCradle();
    },
    StartZoomNear: function() {
        try {
            if (!this.isConnected()) {
                return;
            }
            this.ocxHandle.SendPTZCommand(7, 0, -1);
        } catch (e) {
            if (__DEBUG) {
                alert("StartZoomNear Error: \n" + e.description);
            }
        }
    },
    StopZoomNear: function() {
        this.HoldCradle();
    },

    //FOCUS
    StartFocusFar: function() {
        try {
            if (!this.isConnected()) {
                return;
            }
            this.ocxHandle.SendPTZCommand(6, 0, -1);
        } catch (e) {
            if (__DEBUG) {
                alert("StartFocusFar Error: \n" + e.description);
            }
        }
    },
    StopFocusFar: function() {
        this.HoldCradle();
    },
    StartFocusNear: function() {
        try {
            if (!this.isConnected()) {
                return;
            }
            this.ocxHandle.SendPTZCommand(5, 0, -1);
        } catch (e) {
            if (__DEBUG) {
                alert("StartFocusNear Error: \n" + e.description);
            }
        }
    },
    StopFocusNear: function() {
        this.HoldCradle();
    },

    //IRIS
    StartIrisLarge: function() {
        try {
            if (!this.isConnected()) {
                return;
            }
            this.ocxHandle.SendPTZCommand(10, 0, -1);
        } catch (e) {
            if (__DEBUG) {
                alert("StartIrisLarge Error: \n" + e.description);
            }
        }
    },
    StopIrisLarge: function() {
        this.HoldCradle();
    },
    StartIrisSmall: function() {
        try {
            if (!this.isConnected()) {
                return;
            }
            this.ocxHandle.SendPTZCommand(9, 0, -1);
        } catch (e) {
            if (__DEBUG) {
                alert("StartIrisSmall Error: \n" + e.description);
            }
        }
    },
    StopIrisSmall: function() {
        this.HoldCradle();
    },

    //pre-set points.
    //Check whether it's valid pre-set point value.
    ValidatePrePoint: function(PrePointValue) {
        if (isNaN(PrePointValue) || PrePointValue.length == 0 || PrePointValue < this.nMinPrePointValue || PrePointValue > this.nMaxPrePointValue) {
            return false;
        }
        return true;
    },
    SetPrePoint: function(nPoint) {
        try {
            if (!this.isConnected()) {
                return;
            }
            this.ocxHandle.SendPTZCommand(12, nPoint, -1);
        } catch (e) {
            if (__DEBUG) {
                alert("SetPrePoint Error: \n" + e.description);
            }
        }
    },
    CallPrePoint: function(nPoint) {
        try {
            if (!this.isConnected()) {
                return;
            }
            this.ocxHandle.SendPTZCommand(13, nPoint, -1);
        } catch (e) {
            if (__DEBUG) {
                alert("CallPrePoint Error: \n" + e.description);
            }
        }
    }
}

function SetImage(Cmd, Flag, value) {
    if (DeviceManager.ocxHandle != null) {
        if (DeviceManager.bInitColorParam) //初始化之前不判断用户权限，初始化之后的用户操作需要判断权限
        {
            if (DeviceManager.ocxHandle.GetServerConfig(WM_USERRIGHT, 0) & 2) //判断是否有设置参数的权限
            {
                DeviceManager.ocxHandle.SetServerConfig(Cmd, Flag, value);
            } else {
                alert(DeviceManager.lang["MSG"]["permission_warming"].text);
            }
        } else {
            DeviceManager.ocxHandle.SetServerConfig(Cmd, Flag, value);
        }
    }
}

//写入Flash
CMD_UPDATEFLASH = 0x00000003;
//设置图片单属性
DMS_NET_SET_COLORCFG_SINGLE = 0x020223;
DMS_COLOR_SET_DEFORSAVE = 0; //默认或刷新
DMS_COLOR_SET_SAVE = 1;
DMS_COLOR_SET_DEF = 2;
DMS_COLOR_SET_BRIGHTNESS = 0x00000001;
DMS_COLOR_SET_HUE = 0x00000002;
DMS_COLOR_SET_CONTRAST = 0x00000008;
DMS_COLOR_SET_DEFINITION = 0x00000200;
DMS_COLOR_SET_SCENE = 0x00000400; //场景
DMS_COLOR_SET_MIRROR = 0x00040000; //镜像
//获取图片普通属性
DMS_NET_GET_COLORCFG = 0x020220;
DMS_COLOR_GET_BRIGHTNESS = 0x00000001;
DMS_COLOR_GET_HUE = 0x00000002;
DMS_COLOR_GET_CONTRAST = 0x00000008;
DMS_COLOR_GET_DEFINITION = 0x00000200;
//获取图片高级属性
DMS_NET_GET_ENHANCED_COLOR = 0x020226;
DMS_COLOR_GET_SCENE = 0x00000400;
DMS_COLOR_GET_MIRROR = 0x00040000;
//设置网络参数
CMD_SET_NETWORK = 0x00000010;
//获取网络参数
CMD_GET_NETWORK = 0x10000009;
NetWorkStructMember_VideoStandard = 0;
//获取当前用户权限的命令
WM_USERRIGHT = 0x0800;

//Initialize ImageParams，初始化图像参数控件
var BrightSlider;
var ContrastSlider;
var HueSlider;
var DefinitionSlider;

function InitImageParams() {
    nBrightness = 0;
    nContrast = 0;
    nHue = 0;
    nDefinition = 0;
    nMirror = 0;
    nScene = 0;
    nFrequency = 0;
    if (DeviceManager.ocxHandle != null) {
        nBrightness = DeviceManager.ocxHandle.GetServerConfig(DMS_NET_GET_COLORCFG, DMS_COLOR_GET_BRIGHTNESS);
        nContrast = DeviceManager.ocxHandle.GetServerConfig(DMS_NET_GET_COLORCFG, DMS_COLOR_GET_CONTRAST);
        nHue = DeviceManager.ocxHandle.GetServerConfig(DMS_NET_GET_COLORCFG, DMS_COLOR_GET_HUE);
        nDefinition = DeviceManager.ocxHandle.GetServerConfig(DMS_NET_GET_COLORCFG, DMS_COLOR_GET_DEFINITION);
        nMirror = DeviceManager.ocxHandle.GetServerConfig(DMS_NET_GET_ENHANCED_COLOR, DMS_COLOR_GET_MIRROR);
        nScene = DeviceManager.ocxHandle.GetServerConfig(DMS_NET_GET_ENHANCED_COLOR, DMS_COLOR_GET_SCENE);
        nFrequency = DeviceManager.ocxHandle.GetServerConfig(CMD_GET_NETWORK, NetWorkStructMember_VideoStandard);
        DeviceManager.nFrequency = nFrequency;
    }

    //BrightSlider--------------------------------------------------------------------------------
    if (BrightSlider == undefined) {
        BrightSlider = new neverModules.modules.slider({ targetId: "sliderBright", sliderCss: "imageSlider1", barCss: "imageBar1", min: 0, max: 255, hints: "" });
        BrightSlider.onchange = function() {
            if (DeviceManager.bInitColorParam)
            {
                $("#BrightVal").html(BrightSlider.getValue());
                SetImage(DMS_NET_SET_COLORCFG_SINGLE, DMS_COLOR_SET_BRIGHTNESS, BrightSlider.getValue());
            }
        };
        BrightSlider.create();
    }
    BrightSlider.setValue(nBrightness);
    $("#BrightVal").html(nBrightness);

    //ContrastSlider--------------------------------------------------------------------------------
    if (ContrastSlider == undefined) {
        ContrastSlider = new neverModules.modules.slider({ targetId: "sliderContrast", sliderCss: "imageSlider1", barCss: "imageBar1", min: 0, max: 255, hints: "" });
        ContrastSlider.onchange = function() {
            if (DeviceManager.bInitColorParam)
            {
                $("#ContrastVal").html(ContrastSlider.getValue());
                SetImage(DMS_NET_SET_COLORCFG_SINGLE, DMS_COLOR_SET_CONTRAST, ContrastSlider.getValue());
            }
        };
        ContrastSlider.create();
    }
    ContrastSlider.setValue(nContrast);
    $("#ContrastVal").html(nContrast);

    //HueSlider--------------------------------------------------------------------------------
    if (HueSlider == undefined) {
        HueSlider = new neverModules.modules.slider({ targetId: "sliderHue", sliderCss: "imageSlider1", barCss: "imageBar1", min: 0, max: 255, hints: "" });
        HueSlider.onchange = function() {
            if (DeviceManager.bInitColorParam)
            {
                $("#HueVal").html(HueSlider.getValue());
                SetImage(DMS_NET_SET_COLORCFG_SINGLE, DMS_COLOR_SET_HUE, HueSlider.getValue());
            }
        };
        HueSlider.create();
    }
    HueSlider.setValue(nHue);
    $("#HueVal").html(nHue);

    //DefinitionSlider--------------------------------------------------------------------------------
    if (DefinitionSlider == undefined) {
        DefinitionSlider = new neverModules.modules.slider({ targetId: "sliderDefinition", sliderCss: "imageSlider1", barCss: "imageBar1", min: 0, max: 255, hints: "" });
        DefinitionSlider.onchange = function() {
            if (DeviceManager.bInitColorParam)
            {
                $("#DefinitionVal").html(DefinitionSlider.getValue());
                SetImage(DMS_NET_SET_COLORCFG_SINGLE, DMS_COLOR_SET_DEFINITION, DefinitionSlider.getValue());
            }
        };
        DefinitionSlider.create();
    }
    DefinitionSlider.setValue(nDefinition);
    $("#DefinitionVal").html(nDefinition);
    
    //Mirror
    $("[name='MirrorV']").each(function(idx){ $(this).attr("checked", (idx == 0 && (nMirror & 1)) || (idx == 1 && (nMirror & 1) == 0)); });
    $("[name='MirrorH']").each(function(idx){ $(this).attr("checked", (idx == 0 && (nMirror & 2)) || (idx == 1 && (nMirror & 2) == 0)); });

    //Frequency
    $("[name='Frequency']").each(function(idx){ $(this).attr("checked", (idx == 0 && nFrequency == 1) || (idx == 1 && nFrequency == 0)); });

    //Scene
    $("#SceneSelect").val(nScene);

    DeviceManager.bInitColorParam = true;
}

function OnChangeMirror() {
    nMirrorV = $("input[name='MirrorV']:checked").val();
    nMirrorH = $("input[name='MirrorH']:checked").val();

    nMirror = nMirrorV | nMirrorH;
    SetImage(DMS_NET_SET_COLORCFG_SINGLE, DMS_COLOR_SET_MIRROR, nMirror);
}

function OnChangeVideoStandard() {
    nFrequency = $("input[name='Frequency']:checked").val();
    if (nFrequency != DeviceManager.nFrequency) {
        DeviceManager.nFrequency = nFrequency;
        SetImage(CMD_SET_NETWORK, NetWorkStructMember_VideoStandard, nFrequency);
    }
}

function OnChangeScene() {
    var Scene = $("#SceneSelect").val();
    SetImage(DMS_NET_SET_COLORCFG_SINGLE, DMS_COLOR_SET_SCENE, Scene);
}

/*function OnChangeInfrared()()
{
	var Infrared = document.getElementById("InfraredSelect");
	var index = Infrared.selectedIndex;
}*/

function OnChangeStreamType() {
    if (DeviceManager.ocxHandle != null) {
        nStreamType = $("#StreamSelect").val();
        try {
            if (!DeviceManager.bChannelOpened[DeviceManager.GetCurrentChannel()]) {
                return;
            }
            DeviceManager.CloseChannel(DeviceManager.GetCurrentChannel());
            DeviceManager.OpenChannel(DeviceManager.GetCurrentChannel());

            var Scale = $("#ScaleSelect").val();
            if (Scale == 1) {
                ResizeBody();
            }
        } catch (e) {
            if (__DEBUG) {
                alert("StreamTypeSwitch Error: \n" + e.description);
            }
        }
    }
}

function OnChangeScale() {
    if (DeviceManager.ocxHandle != null) {
        nScale = $("#ScaleSelect").val();
        switch (nScale) {
            case 0:
                nSetScreenType = 0;
                break;
            case 1:
                nSetScreenType = 1;
                break;
            case 2:
                nSetScreenType = 2;
                break;
            default:
                break;
        }
        DeviceManager.bInit = true;
        ResizeBody();
    }
}

function OnRefreshParams() {
    if (DeviceManager.ocxHandle != null) {
        try {
            if (!DeviceManager.bChannelOpened[DeviceManager.GetCurrentChannel()]) {
                return;
            }
            //SetImage(DMS_NET_SET_COLORCFG_SINGLE, DMS_COLOR_SET_DEFORSAVE, DMS_COLOR_SET_SAVE);
            InitImageParams();
        } catch (e) {
            if (__DEBUG) {
                alert("DefaultParamsSwitch Error: \n" + e.description);
            }
        }
    }
}

function OnDefaultParams() {
    if (DeviceManager.ocxHandle != null) {
        try {
            if (!DeviceManager.bChannelOpened[DeviceManager.GetCurrentChannel()]) {
                return;
            }
            SetImage(DMS_NET_SET_COLORCFG_SINGLE, DMS_COLOR_SET_DEFORSAVE, DMS_COLOR_SET_DEF);
            InitImageParams();
        } catch (e) {
            if (__DEBUG) {
                alert("DefaultParamsSwitch Error: \n" + e.description);
            }
        }
    }
}

function OnSaveParams() {
    if (DeviceManager.ocxHandle != null) {
        try {
            SetImage(CMD_UPDATEFLASH, 0, 0);
        } catch (e) {
            if (__DEBUG) {
                alert("SaveParams Error: \n" + e.description);
            }
        }
    }
}

function ResizeBody() {
    
    if ($("#mainBox").css("display") == "none")
    {
        return;
    }

    var Scale = (DeviceManager.bInit || DeviceManager.page == 0) ? $("#ScaleSelect").val() : 2;
    
    var width = document.documentElement.clientWidth;
    var height = document.documentElement.clientHeight;
    
    var nVideoWidth = 0;
    var nVideoHeight = 0;
    if (Scale == 1) { // 原始尺寸
        nVideoWidth = DeviceManager.ocxHandle.GetChannelVideoWidth(DeviceManager.GetCurrentChannel());
        nVideoHeight = DeviceManager.ocxHandle.GetChannelVideoHeight(DeviceManager.GetCurrentChannel());
        if (nVideoWidth + 300 > width)
        {
            width = nVideoWidth + 300;
        }
        if (nVideoHeight + 50 > height)
        {
            height = nVideoHeight + 50;
        }
    }

    var mainWidth = width < 820 ? 820 : width;
    $("#mainBox").css("width", mainWidth + "px");
    $("#topBanner").css("width", mainWidth + "px");

    var menuWidth = mainWidth - 20 - 250 - 15;
    $("#playBox").css("width", 250 + "px");
    $("#menu").css("width", menuWidth + "px");
    $("#menuInner").css("width", menuWidth - 130 + "px");

    var ContainerBoxWidth = mainWidth - 30;
    var ContainerBOxHeight = height < 600 ? 560 : height - 50;
    $("#ContainerBox").css("width", ContainerBoxWidth + "px");
    $("#ContainerBox").css("height", ContainerBOxHeight + "px");

    var nVerticalFreeSpace = ContainerBOxHeight - (236 + 278 + 50);
    var nVerticalInterval = nVerticalFreeSpace / 3;

    if (nVerticalInterval <= 0) {
        nVerticalInterval = 0;
    } else if (nVerticalInterval > 20)
    {
        nVerticalInterval = 20;
    }
    nVerticalInterval = nVerticalInterval + nVerticalInterval / 4;

    $("#ptzBox").css("marginTop", nVerticalInterval + "px");
    $("#ImageBox").css("marginTop", nVerticalInterval + "px");
    $("#StreamAndScaleBox").css("marginTop", nVerticalInterval + "px");

    var nOcxWidth = (DeviceManager.page == 0) ? (ContainerBoxWidth - 250 - 10) : (ContainerBoxWidth - 2);
    var nOcxHeight = ContainerBOxHeight;

    $("#ocxBox").css("width", nOcxWidth + "px");
    $("#ocxBox").css("height", nOcxHeight + "px");
    $("#ocxDiv").css("width", nOcxWidth + "px");
    $("#ocxDiv").css("height", nOcxHeight + "px");

    var nShowWidth = nOcxWidth;
    var nShowHeight = nOcxHeight;

    if (Scale == 0) { //默认尺寸16:9
        if (nShowWidth / nShowHeight > 1.77) {
            nShowWidth = parseInt(nShowHeight * 16 / 9);
        } else {
            nShowHeight = parseInt(nShowWidth * 9 / 16);
        }
    } else if (Scale == 1) { // 原始尺寸
        if (nVideoWidth > 0 && nVideoHeight > 0)
        {
            nShowWidth = nVideoWidth;
            nShowHeight = nVideoHeight;
        }
    }

    $("#tlocx").css("width", nShowWidth + "px");
    $("#tlocx").css("height", nShowHeight + "px");
    setTimeout(function() { DeviceManager.ocxHandle.ResizeWnd(DeviceManager.page, nShowWidth, nShowHeight); }, 10);
}

//取得cookie    
function getCookie(name) {
    var nameEQ = name + "=";
    var ca = document.cookie.split(';'); //把cookie分割成组    
    for (var i = 0; i < ca.length; i++) {
        var c = ca[i]; //取得字符串    
        while (c.charAt(0) == ' ') { //判断一下字符串有没有前导空格    
            c = c.substring(1, c.length); //有的话，从第二位开始取    
        }
        if (c.indexOf(nameEQ) == 0) { //如果含有我们要的name    
            return unescape(c.substring(nameEQ.length, c.length)); //解码并截取我们要值 
        }
    }
    return false;
}

//清除cookie    
function clearCookie(name) {
    var liveDate = new Date();
    liveDate.setTime(liveDate.getTime() - liveDate.getTime() - liveDate.getTime());
    setCookie(name, "", liveDate);
    //setCookie(name, "", -1); 
}

//设置cookie    
/*function setCookie(name, value, seconds) {    
 	seconds = seconds || 0;   //seconds有值就直接赋值，没有为0，这个根php不一样。    
 	var expires = "";    
 	if (seconds != 0 ) {      //设置cookie生存时间    
 		var date = new Date();    
 		date.setTime(date.getTime()+seconds);    
 		expires = "; expires="+date.toGMTString();    
 	}
	//var domain = ";domain="+location.hostname;
 	//document.cookie = name+"="+escape(value)+expires+domain+"; path=/";   //转码并赋值   
	//document.cookie = name+"="+escape(value)+expires+domain+"; path=/";
};    */
function setCookie(name, value) { // 设置Cookie 
    document.cookie = name + "=" + encodeURI(value);
    // 直接设置即可 
}

function RecordRestart() {
    if (DeviceManager.bRecordOn[DeviceManager.GetCurrentChannel()])
    {
        DeviceManager.RecordPro(DeviceManager.GetCurrentChannel(), 0);
        DeviceManager.RecordPro(DeviceManager.GetCurrentChannel(), 1);
        DeviceManager.recTimer = setTimeout(function() { RecordRestart(); }, RecordTime);
    }
    else
    {
        DeviceManager.recTimer = null;
    }
}

$(window).load(function() {
    DeviceManager.Init();
});

var resizeTimer = null;
$(window).resize(function() {
    resizeTimer = (nBrowser != "ie" && resizeTimer) ? null : setTimeout(function() { ResizeBody(); }, 10);
});

$(window).unload(function() {
    DeviceManager.UnInit();
});
