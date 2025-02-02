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

package com.rhomobile.rhodes.osfunctionality;

import com.rhomobile.rhodes.Logger;
import com.rhomobile.rhodes.RhoConf;
import com.rhomobile.rhodes.event.CalendarIDsProvider;
import com.rhomobile.rhodes.event.CalendarIDsProviderFroyo;
import com.rhomobile.rhodes.util.ContextFactory;

import android.content.Context;
import android.content.pm.ActivityInfo;
import android.view.Display;
import android.view.Surface;
import android.view.WindowManager;
import android.webkit.WebSettings;

//Android 2.2
class AndroidFunctionality08 extends AndroidFunctionality07 implements
        AndroidFunctionality {

    public int getDeviceRotation(Context context) {
        Display display = ((WindowManager) context.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        int rotation = display.getRotation();
        return rotation;
    }

    @Override
    public int getScreenOrientation(Context context) {
        Display display = ((WindowManager) context.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();

        @SuppressWarnings("deprecation")
        int width = display.getWidth();
        @SuppressWarnings("deprecation")
        int height = display.getHeight();

        switch(display.getRotation()){
        case Surface.ROTATION_0:
        case Surface.ROTATION_90:
            return (width < height) ? ActivityInfo.SCREEN_ORIENTATION_PORTRAIT : ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE;
        case Surface.ROTATION_180:
        case Surface.ROTATION_270:
            return (width < height) ? ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT : ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE;
        default:
            return ActivityInfo.SCREEN_ORIENTATION_UNSPECIFIED;
        }
    }
    
    @Override
    protected void setWebPlugins(WebSettings settings) {
        if(RhoConf.getBool("enable_web_plugins")) {
            settings.setPluginState(WebSettings.PluginState.ON);
            Logger.I(TAG, "Set web plugins state 'ON'");
        }
        else {
            settings.setPluginState(WebSettings.PluginState.OFF);
            Logger.I(TAG, "Set web plugins state 'OFF'");
        }
    }
    
    @Override
    public CalendarIDsProvider buildCalendarIDsProvider() {
    	return new CalendarIDsProviderFroyo();
    }

}