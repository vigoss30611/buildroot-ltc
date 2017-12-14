/*
  Copyright (c) 2009 Dave Gamble

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

// Copyright (C) 2016 InfoTM, warits.wang@infotm.com
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef Json_H
#define Json_H
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

/* Json Types: */
#define Json_False 0
#define Json_True 1
#define Json_NULL 2
#define Json_Number 3
#define Json_String 4
#define Json_Array 5
#define Json_Object 6

#define Json_IsReference 256

#define JSON_WRONGDESC  "wrong description file"

class Json {
private:
    int strcasecmp(const char *s1,const char *s2)
    {
        if (!s1) return (s1==s2)?0:1;if (!s2) return 1;
        for(; tolower(*s1) == tolower(*s2); ++s1, ++s2)    if(*s1 == 0)    return 0;
        return tolower(*(const unsigned char *)s1) - tolower(*(const unsigned char *)s2);
    }

    /* Predeclare these prototypes. */
//    const char *parse_value(Json *item,const char *value);
//    const char *parse_array(Json *item,const char *value);
//    const char *parse_object(Json *item,const char *value);

    /* Utility to jump whitespace and cr/lf */
    const char *skip(const char *in) {while (in && *in && (unsigned char)*in<=32) in++; return in;}

    /* Parse the input text to generate a number, and populate the result into item. */
    const char *parse_number(Json *item,const char *num)
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

    unsigned parse_hex4(const char *str)
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

    /* Parse the input text into an unescaped cstring, and populate item. */
    const unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

    const char *parse_string(Json *item,const char *str)
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
    const char *parse_array(Json *item,const char *value)
    {
        Json *child;
        if (*value!='[')    {ep=value;return 0;}    /* not an array! */

        item->type=Json_Array;
        value=skip(value+1);
        if (*value==']') return value+1;    /* empty array. */

        item->child=child=new Json();
        if (!item->child) return 0;         /* memory fail */
        value=skip(parse_value(child,skip(value)));    /* skip any spacing, get the value. */
        if (!value) return 0;

        while (*value==',')
        {
            Json *new_item;
            if (!(new_item=new Json())) return 0;     /* memory fail */
            child->next=new_item;new_item->prev=child;child=new_item;
            value=skip(parse_value(child,skip(value+1)));
            if (!value) return 0;    /* memory fail */
        }

        if (*value==']') return value+1;    /* end of array */
        ep=value;return 0;    /* malformed. */
    }

    /* Build an object from the text. */
    const char *parse_object(Json *item,const char *value)
    {
        Json *child;
        if (*value!='{')    {ep=value;return 0;}    /* not an object! */

        item->type=Json_Object;
        value=skip(value+1);
        if (*value=='}') return value+1;    /* empty array. */

        item->child=child=new Json();
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
    const char *parse_value(Json *item,const char *value)
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

    class Json *next,*prev;    //next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
    class Json *child;        // An array or object item will have a child pointer pointing to a chain of the items in the array/object. */
    const char *ep;

public:
    int type;                // The type of the item, as above. */
    char *valuestring;        // The item's string, if type==Json_String */
    int valueint;            // The item's number, if type==Json_Number */
    double valuedouble;        // The item's number, if type==Json_Number */

    char *string;            // The item's name string, if this item is the child of, or is in the list of subitems of an object. */

public:
    Json() {
        next = prev = child = NULL;
        valuestring = string = 0;
        valueint = valuedouble = 0;
        type = Json_Object;
    }

    ~Json() {
        if (!(type&Json_IsReference) && child) delete child;
        if (!(type&Json_IsReference) && valuestring) free(valuestring);
        if (string) free(string);
        if (next) delete next;
    }

    Json(std::string jss) {
        ep=0;
        const char *end=parse_value(this,skip(jss.c_str()));
        if (!end)    throw JSON_WRONGDESC;
    }

    void Load(std::string fjs) {
        std::ifstream in(fjs, std::ios::in);
        std::istreambuf_iterator<char> a(in), b;
        std::string jss(a, b);
        in.close();

        ep=0;
        const char *end=parse_value(this,skip(jss.c_str()));
        if (!end)    throw JSON_WRONGDESC;
    }

    const char *EP() { return ep; }
    Json *Next() { return next; }
    Json *Child() { return child; }

    Json *GetObject(const char* s) {
        Json *c = child;
        while (c && strcasecmp(c->string,s)) c=c->next;
        return c;
    }

    const char * GetString(const char *s) {
        Json *c = GetObject(s);
        return c? c->valuestring: NULL;
    }

    int GetInt(const char* s) {
        Json *c = GetObject(s);
        return c? c->valueint: 0;
    }

    double GetDouble(const char * s) {
        Json *c = GetObject(s);
        return c? c->valuedouble: 0;
    }
};

#endif // Json_H
