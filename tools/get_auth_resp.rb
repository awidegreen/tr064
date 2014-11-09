#!/usr/bin/env ruby
# example from tr064 first steps from AVM

require 'digest'

uid   = "foo"
pwd   = "bar"
realm = "L!bTr064 SOAP-Auth"
nonce = "38CDA2685E285E51"
 
md5_secret   = Digest::MD5.new
md5_secret <<  uid + ":" + realm + ":" + pwd 

md5_response = Digest::MD5.new
md5_response << md5_secret.hexdigest + ":" + nonce 

puts md5_response.hexdigest
