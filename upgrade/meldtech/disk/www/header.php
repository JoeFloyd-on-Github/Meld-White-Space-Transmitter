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

/*      header.php - $Revision: 93 $ $Date: 2012-10-24 17:35:46 -0400 (Wed, 24 Oct 2012) $ $Author: mario $
 *      This script displays a the header webpage containing information 
 *      about the firmware/radio
 */

  //READ THE FCCID AND SERIAL NUMBER
  $id_file = "/etc/config/fccidSerialInfo";
  $id_file_handle = fopen($id_file, "r") or die("File ".$id_file." cannot be accessed");
  while (!feof($id_file_handle)) {                 // Loop through each line
    $line_of_text = fgets($id_file_handle);        // Get the current line

    $field = getFccidSerialInfo($line_of_text, TRUE);
    $value = getFccidSerialInfo($line_of_text, FALSE);

    if ($field == "Serial Number") {
      $serialNumber = $value;
    } else if ($field == "FCCID") {
      $fccid = $value;
    } else if ($field == "StreamingIpAddress") {
      $streaming = $value;
    } else if ($field == "NXP Version") {
      $nxp = $value;
    }

  }
  fclose($id_file_handle);

  //READ THE SBI CONFIG FILE FOR THE FORM DATA
  $sbi_file = "/etc/config/sbi";
  $file_handle = fopen($sbi_file, "r") or die("File ".$sbi_file." cannot be accessed");

  while (!feof($file_handle)) {                 // Loop through each line
    $line_of_text = fgets($file_handle);        // Get the current line

    $field = getFieldOrValueFromFile($line_of_text, TRUE);
    $value = getFieldOrValueFromFile($line_of_text, FALSE);

    if ($field == "ContactName") {                      // CONTACT NAME
      $contactName = $value;
    } else if ($field == "DeviceOwner") {               // DEVICE OWNER
      $ownerName= $value;
    } else if ($field == "ContactPhone") {              // CONTACT PHONE
      $phoneNumber = $value;
    } else if ($field == "ContactEmail") {              // CONTACT EMAIL
      $emailAddress = $value;
    } else if ($field == "ContactStreet") {             // CONTACT STREET
      $streetAddress = $value;
    } else if ($field == "ContactCity") {               // CONTACT CITY
      $city = $value;
    } else if ($field == "ContactState") {              // CONTACT STATE
      $state = $value;
    } else if ($field == "ContactZip") {                // CONTACT ZIP
      $zip = $value;
    } else if ($field == "ContactCountry") {               // Country Code
      $country = $value;
    } else if ($field == "Latitude") {                  // LATITUDE
      $latitude = $value;
    } else if ($field == "Longitude") {                 // LONGITUDE
      $longitude = $value;
    } else if ($field == "PreferredChannel") {		// PREFERRED CHANNELS
      $preferred_channels = $value;
    } else if ($field == "EnumeratedTransmitPower") {                     // POWER
      $power = $value;
    } else if ($field == "DeviceType") {                // DEVICE TYPE
      $deviceType = $value;
    } else if ($field == "AntennaHeightAglMeters") {    // ANTENNA HEIGHT
      $antennaHeight = $value;
    } else if ($field == "Version") {
      $version = $value;
    } else if ($field == "Date") {
      $date = $value;
    }

  }

  fclose($file_handle);

  // GET THE WSAGENT VERSION NUMBER
  $split_version = explode(":",exec('whitespaceagent -v'));
  $mini_split_version = explode(" ",$split_version[1]);
  $wsagent_version = $mini_split_version[0];
?>
<div id="menubar">
<h2 class="navigation"><a id="navigation" name="navigation">Navigation</a></h2>
<div class="hostinfo">
<table border="0">
     <tr>
           <td><img src="images/meld.jpg" /></td>
           <td> </td>
           <td style="color: #FF0000">
                <?php echo $fccid." - ".$serialNumber; ?><br />
                Version: <?php echo $nxp . " - " . $version . " - " . $wsagent_version; ?> <br />
                Release Date: <?php echo $date; ?>
            </td>

                 </tr>
    </table>
</div>


<div class="clear"></div>
</div>


