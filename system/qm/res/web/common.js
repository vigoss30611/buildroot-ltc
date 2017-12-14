// JavaScript Document
/*********************************放置其它页面用得到公共函数************************/
var dialogTop = (window.screen.height - 600) / 2 - 65;
var dialogLeft = (window.screen.width - 820) / 2;
var minDialogTop = (window.screen.height - 400) / 2 - 65;
var minDialogLeft = (window.screen.width - 300) / 2;

var features = "dialogWidth:820px;dialogHeight:600px;dialogTop:" + dialogTop + ";dialogLeft:" + dialogLeft + ";center:yes;status:no;scroll:no;help:no;resizable:no;";
var featuresAuto = "dialogWidth:960px;dialogHeight:600px;dialogTop:" + dialogTop + ";dialogLeft:" + dialogLeft + ";center:yes;status:no;scroll:auto;help:no;resizable:no;";
var minFeatures = "dialogWidth:300px;dialogHeight:400px;dialogTop:" + minDialogTop + ";dialogLeft:" + minDialogLeft + ";center:yes;status:no;scroll:no;help:no;resizable:no;";
var __DEBUG = 0;
var nBrowser/*浏览器*/, nBrowserVersion/*浏览器版本号*/, nDownloadPath;
var nDownloadPath = "C:\\NVFile\\DownLoad\\";
Date.prototype.Format = function (fmt) { //author: meizz 
    var o = {
        "M+": this.getMonth() + 1, //月份 
        "d+": this.getDate(), //日 
        "h+": this.getHours(), //小时 
        "m+": this.getMinutes(), //分 
        "s+": this.getSeconds(), //秒 
        "q+": Math.floor((this.getMonth() + 3) / 3), //季度 
        "S": this.getMilliseconds() //毫秒 
    };
    if (/(y+)/.test(fmt)) fmt = fmt.replace(RegExp.$1, (this.getFullYear() + "").substr(4 - RegExp.$1.length));
    for (var k in o)
        if (new RegExp("(" + k + ")").test(fmt)) fmt = fmt.replace(RegExp.$1, (RegExp.$1.length == 1) ? (o[k]) : (("00" + o[k]).substr(("" + o[k]).length)));
    return fmt;
}
function GetLanguage() {
    return getCookie("language");
}

function SetPlayStream(Type) {
    setCookie("Stream Type", Type, 365);
}

function GetPlayStream() {
    if (checkCookie("Stream Type") == false) {
        return '11';
    } else {
        return getCookie("Stream Type");
    }
}

function SetAutoLogin(Type) {
    setCookie("auto login", Type, 365);
}

function GetAutoLogin() {
    if (checkCookie("auto login") == false) {
        return '0';
    } else {
        return getCookie("auto login");
    }
}

function getCookie(c_name) {
    var i, x, y, ARRcookies = document.cookie.split(";");
    for (i = 0; i < ARRcookies.length; i++) {
        x = ARRcookies[i].substr(0, ARRcookies[i].indexOf("="));
        y = ARRcookies[i].substr(ARRcookies[i].indexOf("=") + 1);
        x = x.replace(/^\s+|\s+$/g, "");
        if (x == c_name) {
            return unescape(y);
        }
    }
    return null;
}

function setCookie(c_name, value, exdays) {
    var exdate = new Date();
    exdate.setDate(exdate.getDate() + exdays);
    var c_value = escape(value) + ((exdays == null) ? "": "; expires=" + exdate.toUTCString());
    document.cookie = c_name + "=" + c_value;
}

function checkCookie(c_name) {
    var username = getCookie(c_name);
    if (username != null && username != "") {
        return true;
    } else {
        return false;
    }
}
function delCookie(name) //删除cookie
{

    var exp = new Date();

    exp.setTime(exp.getTime() - 1);

    var cval = getCookie(name);

    if (cval != null) document.cookie = name + "=" + cval + ";expires=" + exp.toGMTString();

}

function GetBrowserVer() {
    //1、浏览器版本号函数：
    var br = navigator.userAgent.toLowerCase();
    var browserVer = (br.match(/.+(?:rv|it|ra|ie)[\/: ]([\d.]+)/) || [0, '0'])[1];
    var brower; //得到浏览器名称
    //2、js浏览器判断函数
    var browserName = navigator.userAgent.toLowerCase();
    if (/msie/i.test(browserName) && !/opera/.test(browserName)) {
        brower = "IE";
    } else if (/firefox/i.test(browserName)) {
        brower = "Firefox";
    } else if (/chrome/i.test(browserName) && /webkit/i.test(browserName) && /mozilla/i.test(browserName)) {
        brower = "Chrome";
    } else if (/opera/i.test(browserName)) {
        brower = "Opera";
    } else if (/webkit/i.test(browserName) && !(/chrome/i.test(browserName) && /webkit/i.test(browserName) && /mozilla/i.test(browserName))) {
        brower = "Safari";
    } else {
        brower = "unKnow";
    }

    return {
        brower: brower,
        ver: browserVer
    };
}
//取IE版本号
function IEVersion () {
        try {
            var appVersion = navigator.appVersion;
            var re = new RegExp("MSIE ([0-9]+)");
            re.exec(appVersion);
            return parseInt(RegExp.$1);
        }
        catch (e) {
            if (__DEBUG) {
                alert("IE Version Error: \n" + e.description);
            }
        }
    }
//取浏览器及版本号
function GetBbowser () {
        try {
            var browserMatch = UserAgentMatch(navigator.userAgent.toLowerCase());
            if (browserMatch.browser) {
                nBrowser = browserMatch.browser.toLowerCase();
                nBrowserVersion = browserMatch.version;
            }
        }
        catch (e) {
            if (__DEBUG) {
                alert("IE Version Error: \n" + e.description);
            }
        }
    }
function UserAgentMatch(userAgent) {
        var rMsie = /(msie\s|trident.*rv:)([\w.]+)/,
					rFirefox = /(firefox)\/([\w.]+)/,
					rOpera = /(opera).+version\/([\w.]+)/,
					rChrome = /(chrome)\/([\w.]+)/,
					rSafari = /version\/([\w.]+).*(safari)/;
        var browser;
        var version;

        var match = rMsie.exec(userAgent);
        if (match != null) {
            return { browser: "IE", version: match[2] || "0" };
        }
        var match = rFirefox.exec(userAgent);
        if (match != null) {
            return { browser: match[1] || "", version: match[2] || "0" };
        }
        var match = rOpera.exec(userAgent);
        if (match != null) {
            return { browser: match[1] || "", version: match[2] || "0" };
        }
        var match = rChrome.exec(userAgent);
        if (match != null) {
            return { browser: match[1] || "", version: match[2] || "0" };
        }
        var match = rSafari.exec(userAgent);
        if (match != null) {
            return { browser: match[2] || "", version: match[1] || "0" };
        }
        if (match != null) {
            return { browser: "", version: "0" };
        }
    }
function isNumOrEmpty(value) {
    if (!value && typeof (value) != "undefined") {////if (!value && typeof (value) != "undefined" && value != 0) {
        return true;
    }
    else if (value == "undefined") {
        return true;
    }
    else {
        return false;
    }
}
//遍历Repeater1里的checkbox传值
function listCheckBox() {
    //document.getElementById("hidQuotationIds").value = "";
    var table = document.getElementById("RecordListTable");
    var tr = table.getElementsByTagName("tr");
    var head = "[";
    var end = "]";
    var ss;
    var pattem = /^\d+(\.\d+)?$/;
    for (i = 1; i < tr.length; i++) {
        var chk = tr[i].getElementsByTagName("td")[0].getElementsByTagName("input")[0];
        if (chk) {
            if (chk.checked) {
                if (tr[i].getElementsByTagName("td")[0].getElementsByTagName("input")[0].getAttribute("filename"))
                    ss += '{"filename"' + ':"' + tr[i].getElementsByTagName("td")[0].getElementsByTagName("input")[0].getAttribute("filename") + '"},';
            }
        }
    }
    if (ss) {
        ss =head + ss.substring(0, ss.length - 1)+end;
        //document.getElementById("hidQuotationIds").value = ss.replace("undefined", '');
        //alert(ss.replace("undefined", ''));
        return ss.replace("undefined", '');
    }
}
//全选
function CheckAll(TableName, obj, name) {
    /*var elements = document.getElementById(TableName).getElementsByTagName("input");
    for (var i = 0; i < elements.length; i++) {
        if (elements[i].type == 'checkbox') {
            if (elements[i] != obj && elements[i] != document.getElementById("checkNone")) {
                elements[i].checked = obj.checked;
            }
        }
    } */
    //alert(TableName); alert(obj.checked); alert(name);
    var leg = $("#" + TableName + " tr").length - 1;
    $("#" + TableName + " tr:gt(0):lt(" + leg + ")").each(function () {
        $(this).children("td").eq(0).children('input[type="checkbox"][id="' + name + '"]').each(function () {
            $(this).prop("checked", obj.checked);//attr第二次时有问题chrome
        });        
    });
}
//检查已选数量
function CheckNum(TableName, obj, name,max) {
    var nCheckNum = 0;
    var leg = $("#" + TableName + " tr").length - 1;
    $("#" + TableName + " tr:gt(0):lt(" + leg + ")").each(function () {
        $(this).children("td").eq(0).children('input[type="checkbox"][id="' + name + '"]').each(function () {
            if ($(this).prop("checked")) {
                nCheckNum++;
            }
        });
    });
    if (parseInt(nCheckNum) > parseInt(max)) {
        $(obj).prop("checked", false);
        alert("选择记录数量不能大于"+ max +"!");
    }
}
//反选
function CheckNone(formName, obj, name) {
    var elements = document.getElementById(formName).getElementsByTagName("input");
    var a = 0;
    for (var i = 0; i < elements.length; i++) {
        if (elements[i].type == 'checkbox') {
            if (elements[i] != obj && elements[i] != document.getElementById("checkAll")) {
                if (elements[i].checked) {
                    elements[i].checked = false;
                    a = parseInt(a) + 1;
                }
                else {
                    elements[i].checked = true;
                }
            }
        }
    }
    if (a != 0) {
        document.getElementById("checkAll").checked = false;
    }
    else {
        document.getElementById("checkAll").checked = true;
    }
}
function obj2str(o) {
    var r = [];
    if (typeof o == "string" || o == null) {
        return o;
    }
    if (typeof o == "object") {
        if (!o.sort) {
            r[0] = "{";
            for (var i in o) {
                r[r.length] = i;
                r[r.length] = ":";
                r[r.length] = obj2str(o[i]);
                r[r.length] = "，";
            }
            r[r.length - 1] = "}";
        } else {
            r[0] = "[";
            for (var i = 0; i < o.length; i++) {
                r[r.length] = obj2str(o[i]);
                r[r.length] = "，";
            }
            r[r.length - 1] = "]";
        }
        return r.join("");
    }
    return o.toString();
}
function strToObj(json) {
    return eval('(' + json + ')');
}
function HourToSecond(Time) {
    return parseInt(Time.split(':')[0]) * 60 * 60 + parseInt(Time.split(':')[1]) * 60 + parseInt(Time.split(':')[2]);
}
function SecondToHour(Time) {
    var theTime = parseInt(Time); // 秒
    var theTime1 = 0; // 分
    var theTime2 = 0; // 小时
    // alert(theTime);
    if (theTime > 60) {
        theTime1 = parseInt(theTime / 60);
        theTime = parseInt(theTime % 60);
        // alert(theTime1+"-"+theTime);
        if (theTime1 > 60) {
            theTime2 = parseInt(theTime1 / 60);
            theTime1 = parseInt(theTime1 % 60);
        }
    }
    var result = parseInt(theTime);
    if (result == 0) {
        result = "00";
    } else if (result < 10) {
        result = "0" + result;
    }

    if (theTime1 == 0) {
        result = "00:" + result;
    } else if (theTime1 > 0 && theTime1 < 10) {
        result ="0"+ parseInt(theTime1) + ":" + result;
    }else {
        result = parseInt(theTime1) + ":" + result;
    }

    if (theTime2 == 0) {
        result = "00:" + result;
    } else if (theTime2 > 0 && theTime2 < 10) {
        result = "0" + parseInt(theTime2) + ":" + result;
    } else {
        result = parseInt(theTime2) + ":" + result;
    }
    return result;
}
//删除数组元素http://www.cnblogs.com/qiantuwuliang/archive/2010/09/01/1814706.html
Array.prototype.del = function (n) {//n表示第几项，从0开始算起。
    //prototype为对象原型，注意这里为对象增加自定义方法的方法。
    if (n < 0)//如果n<0，则不进行任何操作。
        return this;
    else
        return this.slice(0, n).concat(this.slice(n + 1, this.length));
    /*
　　　concat方法：返回一个新数组，这个新数组是由两个或更多数组组合而成的。
　　　　　　　　　这里就是返回this.slice(0,n)/this.slice(n+1,this.length)
　　 　　　　　　组成的新数组，这中间，刚好少了第n项。
　　　slice方法： 返回一个数组的一段，两个参数，分别指定开始和结束的位置。
　　*/
}
