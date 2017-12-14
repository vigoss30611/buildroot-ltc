// File: lang.js
// Description: language packet.
// Author: huangsanyi

var g_lang = [];

// Chinese
g_lang["zh-cn"] = {
    "LANG_ID": "zh-cn",
    "UI": {
        "langlbl": {
            text: "语&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;言:&nbsp;&nbsp;"
            //tip: ""
        },
        "userNamelbl": {
            text: "用&nbsp;&nbsp;户&nbsp;&nbsp;名:&nbsp;&nbsp;&nbsp;"
            //tip: ""
        },
        "userPwdlbl": {
            text: "密&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;码: &nbsp;&nbsp;"
            //tip: ""
        },
        "nTypelbl": {
            text: "码流类型:&nbsp;&nbsp;"
            //tip: ""
        },
        "loginBtn": {
            text: "<span style=\"color:#FFFFFF\">登陆</span>"
            //tip: ""
        },
        "cancelloginBtn": {
            text: "<span style=\"color:#FFFFFF\">取消</span>"
            //tip: ""
        },
        "downloadBox": {
            text: "<p><a href=\"TLNVOCX_setup.exe\" title=\"TLNVOCX setup\">点击此处手动下载安装ActiveX控件</a></p>"
            //tip: ""
        },
        "noticeBox": {
            text: "<p style=\"color:#D0D0D0\">如果是您第一次登录，请按照提示下载安装控件。</p>"
            //tip: ""
        },
        "liveBtn": {
            text: "实时预览",
            tip: "实时预览监控画面"
        },
        "playbackBtn": {
            text: "回放",
            tip: "录像回放"
        },
        "setupBtn": {
            text: "设置",
            tip: "设置服务器参数"
        },
        "defaultScaleBtn": {
            //text: "默认尺寸",
            tip: "默认尺寸"
        },
        "originalBtn": {
            //text: "原始尺寸",
            tip: "原始尺寸"
        },
        "fitWindowBtn": {
            //text: "适应窗口",
            tip: "适应窗口"
        },
        "mainStreamBtn": {
            text: "主码流",
            tip: "主码流"
        },
        "subStreamBtn": {
            text: "子码流",
            tip: "子码流"
        },
        "downloadPlayerBtn": {
            text: "下载播放器",
            tip:"下载播放器"
        },
        "logoutBtn": {
            text: "退出",
            tip: "退出系统"
        },

        "playT": {
            text: "播放控制"
            //tip: ""
        },
        "ChannelBtn": {
            //text: "",
            tip: "关闭通道"
        },
        "snapBtn": {
            //text: "",
            tip: "抓图"
        },
        "recordBtn": {
            //text: "",
            tip: "启动录像"
        },
        "talkBtn": {
            //text: "",
            tip: "对讲"
        },
        "VoiceBtn": {
            //text: "",
            tip: "声音开关"
        },
        "zoomBtn": {
            //text: "",
            tip: "电子放大"
        },
        "ptzT": {
            text: "云台控制"
            //tip: ""
        },
        "ptzupleft":{
            tip:"云台左上"
        },
        "ptzup": {
            //text: ""
            tip: "云台向上"
        },
        "ptzupright":{
            tip:"云台右上"
        },
        "ptzleft": {
            //text: ""
            tip: "云台向左"
        },
        "ptzdown": {
            //text: ""
            tip: "云台向下"
        },
        "ptzright": {
            //text: ""
            tip: "云台向右"
        },
        "ptzscan": {
            //text: ""
            tip: "云台自动扫描"
        },
        "ptzdownleft":{
            tip: "云台左下"
        },
        "ptzdownRight":{
            tip: "云台右下"
        },
        "zoomFar": {
            //text: ""
            tip: "拉近"
        },
        "zoomNear": {
            //text: ""
            tip: "拉远"
        },
        "zoomlbl": {
            text: "变倍"
            //tip:""
        },
        "focusFar": {
            //text: ""
            tip: "焦距变大"
        },
        "focusNear": {
            //text: ""
            tip: "焦距变小"
        },
        "focuslbl": {
            text: "聚焦"
            //tip:""
        },
        "irisLarge": {
            //text: ""
            tip: "光圈增大"
        },
        "irisSmall": {
            //text: ""
            tip: "光圈减小"
        },
        "irislbl": {
            text: "光圈"
            //tip:""
        },
        "wipper": {
            //text: ""
            tip: "雨刷"
        },
        "light": {
            //text: ""
            tip: "辅助灯光"
        },
        "prepointlbl": {
            text: "预置位:"
            //tip: ""
        },
        "setPrepointBtn": {
            tip: "设置预置位"
            //tip: ""
        },
        "callPrepointBtn": {
            tip: "前往预置位"
            //tip: ""
        },
        "delPrePointBtn": {
            tip: "删除预置位"
        },
        "cruisePathlbl": {
            text: "巡航路径:"
        },
        "playCruisePath": {
            tip: "开始巡航"
        },
        "stopCruisePath":
        {
            tip:"停止巡航"
        },
        "BrightImg": {
            tip: "亮度"
        },
        "ContrastImg": {
            tip: "对比度"
        },
        "HueImg": {
            tip: "色度"
        },
        "DefinitionImg": {
            tip: "清晰度"
        },
        "VerticalMirrorlbl":{
            text:"垂直镜像:"
        },
        "MirrorVOpen":{
            text:"开"
        },
        "MirrorVClose":{
            text:"关"
        },
        "HorizontalMirrorlbl": {
            text:"水平镜像:"
        },
        "MirrorHOpen":{
            text:"开"
        },
        "MirrorHClose":{
            text:"关"
        },
        "Frequencylbl": {
            text:"频率:"
        },
        "Scenelbl": {
            text:"场景:"
        },
        "Infraredlbl": {
            text:"红外触发电平:"
        },
        "imageParamRefresh": {
            text: "<span style=\"color:#FFFFFF;text-align:center\">刷新</span>"
        },
        "imageParamDefault": {
            text: "<span style=\"color:#FFFFFF;\">重置</span>"
        },
        "imageParamSave": {
            text: "<span style=\"color:#FFFFFF;\">保存</span>"
        },
        "Streamlbl": {
            text:"码流类型:"
        },
        "Scalelbl": {
            text: "图像尺寸:"
        }
    },
    "MSG": {
        "net_type_items": {
            "lan": "主码流",
            "wan": "子码流"
        },
        "scrRatio_items": {
            "0": "适应窗口",
            "1": "默认比例"
        },
        "scrRatio_items_X1": {
            "0": "适应窗口",
            "1": "原始尺寸"
        },
        "ImageScale_type": {
            "0": "默认尺寸16:9",
            "1": "原始尺寸",
            "2": "适应窗口"
        },
        "Scene_type": {
            "0": "户外",
            "1": "户内",
            "2": "手动",
            "3": "自动"
        },
        "Infrared_Level": {
            "0": "低电平",
            "1": "高电平"
        },
        "Radio_State": {
            "0": "开",
            "1": "关"
        },
        "ChannelBtn": {
            "0": "打开通道",
            "1": "关闭通道"
        },
        "recordBtn": {
            "0": "启动录像",
            "1": "停止录像"
        },
        "blank_user_pwd": {
            text: "用户名和密码不能为空！"
            //tip:
        },
        "snap_ok": {
            text: "抓图成功!"
            //
        },
        "snap_fail": {
            text: "抓图失败！"
            //
        },
        "invalid_prepoint": {
            text: "对不起，您输入的预置点值无效！\n有效的预置点取值范围为： __min__ - __max__。"
            //tip: ""
        },
        "not_signin": {
            text: "你尚未登录！"
        },
        "ocx_not_installed": {
            text: "NVOCX控件尚未正确的安装！您需要现在下载安装吗 ?\n\n 点击确定开始下载安装包或从右侧的超链接手动下载！"
        },
        "ocx_new_version": {
            text: "设备上有新版本的NVOCX控件！您需要手动下载安装更新吗？\n\n 点击确定开始下载安装包或从右侧的超链接自己下载安装包！"
        },
        "restart_warming": {
            text:"此操作将会导致IPC重启，是否继续？"
        },
        "permission_warming": {
            text:"权限不足，操作无效！"
        }
    }
};

//English
g_lang["en-us"] = {
    "LANG_ID": "en-us",
    "UI": {
        "langlbl": {
            text: "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Language:&nbsp;&nbsp;&nbsp;"
            //tip: ""
        },
        "userNamelbl": {
            text: "&nbsp;&nbsp;&nbsp;User Name:&nbsp;&nbsp;&nbsp;"
            //tip: ""
        },
        "userPwdlbl": {
            text: "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Password:&nbsp;&nbsp;&nbsp;"
            //tip: ""
        },
        "nTypelbl": {
            text: "Stream&nbsp;Type:&nbsp;&nbsp;&nbsp;"
            //tip: ""
        },
        "loginBtn": {
            text: "<span style=\"color:#FFFFFF\">Login</span>"
            //tip: ""
        },
        "cancelloginBtn": {
            text: "<span style=\"color:#FFFFFF\">Cancel</span>"
            //tip: ""
        },
        "downloadBox": {
            text: "<p><a href=\"TLNVOCX_setup.exe\" title=\"TLNVOCX setup\">DownLoad And Install ActiveX</a></p>"
            //tip: ""
        },
        "noticeBox": {
            text: "<p style=\"color:#D0D0D0\">If this is your first time to login, please download and install the ActiveX control.</p>"
            //tip: ""
        },
        "liveBtn": {
            text: "LIVE",
            tip: "LIVE SHOW"
        },
        "playbackBtn": {
            text: "PLAY BACK",
            tip: "Play Back Video"
        },
        "setupBtn": {
            text: "SETTING",
            tip: "Setup"
        },
        "defaultScaleBtn": {
            //text: "DEFAULT SCALE",
            tip: "DEFAULT SCALE"
        },
        "originalBtn": {
            //text: "ORIGINAL",
            tip: "ORIGINAL"
        },
        "fitWindowBtn": {
            //text: "FIT WINDOW",
            tip: "FIT WINDOW"
        },
        "mainStreamBtn": {
            text: "Main Stream",
            tip: "Main Stream"
        },
        "subStreamBtn": {
            text: "Sub Stream",
            tip: "Sub Stream"
        },
        "downloadPlayerBtn": {
            text: "DownLoad Player",
            tip:"DownLoad Player"
        },
        "logoutBtn": {
            text: "LOGOUT",
            tip: "LogOut"
        },
        "playT": {
            text: "PLAY CONTROL"
            //tip: ""
        },
        "ChannelBtn": {
            //text: "",
            tip: "Close Channel"
        },
        "snapBtn": {
            //text: "",
            tip: "Snap"
        },
        "recordBtn": {
            //text: "",
            tip: "Start Record"
        },
        "talkBtn": {
            //text: "",
            tip: "Talk"
        },
        "VoiceBtn": {
            //text: "",
            tip: "Voice Switch"
        },
        "zoomBtn": {
            //text: "",
            tip: "zoomBtn"
        },
        "ptzT": {
            text: "PTZ CONTROL"
            //tip: ""
        },
        "ptzupleft":{
            tip:"top-left"
        },
        "ptzup": {
            //text: ""
            tip: "Upward"
        },
        "ptzupright":{
            tip:"top-right"
        },
        "ptzleft": {
            //text: ""
            tip: "Leftward"
        },
        "ptzdown": {
            //text: ""
            tip: "Downward"
        },
        "ptzright": {
            //text: ""
            tip: "Rightward"
        },
        "ptzscan": {
            //text: ""
            tip: "Auto Scan"
        },
        "ptzdownleft":{
            tip: "bottom-left"
        },
        "ptzdownRight":{
            tip: "bottom-right"
        },
        "zoomFar": {
            //text: ""
            tip: "Zoom Far"
        },
        "zoomNear": {
            //text: ""
            tip: "Zoom Near"
        },
        "zoomlbl": {
            text: "Zoom"
            //tip:""
        },
        "focusFar": {
            //text: ""
            tip: "Focus Far"
        },
        "focusNear": {
            //text: ""
            tip: "Focus Near"
        },
        "focuslbl": {
            text: "Focus"
            //tip:""
        },
        "irisLarge": {
            //text: ""
            tip: "IRIS Large"
        },
        "irisSmall": {
            //text: ""
            tip: "IRIS Small"
        },
        "irislbl": {
            text: "Iris"
            //tip:""
        },
        "wipper": {
            //text: ""
            tip: "Wipper"
        },
        "light": {
            //text: ""
            tip: "Light"
        },
        "prepointlbl": {
            text: "PreSet:"
            //tip: ""
        },
        "setPrepointBtn": {
            //text: "Set"
            tip: "Set"
        },
        "callPrepointBtn": {
            //text: ""
            tip: "Call"
        },
        "delPrePointBtn": {
            tip: "Delete"
        },
        "cruisePathlbl": {
            text: "Cruise Path:"
        },
        "playCruisePath": {
            tip: "Start"
        },
        "stopCruisePath":
        {
            tip:"Stop"
        },
        "BrightImg": {
            tip: "Brightness"
        },
        "ContrastImg": {
            tip: "Contrast"
        },
        "HueImg": {
            tip: "Hue"
        },
        "DefinitionImg": {
            tip: "Definition"
        },
        "VerticalMirrorlbl":{
            text: "Vertical Mirror:"
        },
        "MirrorVOpen":{
            text:"Open"
        },
        "MirrorVClose":{
            text:"Close"
        },
        "HorizontalMirrorlbl": {
            text: "Horizontal Mirror:"
        },
        "MirrorHOpen":{
            text:"Open"
        },
        "MirrorHClose":{
            text:"Close"
        },
        "Frequencylbl": {
            text: "Frequency:"
        },
        "Scenelbl": {
            text: "Scene:"
        },
        "Infraredlbl": {
            text: "Infrared:"
        },
        "imageParamRefresh": {
            text: "<span style=\"color:#FFFFFF;\">Refresh</span>"
        },
        "imageParamDefault": {
            text: "<span style=\"color:#FFFFFF;\">Default</span>"
        },
        "imageParamSave": {
            text: "<span style=\"color:#FFFFFF;\">Save</span>"
        },
        "Streamlbl": {
            text: "StreamType:"
        },
        "Scalelbl": {
            text: "ImageScale:"
        }
    },
    "MSG": {
        "net_type_items": {
            "lan": "Main Stream",
            "wan": "Sub Stream"
        },
        "scrRatio_items": {
            "0": "Fit Window",
            "1": "Default Scale"
        },
        "scrRatio_items_X1": {
            "0": "Fit Window",
            "1": "Original"
        },
        "ImageScale_type": {
            "0": "Default Scale 16:9",
            "1": "Original Scale",
            "2": "Fit Window"
        },
        "Scene_type": {
            "0": "Outdoor",
            "1": "Indoor",
            "2": "Manual",
            "3": "Auto"
        },
        "Infrared_Level": {
            "0": "Low Level",
            "1": "Hight Level"
        },
        "Radio_State": {
            "0": "Open",
            "1": "Close"
        },
        "ChannelBtn": {
            "0": "Open Channel",
            "1": "Close Channel"
        },
        "recordBtn": {
            "0": "Start Record",
            "1": "Stop Record"
        },
        "blank_user_pwd": {
            text: "Blank username or password can not be accepted."
            //tip:
        },
        "snap_ok": {
            text: "Snap Succeed."
            //
        },
        "snap_fail": {
            text: "Snap Failed."
            //
        },
        "invalid_prepoint": {
            text: "Sorry, but you have not provided a valid preset point.\nValid pre-point value is between __min__ and __max__."
            //tip: ""
        },
        "not_signin": {
            text: "You have not signed in yet."
        },
        "ocx_not_installed": {
            text: "The OCX has not been installed yet. Are you sure to install it now ?\n\n Click OK to download the install packet or download it through the hyper-link on the right yourself.."
        },
        "ocx_new_version": {
            text: "There is a new version if OCX available. Are you sure to update it mannually ?\n\n Click OK to download the install packet or download it through the hyper-link on the right yourself.."
        },
        "restart_warming": {
            text:"This operation will result in the IPC restart, Do you want to continue?"
        },
        "permission_warming": {
            text:"permission denied,invalid operation！"
        }
    }
};

//more language packets can be placed here.