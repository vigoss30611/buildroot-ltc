#include <iostream>
#include "Json.h"

using namespace std;
void rJS(Json *js, int indent) {
  for(int i = 0; i < indent; i++) cout << "  " << flush;

  cout << "\"" << (js->string? js->string: "anonymous") << "\": " << flush;
  if(!js) cout << "error object" << endl;
  switch(js->type) {
    case Json_False:
    case Json_True:
      cout << (js->valueint? "true": "false") << endl;
      break ;
    case Json_NULL:
      cout << "null" << endl;
      break ;
    case Json_Number:
      cout << js->valueint << ", " << js->valuedouble << endl;
      break ;
    case Json_String:
      cout << "\"" << js->valuestring << "\"" << endl;
      break ;
    case Json_Array:
      cout << "array: cannot parse" << endl;
      break ;
    case Json_Object:
      cout << "\\" << endl;
      rJS(js->Child(), indent + 1);
  }

  if(js->Next()) {
      rJS(js->Next(), indent);
  }
}

int main(int argc, char *argv[])
{
    Json *js1= new Json("{\n\"name\": \"Jack (\\\"Bee\\\") Nimble\", \n\"format\": {\"type\":       \"rect\", \n\"width\":      1920, \n\"height\":     1080, \n\"interlace\":  false,\"frame rate\": 24\n}\n}");
    Json *js2=new Json("[\"Sunday\", \"Monday\", \"Tuesday\", \"Wednesday\", \"Thursday\", \"Friday\", \"Saturday\"]");
    Json *js3=new Json("[\n    [0, -1, 0],\n    [1, 0, 0],\n    [0, 0, 1]\n    ]\n");
//    Json *js4=new Json("{\n        \"Image\": {\n            \"Width\":  800,\n            \"Height\": 600,\n            \"Title\":  \"View from 15th Floor\",\n            \"Thumbnail\": {\n                \"Url\":    \"http:/*www.example.com/image/481989943\",\n                \"Height\": 125,\n                \"Width\":  \"100\"\n            },\n            \"IDs\": [116, 943, 234, 38793]\n        }\n    }");
//    Json *js5=new Json("[\n     {\n     \"precision\": \"zip\",\n     \"Latitude\":  37.7668,\n     \"Longitude\": -122.3959,\n     \"Address\":   \"\",\n     \"City\":      \"SAN FRANCISCO\",\n     \"State\":     \"CA\",\n     \"Zip\":       \"94107\",\n     \"Country\":   \"US\"\n     },\n     {\n     \"precision\": \"zip\",\n     \"Latitude\":  37.371991,\n     \"Longitude\": -122.026020,\n     \"Address\":   \"\",\n     \"City\":      \"SUNNYVALE\",\n     \"State\":     \"CA\",\n     \"Zip\":       \"94085\",\n     \"Country\":   \"US\"\n     }\n     ]");


    rJS(js1, 0);
    rJS(js2, 0);
    rJS(js3, 0);
//    rJS(js4, 0);
//    rJS(js5, 0);

delete js1;
delete js2;
delete js3;
    Json js;

    js.Load(argv[1]);
    rJS(&js, 0);

  return 0;
}

/*
 * valgrind test summary:
 *
==21530== Memcheck, a memory error detector
==21530== Using Valgrind-3.10.0 and LibVEX; rerun with -h for copyright info
==21530== Command: ./a.out /nfs/vbjs/dual_720_1080.json -v --track-origins=yes
==21530== 
==21530== Conditional jump or move depends on uninitialised value(s)
==21530==    at 0x4015F2: rJS(Json*, int) (Test.cpp:8)
==21530==    by 0x40191B: main (Test.cpp:46)
==21530== 
==21530== Conditional jump or move depends on uninitialised value(s)
==21530==    at 0x4017C0: rJS(Json*, int) (Test.cpp:32)
==21530==    by 0x40191B: main (Test.cpp:46)
==21530== 
==21530== Conditional jump or move depends on uninitialised value(s)
==21530==    at 0x4015F2: rJS(Json*, int) (Test.cpp:8)
==21530==    by 0x40192C: main (Test.cpp:47)
==21530== 
==21530== Conditional jump or move depends on uninitialised value(s)
==21530==    at 0x4017C0: rJS(Json*, int) (Test.cpp:32)
==21530==    by 0x40192C: main (Test.cpp:47)
==21530== 
==21530== Conditional jump or move depends on uninitialised value(s)
==21530==    at 0x4015F2: rJS(Json*, int) (Test.cpp:8)
==21530==    by 0x40193D: main (Test.cpp:48)
==21530== 
==21530== Conditional jump or move depends on uninitialised value(s)
==21530==    at 0x4017C0: rJS(Json*, int) (Test.cpp:32)
==21530==    by 0x40193D: main (Test.cpp:48)
==21530== 
==21530== Conditional jump or move depends on uninitialised value(s)
==21530==    at 0x402BE4: Json::~Json() (Json.h:234)
==21530==    by 0x40194E: main (Test.cpp:52)
==21530== 
==21530== Conditional jump or move depends on uninitialised value(s)
==21530==    at 0x402C01: Json::~Json() (Json.h:235)
==21530==    by 0x40194E: main (Test.cpp:52)
==21530== 
==21530== Conditional jump or move depends on uninitialised value(s)
==21530==    at 0x402C1E: Json::~Json() (Json.h:236)
==21530==    by 0x40194E: main (Test.cpp:52)
==21530== 
==21530== Conditional jump or move depends on uninitialised value(s)
==21530==    at 0x402BE4: Json::~Json() (Json.h:234)
==21530==    by 0x401967: main (Test.cpp:53)
==21530== 
==21530== Conditional jump or move depends on uninitialised value(s)
==21530==    at 0x402C01: Json::~Json() (Json.h:235)
==21530==    by 0x401967: main (Test.cpp:53)
==21530== 
==21530== Conditional jump or move depends on uninitialised value(s)
==21530==    at 0x402C1E: Json::~Json() (Json.h:236)
==21530==    by 0x401967: main (Test.cpp:53)
==21530== 
==21530== Conditional jump or move depends on uninitialised value(s)
==21530==    at 0x402BE4: Json::~Json() (Json.h:234)
==21530==    by 0x401980: main (Test.cpp:54)
==21530== 
==21530== Conditional jump or move depends on uninitialised value(s)
==21530==    at 0x402C01: Json::~Json() (Json.h:235)
==21530==    by 0x401980: main (Test.cpp:54)
==21530== 
==21530== Conditional jump or move depends on uninitialised value(s)
==21530==    at 0x402C1E: Json::~Json() (Json.h:236)
==21530==    by 0x401980: main (Test.cpp:54)
==21530== 
==21530== 
==21530== HEAP SUMMARY:
==21530==     in use at exit: 0 bytes in 0 blocks
==21530==   total heap usage: 140 allocs, 140 frees, 16,719 bytes allocated
==21530== 
==21530== All heap blocks were freed -- no leaks are possible
==21530== 
==21530== For counts of detected and suppressed errors, rerun with: -v
==21530== Use --track-origins=yes to see where uninitialised values come from
==21530== ERROR SUMMARY: 15 errors from 15 contexts (suppressed: 0 from 0)
*/

