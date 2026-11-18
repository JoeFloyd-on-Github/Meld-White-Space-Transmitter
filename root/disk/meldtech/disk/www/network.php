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

/*      network.php - $Revision: 28 $ $Date: 2012-08-17 11:42:55 -0400 (Fri, 17 Aug 2012) $ $Author: mario $
 *	This script displays a webpage in which networkparameters
 *	can be changed
 */

  include('processor.php');

  // DEFINE THE FILES
  $file = "/etc/init.d/setipeth0";
  $tmp_file = "/etc/init.d/tmp_setipeth0";
  $dns_file = "/etc/resolv.conf";
  $sbi_file = "/etc/config/sbi";

  // VARIABLE DEFINITIONS
  $configuration_type = "configuration-type";
  $static_ip = "static-ip";
  $subnet_mask = "subnet";
  $gateway_ip = "gateway";
  $dns1_server = "dns1";
  $dns2_server = "dns2";
  $streaming_ip = "streamingIP";

  $config_dns = "nameserver";

  $config_streaming = "StreamingIpAddress";

  // ------ POST -------
  if(isset($_POST['submit'])) {
  // OPEN THE FILE, WRITE TO ANOTHER AND REWRITE THE ORIGINAL
    $file_handle = fopen($file, 'r') or die("File $file cannot be accessed for updating");
    $w_file_handle = fopen($tmp_file, 'w') or die("File $tmp_file cannot be opened for writing");

    // LOOP THROUGH THE FILE BEING READ
    while(!feof($file_handle)) {
      $temp=fgets($file_handle);                        // READ EACH LINE
      if (strpos($temp, $config_dhcp) !== false) {              	// DHCP
        $temp = $config_dhcp . $_POST[$configuration_type] . "\n";
      } else if (strpos($temp, $config_ip) !== false) {			// IP ADDRESS
        $temp = $config_ip . $_POST[$static_ip] . "\n";
      } else if (strpos($temp, $config_subnet) !== false) {		// SUBNET MASK
        $temp = $config_subnet . $_POST[$subnet_mask] . "\n";
      } else if (strpos($temp, $config_gateway) !== false) {		// GATEWAY
        $temp = $config_gateway . $_POST[$gateway_ip] . "\n";
      }
      fwrite($w_file_handle, $temp);                    // WRITE THE LINE TO THE NEW TEMP FILE
    }

    fclose($file_handle);                               // CLOSE THE FILE READ
    fclose($w_file_handle);                             // CLOSE THE FILE WE WROTE

    // MOVE THE TEMP FILE TO THE ORIGINAL
    if (!rename($tmp_file, $file)) {
      $SETIP_SUCCESS = false;
    } else {
      exec('chmod +x ' . $file);
      $SETIP_SUCCESS = true;
    }

    if ($SETIP_SUCCESS) {
      // DO NOT TRY TO WRITE THE DNS FILE IF USING DHCP
      if ($_POST[$configuration_type] != "1") {
        $w_file_handle = fopen($dns_file, 'w') or die("File $dns_file cannot be opened for writing");
        $temp = $config_dns . " " . $_POST[$dns1_server] . "\n";
        fwrite($w_file_handle, $temp);                    // WRITE THE LINE TO THE NEW TEMP FILE
        $temp = $config_dns . " " . $_POST[$dns2_server] . "\n";
        fwrite($w_file_handle, $temp);                    // WRITE THE LINE TO THE NEW TEMP FILE
        fclose($w_file_handle);                             // CLOSE THE FILE WE WROTE
      }

      // WRITE STREAMING IP ADDRESS
      $file_handle = fopen($sbi_file, 'r') or die("File $sbi_file cannot be accessed for updating");
      $w_file_handle = fopen($tmp_file, 'w') or die("File $tmp_file cannot be opened for writing");
      while(!feof($file_handle)) {
        $temp = fgets($file_handle);
        if (strpos($temp, $config_streaming) !== false) {
          $temp = buildLine($config_streaming,  $_POST[$streaming_ip]);
        }
        fwrite($w_file_handle, $temp);
      }
      fclose($file_handle);
      fclose($w_file_handle);
      if (!rename($tmp_file, $sbi_file)) {
	$SUCCESS = false;
      } else {
	$SUCCESS = true;
      }
    }
    // RESET THE INTERFACE
    exec($file);

    // RESTART THE whitespaceagent
    exec('wsagent_stop');
    exec('wsagent_start');

    sleep(3);
 }
  // ----------- DONE WITH POST ------------

  //------------- READ THE IP CONFIGURATION FROM FILE -------------------
  $file_handle = fopen($file, 'r') or die("File ".$file." cannot be accessed");
  while(!feof($file_handle)) {
    $temp = fgets($file_handle);
    if (strpos($temp, $config_dhcp) !== false) {
      $chars = explode("=", $temp);
      $dhcp = trim($chars[1]);
    } else if (strpos($temp, $config_ip) !== false) {
      $chars = explode("=", $temp);
      $ip_address = trim($chars[1]);
    } else if (strpos($temp, $config_subnet) !== false) {
      $chars = explode("=", $temp);
      $subnet = trim($chars[1]);
    } else if (strpos($temp, $config_gateway) !== false) {
      $chars = explode("=", $temp);
      $gateway = trim($chars[1]);
    }
  }
  fclose($file_handle);
  //------------- DONE READING THE IP CONFIGURATION FILE --------------

  //------------- READ THE DNS CONFIGURATION FROM FILE -------------------
  $file_handle = fopen($dns_file, 'r') or die("File ".$dns_file." cannot be accessed");
  while(!feof($file_handle)) {
    $temp = fgets($file_handle);
    if ((strpos($temp, $config_dns) !== false) && $dns == "") {
      $chars = explode(" ", $temp);
      $dns = trim($chars[1]);
    } else if ((strpos($temp, $config_dns) !== false) && $dns != "") {
      $chars = explode(" ", $temp);
      $dns1 = trim($chars[1]);
    }
  }
  fclose($file_handle);
  //------------- DONE READING THE DNS CONFIGURATION FILE --------------

  //------------- OVERRIDE THE IP CONFIGURATION VARIABLES IF ON DHCP ---------------
  if ($dhcp == "1") {
    $ip_route = explode(" ", exec("ip route | grep src"));	// IP ADDRESS
    $ip_address = $ip_route[5];

    $ifconfig = explode("Mask:", exec("ifconfig eth0 | grep Mask"));	// SUBNET
    $subnet = $ifconfig[1];
    $ip_route_gw = explode(" ", exec("ip route | grep via"));	// GATEWAY
    $gateway = $ip_route_gw[2];
  }
  //------------- DONE OVERRIDING THE IP CONFIGURATION VARIABLES ---------------
?>
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" xmlns:xml="http://www.w3.org/XML/1998/namespace" lang="en">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta http-equiv="Content-Script-Type" content="text/javascript" />
<link rel="stylesheet" type="text/css" media="screen" href="resources/cascade.css" />
<script type="text/javascript" src="resources/xhr.js"></script>
<title>MELDTECH - Network Settings</title>
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

                        <li class="tabmenu-item-achannels">
                                <a href="status.php">Status</a>

                        </li>

                        <li class="tabmenu-item-advanced active">
                                <a href="preferences.php">Advanced</a>

                        </li>

        </ul>
        <br style="clear:both" />
        
	<div class="tabmenu2">
	<ul class="tabmenu l2">
	<li class="tabmenu-item-preferences">
		<a href="preferences.php">Preferences</a>
	</li>

	<li class="tabmenu-item-network active">
		<a href="network.php">Network</a>
	</li>
	
	<li class="tabmenu-item-achannels">
		<a href="upgrade.php">Tools</a>
	</li>
	</ul>
																													<br style="clear:both" />
	</div>
        </div>

        </div>


	<div id="maincontent">
<form method="post" name="cbi" action="<?php echo htmlentities($_SERVER['PHP_SELF']); ?>" enctype="multipart/form-data" onreset="return cbi_validate_reset(this)" onsubmit="return confirm('If you perform this action, you will have to specify a new IP address in your browser when the page goes blank!\nAre you sure you want to change the network configuration?')">
        <div>
                <script type="text/javascript" src="resources/cbi.js"></script>
                <input type="hidden" name="cbi.submit" value="1" />
                <input type="submit" value="Save" class="hidden" />
        </div>


<div class="cbi-map" id="cbi-channels">
<?php
  if(isset($_POST['submit'])) {
    if ($SUCCESS) {
      echo "<font color='red'>Network settings successfully saved & applied!</font><br />Please go to the appropriate IP address if it has been changed";
    } else {
      echo "<font color='red'>There was a problem saving the Network Configuration. Please try again!</font>";
    }
  }
?>
        
<h2><a id="content" name="content">Network Configuration</a></h2>
<fieldset class="cbi-section" id="cbi-preferences">
	<b>IP Settings</b>
	<div class="cbi-section-descr"></div>
                <fieldset class="cbi-section-node" id="cbi-sbi-meldtech">

        <div class="cbi-value" id="cbi-sbi-meldtech-Power"><label class="cbi-value-title" for="cbid.sbi.meldtech.type">Configuration Type</label>
                <div class="cbi-value-field">

        <select class="cbi-input-select" onchange="cbi_d_update(this.id)" id="cbid.sbi.meldtech.Type" name="<?php echo $configuration_type; ?>" size="1">
        <?php
            echo "<option value=\"1\" "; if($dhcp=="1") echo "selected"; echo ">DHCP</option>";
            echo "<option value=\"0\" "; if($dhcp=="0") echo "selected"; echo ">Static IP</option>";
        ?>
        </select>

</div></div>

<div class="cbi-value" id="cbi-sbi-meldtech-staticIp"><label class="cbi-value-title" for="cbid.sbi.meldtech.staticIp">IP Address</label>
                <div class="cbi-value-field">

        <input type="text" class="cbi-input-text" onchange="cbi_d_update(this.id)" name="<?php echo $static_ip;  ?>" id="cbid.sbi.meldtech.staticIp" value="<?php echo $ip_address; ?>" />
<br />

</div></div>

<div class="cbi-value" id="cbi-sbi-meldtech-subnet"><label class="cbi-value-title" for="cbid.sbi.meldtech.subnet">Subnet Mask</label>
                <div class="cbi-value-field">

        <input type="text" class="cbi-input-text" onchange="cbi_d_update(this.id)" name="<?php echo $subnet_mask; ?>" id="cbid.sbi.meldtech.subnet" value="<?php echo $subnet; ?>" />
<br />

</div></div>

<div class="cbi-value" id="cbi-sbi-meldtech-gateway"><label class="cbi-value-title" for="cbid.sbi.meldtech.gateway">Gateway Address</label>
                <div class="cbi-value-field">

        <input type="text" class="cbi-input-text" onchange="cbi_d_update(this.id)" name="<?php echo $gateway_ip; ?>" id="cbid.sbi.meldtech.gateway" value="<?php echo $gateway; ?>" />
<br />

</div></div>

<div class="cbi-value" id="cbi-sbi-meldtech-dns"><label class="cbi-value-title" for="cbid.sbi.meldtech.dns">Primary DNS Server</label>
                <div class="cbi-value-field">

        <input type="text" class="cbi-input-text" onchange="cbi_d_update(this.id)" name="<?php echo $dns1_server; ?>" id="cbid.sbi.meldtech.dns" value="<?php echo $dns; ?>" />
<br />

</div></div>

<div class="cbi-value" id="cbi-sbi-meldtech-dns1"><label class="cbi-value-title" for="cbid.sbi.meldtech.dns1">Secondary DNS Server</label>
                <div class="cbi-value-field">

        <input type="text" class="cbi-input-text" onchange="cbi_d_update(this.id)" name="<?php echo $dns2_server; ?>" id="cbid.sbi.meldtech.dns1" value="<?php echo $dns1; ?>" />
<br />

</div></div>

                </fieldset>
                <br />

</fieldset>
<fieldset class="cbi-section" id="cbi-preferences">
	<b>Streaming Settings</b>
	<div class="cbi-section-descr"></div>
                <fieldset class="cbi-section-node" id="cbi-sbi-meldtech">

<div class="cbi-value" id="cbi-sbi-meldtech-streamingIP"><label class="cbi-value-title" for="cbid.sbi.meldtech.streamingIP">Streaming IP Address</label>
                <div class="cbi-value-field">

        <input type="text" class="cbi-input-text" onchange="cbi_d_update(this.id)" name="<?php echo $streaming_ip;  ?>" id="cbid.sbi.meldtech.streamingIP" value="<?php echo $streaming; ?>" />
<br />

</div></div>

                </fieldset>
                <br />

</fieldset>
        <div class="cbi-page-actions">

                <input class="cbi-button cbi-button-apply" type="submit" name="submit" value="Save &amp; Apply" />

                <script type="text/javascript">cbi_d_update();</script>
 
</div>
<h3>© 2012 Spectrum Bridge, Inc. All Rights Reserved.</h3>
       </div></form>

<div class="clear"></div>
</div>
</div>
</body>
</html>
