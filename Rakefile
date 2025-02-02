#------------------------------------------------------------------------
# (The MIT License)
# 
# Copyright (c) 2008-2011 Rhomobile, Inc.
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# 
# http://rhomobile.com
#------------------------------------------------------------------------

require 'find'
require 'erb'
#require 'rdoc/task'
require 'digest/sha2'
require 'rexml/document'
require 'pathname'

#Look, another big fat hack. Make it so we can remove tasks from rake -T by setting comment to nil
module Rake
  class Task
    attr_accessor :comment
  end
end

#TODO check generate api 
class GeneratorTimeChecker
  
  @@latest_update_time = nil
  @@app_path           = nil
  @@is_run_always      = false
  @@cached_time        = nil
  @@do_cache           = false
   
  def find_latest_modified_date(path)
    #puts 'find_latest_modified_date.path=' + path.to_s
    
    latest_mod_time = nil      
    templates_dir   = Dir.new(path)
    
    templates_dir.each { |item|       
      if item == '.' || item == '..'
        next        
      end
  
      full_path = File.join(path, item)
      mod_time  = nil
      
      if File.directory?(full_path)
        mod_time = find_latest_modified_date(full_path)
        
        if mod_time.nil?
          next
        end            
      else
        mod_time = File.mtime(full_path)
      end
      
      if latest_mod_time.nil?
        latest_mod_time = mod_time
      else
        if latest_mod_time < mod_time
          latest_mod_time = mod_time
        end
      end
    }
    
    return latest_mod_time
  end
  
  def init(startdir, app_path)
    @@app_path       = app_path
    templates_path   = File.join(startdir, "res", "generators", "templates", "api")
    rhogen_path      = File.join(startdir, "res", "generators", "rhogen.rb")
    
    api_time         = find_latest_modified_date(templates_path)
    rhogen_time      = File.mtime(rhogen_path)
    last_update_time = nil
    
    last_change_data = find_latest_modified_date(templates_path)
    time_cache_path  = File.join(@@app_path, "bin", "tmp", "rhogen.time")
     
    if last_change_data.nil?
      @@is_run_always = true
    end
    
    if File.exist? time_cache_path
      time_cache_file = File.new(time_cache_path)
      @@cached_time   = Time.parse(time_cache_file.gets)
      time_cache_file.close
      
      @@latest_update_time = rhogen_time >= api_time ? rhogen_time : api_time    
      puts 'cached time=' + @@latest_update_time.to_s
            
      if @@cached_time < @@latest_update_time
        @@is_run_always = true
      end       
    else
      @@is_run_always = true
    end
  end
  
  def check(xmlpath)
    @@do_cache = false

    extpath  = File.dirname(xmlpath)
    xml_time = File.mtime(File.new(xmlpath))
    
    # for generate in first time
    if @@is_run_always
      @@do_cache = true
    elsif !(File.exist? File.join(extpath, "shared", "generated"))              ||
       !(File.exist? File.join(extpath, "platform", "android", "generated")) ||
       !(File.exist? File.join(extpath, "platform", "iphone", "generated"))  ||
#       !(File.exist? File.join(extpath, "platform", "osx", "generated"))     ||
#       !(File.exist? File.join(extpath, "platform", "wm", "generated"))      ||
#       !(File.exist? File.join(extpath, "platform", "wp8", "generated"))     ||
       !(File.exist? File.join(extpath, "..", "public", "api", "generated"))
      @@do_cache = true
    elsif @@cached_time < xml_time
      @@do_cache = true
    end 
    
    return @@do_cache
  end
  
  def update()
    if @@do_cache == false
      return
    end
    
    time_cache_path  = File.join(@@app_path, "bin", "tmp", "rhogen.time")

    FileUtils.mkdir(File.join(@@app_path, "bin") ) unless File.exist? File.join(@@app_path, "bin")
    FileUtils.mkdir(File.join(@@app_path, "bin", "tmp") ) unless File.exist? File.join(@@app_path, "bin", "tmp")
    
    time_cache_file = File.new(time_cache_path, "w+")
    time_cache_file.puts Time.new
    time_cache_file.close()
  end
end

# Restore process error mode on Windows.
# Error mode controls wether system error message boxes will be shown to user.
# Java disables message boxes and we enable them back.
#if RUBY_PLATFORM =~ /(win|w)32$/
#  require 'win32/process'
#  class WindowsError
#    include Windows::Error
#  end
#  WindowsError.new.SetErrorMode(0)
#end

$app_basedir = pwd
$startdir = File.dirname(__FILE__)
$startdir.gsub!('\\', '/')

chdir File.dirname(__FILE__)

require File.join(pwd, 'lib/build/jake.rb')

load File.join(pwd, 'platform/bb/build/bb.rake')
load File.join(pwd, 'platform/android/build/android.rake')
load File.join(pwd, 'platform/iphone/rbuild/iphone.rake')
load File.join(pwd, 'platform/wm/build/wm.rake')
load File.join(pwd, 'platform/linux/tasks/linux.rake')
load File.join(pwd, 'platform/wp7/build/wp.rake')
load File.join(pwd, 'platform/wp8/build/wp.rake')
load File.join(pwd, 'platform/symbian/build/symbian.rake')
load File.join(pwd, 'platform/osx/build/osx.rake')

def get_dir_hash(dir, init = nil)
  hash = init
  hash = Digest::SHA2.new if hash.nil?
  Dir.glob(dir + "/**/*").each do |f|
    hash << f
    hash.file(f) if File.file? f
  end
  hash
end

namespace "framework" do
  task :spec do
    loadpath = $LOAD_PATH.inject("") { |load_path,pe| load_path += " -I" + pe }

    rhoruby = ""

    if RUBY_PLATFORM =~ /(win|w)32$/
      rhoruby = 'res\\build-tools\\RhoRuby'
    elsif RUBY_PLATFORM =~ /darwin/
      rhoruby = 'res/build-tools/RubyMac'
    else
      rhoruby = 'res/build-tools/rubylinux'
    end
   
    puts `#{rhoruby}  -I#{File.expand_path('spec/framework_spec/app/')} -I#{File.expand_path('lib/framework')} -I#{File.expand_path('lib/test')} -Clib/test framework_test.rb`
  end
end


$application_build_configs_keys = ['security_token', 'encrypt_database', 'android_title', 'iphone_db_in_approot', 'iphone_set_approot', 'iphone_userpath_in_approot', "motorola_license", "motorola_license_company","name"]
$winxpe_build = false
      
def make_application_build_config_header_file
  f = StringIO.new("", "w+")      
  f.puts "// WARNING! THIS FILE IS GENERATED AUTOMATICALLY! DO NOT EDIT IT MANUALLY!"
  #f.puts "// Generated #{Time.now.to_s}"
  f.puts ""
  f.puts "#include <string.h>"
  f.puts ""
  f.puts '#include "app_build_configs.h"'
  if $rhosimulator_build
    f.puts '#include "common/RhoSimConf.h"'
  end
  f.puts ""
      
  f.puts 'static const char* keys[] = { ""'
  $application_build_configs.keys.each do |key|
    f.puts ',"'+key+'"'
  end
  f.puts '};'
  f.puts ''
  
  count = 1

  f.puts 'static const char* values[] = { ""'
  $application_build_configs.keys.each do |key|
    value = $application_build_configs[key].to_s().gsub('\\', "\\\\\\")
    value = value.gsub('"', "\\\"")
    f.puts ',"'+ value +'"'
    count = count + 1
  end
  f.puts '};'
  f.puts ''

  f.puts '#define APP_BUILD_CONFIG_COUNT '+count.to_s
  f.puts ''
  f.puts 'const char* get_app_build_config_item(const char* key) {'
  f.puts '  int i;'
  if $rhosimulator_build
    f.puts '  if (strcmp(key, "security_token") == 0) {'
    f.puts '    return rho_simconf_getString("security_token");'
    f.puts '  }'
  end
  f.puts '  for (i = 1; i < APP_BUILD_CONFIG_COUNT; i++) {'
  f.puts '    if (strcmp(key, keys[i]) == 0) {'
  f.puts '      return values[i];'
  f.puts '    }'
  f.puts '  }'
  f.puts '  return 0;'
  f.puts '}'
  f.puts ''
  
  Jake.modify_file_if_content_changed(File.join($startdir, "platform", "shared", "common", "app_build_configs.c"), f)
end

def make_application_build_capabilities_header_file
  f = StringIO.new("", "w+")      
  f.puts "// WARNING! THIS FILE IS GENERATED AUTOMATICALLY! DO NOT EDIT IT MANUALLY!"
  #f.puts "// Generated #{Time.now.to_s}"
  f.puts ""

  caps = []
 
  capabilities = $app_config["capabilities"]

  if capabilities != nil && capabilities.is_a?(Array)
     capabilities.each do |cap|
        caps << cap
     end
  end

  caps.sort.each do |cap|
     f.puts '#define APP_BUILD_CAPABILITY_'+cap.upcase
  end

  f.puts ''
  
  Jake.modify_file_if_content_changed(File.join($startdir, "platform", "shared", "common", "app_build_capabilities.h"), f)
end

def make_application_build_config_java_file

    f = StringIO.new("", "w+")
    f.puts "// WARNING! THIS FILE IS GENERATED AUTOMATICALLY! DO NOT EDIT IT MANUALLY!"
    #f.puts "// Generated #{Time.now.to_s}"

    f.puts "package com.rho;"
    f.puts ""
    f.puts "public class AppBuildConfig {"

    f.puts 'static final String keys[] = { ""'
    $application_build_configs.keys.each do |key|
      f.puts ',"'+key+'"'
    end
    f.puts '};'
    f.puts ''

    count = 1

    f.puts 'static final String values[] = { ""'
    $application_build_configs.keys.each do |key|
      f.puts ',"'+$application_build_configs[key]+'"'
      count = count + 1
    end
    f.puts '};'
    f.puts ''

    f.puts 'static final int APP_BUILD_CONFIG_COUNT = '+count.to_s + ';'
    f.puts ''
    f.puts 'public static String getItem(String key){'
    f.puts '  for (int i = 1; i < APP_BUILD_CONFIG_COUNT; i++) {'
    f.puts '    if ( key.compareTo( keys[i]) == 0) {'
    f.puts '      return values[i];'
    f.puts '    }'
    f.puts '  }'
    f.puts '  return null;'
    f.puts '}'
    f.puts "}"

    Jake.modify_file_if_content_changed( File.join( $startdir, "platform/bb/RubyVM/src/com/rho/AppBuildConfig.java" ), f )
end

def update_rhoprofiler_java_file
    use_profiler = $app_config['profiler'] || ($app_config[$current_platform] && $app_config[$current_platform]['profiler'])
    use_profiler = use_profiler && use_profiler.to_i() != 0 ? true : false
    
    content = ""
    File.open( File.join( $startdir, "platform/bb/RubyVM/src/com/rho/RhoProfiler.java" ), 'rb' ){ |f| content = f.read() }
    is_find = nil

    if use_profiler
        is_find = content.sub!( 'RHO_STRIP_PROFILER = true;', 'RHO_STRIP_PROFILER = false;' )
    else
        is_find = content.sub!( 'RHO_STRIP_PROFILER = false;', 'RHO_STRIP_PROFILER = true;' )
    end    
    
    if is_find
        puts "RhoProfiler.java has been modified: RhoProfiler is " + (use_profiler ? "enabled!" : "disabled!")
        File.open( File.join( $startdir, "platform/bb/RubyVM/src/com/rho/RhoProfiler.java" ), 'wb' ){ |f| f.write(content) }
    end
end

def update_rhodefs_header_file
    use_profiler = $app_config['profiler'] || ($app_config[$current_platform] && $app_config[$current_platform]['profiler'])
    use_profiler = use_profiler && use_profiler.to_i() != 0 ? true : false
    
    content = ""
    File.open( File.join( $startdir, "platform/shared/common/RhoDefs.h" ), 'rb' ){ |f| content = f.read() }
    is_find = nil

    if use_profiler
        is_find = content.sub!( '#define RHO_STRIP_PROFILER 1', '#define RHO_STRIP_PROFILER 0' )
    else
        is_find = content.sub!( '#define RHO_STRIP_PROFILER 0', '#define RHO_STRIP_PROFILER 1' )
    end    
    
    if is_find
        puts "RhoDefs.h has been modified: RhoProfiler is " + (use_profiler ? "enabled!" : "disabled!")
        File.open( File.join( $startdir, "platform/shared/common/RhoDefs.h" ), 'wb' ){ |f| f.write(content) }
    end
end

namespace "clean" do
  task :common => "config:common" do
    
    if $config["platform"] == "bb"
      return
    end
    
    extpaths = $app_config["extpaths"]
      
    $app_config["extensions"].each do |extname|
      puts 'ext - ' + extname
       
      extpath = nil
      extpaths.each do |p|
         ep = File.join(p, extname)
         if File.exists?( ep ) && is_ext_supported(ep)
           extpath = ep
           break
         end
      end    

      if extpath.nil?        
        extpath = find_ext_ingems(extname) 
        if extpath
          extpath = nil unless is_ext_supported(extpath)
        end    
      end
         
      if (extpath.nil?) && (extname != 'rhoelements-license') && (extname != 'motoapi')
        raise "Can't find extension '#{extname}'. Aborting build.\nExtensions search paths are:\n#{extpaths}"
      end
           
      unless extpath.nil?
        extyml = File.join(extpath, "ext.yml")
        puts "extyml " + extyml 
        
        if File.file? extyml
          extconf = Jake.config(File.open(extyml))
          type    = extconf["exttype"]
          wm_type = extconf["wm"]["exttype"] if extconf["wm"]
       
          if type != "prebuilt" && wm_type != "prebuilt"      
            rm_rf  File.join(extpath, "ext", "shared", "generated")
            rm_rf  File.join(extpath, "ext", "platform", "android", "generated")
            rm_rf  File.join(extpath, "ext", "platform", "iphone", "generated")
            rm_rf  File.join(extpath, "ext", "platform", "osx", "generated")
            rm_rf  File.join(extpath, "ext", "platform", "wm", "generated")
            rm_rf  File.join(extpath, "ext", "platform", "wp8", "generated") 
            rm_rf  File.join(extpath, "ext", "public", "api", "generated")
          end
        end
      end    
    end
  end  
end

namespace "config" do
  task :common do
    
    $binextensions = []
    $app_extensions_list = {}
    buildyml = 'rhobuild.yml'

    buildyml = ENV["RHOBUILD"] unless ENV["RHOBUILD"].nil?
    $config = Jake.config(File.open(buildyml))
    $config["platform"] = $current_platform if $current_platform
    $config["env"]["app"] = "spec/framework_spec" if $rhosimulator_build
    
    if RUBY_PLATFORM =~ /(win|w)32$/
      $all_files_mask = "*.*"
      $rubypath = "res/build-tools/RhoRuby.exe"
    else
      $all_files_mask = "*"
      if RUBY_PLATFORM =~ /darwin/
        $rubypath = "res/build-tools/RubyMac"
      else
        $rubypath = "res/build-tools/rubylinux"
      end
    end

	  $app_path = ENV["RHO_APP_PATH"] if ENV["RHO_APP_PATH"] && $app_path.nil?

    if $app_path.nil? #if we are called from the rakefile directly, this wont be set
      #load the apps path and config

      $app_path = $config["env"]["app"]
      unless File.exists? $app_path
        puts "Could not find rhodes application. Please verify your application setting in #{File.dirname(__FILE__)}/rhobuild.yml"
        exit 1
      end
    end

	  ENV["RHO_APP_PATH"] = $app_path.to_s
    ENV["ROOT_PATH"] = $app_path.to_s + '/app/'
    ENV["APP_TYPE"] = "rhodes"

    $app_config = Jake.config(File.open(File.join($app_path, "build.yml")))

    Jake.set_bbver($app_config["bbver"].to_s)
    
    extpaths = []

    if $app_config["paths"] and $app_config["paths"]["extensions"]
      if $app_config["paths"]["extensions"].is_a? String
		p = $app_config["paths"]["extensions"]
		unless Pathname.new(p).absolute?
			p = File.expand_path(File.join($app_path,p))
		end
		extpaths << p
      elsif $app_config["paths"]["extensions"].is_a? Array
		$app_config["paths"]["extensions"].each do |p|
			unless Pathname.new(p).absolute?
				p = File.expand_path(File.join($app_path,p))
			end
			extpaths << p
		end
		#extpaths += $app_config["paths"]["extensions"]
      end
    end
    if $config["env"]["paths"]["extensions"]
      #extpaths << $config["env"]["paths"]["extensions"]
      env_path_exts = $config["env"]["paths"]["extensions"]
      if env_path_exts.is_a? String
        extpaths << p
      elsif env_path_exts.is_a? Array
        env_path_exts.each do |p|
          extpaths << p
        end
      end
    end
    extpaths << File.join($app_path, "extensions")
    extpaths << File.join($startdir, "lib","commonAPI")
    extpaths << File.join($startdir, "lib","extensions")
    $app_config["extpaths"] = extpaths
    
    if $app_config["build"] and $app_config["build"] == "release"
      $debug = false
    else
      $debug = true
    end
    
    extensions = []
    extensions << "coreapi" #unless $app_config['re_buildstub']
    extensions += $app_config["extensions"] if $app_config["extensions"] and
       $app_config["extensions"].is_a? Array
    extensions += $app_config[$config["platform"]]["extensions"] if $app_config[$config["platform"]] and
       $app_config[$config["platform"]]["extensions"] and $app_config[$config["platform"]]["extensions"].is_a? Array
    extensions += get_extensions
    extensions << "rhoconnect-client" if $rhosimulator_build
    extensions << "json"
    $app_config["extensions"] = extensions.uniq
    
    capabilities = []
    capabilities += $app_config["capabilities"] if $app_config["capabilities"] and
       $app_config["capabilities"].is_a? Array
    capabilities += $app_config[$config["platform"]]["capabilities"] if $app_config[$config["platform"]] and
       $app_config[$config["platform"]]["capabilities"] and $app_config[$config["platform"]]["capabilities"].is_a? Array
    $app_config["capabilities"] = capabilities

    application_build_configs = {}

    #Process rhoelements settings
    if $current_platform == "wm" || $current_platform == "android"
        if $app_config["app_type"] == 'rhoelements'
        
            if !$app_config["capabilities"].index('non_motorola_device')        
                $app_config["capabilities"] += ["motorola"] unless $app_config["capabilities"].index("motorola")
                $app_config["extensions"] += ["rhoelementsext"]
                $app_config["extensions"] += ["motoapi"] #extension with plug-ins
                
                #check for RE2 plugins
                plugins = ""
                $app_config["extensions"].each do |ext|
                    if ( ext.start_with?('moto-') )
                        plugins += ',' if plugins.length() > 0
                        plugins += ext[5, ext.length()-5]
                    end
                end
                
                if plugins.length() == 0
                    plugins = "ALL"    
                end
                
                application_build_configs['moto-plugins'] = plugins if plugins.length() > 0
                
            end
            
            if !$app_config["capabilities"].index('native_browser') && $current_platform != "android"
                $app_config["capabilities"] += ["motorola_browser"] unless $app_config["capabilities"].index('motorola_browser')
            end
        end

        application_build_configs['shared-runtime'] = '1' if $app_config["capabilities"].index('shared_runtime')

        if $app_config["capabilities"].index("motorola_browser")
            $app_config['extensions'] += ['webkit-browser'] unless $app_config['extensions'].index('webkit-browser')
        end
        
        if $app_config["extensions"].index("webkit-browser")
            $app_config["capabilities"] += ["webkit_browser"]
            $app_config["extensions"].delete("webkit-browser")
        end
        
        if  $app_config["capabilities"].index("webkit_browser") || ($app_config["capabilities"].index("motorola") && $current_platform != "android")
            #contains wm and android libs for webkit browser
            $app_config["extensions"] += ["rhoelements"] unless $app_config['extensions'].index('rhoelements')
        end
    end

    if $app_config["app_type"] == 'rhoelements'
    
        # add rawsensors extension for rhoelements app
        if $current_platform == "iphone" || $current_platform == "android"
            if !$app_config['extensions'].index('rhoelementsext')
                $app_config["extensions"] += ["rawsensors"] unless $app_config['extensions'].index('rawsensors')
                $app_config["extensions"] += ["audiocapture"] unless $app_config['extensions'].index('audiocapture')
            end
        end
        
        if $current_platform == "wm"
            $app_config['extensions'] = $app_config['extensions'] | ['barcode']
            $app_config['extensions'] = $app_config['extensions'] | ['indicators']
            $app_config['extensions'] = $app_config['extensions'] | ['cardreader']
        end    
        
        if $current_platform == "iphone"
            $app_config['extensions'] = $app_config['extensions'] | ['barcode']
            $app_config['extensions'] = $app_config['extensions'] | ['signature']
        end    

        if $current_platform == "android"
            $app_config['extensions'] = $app_config['extensions'] | ['signature']
            $app_config['extensions'] = $app_config['extensions'] | ['cardreader']
        end    
        
    end

    if $app_config['extensions'].index('rhoelementsext')
        $app_config["extensions"].delete("rawsensors")
        $app_config["extensions"].delete("audiocapture")
    end

    $hidden_app = $app_config["hidden_app"].nil?() ? "0" : $app_config["hidden_app"]
    
    #application build configs

    $application_build_configs_keys.each do |key|
      value = $app_config[key]
      if $app_config[$config["platform"]] != nil
        if $app_config[$config["platform"]][key] != nil
          value = $app_config[$config["platform"]][key]
        end
      end
      if value != nil
        application_build_configs[key] = value
      end
    end	
    $application_build_configs = application_build_configs
    #check for rhoelements gem
    $rhoelements_features = ""
    if $app_config['extensions'].index('barcode')
        #$app_config['extensions'].delete('barcode')
        $rhoelements_features += "- Barcode extension\n"
    end
    if $app_config['extensions'].index('nfc')
        #$app_config['extensions'].delete('nfc')
        $rhoelements_features += "- NFC extension\n"
    end
    if $app_config['extensions'].index('rawsensors')
        #$app_config['extensions'].delete('rawsensors')
        $rhoelements_features += "- Raw Sensors\n"
    end
    if $app_config['extensions'].index('audiocapture')
        #$app_config['extensions'].delete('audiocapture')
        $rhoelements_features += "- Audio Capture\n"
    end
    if $app_config['extensions'].index('signature') && (($current_platform == "iphone") || ($current_platform == "android"))
        $rhoelements_features += "- Signature Capture\n"
    end
    
    if $current_platform == "wm"
        $rhoelements_features += "- Windows Mobile/Windows CE platform support\n"
    end

    if $current_platform == "win32" && !$is_rho_simulator
        $rhoelements_features += "- Windows Desktop platform support\n"
    end
    
    if $application_build_configs['encrypt_database'] && $application_build_configs['encrypt_database'].to_s == '1'
        #$application_build_configs.delete('encrypt_database')
        $rhoelements_features += "- Database encryption\n"
    end

    if $app_config["capabilities"].index("motorola")
        $rhoelements_features += "- Motorola device capabilities\n"                
    end

    if $app_config['extensions'].index('webkit-browser')
        $rhoelements_features += "- Motorola WebKit Browser\n"                
    end

    if $app_config['extensions'].index('rho-javascript')
        $rhoelements_features += "- Javascript API for device capabilities\n"                
    end

    if File.exist?(File.join($app_path, "license.yml"))
        license_config = Jake.config(File.open(File.join($app_path, "license.yml")))    
    
        if ( license_config )
            $application_build_configs["motorola_license"] = license_config["motorola_license"] if license_config["motorola_license"]
            $application_build_configs["motorola_license_company"] = license_config["motorola_license_company"] if license_config["motorola_license_company"]
        end    
    end    
    
    $invalid_license = false

    if $rhoelements_features.length() > 0

        $app_config['extensions'] << 'rhoelements-license'

        #check for RhoElements gem and license
	    if  !$app_config['re_buildstub']	
            begin
                require "rhoelements"
                
                $rhoelements_features = ""
                
                # check license
                is_ET1 = (($current_platform == "android") and ($app_config["capabilities"].index("motorola")))
                is_win_platform = (($current_platform == "wm") or ($current_platform == "win32") or $is_rho_simulator )

                if (!is_ET1) and (!is_win_platform)
                     # check the license parameter
                     if (!$application_build_configs["motorola_license"]) or (!$application_build_configs["motorola_license_company"])
                        $invalid_license = true
                     end
                end
                
            rescue Exception => e
                if $app_config['extensions'].index('barcode')
                    $app_config['extensions'].delete('barcode')
                end
                if $app_config['extensions'].index('nfc')
                    $app_config['extensions'].delete('nfc')
                end
                if $app_config['extensions'].index('rawsensors')
                    $app_config['extensions'].delete('rawsensors')
                end
                if $app_config['extensions'].index('audiocapture')
                    $app_config['extensions'].delete('audiocapture')
                end
                if $app_config['extensions'].index('rho-javascript')
                    $app_config['extensions'].delete('rho-javascript')
                end
                
                if $application_build_configs['encrypt_database'] && $application_build_configs['encrypt_database'].to_s == '1'
                    $application_build_configs.delete('encrypt_database')
                end
            end
        end
    end

    if $current_platform == "win32" && $winxpe_build == true
      $app_config['capabilities'] << 'winxpe'
    end
        
    $app_config['extensions'].uniq!() if $app_config['extensions']
    $app_config['capabilities'].uniq!() if $app_config['capabilities']

    $app_config['extensions'].delete("mspec") if !$debug && $app_config['extensions'].index('mspec')
    $app_config['extensions'].delete("rhospec") if !$debug && $app_config['extensions'].index('rhospec')
    
    if $invalid_license
        $application_build_configs["motorola_license"] = '123' if !$application_build_configs["motorola_license"]
        $application_build_configs["motorola_license_company"] = 'WRONG' if !$application_build_configs["motorola_license_company"]
    end

    $rhologhostport = $config["log_host_port"] 
    $rhologhostport = 52363 unless $rhologhostport
    begin
	    $rhologhostaddr = Jake.localip()
    rescue Exception => e      
        puts "Jake.localip() error : #{e}"  
    end

    $obfuscate_js = (($app_config["obfuscate"].nil? || $app_config["obfuscate"]["js"].nil?) ? nil : 1 )
    $obfuscate_css = (($app_config["obfuscate"].nil? || $app_config["obfuscate"]["css"].nil?) ? nil : 1 )
    $obfuscate_exclude = ($app_config["obfuscate"].nil? ? nil : $app_config["obfuscate"]["exclude_dirs"] )
    $obfuscator = 'res/build-tools/yuicompressor-2.4.7.jar'

    platform_task = "config:#{$current_platform}:app_config"
    Rake::Task[platform_task].invoke if Rake::Task.task_defined? platform_task
    
    puts "$app_config['extensions'] : #{$app_config['extensions'].inspect}"   
    puts "$app_config['capabilities'] : #{$app_config['capabilities'].inspect}"   

    if $current_platform == "bb"  
      make_application_build_config_java_file()
      update_rhoprofiler_java_file()
    elsif $current_platform == "wp"  
    else
      make_application_build_config_header_file    
      make_application_build_capabilities_header_file
      update_rhodefs_header_file
    end

  end

  task :qt do
    $qtdir = ENV['QTDIR']
    unless (!$qtdir.nil?) and ($qtdir !~/^\s*$/) and File.directory?($qtdir)
      puts "\nPlease, set QTDIR environment variable to Qt root directory path"
      exit 1
    end
    $qmake = File.join($qtdir, 'bin/qmake')
    $macdeployqt = File.join($qtdir, 'bin/macdeployqt')
  end

  task :rhosimulator do
    $rhosimulator_build = true
  end

  out = `javac -version 2>&1`
  puts "\n\nYour java bin folder does not appear to be on your path.\nThis is required to use rhodes.\n\n" unless $? == 0
end

def copy_assets(asset)
  
  dest = File.join($srcdir,'apps/public')
  
  cp_r asset + "/.", dest, :preserve => true, :remove_destination => true 
  
end

def clear_linker_settings
  if $config["platform"] == "iphone"
#    outfile = ""
#    IO.read($startdir + "/platform/iphone/rhorunner.xcodeproj/project.pbxproj").each_line do |line|
#      if line =~ /EXTENSIONS_LDFLAGS = /
#        outfile << line.gsub(/EXTENSIONS_LDFLAGS = ".*"/, 'EXTENSIONS_LDFLAGS = ""')
#      else
#        outfile << line
#      end
#    end
#    File.open($startdir + "/platform/iphone/rhorunner.xcodeproj/project.pbxproj","w") {|f| f.write outfile}
#    ENV["EXTENSIONS_LDFLAGS"] = ""

    $ldflags = ""
  end

end

def add_linker_library(libraryname)
#  if $config["platform"] == "iphone"
#    outfile = ""
#    IO.read($startdir + "/platform/iphone/rhorunner.xcodeproj/project.pbxproj").each_line do |line|
#      if line =~ /EXTENSIONS_LDFLAGS = /
#        outfile << line.gsub(/";/, " $(TARGET_TEMP_DIR)/#{libraryname}\";")
#      else
#        outfile << line
#      end
#    end
#    File.open($startdir + "/platform/iphone/rhorunner.xcodeproj/project.pbxproj","w") {|f| f.write outfile}
#  end
      simulator = $sdk =~ /iphonesimulator/

      if ENV["TARGET_TEMP_DIR"] and ENV["TARGET_TEMP_DIR"] != ""
        tmpdir = ENV["TARGET_TEMP_DIR"]
      else
        tmpdir = $startdir + "/platform/iphone/build/rhorunner.build/#{$configuration}-" +
          ( simulator ? "iphonesimulator" : "iphoneos") + "/rhorunner.build"
      end
  $ldflags << "#{tmpdir}/#{libraryname}\n" unless $ldflags.nil?
end

def set_linker_flags
  if $config["platform"] == "iphone"
      simulator = $sdk =~ /iphonesimulator/
      if ENV["TARGET_TEMP_DIR"] and ENV["TARGET_TEMP_DIR"] != ""
        tmpdir = ENV["TARGET_TEMP_DIR"]
      else
        tmpdir = $startdir + "/platform/iphone/build/rhorunner.build/#{$configuration}-" +
          ( simulator ? "iphonesimulator" : "iphoneos") + "/rhorunner.build"
      end
      mkdir_p tmpdir unless File.exist? tmpdir
      File.open(tmpdir + "/rhodeslibs.txt","w") { |f| f.write $ldflags }
#    ENV["EXTENSIONS_LDFLAGS"] = $ldflags
#    puts `export $EXTENSIONS_LDFLAGS`
  end

end

def add_extension(path,dest)
  puts 'add_extension - ' + path.to_s + " - " + dest.to_s
  
  start = pwd
  chdir path if File.directory?(path)
  puts 'chdir path=' + path.to_s
   
  Dir.glob("*").each do |f|
    cp_r f,dest unless f =~ /^ext(\/|(\.yml)?$)/ || f =~ /^app/  || f =~ /^public/
  end  

  if $current_platform == "bb"
    FileUtils.cp_r 'app', File.join( dest, "apps/app" ) if File.exist? 'app'
    FileUtils.cp_r 'public', File.join( dest, "apps/public" ) if File.exist? 'public'
  else
    FileUtils.cp_r('app', File.join( File.dirname(dest), "apps/app" ).to_s) if File.exist? 'app'
    FileUtils.cp_r('public', File.join( File.dirname(dest), "apps").to_s) if File.exist? 'public'
  end
  
  chdir start
end

def find_ext_ingems(extname)
  extpath = nil
  begin
    $rhodes_extensions = nil
	  $rhodes_join_ext_name = false
	
    require extname
    if $rhodes_extensions
        extpath = $rhodes_extensions[0]
        $app_config["extpaths"] << extpath
		if $rhodes_join_ext_name
			extpath = File.join(extpath,extname)
		end
    end    
  rescue Exception => e      
    puts "exception: #{e}"  
  end
  
  extpath
end
def write_modules_js(filename, modules)
    f = StringIO.new("", "w+")
    f.puts "// WARNING! THIS FILE IS GENERATED AUTOMATICALLY! DO NOT EDIT IT MANUALLY!"

    if modules
        modules.each do |m|
            modulename = m.gsub(/^(|.*[\\\/])([^\\\/]+)\.js$/, '\2')
            f.puts( "// Module #{modulename}\n\n" )
            f.write(File.read(m))
        end
    end
    
    Jake.modify_file_if_content_changed(filename, f)
end

def is_ext_supported(extpath)
    extyml = File.join(extpath, "ext.yml")    
    res = true
    if File.file? extyml
      extconf = Jake.config(File.open(extyml))
      if extconf["platforms"]
        res = extconf["platforms"].index($current_platform) != nil
      end  
    end
    res    
end

def init_extensions(startdir, dest)
  extentries = []
  nativelib = []
  extlibs = []
  extjsmodulefiles = []
  startJSModules = []
  endJSModules = []
  extcsharplibs = []
  extcsharpentries = []
  extscsharp = nil

  extpaths = $app_config["extpaths"]  

  rhoapi_js_folder = nil
  unless dest.nil?
    rhoapi_js_folder = File.join( File.dirname(dest), "apps/public/api" )
    puts "rhoapi_js_folder: #{rhoapi_js_folder}"
  end

  puts 'init extensions'

  # TODO: checker init
  gen_checker = GeneratorTimeChecker.new        
  gen_checker.init($startdir, $app_path)

  $app_config["extensions"].each do |extname|  
    puts 'ext - ' + extname
    
    extpath = nil
    extpaths.each do |p|    
      ep = File.join(p, extname)
      if File.exists?( ep ) && is_ext_supported(ep)
        extpath = ep
        break
      end
    end    

    if extpath.nil?        
        extpath = find_ext_ingems(extname) 
        if extpath
          extpath = nil unless is_ext_supported(extpath)
        end    
    end
      
    if (extpath.nil?) && (extname != 'rhoelements-license') && (extname != 'motoapi')
      raise "Can't find extension '#{extname}'. Aborting build.\nExtensions search paths are:\n#{extpaths}"
    end

    $app_extensions_list[extname] = extpath
   
    unless extpath.nil?      
      puts 'iter=' + extpath.to_s    

      if $config["platform"] != "bb"
        extyml = File.join(extpath, "ext.yml")
        puts "extyml " + extyml 
        
        if File.file? extyml
          extconf = Jake.config(File.open(extyml))
          entry = extconf["entry"]
          nlib = extconf["nativelibs"]
          type = extconf["exttype"]
          wm_type = extconf["wm"]["exttype"] if extconf["wm"]
          xml_api_paths = extconf["xml_api_paths"]
          csharp_impl = $config["platform"] == "wp8" && (!extconf['wp8'].nil?) && (!extconf['wp8']['csharp_impl'].nil?) ? true : false
        
          if nlib != nil
            nlib.each do |libname|
              nativelib << libname
            end
          end
          
          # ? if csharp_impl ... extcsharpentries ...
          extentries << entry unless entry.nil?

          if type.to_s() != "nativelib"
            libs = extconf["libraries"]
            libs = [] unless libs.is_a? Array
            if (!extconf[$config["platform"]].nil?) && (extconf[$config["platform"]]["libraries"].is_a? Array)
              libs = libs + extconf[$config["platform"]]["libraries"]
            end
            if $config["platform"] == "wm" || $config["platform"] == "win32" || $config["platform"] == "wp8"
              libs.each do |lib|
                extlibs << lib + (csharp_impl ? "Lib" : "") + ".lib"
                extlibs << lib + "Runtime.lib" if csharp_impl
                extcsharplibs << lib + (csharp_impl ? "Lib" : "") + ".lib" if csharp_impl
                extcsharplibs << lib + "Runtime.lib" if csharp_impl
              end
            else
              libs.map! { |lib| "lib" + lib + ".a" }
              extlibs += libs
            end
          end

          if xml_api_paths && type != "prebuilt" && wm_type != "prebuilt"
            xml_api_paths    = xml_api_paths.split(',')
                                                          
            xml_api_paths.each do |xml_api|
              xml_path = File.join(extpath, xml_api.strip())                
              
              #api generator
              if gen_checker.check(xml_path)      
                puts 'start running rhogen with api key'
                Jake.run3("#{$startdir}/bin/rhogen api #{xml_path}")
              end
            end
            
          end
        end 
        
        unless rhoapi_js_folder.nil?
          Dir.glob(extpath + "/public/api/*.js").each do |f|
              if f.downcase().end_with?("rhoapi.js")
                startJSModules << f
              elsif f.downcase().end_with?("rho.database.js")
                endJSModules << f
              else
                extjsmodulefiles << f
              end  
          end
          Dir.glob(extpath + "/public/api/generated/*.js").each do |f|
              extjsmodulefiles << f
          end
              
        end
        
      end
      
      add_extension(extpath, dest) unless dest.nil?  
    end    
  end

  #TODO: checker update
  gen_checker.update
  
  exts = File.join($startdir, "platform", "shared", "ruby", "ext", "rho", "extensions.c")
  if $config["platform"] == "wp8"
    extscsharp = File.join($startdir, "platform", "wp8", "rhodes", "CSharpExtensions.cs")
    extscsharpcpp = File.join($startdir, "platform", "wp8", "rhoruntime", "CSharpExtensions.cpp")
  end
  puts "exts " + exts

  extjsmodulefiles = startJSModules.concat( extjsmodulefiles )
  extjsmodulefiles = extjsmodulefiles.concat(endJSModules)

  # deploy Common API JS implementation
  if extjsmodulefiles.count > 0
    puts 'extjsmodulefiles=' + extjsmodulefiles.to_s
  
    rm_rf rhoapi_js_folder if Dir.exist?(rhoapi_js_folder)
    mkdir_p rhoapi_js_folder
    write_modules_js(File.join(rhoapi_js_folder, "rhoapi-modules.js"), extjsmodulefiles)
  end

  if $config["platform"] != "bb"
    f = StringIO.new("", "w+")
    f.puts "// WARNING! THIS FILE IS GENERATED AUTOMATICALLY! DO NOT EDIT IT MANUALLY!"

    if !extscsharp.nil?
      f_csharp = StringIO.new("", "w+")
      f_csharp.puts "// WARNING! THIS FILE IS GENERATED AUTOMATICALLY! DO NOT EDIT IT MANUALLY!"
      f_csharp_cpp = StringIO.new("", "w+")
      f_csharp_cpp.puts "// WARNING! THIS FILE IS GENERATED AUTOMATICALLY! DO NOT EDIT IT MANUALLY!"
    end

    if $config["platform"] == "wm" || $config["platform"] == "win32" || $config["platform"] == "wp8"
      # Add libraries through pragma
      extlibs.each do |lib|
        f.puts "#pragma comment(lib, \"#{lib}\")"
      end

      nativelib.each do |lib|
        f.puts "#pragma comment(lib, \"#{lib}\")"
      end
    end

    extentries.each do |entry|
      f.puts "extern void #{entry}(void);"
    end

    f.puts "void Init_Extensions(void) {"
    extentries.each do |entry|
      f.puts "    #{entry}();"
    end
    f.puts "}"

    Jake.modify_file_if_content_changed( exts, f )

    if !extscsharp.nil?
      # C# extensions initialization
      f_csharp.puts "namespace rhodes {"
      f_csharp.puts "    public static class CSharpExtensions {"
      f_csharp.puts "        public static void InitializeExtenstions() {"
      extcsharpentries.each do |entry|
        f_csharp.puts "            #{entry}();"
      end
      f_csharp.puts "        }"
      f_csharp.puts "    }"
      f_csharp.puts "}"
      Jake.modify_file_if_content_changed( extscsharp, f_csharp )
      # C++ runtime libraries linking
      extcsharplibs.each do |lib|
        f_csharp_cpp.puts "#pragma comment(lib, \"#{lib}\")"
      end
      Jake.modify_file_if_content_changed( extscsharpcpp, f_csharp_cpp )
    end

    extlibs.each { |lib| add_linker_library(lib) }
    nativelib.each { |lib| add_linker_library(lib) }

    set_linker_flags    
  end
  
  unless $app_config["constants"].nil?
    File.open("rhobuild.rb","w") do |file|
      file << "module RhoBuild\n"
      $app_config["constants"].each do |key,value|
        value.gsub!(/"/,"\\\"")
        file << "  #{key.upcase} = \"#{value}\"\n"
      end
      file << "end\n"
    end
  end

  if $excludeextlib and (not dest.nil?)
      chdir dest
      $excludeextlib.each {|e| Dir.glob(e).each {|f| rm f}}
  end
  puts "end of init extension"
  #exit
end

def public_folder_cp_r(src_dir,dst_dir,level,obfuscate)
  mkdir_p dst_dir if not File.exists? dst_dir
  Dir.foreach(src_dir) do |filename|
    next if filename.eql?('.') || filename.eql?('..')
    filepath = src_dir + '/' + filename
    dst_path = dst_dir + '/' + filename
    if File.directory?(filepath)
      public_folder_cp_r(filepath,dst_path,(level+1),((obfuscate==1) && ((level>0) || $obfuscate_exclude.nil? || !$obfuscate_exclude.include?(filename)) ? 1 : 0))
    else
      if (obfuscate==1) && (((!$obfuscate_js.nil?) && File.extname(filename).eql?(".js")) || ((!$obfuscate_css.nil?) && File.extname(filename).eql?(".css")))
        puts Jake.run('java',['-jar', $obfuscator, filepath, '-o', dst_path])
        unless $? == 0
          puts "Obfuscation error"
          exit 1
        end
      else
        cp filepath, dst_path, :preserve => true
      end
    end
  end
end

def copy_rhoconfig(source, target)
  override = get_config_override_params
  mentioned = Set.new

  lines = []

  # read file and edit overriden parameters
  File.open(source, 'r') do |file|
    while line = file.gets
      match = line.match(/^(\s*)(\w+)(\s*=\s*)/)
      if match
        name = match[2]
        if override.has_key?(name)
          lines << "#{match[1]}#{name}#{match[3]}#{override[name]}"
          mentioned << name
          next
        end
      end
      lines << line
    end
  end

  # append rest of overriden parameters to text
  override.each do |key, value|
    if !mentioned.include?(key)
      lines << ''
      lines << "#{key} = #{value}"
    end
  end

  # write text to target file
  File.open(target, 'w') do |file|
    lines.each { |l| file.puts l }
  end
end

def common_bundle_start(startdir, dest)
  puts "common_bundle_start"
  
  app = $app_path
  rhodeslib = "lib/framework"

  rm_rf $srcdir
  mkdir_p $srcdir
  mkdir_p dest if not File.exists? dest
  mkdir_p File.join($srcdir,'apps')

  start = pwd
  chdir rhodeslib

  Dir.glob("*").each { |f|
    if f.to_s == "autocomplete"
      next
    end
          
    cp_r f,dest, :preserve => true 
  }

  chdir dest
  Dir.glob("**/rhodes-framework.rb").each {|f| rm f}
  Dir.glob("**/erb.rb").each {|f| rm f}
  Dir.glob("**/find.rb").each {|f| rm f}
  $excludelib.each {|e| Dir.glob(e).each {|f| rm f}}

  chdir start
  clear_linker_settings

  init_extensions(startdir, dest)

  chdir startdir
  #throw "ME"
  cp_r app + '/app',File.join($srcdir,'apps'), :preserve => true
  if File.exists? app + '/public'
    if $obfuscate_js.nil? && $obfuscate_css.nil?
      cp_r app + '/public', File.join($srcdir,'apps'), :preserve => true 
    else
      public_folder_cp_r app + '/public', File.join($srcdir,'apps/public'), 0, 1
    end
  end
  copy_rhoconfig(File.join(app, 'rhoconfig.txt'), File.join($srcdir, 'apps', 'rhoconfig.txt'))

  if $app_config["app_type"] == 'rhoelements'
    $config_xml = nil
    if $app_config[$config["platform"]] && $app_config[$config["platform"]]["rhoelements"] && $app_config[$config["platform"]]["rhoelements"]["config"] && (File.exists? File.join(app, $app_config[$config["platform"]]["rhoelements"]["config"]))
      $config_xml = File.join(app, $app_config[$config["platform"]]["rhoelements"]["config"])
    elsif $app_config["rhoelements"] && $app_config["rhoelements"]["config"] && (File.exists? File.join(app, $app_config["rhoelements"]["config"]))
      $config_xml = File.join(app, $app_config["rhoelements"]["config"])
    end
    if $current_platform == "wm"
      if !($config_xml.nil?)
        cp $config_xml, File.join($srcdir,'apps/Config.xml'), :preserve => true
      end
    end
  end

  app_version = "\r\napp_version='#{$app_config["version"]}'"  
  app_version += "\r\ntitle_text='#{$app_config["name"]}'"  if $current_platform == "win32"
  
  File.open(File.join($srcdir,'apps/rhoconfig.txt'), "a"){ |f| f.write(app_version) }
  File.open(File.join($srcdir,'apps/rhoconfig.txt.timestamp'), "w"){ |f| f.write(Time.now.to_f().to_s()) }
  
  unless $debug
    rm_rf $srcdir + "/apps/app/SpecRunner"
    rm_rf $srcdir + "/apps/app/spec"
    rm_rf $srcdir + "/apps/app/mspec.rb"
    rm_rf $srcdir + "/apps/app/spec_runner.rb"
  end


  copy_assets($assetfolder) if ($assetfolder and File.exists? $assetfolder)

  replace_platform = $config['platform']
  replace_platform = "bb6" if $bb6
  #replace_platform = "wm" if replace_platform == 'win32'

  [File.join($srcdir,'apps'), ($current_platform == "bb" ? File.join($srcdir,'res') : File.join($srcdir,'lib/res'))].each do |folder|
      chdir folder
      
      Dir.glob("**/*.#{replace_platform}.*").each do |file|
        oldfile = file.gsub(Regexp.new(Regexp.escape('.') + replace_platform + Regexp.escape('.')),'.')
        rm oldfile if File.exists? oldfile
        mv file,oldfile
      end
      
      Dir.glob("**/*.wm.*").each { |f| rm f }
      Dir.glob("**/*.win32.*").each { |f| rm f }
	  Dir.glob("**/*.wp.*").each { |f| rm f }
	  Dir.glob("**/*.wp8.*").each { |f| rm f }
	  Dir.glob("**/*.sym.*").each { |f| rm f }
      Dir.glob("**/*.iphone.*").each { |f| rm f }
      Dir.glob("**/*.bb.*").each { |f| rm f }
      Dir.glob("**/*.bb6.*").each { |f| rm f }
      Dir.glob("**/*.android.*").each { |f| rm f }
      Dir.glob("**/.svn").each { |f| rm_rf f }
      Dir.glob("**/CVS").each { |f| rm_rf f }
  end  
end

def create_manifest
    require File.dirname(__FILE__) + '/lib/framework/rhoappmanifest'
    
    fappManifest = Rho::AppManifest.enumerate_models(File.join($srcdir, 'apps/app'))
    content = fappManifest.read();
    
    File.open( File.join($srcdir,'apps/app_manifest.txt'), "w"){|file| file.write(content)}    
end

def process_exclude_folders(excluded_dirs=[])
  excl = excluded_dirs

  exclude_platform = $config['platform']
  exclude_platform = "bb6" if $bb6
  #exclude_platform = "wm" if exclude_platform == 'win32'

  if $app_config["excludedirs"]
      excl += $app_config["excludedirs"]['all'] if $app_config["excludedirs"]['all']
      excl += $app_config["excludedirs"][exclude_platform] if $app_config["excludedirs"][exclude_platform]
  end
      
  if  $config["excludedirs"]    
      excl += $config["excludedirs"]['all'] if $config["excludedirs"]['all']
      excl += $config["excludedirs"][exclude_platform] if $config["excludedirs"][exclude_platform]
  end  
  
  if excl.size() > 0
      chdir File.join($srcdir)#, 'apps')
  
      excl.each do |mask|
        Dir.glob(mask).each {|f| puts "f: #{f}"; rm_rf f}
      end
      
      chdir File.join($srcdir, 'apps')
  
      excl.each do |mask|
        Dir.glob(mask).each {|f| puts "f: #{f}"; rm_rf f}
      end
      
  end

end

def get_config_override_params
    override = {}
    ENV.each do |key, value|
        key.match(/^rho_override_(.+)$/) do |match|
            override[match[1]] = value
        end
    end
    return override
end

def get_extensions
    value = ENV['rho_extensions']
    return value.split(',') if value
    return []
end

namespace "build" do
  namespace "bundle" do
    task :xruby do
      #needs $config, $srcdir, $excludelib, $bindir
      app = $app_path
  	  jpath = $config["env"]["paths"]["java"]
      startdir = pwd
      dest =  $srcdir
      xruby =  File.dirname(__FILE__) + '/res/build-tools/xruby-0.3.3.jar'
      compileERB = "lib/build/compileERB/bb.rb"
      rhodeslib = File.dirname(__FILE__) + "/lib/framework"
      
      common_bundle_start(startdir,dest)

      process_exclude_folders()
      cp_r File.join(startdir, "platform/shared/db/res/db"), File.join($srcdir, 'apps')
      
      chdir startdir
      
      #create manifest
      create_manifest
      
      #"compile ERB"
      #ext = ".erb"
      #Find.find($srcdir) do |path|
      #  if File.extname(path) == ext
      #    rbText = ERB.new( IO.read(path) ).src
      #    newName = File.basename(path).sub('.erb','_erb.rb')
      #    fName = File.join(File.dirname(path), newName)
      #    frb = File.new(fName, "w")
      #    frb.write( rbText )
      #    frb.close()
      #  end
      #end
      cp   compileERB, $srcdir
      puts "Running bb.rb"

      puts `#{$rubypath} -I"#{rhodeslib}" "#{$srcdir}/bb.rb"`
      unless $? == 0
        puts "Error interpreting erb code"
        exit 1
      end

      rm "#{$srcdir}/bb.rb"

      chdir $bindir
      # -n#{$bundleClassName}
      output = `java -jar "#{xruby}" -v -c RhoBundle 2>&1`
      output.each_line { |x| puts ">>> " + x  }
      unless $? == 0
        puts "Error interpreting ruby code"
        exit 1
      end
      chdir startdir
      chdir $srcdir
  
      Dir.glob("**/*.rb") { |f| rm f }
      Dir.glob("**/*.erb") { |f| rm f }
=begin
      # RubyIDContainer.* files takes half space of jar why we need it?
      Jake.unjar("../RhoBundle.jar", $tmpdir)
      Dir.glob($tmpdir + "/**/RubyIDContainer.class") { |f| rm f }
      rm "#{$bindir}/RhoBundle.jar"
      chdir $tmpdir
      puts `jar cf #{$bindir}/RhoBundle.jar #{$all_files_mask}`      
      rm_rf $tmpdir
      mkdir_p $tmpdir
      chdir $srcdir
=end  

      puts `"#{File.join(jpath,'jar')}" uf ../RhoBundle.jar apps/#{$all_files_mask}`
      unless $? == 0
        puts "Error creating Rhobundle.jar"
        exit 1
      end
      chdir startdir
      
    end

    # its task for compiling ruby code in rhostudio
    # TODO: temporary fix I hope. This code is copied from line 207 of this file
    task :rhostudio => ["config:wm"] do

      if RUBY_PLATFORM =~ /(win|w)32$/
        $all_files_mask = "*.*"
        $rubypath = "res/build-tools/RhoRuby.exe"
      else
        $all_files_mask = "*"
        if RUBY_PLATFORM =~ /darwin/
          $rubypath = "res/build-tools/RubyMac"
        else
          $rubypath = "res/build-tools/rubylinux"
        end
      end

      Rake::Task["build:bundle:noxruby"].invoke

      Jake.build_file_map( File.join($srcdir, "apps"), "rhofilelist.txt" )
    end
    
    task :noxruby, :exclude_dirs do |t, args|
      exclude_dirs = args[:exclude_dirs]
      excluded_dirs = []
      if (!exclude_dirs.nil?) && (exclude_dirs !~ /^\s*$/)
        excluded_dirs = exclude_dirs.split(':')
      end

      app = $app_path
      rhodeslib = File.dirname(__FILE__) + "/lib/framework"
      compileERB = "lib/build/compileERB/default.rb"
      compileRB = "lib/build/compileRB/compileRB.rb"
      startdir = pwd
      dest = $srcdir + "/lib"      

      common_bundle_start(startdir,dest)
      process_exclude_folders(excluded_dirs)
      chdir startdir
      
      create_manifest
      
      cp   compileERB, $srcdir
      puts "Running default.rb"

      puts `#{$rubypath} -I"#{rhodeslib}" "#{$srcdir}/default.rb"`
      unless $? == 0
        puts "Error interpreting erb code"
        exit 1
      end

      rm "#{$srcdir}/default.rb"

      cp   compileRB, $srcdir
      puts "Running compileRB"
      puts `#{$rubypath} -I"#{rhodeslib}" "#{$srcdir}/compileRB.rb"`
      unless $? == 0
        puts "Error interpreting ruby code"
        exit 1
      end

      chdir $srcdir
      Dir.glob("**/*.rb") { |f| rm f }
      Dir.glob("**/*.erb") { |f| rm f }
  
      chdir startdir

      cp_r "platform/shared/db/res/db", $srcdir 
    end
    
    task :upgrade_package do
    
      $bindir = File.join($app_path, "bin") 
      $current_platform = 'empty'
      $srcdir = File.join($bindir, "RhoBundle")
    
      $targetdir = File.join($bindir, "target")
      $excludelib = ['**/builtinME.rb','**/ServeME.rb','**/dateME.rb','**/rationalME.rb']
      $tmpdir = File.join($bindir, "tmp")
      $appname = $app_config["name"]
      $appname = "Rhodes" if $appname.nil?
      $vendor = $app_config["vendor"]
      $vendor = "rhomobile" if $vendor.nil?
      $vendor = $vendor.gsub(/^[^A-Za-z]/, '_').gsub(/[^A-Za-z0-9]/, '_').gsub(/_+/, '_').downcase
      $appincdir = File.join $tmpdir, "include"

      Rake::Task["config:common"].invoke

      Rake::Task["build:bundle:noxruby"].invoke
      
      new_zip_file = File.join($srcdir, "apps", "upgrade_bundle.zip")
      
      if RUBY_PLATFORM =~ /(win|w)32$/
        begin
      
          require 'rubygems'
          require 'zip/zip'
          require 'find'
          require 'fileutils'
          include FileUtils

          root = $srcdir
          
          new_zip_file = File.join($srcdir, "upgrade_bundle.zip")

          Zip::ZipFile.open(new_zip_file, Zip::ZipFile::CREATE)do |zipfile|
            Find.find(root) do |path|
                  Find.prune if File.basename(path)[0] == ?.
                  dest = /apps\/(\w.*)/.match(path)
                  if dest
                      puts '     add file to zip : '+dest[1].to_s
                      zipfile.add(dest[1],path)
                  end
            end 
          end
        rescue Exception => e
          puts 'ERROR !'
          puts 'Require "rubyzip" gem for make zip file !'
          puts 'Install gem by "gem install rubyzip"'
        end        
      else
        chdir File.join($srcdir, "apps")
        sh %{zip -r upgrade_bundle.zip .}
      end

      cp   new_zip_file, $bindir
      
      rm   new_zip_file
      
    end
    

    task :noiseq do
      app = $app_path
      rhodeslib = File.dirname(__FILE__) + "/lib/framework"
	    compileERB = "lib/build/compileERB/bb.rb"
      startdir = pwd
      dest = $srcdir + "/lib"      

      common_bundle_start(startdir,dest)
      process_exclude_folders
      chdir startdir
      
      create_manifest
  
	  cp   compileERB, $srcdir
      puts "Running bb.rb"

      puts `#{$rubypath} -I#{rhodeslib} "#{$srcdir}/bb.rb"`
      unless $? == 0
        puts "Error interpreting erb code"
        exit 1
      end

      rm "#{$srcdir}/bb.rb"

      chdir $srcdir
      Dir.glob("**/*.erb") { |f| rm f }

	  chdir startdir
      cp_r "platform/shared/db/res/db", $srcdir 
    end
  end
end


# Simple rakefile that loads subdirectory 'rhodes' Rakefile
# run "rake -T" to see list of available tasks

#desc "Get versions"
task :get_version do

  #genver = "unknown"
  iphonever = "unknown"
  #symver = "unknown"
  wmver = "unknown"
  androidver = "unknown"
  

  # File.open("res/generators/templates/application/build.yml","r") do |f|
  #     file = f.read
  #     if file.match(/version: (\d+\.\d+\.\d+)/)
  #       genver = $1
  #     end
  #   end

  File.open("platform/iphone/Info.plist","r") do |f|
    file = f.read
    if file.match(/CFBundleVersion<\/key>\s+<string>(\d+\.\d+\.*\d*)<\/string>/)
      iphonever =  $1
    end
  end

  # File.open("platform/symbian/build/release.properties","r") do |f|
  #     file = f.read
  #     major = ""
  #     minor = ""
  #     build = ""
  # 
  #     if file.match(/release\.major=(\d+)/)
  #       major =  $1
  #     end
  #     if file.match(/release\.minor=(\d+)/)
  #       minor =  $1
  #     end
  #     if file.match(/build\.number=(\d+)/)
  #       build =  $1
  #     end
  # 
  #     symver = major + "." + minor + "." + build
  #   end

  File.open("platform/android/Rhodes/AndroidManifest.xml","r") do |f|
    file = f.read
    if file.match(/versionName="(\d+\.\d+\.*\d*)"/)
      androidver =  $1
    end
  end

  gemver = "unknown"
  rhodesver = "unknown"
  frameworkver = "unknown"

  File.open("lib/rhodes.rb","r") do |f|
    file = f.read
    if file.match(/VERSION = '(\d+\.\d+\.*\d*)'/)
      gemver =  $1
    end
  end

  File.open("lib/framework/rhodes.rb","r") do |f|
    file = f.read
    if file.match(/VERSION = '(\d+\.\d+\.*\d*)'/)
      rhodesver =  $1
    end
  end

  File.open("lib/framework/version.rb","r") do |f|
    file = f.read
    if file.match(/VERSION = '(\d+\.\d+\.*\d*)'/)
      frameworkver =  $1
    end
  end

  

  puts "Versions:"
  #puts "  Generator:        " + genver
  puts "  iPhone:           " + iphonever
  #puts "  Symbian:          " + symver
  #puts "  WinMo:            " + wmver
  puts "  Android:          " + androidver
  puts "  Gem:              " + gemver
  puts "  Rhodes:           " + rhodesver
  puts "  Framework:        " + frameworkver
end

#desc "Set version"
task :set_version, [:version] do |t,args|
  throw "You must pass in version" if args.version.nil?
  ver = args.version.split(/\./)
  major = ver[0]
  minor = ver[1]
  build = ver[2]
  
  throw "Invalid version format. Must be in the format of: major.minor.build" if major.nil? or minor.nil? or build.nil?

  verstring = major+"."+minor+"."+build
  origfile = ""

  # File.open("res/generators/templates/application/build.yml","r") { |f| origfile = f.read }
  #   File.open("res/generators/templates/application/build.yml","w") do |f|
  #     f.write origfile.gsub(/version: (\d+\.\d+\.\d+)/, "version: #{verstring}")
  #   end
  

  File.open("platform/iphone/Info.plist","r") { |f| origfile = f.read }
  File.open("platform/iphone/Info.plist","w") do |f| 
    f.write origfile.gsub(/CFBundleVersion<\/key>(\s+)<string>(\d+\.\d+\.*\d*)<\/string>/, "CFBundleVersion</key>\n\t<string>#{verstring}</string>")
  end

  # File.open("platform/symbian/build/release.properties","r") { |f| origfile = f.read }
  # File.open("platform/symbian/build/release.properties","w") do |f|
  #   origfile.gsub!(/release\.major=(\d+)/,"release.major=#{major}")
  #   origfile.gsub!(/release\.minor=(\d+)/,"release.minor=#{minor}")
  #   origfile.gsub!(/build\.number=(\d+)/,"build.number=#{build}")
  #   f.write origfile
  # end

  File.open("platform/android/Rhodes/AndroidManifest.xml","r") { |f| origfile = f.read }
  File.open("platform/android/Rhodes/AndroidManifest.xml","w") do |f|
    origfile.match(/versionCode="(\d+)"/)
    vercode = $1.to_i + 1
    origfile.gsub!(/versionCode="(\d+)"/,"versionCode=\"#{vercode}\"")
    origfile.gsub!(/versionName="(\d+\.\d+\.*\d*)"/,"versionName=\"#{verstring}\"")

    f.write origfile
  end
  ["lib/rhodes.rb","lib/framework/rhodes.rb","lib/framework/version.rb"].each do |versionfile|
  
    File.open(versionfile,"r") { |f| origfile = f.read }
    File.open(versionfile,"w") do |f|
      origfile.gsub!(/^(\s*VERSION) = '(\d+\.\d+\.*\d*)'/, '\1 = \''+ verstring + "'")     
      f.write origfile
    end
  end

  Rake::Task[:get_version].invoke  
end



namespace "buildall" do
  namespace "bb" do
    #    desc "Build all jdk versions for blackberry"
    task :production => "config:common" do
      $config["env"]["paths"].each do |k,v|
        if k.to_s =~ /^4/
          puts "BUILDING VERSION: #{k}"
          $app_config["bbver"] = k
#          Jake.reconfig($config)
 
          #reset all tasks used for building
          Rake::Task["config:bb"].reenable
          Rake::Task["build:bb:rhobundle"].reenable
          Rake::Task["build:bb:rhodes"].reenable
          Rake::Task["build:bb:rubyvm"].reenable
          Rake::Task["device:bb:dev"].reenable
          Rake::Task["device:bb:production"].reenable
          Rake::Task["device:bb:rhobundle"].reenable
          Rake::Task["package:bb:dev"].reenable
          Rake::Task["package:bb:production"].reenable
          Rake::Task["package:bb:rhobundle"].reenable
          Rake::Task["package:bb:rhodes"].reenable
          Rake::Task["package:bb:rubyvm"].reenable
          Rake::Task["device:bb:production"].reenable
          Rake::Task["clean:bb:preverified"].reenable

          Rake::Task["clean:bb:preverified"].invoke
          Rake::Task["device:bb:production"].invoke
        end
      end

    end
  end
end


task :gem do
  puts "Removing old gem"
  rm_rf Dir.glob("rhodes*.gem")
  puts "Copying Rakefile"
  cp "Rakefile", "rakefile.rb"
  
  puts "Building manifest"
  out = ""
  Dir.glob("**/*") do |fname| 
    # TODO: create exclusion list
    next unless File.file? fname
    next if fname =~ /rhoconnect-client/
    next if fname =~ /^spec\/api_generator_spec/

    out << fname + "\n"
  end
  File.open("Manifest.txt",'w') {|f| f.write(out)}

  puts "Loading gemspec"
  require 'rubygems'
  spec = Gem::Specification.load('rhodes.gemspec')

  puts "Building gem"
  gemfile = Gem::Builder.new(spec).build
end

namespace "rhomobile-debug" do
    task :gem do
      puts "Removing old gem"
      rm_rf Dir.glob("rhomobile-debug*.gem")
      rm_rf "rhomobile-debug"
      
      mkdir_p "rhomobile-debug"
      mkdir_p "rhomobile-debug/lib"
      cp 'lib/extensions/debugger/debugger.rb', "rhomobile-debug/lib", :preserve => true
      cp 'lib/extensions/debugger/README.md', "rhomobile-debug", :preserve => true
      cp 'lib/extensions/debugger/LICENSE', "rhomobile-debug", :preserve => true
      cp 'lib/extensions/debugger/CHANGELOG', "rhomobile-debug", :preserve => true
      
      cp 'rhomobile-debug.gemspec', "rhomobile-debug", :preserve => true
      
      startdir = pwd
      chdir 'rhomobile-debug'
      
      puts "Loading gemspec"
      require 'rubygems'
      spec = Gem::Specification.load('rhomobile-debug.gemspec')

      puts "Building gem"
      gemfile = Gem::Builder.new(spec).build
      
      Dir.glob("rhomobile-debug*.gem").each do |f|
        cp f, startdir, :preserve => true      
      end

      chdir startdir
      rm_rf "rhomobile-debug"
    end
end

task :tasks do
  Rake::Task.tasks.each {|t| puts t.to_s.ljust(27) + "# " + t.comment.to_s}
end

task :switch_app => "config:common" do
  rhobuildyml = File.dirname(__FILE__) + "/rhobuild.yml"
  if File.exists? rhobuildyml
    config = YAML::load_file(rhobuildyml)
  else
    puts "Cant find rhobuild.yml"
    exit 1
  end
  config["env"]["app"] = $app_path.gsub(/\\/,"/")
  File.open(  rhobuildyml, 'w' ) do |out|
    YAML.dump( config, out )
  end
end


#Rake::RDocTask.new do |rd|
#RDoc::Task.new do |rd|
#  rd.main = "README.textile"
#  rd.rdoc_files.include("README.textile", "lib/framework/**/*.rb")
#end
#Rake::Task["rdoc"].comment=nil
#Rake::Task["rerdoc"].comment=nil

#task :rdocpush => :rdoc do
#  puts "Pushing RDOC. This may take a while"
#  `scp -r html/* dev@dev.rhomobile.com:dev.rhomobile.com/rhodes/`
#end

namespace "build" do
    #    desc "Build rhoconnect-client package"
    task :rhoconnect_client do

        ver = File.read("rhoconnect-client/version").chomp #.gsub(".", "_")
        zip_name = "rhoconnect-client-"+ver+".zip"

        bin_dir = "rhoconnect-client-bin"
        src_dir = bin_dir + "/rhoconnect-client-"+ver #"/src"        
        shared_dir = src_dir + "/platform/shared"        
        rm_rf bin_dir
        rm    zip_name if File.exists? zip_name
        mkdir_p bin_dir
        mkdir_p src_dir

        cp_r 'rhoconnect-client', src_dir, :preserve => true
        
        mv src_dir+"/rhoconnect-client/license", src_dir
        mv src_dir+"/rhoconnect-client/README.textile", src_dir
        mv src_dir+"/rhoconnect-client/version", src_dir
        mv src_dir+"/rhoconnect-client/changelog", src_dir
                
        Dir.glob(src_dir+"/rhoconnect-client/**/*").each do |f|
		    #puts f
		    
            rm_rf f if f.index("/build/") || f.index(".DS_Store")         
 
        end
		
        mkdir_p shared_dir
        
        Dir.glob("platform/shared/*").each do |f|
            next if f == "platform/shared/ruby" || f == "platform/shared/rubyext" || f == "platform/shared/xruby" || f == "platform/shared/shttpd" ||
                f == "platform/shared/stlport"  || f == "platform/shared/qt"
            #puts f                
            cp_r f, shared_dir #, :preserve => true                        
        end
        startdir = pwd
        chdir bin_dir
        puts `zip -r #{File.join(startdir, zip_name)} *`
                
        chdir startdir
        
        rm_rf bin_dir
    end
end

namespace "run" do

    task :set_rhosimulator_flag do
        $is_rho_simulator = true    
    end

    task :rhosimulator_base => [:set_rhosimulator_flag, "config:common"] do
        puts "rho_reload_app_changes : #{ENV['rho_reload_app_changes']}"
        $path = ""
        $args = ["-approot='#{$app_path}'", "-rhodespath='#{$startdir}'"]
        $args << "-security_token=#{ENV['security_token']}" if !ENV['security_token'].nil?
        cmd = nil

        if RUBY_PLATFORM =~ /(win|w)32$/
            if $config['env']['paths']['rhosimulator'] and $config['env']['paths']['rhosimulator'].length() > 0
                $path = File.join( $config['env']['paths']['rhosimulator'], "rhosimulator.exe" )
            else
                $path = File.join( $startdir, "platform/win32/RhoSimulator/rhosimulator.exe" )
            end
        elsif RUBY_PLATFORM =~ /darwin/
            if $config['env']['paths']['rhosimulator'] and $config['env']['paths']['rhosimulator'].length() > 0
                $path = File.join( $config['env']['paths']['rhosimulator'], "RhoSimulator.app" )
            else
                $path = File.join( $startdir, "platform/osx/bin/RhoSimulator/RhoSimulator.app" )
            end
            cmd = 'open'
            $args.unshift($path, '--args')
        else
            $args << ">/dev/null"
            $args << "2>/dev/null"
        end

        $appname = $app_config["name"].nil? ? "Rhodes" : $app_config["name"]
        if !File.exists?($path)
            puts "Cannot find RhoSimulator: '#{$path}' does not exists"
            puts "Check sdk path in build.yml - it should point to latest rhodes (run 'set-rhodes-sdk' in application folder) OR"

            if $config['env']['paths']['rhosimulator'] and $config['env']['paths']['rhosimulator'].length() > 0
                puts "Check 'env:paths:rhosimulator' path in '<rhodes>/rhobuild.yml' OR"
            end
            
            puts "Install Rhodes gem OR"
            puts "Install RhoSimulator and modify 'env:paths:rhosimulator' section in '<rhodes>/rhobuild.yml'"
            exit 1
        end

        sim_conf = "rhodes_path='#{$startdir}'\r\n"
        sim_conf += "app_version='#{$app_config["version"]}'\r\n"
        sim_conf += "app_name='#{$appname}'\r\n"
        if ( ENV['rho_reload_app_changes'] )
            sim_conf += "reload_app_changes=#{ENV['rho_reload_app_changes']}\r\n"        
        else
            sim_conf += "reload_app_changes=1\r\n"                    
        end

        if $config['debug']
            sim_conf += "debug_port=#{$config['debug']['port']}\r\n"
        else
            sim_conf += "debug_port=\r\n"
        end    
        
        if $config['debug'] && $config['debug']['host'] && $config['debug']['host'].length() > 0
            sim_conf += "debug_host='#{$config['debug']['host']}'\r\n"        
        else    
            sim_conf += "debug_host='127.0.0.1'\r\n"
        end
        
        sim_conf += $rhosim_config if $rhosim_config

        #check gem extensions
        config_ext_paths = ""
        extpaths = $app_config["extpaths"]
        extjsmodulefiles = []
        startJSModules = []
        endJSModules = []
        
        rhoapi_js_folder = File.join( $app_path, "public/api" )
        puts "rhoapi_js_folder: #{rhoapi_js_folder}"
        
        # TODO: checker init
        gen_checker = GeneratorTimeChecker.new        
        gen_checker.init($startdir, $app_path)
        
        $app_config["extensions"].each do |extname|
        
            extpath = nil
            extpaths.each do |p|
              ep = File.join(p, extname)
              if File.exists? ep
                extpath = ep
                break
              end
            end

            if extpath.nil?
                begin
                    $rhodes_extensions = nil
                    $rhodes_join_ext_name = false
                    require extname
                    
                    if $rhodes_extensions
                        extpath = $rhodes_extensions[0]
          			    if $rhodes_join_ext_name
        				    extpath = File.join(extpath,extname)
		        	    end
                    end
                    
                    config_ext_paths += "#{extpath};" if extpath && extpath.length() > 0 
                rescue Exception => e
                end
            else
            
                # if $config["platform"] != "bb"
                #     extyml = File.join(extpath, "ext.yml")
                #     next if File.file? extyml
                # end
                
                config_ext_paths += "#{extpath};" if extpath && extpath.length() > 0                     
            end    

            if extpath && extpath.length() > 0
              extyml = File.join(extpath, "ext.yml")
              puts "extyml " + extyml 
        
              if File.file? extyml
                extconf = Jake.config(File.open(extyml))
                xml_api_paths  = extconf["xml_api_paths"]
                templates_path = File.join($startdir, "res", "generators", "templates")
                                
                if xml_api_paths
                  xml_api_paths = xml_api_paths.split(',')

                  xml_api_paths.each do |xml_api|
                    xml_path = File.join(extpath, xml_api.strip())

                    #TODO checker check
                    if gen_checker.check(xml_path)
#                      puts 'ruuuuuuuun generatooooooooooooor'
                      cmd_line = "#{$startdir}/bin/rhogen api #{xml_path}"
                      puts "cmd_line: #{cmd_line}"
                      system "#{cmd_line}"
                    end
                  end
                end                
              end

              # TODO: RhoSimulator should look for 'public' at all extension folders!
                unless rhoapi_js_folder.nil?
                  Dir.glob(extpath + "/public/api/*.js").each do |f|
                      #puts "f: #{f}"
                      if f.downcase().end_with?("rhoapi.js")
                        startJSModules << f
                      elsif f.downcase().end_with?("rho.database.js")
                        endJSModules << f
                      else
                        extjsmodulefiles << f
                      end  
                  end
                  Dir.glob(extpath + "/public/api/generated/*.js").each do |f|
                      extjsmodulefiles << f
                  end

                end
              
            end
        end

        #TODO: checker update
        gen_checker.update
        
        # deploy Common API JS implementation
        extjsmodulefiles = startJSModules.concat( extjsmodulefiles )
        extjsmodulefiles = extjsmodulefiles.concat(endJSModules)
        if extjsmodulefiles.count > 0
          mkdir_p rhoapi_js_folder
          
          puts "extjsmodulefiles: #{extjsmodulefiles}"
          write_modules_js(File.join(rhoapi_js_folder, "rhoapi-modules.js"), extjsmodulefiles)
        end

        sim_conf += "ext_path=#{config_ext_paths}\r\n" if config_ext_paths && config_ext_paths.length() > 0 

        security_token = $app_config["security_token"]
        sim_conf += "security_token=#{security_token}\r\n" if !security_token.nil?
        
        fdir = File.join($app_path, 'rhosimulator')
        mkdir fdir unless File.exist?(fdir)
            
        get_config_override_params.each do |key, value|
            if key != 'start_path'
                puts "Override '#{key}' is not supported."
                next
            end
            sim_conf += "#{key}=#{value}\r\n"
        end

        fname = File.join(fdir, 'rhosimconfig.txt')
        File.open(fname, "wb") do |fconf|
            fconf.write( sim_conf )
        end

        if not cmd.nil?
            $path = cmd
        end
    end

    #desc "Run application on RhoSimulator"
    task :rhosimulator => "run:rhosimulator_base" do
        puts 'start rhosimulator'
        Jake.run2 $path, $args, {:nowait => true}
    end

    task :rhosimulator_debug => "run:rhosimulator_base" do
        puts 'start rhosimulator debug'
        Jake.run2 $path, $args, {:nowait => true}

        if RUBY_PLATFORM =~ /darwin/
          while 1
            sleep 1
          end     
        end
    end

end

namespace "build" do
    task :rhosimulator do
        if RUBY_PLATFORM =~ /(win|w)32$/
            Rake::Task["build:win32:rhosimulator"].invoke
        elsif RUBY_PLATFORM =~ /darwin/
            Rake::Task["build:osx:rhosimulator"].invoke
        else
            puts "Sorry, at this time RhoSimulator can be built for Windows and Mac OS X only"
            exit 1
        end
    end

    task :rhosimulator_version do
        $rhodes_version = File.read(File.join($startdir,'version')).chomp
        File.open(File.join($startdir, 'platform/shared/qt/rhodes/RhoSimulatorVersion.h'), "wb") do |fversion|
            fversion.write( "#define RHOSIMULATOR_VERSION \"#{$rhodes_version}\"\n" )
        end
    end
end

namespace :run do
  desc "start rholog(webrick) server"
  task :webrickrhologserver, :app_path  do |t, args|
    puts "Args were: #{args}"
    $app_path = args[:app_path]

    Rake::Task["config:wm"].invoke

    $rhologserver = WEBrick::HTTPServer.new :Port => $rhologhostport

    puts "LOCAL SERVER STARTED ON #{$rhologhostaddr}:#{$rhologhostport}"
    started = File.open($app_path + "/started", "w+")
    started.close

    #write host and port 4 log server     
    $rhologfile = File.open(getLogPath, "w+")

    $rhologserver.mount_proc '/' do |req,res|
        if ( req.body == "RHOLOG_GET_APP_NAME" )
            res.status = 200
            res.chunked = true
            res.body = $app_path
        elsif ( req.body == "RHOLOG_CLOSE" )
            res.status = 200
            res.chunked = true
            res.body = ""

            $rhologserver.shutdown
        else
            $rhologfile.puts req.body
            $rhologfile.flush
            res.status = 200
            res.chunked = true
            res.body = ""
        end    
    end

    ['INT', 'TERM'].each {|signal| 
        trap(signal) {$rhologserver.shutdown}
    }

    $rhologserver.start
    $rhologfile.close

  end
end

at_exit do
  if $app_config && !$app_config["sdk"].nil? 
    puts '********* NOTE: You use sdk parameter in build.yml !****************'
    puts 'To use latest Rhodes gem, run migrate-rhodes-app in application folder or comment sdk in build.yml.'
    puts '************************************************************************'
  end
  
  if (!$rhoelements_features.nil?) && ($rhoelements_features.length() > 0)
    puts '********* WARNING ************************************************************************'
    puts ' The following features are only available in RhoElements v2 and above:'
    puts $rhoelements_features
    puts ' For more information go to http://www.motorolasolutions.com/rhoelements '
    puts '**************************************************************************************'
  end
  
  if $invalid_license
    puts '********* WARNING ************************************************************************'
    puts ' License is required to run RhoElements application.'
    puts ' Please, provide  "motorola_license" and "motorola_license_company" parameters in build.yml.'
    puts ' For more information go to http://www.motorolasolutions.com/rhoelements '    
    puts '**************************************************************************************'
  end
  
end
