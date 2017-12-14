/*
 * Copyright (c) 2016~2021 ShangHai InfoTM Ltd all rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description: videobox json api
 *
 * Author:
 *     beca.zhang <beca.zhang@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  11/08/2017 init
 */
#include <Json.h>

int Json::strcasecmp(const char *s1,const char *s2)
{
    if (!s1) return (s1==s2)?0:1;if (!s2) return 1;
    for(; tolower(*s1) == tolower(*s2); ++s1, ++s2)    if(*s1 == 0)    return 0;
    return tolower(*(const unsigned char *)s1) - tolower(*(const unsigned char *)s2);
}

const char *Json::skip(const char *in) {while (in && *in && (unsigned char)*in<=32) in++; return in;}

/* Parse the input text to generate a number, and populate the result into item. */
const char *Json::parse_number(Json *item,const char *num)
{
    double n=0,sign=1,scale=0;int subscale=0,signsubscale=1;

    if (*num=='-') sign=-1,num++;    /* Has sign? */
    if (*num=='0') num++;            /* is zero */
    if (*num>='1' && *num<='9')    do    n=(n*10.0)+(*num++ -'0');    while (*num>='0' && *num<='9');    /* Number? */
    if (*num=='.' && num[1]>='0' && num[1]<='9') {num++;        do    n=(n*10.0)+(*num++ -'0'),scale--; while (*num>='0' && *num<='9');}    /* Fractional part? */
    if (*num=='e' || *num=='E')        /* Exponent? */
    {    num++;if (*num=='+') num++;    else if (*num=='-') signsubscale=-1,num++;        /* With sign? */
        while (*num>='0' && *num<='9') subscale=(subscale*10)+(*num++ - '0');    /* Number? */
    }

    n=sign*n*pow(10.0,(scale+subscale*signsubscale));    /* number = +/- number.fraction * 10^+/- exponent */

    item->valuedouble=n;
    item->valueint=(int)n;
    item->type=Json_Number;
    return num;
}

unsigned Json::parse_hex4(const char *str)
{
    unsigned h=0;
    if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
    h=h<<4;str++;
    if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
    h=h<<4;str++;
    if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
    h=h<<4;str++;
    if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
    return h;
}

const char *Json::parse_string(Json *item,const char *str)
{
    const char *ptr=str+1;char *ptr2;char *out;int len=0;unsigned uc,uc2;
    if (*str!='\"') {ep=str;return 0;}    /* not a string! */

    while (*ptr!='\"' && *ptr && ++len) if (*ptr++ == '\\') ptr++;    /* Skip escaped quotes. */

    out=(char*)malloc(len+1);    /* This is how long we need for the string, roughly. */
    if (!out) return 0;

    ptr=str+1;ptr2=out;
    while (*ptr!='\"' && *ptr)
    {
        if (*ptr!='\\') *ptr2++=*ptr++;
        else
        {
            ptr++;
            switch (*ptr)
            {
                case 'b': *ptr2++='\b';    break;
                case 'f': *ptr2++='\f';    break;
                case 'n': *ptr2++='\n';    break;
                case 'r': *ptr2++='\r';    break;
                case 't': *ptr2++='\t';    break;
                case 'u':     /* transcode utf16 to utf8. */
                          uc=parse_hex4(ptr+1);ptr+=4;    /* get the unicode char. */

                          if ((uc>=0xDC00 && uc<=0xDFFF) || uc==0)    break;    /* check for invalid.    */

                          if (uc>=0xD800 && uc<=0xDBFF)    /* UTF16 surrogate pairs.    */
                          {
                              if (ptr[1]!='\\' || ptr[2]!='u')    break;    /* missing second-half of surrogate.    */
                              uc2=parse_hex4(ptr+3);ptr+=6;
                              if (uc2<0xDC00 || uc2>0xDFFF)        break;    /* invalid second-half of surrogate.    */
                              uc=0x10000 + (((uc&0x3FF)<<10) | (uc2&0x3FF));
                          }

                          len=4;if (uc<0x80) len=1;else if (uc<0x800) len=2;else if (uc<0x10000) len=3; ptr2+=len;

                          switch (len) {
                              case 4: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
                              case 3: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
                              case 2: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
                              case 1: *--ptr2 =(uc | firstByteMark[len]);
                          }
                          ptr2+=len;
                          break;
                default:  *ptr2++=*ptr; break;
            }
            ptr++;
        }
    }
    *ptr2=0;
    if (*ptr=='\"') ptr++;
    item->valuestring=out;
    item->type=Json_String;
    return ptr;
}

/* Build an array from input text. */
const char *Json::parse_array(Json *item,const char *value)
{
    Json *child;
    if (*value!='[')    {ep=value;return 0;}    /* not an array! */

    item->type=Json_Array;
    value=skip(value+1);
    if (*value==']') return value+1;    /* empty array. */

    item->child=child=new Json();
    if (!item->child) return 0;         /* memory fail */
    child->father = item;
    value=skip(parse_value(child,skip(value)));    /* skip any spacing, get the value. */
    if (!value) return 0;

    while (*value==',')
    {
        Json *new_item;
        if (!(new_item=new Json())) return 0;     /* memory fail */
        child->next=new_item;
        new_item->prev=child;
        new_item->father = child->father;
        child=new_item;
        value=skip(parse_value(child,skip(value+1)));
        if (!value) return 0;    /* memory fail */
    }

    if (*value==']') return value+1;    /* end of array */
    ep=value;return 0;    /* malformed. */
}

/* Build an object from the text. */
const char *Json::parse_object(Json *item,const char *value)
{
    Json *child;
    if (*value!='{')    {ep=value;return 0;}    /* not an object! */

    item->type=Json_Object;
    value=skip(value+1);
    if (*value=='}') return value+1;    /* empty array. */

    item->child=child=new Json();
    child->father = item;
    child->enLayer = (EN_JSON_LAYER)(item->enLayer + 1);
    if (!item->child) return 0;
    value=skip(parse_string(child,skip(value)));
    if (!value) return 0;
    child->string=child->valuestring;child->valuestring=0;
    if (*value!=':') {ep=value;return 0;}    /* fail! */
    value=skip(parse_value(child,skip(value+1)));    /* skip any spacing, get the value. */
    if (!value) return 0;

    while (*value==',')
    {
        Json *new_item;
        if (!(new_item=new Json()))    return 0; /* memory fail */
        new_item->enLayer = child->enLayer;
        new_item->father = child->father;
        child->next=new_item;new_item->prev=child;child=new_item;
        value=skip(parse_string(child,skip(value+1)));
        if (!value) return 0;
        child->string=child->valuestring;child->valuestring=0;
        if (*value!=':') {ep=value;return 0;}    /* fail! */
        value=skip(parse_value(child,skip(value+1)));    /* skip any spacing, get the value. */
        if (!value) return 0;
    }

    if (*value=='}') return value+1;    /* end of array */
    ep=value;return 0;    /* malformed. */
}

/* Parser core - when encountering text, process appropriately. */
const char *Json::parse_value(Json *item,const char *value)
{
    if (!value)                        return 0;    /* Fail on null. */
    if (!strncmp(value,"null",4))    { item->type=Json_NULL;  return value+4; }
    if (!strncmp(value,"false",5))    { item->type=Json_False; return value+5; }
    if (!strncmp(value,"true",4))    { item->type=Json_True; item->valueint=1;    return value+4; }
    if (*value=='\"')                { return parse_string(item,value); }
    if (*value=='-' || (*value>='0' && *value<='9'))    { return parse_number(item,value); }
    if (*value=='[')                { return parse_array(item,value); }
    if (*value=='{')                { return parse_object(item,value); }

    ep=value;return 0;    /* failure. */
}

/* Parse the input text into an unescaped cstring, and populate item. */
const unsigned char Json::firstByteMark[7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};

Json::Json() {
    next = prev = child = father = NULL;
    valuestring = string = 0;
    valueint = valuedouble = 0;
    type = Json_Object;
    bDKeepNext = false;
    enLayer = E_JSON_LAYER_ROOT;
}

Json::~Json() {
    if (!(type&Json_IsReference) && child) {
        delete child;
        child = NULL;
    }
    if (!(type&Json_IsReference) && valuestring) free(valuestring);
    if (string) {
        free(string);
        string = 0;
    }

    if(!bDKeepNext && next)
        delete next;

}

Json::Json(std::string jss) {
    ep=0;
    const char *end=parse_value(this,skip(jss.c_str()));
    if (!end)    throw "wrong videobox description file";
}

void Json::Load(std::string fjs) {
    std::ifstream in(fjs, std::ios::in);
    std::istreambuf_iterator<char> a(in), b;
    std::string jss(a, b);
    in.close();

    if(jss == "")
        jss = "{}";

    ep=0;
    const char *end=parse_value(this,skip(jss.c_str()));
    if (!end)    throw "wrong videobox description file";
}

const char *Json::EP() { return ep; }
Json *Json::Next() { return next; }
Json *Json::Child() { return child; }

Json *Json::GetObject(const char* s) {
    Json *c = child;
    while (c && strcasecmp(c->string,s)) c=c->next;
    return c;
}

const char * Json::GetString(const char *s) {
    Json *c = GetObject(s);
    return c? c->valuestring: NULL;
}

int Json::GetInt(const char* s) {
    Json *c = GetObject(s);
    return c? c->valueint: 0;
}

double Json::GetDouble(const char * s) {
    Json *c = GetObject(s);
    return c? c->valuedouble: 0;
}

int Json::ParseIpuName(char *name, char* ipuname) {
    int len = strlen(name);
    char *p = strchr(name, '-');
    if(p) {
        strncpy(ipuname, name, p-name);
        ipuname[p-name] = '\0';
    } else {
        strncpy(ipuname, name, len+1);
    }
    return  0;
}

//this function called by father pointer by default
/*    {
      |  xxipu: {
      |  |    "ipu": xxx,
      |  |    "args": {
      |  |    |    "key": value
      |  |    },
      |  |    "port": {
      |  |    |    "ol0": {
      |  |    |      |  "key1": value1,
      |  |    |      |  "key2": value2
      |  |    |    },|
      |  |    |    "out": {
      |  |    |      |  "bind": {
      |  |    |      |    |    "key": value
      |  |    |      |  },|
      |  |    |      |  "embezzle": {
      |  |    |      |    |    "key": value
      |  |    |      |    } LAYER_BIND_EMBEZZLE
      |  |    |      } LAYER_PORT
      |  |    } LAYER_ARGS_PORT
      |  } LAYER_IPU
      } LAYER_ROOT
*/
EN_VIDEOBOX_RET Json::JOperaCore(ST_JSON_OPERA *pstOpera, char* ps8IpuName, char *ps8PortName,
        const char *ps8Key, const void *ps8Value, int s32ValueType) {
    char *Name;
    bool bIsItem = false;
    switch (enLayer) {
        case E_JSON_LAYER_ROOT:
            Name = ps8IpuName; //It's child is ipu 
            break;
        case E_JSON_LAYER_IPU: //It's children is args and port
            if(pstOpera->bIsArgs)
                Name = (char*)"args";
            else if(pstOpera->bIsPort)
                Name = (char*)"port";
            break;
        case E_JSON_LAYER_ARGS_PORT:
            if(pstOpera->bIsArgs) {
                Name = (char*)ps8Key;
                bIsItem = true;
            } else if(pstOpera->bIsPort)
                Name = ps8PortName;
            break;
        case E_JSON_LAYER_PORT:
            if(pstOpera->bIsBind)
                Name = (char*)"bind";
            else if(pstOpera->bIsEmbezzle)
                Name = (char*)"embezzle";
            else {
                Name = (char*)ps8Key;
                bIsItem = true;
            }
            break;
        case E_JSON_LAYER_BIND_EMBEZZLE:
            Name = (char*)ps8Key;
            bIsItem = true;
            break;
        default:
            printf("wrong layer\n");
    }
    Json *c = child;
    Json *temp = NULL;
    Json *ipu = NULL;
    while (c && strcasecmp(c->string,Name)) {temp = c; c=c->next;}
    if(!c) {
        if(pstOpera->bDelete || pstOpera->bGet 
                || (!pstOpera->bIsIpu && enLayer == E_JSON_LAYER_ROOT)) {
            return E_VIDEOBOX_INVALID_ARGUMENT;
        }
        ipu = new Json();
        if(!ipu) {
            printf("error, create new json object failed\n");
            return E_VIDEOBOX_MEMORY_ERROR;
        }
        ipu->prev = temp;
        ipu->string = (char*)calloc(strlen(Name) + 1, 1);
        memcpy(ipu->string, Name, strlen(Name) + 1);
        ipu->enLayer = (EN_JSON_LAYER)(enLayer + 1);
        ipu->father = this;

        if(temp)
            temp->next = ipu;
        else
            child = ipu;


        if(enLayer == E_JSON_LAYER_ROOT) { //add "ipu"
            temp = new Json();
            if(!temp) {
                printf("error, create new json object failed\n");
                return E_VIDEOBOX_MEMORY_ERROR;
            }
            temp->type = Json_String;
            temp->string = (char*)calloc(4, 1);
            memcpy(temp->string, "ipu", 4);
            temp->valuestring = (char*)calloc(strlen(ps8IpuName) + 1, 1);
            ParseIpuName(ps8IpuName, temp->valuestring);
            temp->enLayer = E_JSON_LAYER_ARGS_PORT;
            temp->father = ipu;
            ipu->child = temp;
        }

        if(pstOpera->bIsIpu) {
            ipu->type = Json_Object;
            return E_VIDEOBOX_OK;
        }

        if(bIsItem) {
            ipu->type = s32ValueType;
            if(s32ValueType == Json_Number)
                ipu->valueint = *(int*)ps8Value;
            else if(s32ValueType == Json_String) {
                ipu->valuestring = (char*)calloc(strlen((char*)ps8Value) + 1, 1);
                memcpy(ipu->valuestring, (char*)ps8Value, strlen((char*)ps8Value) + 1);
            }
        } else {
            ipu->type = Json_Object;
            ipu->JOperaCore(pstOpera, ipu->string, ps8PortName, ps8Key, ps8Value, s32ValueType);
        }
    } else {
        if(pstOpera->bIsIpu && pstOpera->bSet) {
            printf("%s is already exist!\n", c->string);
            return E_VIDEOBOX_INVALID_ARGUMENT;
        }
        if(bIsItem || (pstOpera->bDelete && c->type == Json_Object && c->child == NULL)) {
            c->type = s32ValueType;
            if(pstOpera->bSet) {
                if(s32ValueType == Json_Number)
                    c->valueint = *(int*)ps8Value;
                else if(s32ValueType == Json_String) {
                    c->valuestring = (char*)calloc(strlen((char*)ps8Value) + 1, 1);
                    memcpy(c->valuestring, (char*)ps8Value, strlen((char*)ps8Value) + 1);
                }
            } else if(pstOpera->bGet) {
                if(s32ValueType == Json_Number)
                    *(int*)ps8Value = c->valueint;
                else if(s32ValueType == Json_String) {
                    if(ps8Value) {
                        memcpy((char*)ps8Value, c->valuestring, strlen(c->valuestring) + 1);
                    } else {
                        return E_VIDEOBOX_NULL_ARGUMENT;
                    }
                }
            } else if(pstOpera->bDelete) {
                if(!temp) {
                    ipu = child;
                    ipu->bDKeepNext = true;
                    if(c->next) {
                        child = c->next;
                        delete ipu;
                    } else {
                        child = NULL;
                        temp = this->father;
                        delete ipu;
                        if(temp)
                            temp->JOperaCore(pstOpera, temp->string, ps8PortName,  //delete blank json object
                                    string, ps8Value, s32ValueType);
                    }
                } else {
                    temp->next = c->next;
                    if(c->next) { 
                        ipu = c->next;
                        ipu->prev = temp;
                    }
                    c->bDKeepNext = true;
                    delete c;
                }
                return E_VIDEOBOX_OK;
            }
        } else {
            c->JOperaCore(pstOpera, c->string, ps8PortName, 
                    ps8Key, ps8Value, s32ValueType);
        }
    }
    return E_VIDEOBOX_OK;
}


void Json::JsonShow(FILE *fp) {
    Json *c = child;
    int i;
    if(enLayer == E_JSON_LAYER_ROOT)
        JSDG(fp,"{\n");
    while (c) {
        for(i = 0; i < c->enLayer; i++)
            JSDG(fp,"\t");
        JSDG(fp,"\"%s\"", c->string);
        if(c->type == Json_Object)
            JSDG(fp,": {\n");
        else if(c->type == Json_Number)
            JSDG(fp,":\t%d", c->valueint);
        else if(c->type == Json_String)
            JSDG(fp,":\t\"%s\"", c->valuestring);

        c->JsonShow(fp);

        if(c->type == Json_Object) {
            for(i = enLayer + 1; i > 0; i--)
                JSDG(fp,"\t");
            JSDG(fp,"}");
        }
        if(c->next)
            JSDG(fp,",\n");
        else
            JSDG(fp,"\n");

        c = c->next;
    }
    if(enLayer == E_JSON_LAYER_ROOT)
        JSDG(fp,"}\n");
}
