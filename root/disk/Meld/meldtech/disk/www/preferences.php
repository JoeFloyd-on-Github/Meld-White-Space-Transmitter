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

/*      preferences.php - $Revision: 113 $ $Date: 2012-11-15 16:44:21 -0500 (Thu, 15 Nov 2012) $ $Author: mario $
 *	This script displays a webpage in which radio parameters
 *	can be changed
 */

  include('processor.php');
  $channels_file = "/etc/config/channels";
  $sbi_file = "/etc/config/sbi";
  $tmp_sbi_file = "/etc/config/tmp_file";

  $C_Power = "EnumeratedTransmitPower";
  $C_PreferredChannels = "PreferredChannel";

  if(isset($_POST['submit'])) {
  // OPEN THE FILE, WRITE TO ANOTHER AND REWRITE THE ORIGINAL
    $file_handle = fopen($sbi_file, 'r') or die("File $sbi_file cannot be accessed for updating");
    $w_file_handle = fopen($tmp_sbi_file, 'w') or die("File $tmp_sbi_file cannot be opened for writing");

    // FLAG TO KNOW IF SERVICE NEEDS TO BE RESTARTED
    $do_restart=0;
    if (getValueFromFile("$C_Power",$sbi_file) != $_POST[$C_Power] || getValueFromFile("$C_PreferredChannels",$sbi_file) != $_POST[$C_PreferredChannels]) {
      $do_restart=1;
    }

    // LOOP THROUGH THE FILE BEING READ
    if ($do_restart) {
      while(!feof($file_handle)) {
	$temp=fgets($file_handle);                        // READ EACH LINE
	if (strpos($temp, $C_Power)) {              	// POWER
	  $temp = buildLine($C_Power, $_POST[$C_Power]);
	} else if (strpos($temp, $C_PreferredChannels)) {	// PREFERRED CHANNELS
	  $temp = buildLine($C_PreferredChannels, $_POST[$C_PreferredChannels]);
	}
	fwrite($w_file_handle, $temp);                    // WRITE THE LINE TO THE NEW TEMP FILE

      }

      fclose($file_handle);                               // CLOSE THE FILE READ
      fclose($w_file_handle);                             // CLOSE THE FILE WE WROTE

      // COPY THE TEMP FILE TO THE ORIGINAL
      if (!copy($tmp_sbi_file, $sbi_file)) {
	  echo "Failed to copy $tmp_sbi_file to $sbi_file...\n";
      }

      // RESTART THE WS AGENT
      exec('wsagent_stop');
      exec("wsagent_start");
    }
  }
?>
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" xmlns:xml="http://www.w3.org/XML/1998/namespace" lang="en">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta http-equiv="Content-Script-Type" content="text/javascript" />
<link rel="stylesheet" type="text/css" media="screen" href="resources/cascade.css" />
<script type="text/javascript" src="resources/xhr.js"></script>
<title>MELDTECH - Radio Preferences</title>
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
	<li class="tabmenu-item-preferences active">
		<a href="preferences.php">Preferences</a>
	</li>

	<li class="tabmenu-item-network">
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
<form method="post" name="cbi" action="<?php echo htmlentities($_SERVER['PHP_SELF']); ?>" enctype="multipart/form-data" onreset="return cbi_validate_reset(this)" onsubmit="return cbi_validate_form(this, 'Some fields are invalid, cannot save values!')">
        <div>
                <script type="text/javascript" src="resources/cbi.js"></script>
                <input type="hidden" name="cbi.submit" value="1" />
                <input type="submit" value="Save" class="hidden" />
        </div>


<div class="cbi-map" id="cbi-channels">
<?php
  if(isset($_POST['submit'])) {
    echo "<font color='red'>Radio Preferences successfully saved & applied!</font><br />Please go to the Status tab to check device status";
  }
?>
        
<h2><a id="content" name="content">Radio Preferences</a></h2>
<fieldset class="cbi-section" id="cbi-preferences">
	<b>RF Settings</b>
	<div class="cbi-section-descr"></div>
                <fieldset class="cbi-section-node" id="cbi-sbi-meldtech">

<!-- POWER LEVEL -->
        <div class="cbi-value" id="cbi-sbi-meldtech-Power"><label class="cbi-value-title" for="cbid.sbi.meldtech.Power">Power Level</label>
                <div class="cbi-value-field">

        <select class="cbi-input-select" onchange="cbi_d_update(this.id)" id="cbid.sbi.meldtech.Power" name="EnumeratedTransmitPower" size="1">
        <?php
          for ($i=1; $i<12; $i++) {
            echo "<option value=\"$i\"";
            if ($power==$i) {
              echo " selected";
            }
            echo ">$i</option>";
          }
        ?>
        </select>


</div></div>

<!-- AVAILABLE CHANNELS -->

        <div class="cbi-value" id="cbi-sbi-meldtech-achannels"><label class="cbi-value-title" for="cbid.sbi.meldtech.achannels">Available Channels</label>
                <div class="cbi-value-field">

<?php
  echo display_channels(getValueFromFile("channels", $channels_file));
?>


</div></div>


<!-- PREFERRED CHANNELS -->
<div class="cbi-value" id="cbi-sbi-meldtech-preferredChannels"><label class="cbi-value-title" for="cbid.sbi.meldtech.PreferredChannels">Preferred Channels</label>
                <div class="cbi-value-field">

        <input type="text" class="cbi-input-text" onchange="cbi_d_update(this.id)" name="<?php echo $C_PreferredChannels; ?>" id="cbid.sbi.meldtech.PreferredChannels" value="<?php echo $preferred_channels; ?>" />
<br />
                        <div class="cbi-value-description">
                                <span class="cbi-value-helpicon"><img src="images/help.gif" alt="help" /></span>
				Specify up to 3 comma separated channels between 21 and 51: 23,42,50
                        </div>


</div></div>

                </fieldset>
                <br />

</fieldset>        <div class="cbi-page-actions">

                <input class="cbi-button cbi-button-apply" type="submit" name="submit" value="Save &amp; Apply" />

                <script type="text/javascript">cbi_d_update();</script>
        </div></form>

</div>
<h3>© 2012 Spectrum Bridge, Inc. All Rights Reserved.</h3>
<div class="clear"></div>
</div>
</div>

</body>
</html>
