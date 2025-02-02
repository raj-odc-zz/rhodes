/*------------------------------------------------------------------------
* (The MIT License)
* 
* Copyright (c) 2008-2011 Rhomobile, Inc.
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
* 
* http://rhomobile.com
*------------------------------------------------------------------------*/

#ifndef _RHOWEBVIEW_H_
#define _RHOWEBVIEW_H_

//#include "../ruby/include/ruby.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

void rho_webview_refresh(int index);
void rho_webview_navigate(const char* url, int index);
void rho_webview_navigate_back();
void rho_webview_navigate_forward();
const char* rho_webview_execute_js(const char* js, int index);
const char* rho_webview_current_location(int index);
int rho_webview_active_tab();
void rho_webview_set_menu_items(unsigned long valMenu);
void rho_webview_full_screen_mode(int enable);
int rho_webview_get_full_screen();
unsigned long rho_webview_get_current_url(int index);
const char* rho_webview_current_location(int index);
void rho_webview_set_cookie(const char *url, const char *cookie);
void rho_webview_setNavigationTimeout(int nTimeout);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //_RHOWEBVIEW_H_
