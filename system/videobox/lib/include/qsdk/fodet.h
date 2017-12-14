// Copyright (C) 2016 InfoTM, yong.yan@infotm.com
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

#ifndef __VIDEOBOX_API_H__
#error YOU SHOULD NOT INCLUDE THIS FILE DIRECTLY, INCLUDE <videobox.h> INSTEAD
#endif

#ifndef __VB_FODET_H__
#define __VB_FODET_H__

#ifndef EVENT_FODET
#define EVENT_FODET "fodet"
#endif
#define MAX_SCAN_NUM 10

struct event_fodet {
	int num;
	int x[MAX_SCAN_NUM];
	int y[MAX_SCAN_NUM];
	int w[MAX_SCAN_NUM];
	int h[MAX_SCAN_NUM];
};

#endif /* __VB_FODET_H__ */
