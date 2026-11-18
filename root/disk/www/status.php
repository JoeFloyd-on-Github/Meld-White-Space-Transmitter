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

/*      status.php - $Revision: 92 $ $Date: 2012-10-23 14:51:17 -0400 (Tue, 23 Oct 2012) $ $Author: mario $
 *      This script creates a webpage in which radio/white space
 *      information is displayed
 */

  include('processor.php');
  $operating_channel_file = "/etc/config/operating";
  $transmitter_file = "/etc/config/transmitter";
  $transmitter_file = "/etc/config/transmitter";
  $registration_file = "/etc/config/registration";
  $channel_status_file = "/etc/config/channel_status";
  $channels_file = "/etc/config/channels";
?>
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" xmlns:xml="http://www.w3.org/XML/1998/namespace" lang="en">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta http-equiv="Content-Script-Type" content="text/javascript" />
<link rel="stylesheet" type="text/css" media="screen" href="resources/cascade.css" />
<script type="text/javascript" src="resources/xhr.js"></script>
<title>MELDTECH - White Space Status</title>
</head>
<body class="lang_en">

<?php include 'header.php'; ?>

<div id="maincontainer">
        <div id="tabmenu">

        <br style="clear:both" />

        <div class="tabmenu1">
        <ul class="tabmenu l1">

                        <li class="tabmenu-item-registration">
                                <a href="registration.php">White Space Registration</a>

                        </li>

                        <li class="tabmenu-item-achannels active">
                                <a href="status.php">Status</a>

                        </li>
                       <li class="tabmenu-item-advanced">
                                <a href="preferences.php">Advanced</a>

                        </li>

        </ul>
        <br style="clear:both" />

        </div>

        </div>


	<div id="maincontent">


<div class="cbi-map" id="cbi-channels">
<h2><a id="content" name="content">Device Status</a></h2>
<!-- COMMENTED OUT
<div class="cbi-map-descr">This is the radio status</div>
-->
<fieldset class="cbi-section" id="cbi-channels-channel">
	<b>Radio Status</b>
	<div class="cbi-section-descr"></div>
	<fieldset class="cbi-section-node" id="cbi-sbi-meldtech">
<ul><li> 
Operating Channel: <b><?php
  echo getValueFromFile("channel", $operating_channel_file);
?>
</b>
 </li></ul>
<ul><li> 
Transmitter status: <b>
<?php
  echo (getValueFromFile("status", $transmitter_file) > 0 ? "Enabled" : "Disabled");
?>
</b>
</li></ul>
</fieldset>
</fieldset>
<br />
<!-- COMMENTED OUT
<div class="cbi-map-descr">These are the available channels at your location</div>
-->
<fieldset class="cbi-section" id="cbi-channels-channel">
	<b>White Space Status</b>
	<div class="cbi-section-descr"></div>
	<fieldset class="cbi-section-node" id="cbi-sbi-meldtech">
<ul><li> 
Registration Status: <b>
<?php
  $status_code = getValueFromFile("status", $registration_file);
  if ($status_code == "99") {
    if ($contactName == "") {
      echo "Registration Form needs information";
    } else {
      echo "Network Problem. Please check your network configuration";
    }
  } else {
    echo getStatusCodeDescription($status_code);
  }
?>
</b>
 </li></ul>

<ul><li> 
Last Registration Request: <b>
<?php
  echo getValueFromFile("time_stamp", $registration_file);
?>
</b>
 </li></ul>
<br />
<ul><li> 
Channel Request Status: <b>
<?php
  echo getStatusCodeDescription(getValueFromFile("status", $channel_status_file));
?></b>
 </li></ul>

<ul><li> 
Last Channel Request: <b>
<?php
  echo getValueFromFile("time_stamp", $channel_status_file);
?>
</b>
 </li></ul>
<br />

	<ul><li> 
	 Available Channels: <b>
<?php
  echo display_channels(getValueFromFile("channels", $channels_file));
?>
</b>
	</li></ul>
</fieldset>
</fieldset>
</div>
<h3>© 2012 Spectrum Bridge, Inc. All Rights Reserved.</h3>
<div class="clear"></div>
</div>
</div>

</body>
</html>
