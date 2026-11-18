<?php
/*
 **************************************************************************
 *
 *
 *  (c) 2012 Copyright Spectrum Bridge, Inc.
 *  All rights reserved.
 *
 *  This software contains confidential and proprietary information.
 *  The content or any derivative work may be used only as authorized
 *  by a licensing agreement from Spectrum Bridge, Inc.  All copies must
 *  include this header.
 *
 *
 **************************************************************************
*/

/*      processor.php - $Revision: 92 $ $Date: 2012-10-23 14:51:17 -0400 (Tue, 23 Oct 2012) $ $Author: mario $
 */

  /****************************************************************
  *
  *    This file has all the functions used by the interface
  *
  ****************************************************************/

    // GLOBAL VARIABLES
    $config_dhcp = "DHCP=";
    $config_ip = "IPADDR=";
    $config_subnet = "SUBNET=";
    $config_gateway = "GATEWAY=";


  // FUNCTION TO RETURN FIELD NAME OR VALUE FROM FILE
  function getFieldOrValueFromFile($line, $field) {
    $chars = explode("' '", $line);        // split the line in 2
    # $chars = preg_split("/' '/", $line);        // split the line in 2
    $return_value;
    if ($field == true) {
      $return_value = substr($chars[0], strpos($chars[0], "'")+1);
    } else {
      $return_value = substr($chars[1], 0, -2);
    }
    return $return_value;
  }

  // FUNCTION TO RETURN FCCID OR SERIAL NUMBER
  function getFccidSerialInfo($line, $field) {
    $chars = explode(":", $line);	// split the line into 2 by ':'
    $return_value;
    if ($field == true) {
      $return_value = $chars[0];
    } else {
      $return_value = $chars[1];
    }
    return $return_value;
  }

  // FUNCTION TO BUILD THE CONFIG PARAMETER 
  function buildLine($field, $value) { 
    return "    option '".$field."' '".$value."'\n";
  }

  // FUNCTION TO GET VALUE FROM FILE
  function getValueFromFile($option, $file) {
    $file_handle = fopen($file, 'r') or die("File ".$file." cannot be accessed");
    while(!feof($file_handle)) {
      $temp = fgets($file_handle);
      if (strpos($temp, $option)) {
        # $chars = preg_split("/' '/", $temp);
        $chars = explode("' '", $temp);
        $return_value = substr($chars[1], 0, -2);
      }
    }
    fclose($file_handle);
    return $return_value;
  }

  // FUNCTION TO RETURN STATUS CODE
  function getStatusCodeDescription($code) {
    $codes = array("Success", "Malformed request", "FCC identifier not recognized or supported", "Reserved", "Device not registered", "FCC designated no channel available", "Country code not recognized", "Device not enrolled", "Country code does not match enrolled country code", "Location outside of regulatory domain", "Antenna height above 30 meters", "HAAT above 250 meters", "FCC ID not in FCC provided list", "Invalid device type", "Channel list data mismatch from registration data", "The device type is not allowed for the FCC authorized equipment class");
    return $codes[$code];
  }

  // FUNCTION TO CLEAR CONTENTS OF sbi FILE
  function clearSbiFile() {
    $file = "/etc/config/sbi";
    $tmp_file = "/etc/config/tmp_file";

    $file_handle = fopen($file, 'r') or die("File $file cannot be accessed for updating");
    $w_file_handle = fopen($tmp_file, 'w') or die("File $tmp_file cannot be opened for writing"); 

    // OPTION DEFINITIONS
    $lat = "Latitude";
    $lon = "Longitude";
    $city = "ContactCity";
    $email = "ContactEmail";
    $phone = "ContactPhone";
    $state = "ContactState";
    $street = "ContactStreet";
    $zip = "ContactZip";
    $owner = "DeviceOwner";
    $pref = "PreferredChannel";
    $power = "EnumeratedTransmitPower";
    $name = "ContactName";
    $stream = "StreamingIpAddress";

    while(!feof($file_handle)) {
      $temp = fgets($file_handle);
      if (strpos($temp, $lat)) {
	$temp = buildLine($lat, "");
      } else if (strpos($temp, $lon)) {
	$temp = buildLine($lon, "");
      } else if (strpos($temp, $city)) {
	$temp = buildLine($city, "");
      } else if (strpos($temp, $email)) {
	$temp = buildLine($email, "");
      } else if (strpos($temp, $phone)) {
	$temp = buildLine($phone, "");
      } else if (strpos($temp, $state)) {
	$temp = buildLine($state, "AL");
      } else if (strpos($temp, $street)) {
	$temp = buildLine($street, "");
      } else if (strpos($temp, $zip)) {
	$temp = buildLine($zip, "");
      } else if (strpos($temp, $owner)) {
	$temp = buildLine($owner, "");
      } else if (strpos($temp, $pref)) {
	$temp = buildLine($pref, "");
      } else if (strpos($temp, $power)) {
	$temp = buildLine($power, "11");
      } else if (strpos($temp, $name)) {
	$temp = buildLine($name, "");
      } else if (strpos($temp, $stream)) {
	$temp = buildLine($stream, "192.168.1.193");
      }

      fwrite($w_file_handle, $temp);
    } // END WHILE LOOP

    fclose($file_handle);
    fclose($w_file_handle);

    if (!rename($tmp_file, $file)) {
      echo "Failed to clear the sbi file!\n";
      return false;
    } else {
      return true;
    }

  } // END clearSbiFile FUNCTION

  // FUNCTION TO RESET THE setipeth0 ADDRESSES
  function resetSetipeth0() {

    global $config_dhcp, $config_ip, $config_subnet, $config_gateway;

    $file = "/etc/init.d/setipeth0";
    $tmp_file = "/etc/init.d/tmp_setipeth0";

    $file_handle = fopen($file, 'r') or die("File $file cannot be accessed for updating");
    $w_file_handle = fopen($tmp_file, 'w') or die("File $tmp_file cannot be opened for writing");

    while(!feof($file_handle)) {
      $temp = fgets($file_handle);
      if (strpos($temp, $config_dhcp) !== false) {
	$temp = $config_dhcp . "0\n";
      } else if (strpos($temp, $config_ip) !== false) {
	$temp = $config_ip . "192.168.1.194\n";
      } else if (strpos($temp, $config_subnet) !== false) {
	$temp = $config_subnet . "255.255.0.0\n";
      } else if (strpos($temp, $config_gateway) !== false) {
	$temp = $config_gateway . "192.168.1.254\n";
      }

      fwrite($w_file_handle, $temp);
    } // END WHILE LOOP

    fclose($file_handle);
    fclose($w_file_handle);

    if (!copy($tmp_file, $file)) {
      echo "Failed to reset setipeth0 file!\n";
      return false;
    } else {
      // NEEDS TO BE TESTED HERE AND ON network.php
      exec('chmod +x ' . $file);
      return true;
    }
  } // END resetSetipeth0 FUNCTION

  // FUNCTION TO CLEAR resolv.conf
  function clearResolv() {
    // MIGHT NOT NEED THIS
  }
  
  // FUNCTION TO DISPLAY TEXT FOR NO CHANNELS
  function display_channels($channels) {
    if ($channels == "") {
      return "No channels available";
    } else {
      return $channels;
    }
  } // END display_channels FUNCTION

?>
