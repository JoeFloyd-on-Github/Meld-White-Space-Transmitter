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

/*      registration.php - $Revision: 76 $ $Date: 2012-10-04 13:26:56 -0400 (Thu, 04 Oct 2012) $ $Author: mario $
 *      This script displays a webpage in which the White Space
 *      registration information can be entered
 */

  include('processor.php');
  // <!-- THIS CODE WILL CHECK IF THE PAGE WAS SUBMITTED TO UPDATE DATA ON THE FORM -->
  $file = "/etc/config/sbi";
  $tmp_file = "/etc/config/tmp_file";

  $C_ContactName = "ContactName";			// CONTACT NAME
  $C_DeviceOwner = "DeviceOwner";			// DEVICE OWNER
  $C_ContactPhone = "ContactPhone";			// CONTACT PHONE
  $C_ContactEmail = "ContactEmail";			// CONTACT EMAIL
  $C_ContactStreet = "ContactStreet";			// CONTACT STREET
  $C_ContactCity = "ContactCity";			// CONTACT CITY
  $C_ContactState = "ContactState";			// CONTACT STATE
  $C_ContactZip = "ContactZip";				// CONTACT ZIP
  $C_ContactCountry= "ContactCountry";			// CONTACT Country
  $C_Latitude = "Latitude";				// LATITUDE
  $C_Longitude = "Longitude";				// LONGITUDE


  if(isset($_POST['submit']))
  {
    // OPEN THE FILE, WRITE TO ANOTHER AND REWRITE THE ORIGINAL
    $file_handle = fopen($file, 'r') or die("File cannot be accessed for updating");    
    $w_file_handle = fopen($tmp_file, 'w') or die("File cannot be opened for writing");

    // LOOP THROUGH THE FILE BEING READ
    while(!feof($file_handle)) {
      $temp=fgets($file_handle);			// READ EACH LINE
      if (strpos($temp, $C_ContactName)) {		// TEST FOR CONTACT NAME 
        $temp = buildLine($C_ContactName, $_POST[$C_ContactName]);
      } else if (strpos($temp, $C_DeviceOwner)) {
	$temp = buildLine($C_DeviceOwner, $_POST[$C_DeviceOwner]);
      } else if (strpos($temp, $C_ContactPhone)) {
	$temp = buildLine($C_ContactPhone, $_POST[$C_ContactPhone]);
      } else if (strpos($temp, $C_ContactEmail)) {
	$temp = buildLine($C_ContactEmail, $_POST[$C_ContactEmail]);
      } else if (strpos($temp, $C_ContactStreet)) {
	$temp = buildLine($C_ContactStreet, $_POST[$C_ContactStreet]);
      } else if (strpos($temp, $C_ContactCity)) {
	$temp = buildLine($C_ContactCity, $_POST[$C_ContactCity]);
      } else if (strpos($temp, $C_ContactState)) {
	$temp = buildLine($C_ContactState, $_POST[$C_ContactState]);
      } else if (strpos($temp, $C_ContactZip)) {
	$temp = buildLine($C_ContactZip, $_POST[$C_ContactZip]);
      } else if (strpos($temp, $C_ContactCountry)) {
	$temp = buildLine($C_ContactCountry, $_POST[$C_ContactCountry]);
      } else if (strpos($temp, $C_Longitude)) {
	$temp = buildLine($C_Longitude, $_POST[$C_Longitude]);
      } else if (strpos($temp, $C_Latitude)) {
	$temp = buildLine($C_Latitude, $_POST[$C_Latitude]);
      }
      fwrite($w_file_handle, $temp);			// WRITE THE LINE TO THE NEW TEMP FILE
    }

    fclose($file_handle);				// CLOSE THE FILE READ
    fclose($w_file_handle);				// CLOSE THE FILE WE WROTE

    // MOVE THE TEMP FILE TO THE ORIGINAL
    if (!rename($tmp_file, $file)) {
	echo "Failed to copy $tmp_file to $file...\n";
    }

    // RESTART THE WS AGENT
    exec('wsagent_stop');
    exec('wsagent_start');
  }
?>


<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" xmlns:xml="http://www.w3.org/XML/1998/namespace" lang="en">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta http-equiv="Content-Script-Type" content="text/javascript" />

<script type="text/javascript">

  function validateForm()
  {
    var cName=document.forms["cbi"]["<?php echo $C_ContactName; ?>"].value;
    if (cName==null || cName=="")
    {
      alert("Contact Name is required!");
      return false;
    }
    
    var oName=document.forms["cbi"]["<?php echo $C_DeviceOwner; ?>"].value;
    if (oName==null || oName=="")
    {
      alert("Owner Name is required!");
      return false;
    }

    var pNumber=document.forms["cbi"]["<?php echo $C_ContactPhone; ?>"].value;
    if (pNumber==null || pNumber=="")
    {
      alert("Phone Number is required!");
      return false;
    }
 
    var eAddress=document.forms["cbi"]["<?php echo $C_ContactEmail; ?>"].value;
    if (eAddress==null || eAddress=="")
    {
      alert("Email Address is required!");
      return false;
    }
 
    var sAddress=document.forms["cbi"]["<?php echo $C_ContactStreet; ?>"].value;
    if (sAddress==null || sAddress=="")
    {
      alert("Street Address is required!");
      return false;
    }
  
    var cCity=document.forms["cbi"]["<?php echo $C_ContactCity; ?>"].value;
    if (cCity==null || cCity=="")
    {
      alert("City is required!");
      return false;
    }
  
    var cState=document.forms["cbi"]["<?php echo $C_ContactState; ?>"].value;
    if (cState==null || cState=="")
    {
      alert("State is required!");
      return false;
    }
 
    var zCode=document.forms["cbi"]["<?php echo $C_ContactZip; ?>"].value;
    if (zCode==null || zCode=="")
    {
      alert("Zip Code is required!");
      return false;
    }
 
    var cCountry=document.forms["cbi"]["<?php echo $C_ContactCountry; ?>"].value;
    if (cCountry==null || cCountry=="")
    {
      alert("Country is required!");
      return false;
    }
 
    var cLat=document.forms["cbi"]["<?php echo $C_Latitude; ?>"].value;
    if (cLat==null || cLat=="")
    {
      alert("Latitude is required!");
      return false;
    }
 
    var cLon=document.forms["cbi"]["<?php echo $C_Longitude; ?>"].value;
    if (cLon==null || cLon=="")
    {
      alert("Longitude is required!");
      return false;
    }


  }
</script>

<link rel="stylesheet" type="text/css" media="screen" href="resources/cascade.css" />
<script type="text/javascript" src="resources/xhr.js"></script>
<title>MELDTECH - White Space Registration</title>
</head>
<body class="lang_en">

<?php include 'header.php'; 
?>
<div id="maincontainer">
        <div id="tabmenu">
        <br style="clear:both" />

        <div class="tabmenu1">
        <ul class="tabmenu l1">

                        <li class="tabmenu-item-registration active">
                                <a href="registration.php">White Space Registration</a>

                        </li>

                        <li class="tabmenu-item-achannels">
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

<form method="post" name="cbi" action="<?php echo htmlentities($_SERVER['PHP_SELF']); ?>" enctype="multipart/form-data" onreset="return cbi_validate_reset(this)" onsubmit="return validateForm()">
	<div>
		<script type="text/javascript" src="resources/cbi.js"></script>
		<input type="hidden" name="cbi.submit" value="1" />
		<input type="submit" value="Save" class="hidden" />
	</div>

<div class="cbi-map" id="cbi-sbi">
<?php
  if(isset($_POST['submit'])) {
    echo "<font color='red'>Registration information successfully saved & applied!</font><br />Please go to the Status tab to check registration status";
  }
?>
	<h2><a id="content" name="content">White Space Registration</a></h2>
	<div class="cbi-map-descr">Please fill out the form below to submit device registration to the White Space Database</div>
<fieldset class="cbi-section" id="cbi-sbi-device">
	<b>Who you are</b>
	<div class="cbi-section-descr"></div>
	


		

		<fieldset class="cbi-section-node" id="cbi-sbi-meldtech">
			


	<div class="cbi-value" id="cbi-sbi-meldtech-ContactName"><label class="cbi-value-title" for="cbid.sbi.meldtech.ContactName">Contact Name</label>
		<div class="cbi-value-field">

	<input type="text" class="cbi-input-text" onchange="cbi_d_update(this.id)" name="<?php echo $C_ContactName; ?>" id="cbid.sbi.meldtech.ContactName" value="<?php echo $contactName; ?>" />
	
			
</div></div>





<div class="cbi-value" id="cbi-sbi-meldtech-DeviceOwner">
	<label class="cbi-value-title" for="cbid.sbi.meldtech.DeviceOwner">Owner Name</label>
	<div class="cbi-value-field">

	<input type="text" class="cbi-input-text" onchange="cbi_d_update(this.id)" name="<?php echo $C_DeviceOwner; ?>" id="cbid.sbi.meldtech.DeviceOwner" value="<?php echo $ownerName; ?>" />
	
	<br />
	<div class="cbi-value-description">
		<span class="cbi-value-helpicon">
		<img src="images/help.gif" alt="help" /></span>
		Company Name or Representative
	</div>
</div></div>





<div class="cbi-value" id="cbi-sbi-meldtech-PhoneNumber"><label class="cbi-value-title" for="cbid.sbi.meldtech.PhoneNumber">Phone Number</label>
		<div class="cbi-value-field">

	<input type="text" class="cbi-input-text" onchange="cbi_d_update(this.id)" name=<?php echo $C_ContactPhone; ?> id="cbid.sbi.meldtech.PhoneNumber" value="<?php echo $phoneNumber; ?>" />
	
			
</div></div>





<div class="cbi-value" id="cbi-sbi-meldtech-ContactEmail"><label class="cbi-value-title" for="cbid.sbi.meldtech.ContactEmail">Email Address</label>
		<div class="cbi-value-field">

	<input type="text" class="cbi-input-text" onchange="cbi_d_update(this.id)" name="<?php echo $C_ContactEmail; ?>" id="cbid.sbi.meldtech.ContactEmail" value="<?php echo $emailAddress; ?>" />
	
			
</div></div>





<div class="cbi-value" id="cbi-sbi-meldtech-ContactStreet"><label class="cbi-value-title" for="cbid.sbi.meldtech.ContactStreet">Street Address</label>
		<div class="cbi-value-field">

	<input type="text" class="cbi-input-text" onchange="cbi_d_update(this.id)" name="ContactStreet" id="cbid.sbi.meldtech.ContactStreet" value="<?php echo $streetAddress; ?>" />
	
			
</div></div>





<div class="cbi-value" id="cbi-sbi-meldtech-ContactCity"><label class="cbi-value-title" for="cbid.sbi.meldtech.ContactCity">City</label>
		<div class="cbi-value-field">

	<input type="text" class="cbi-input-text" onchange="cbi_d_update(this.id)" name="ContactCity" id="cbid.sbi.meldtech.ContactCity" value="<?php echo $city; ?>" />
	
			
</div></div>





<div class="cbi-value" id="cbi-sbi-meldtech-ContactState"><label class="cbi-value-title" for="cbid.sbi.meldtech.ContactState">State</label>
		<div class="cbi-value-field">

	<select class="cbi-input-select" onchange="cbi_d_update(this.id)" name="ContactState" id="cbid.sbi.meldtech.ContactState" size="1" ?>" />
		<option value="AL" <?PHP if($state=="AL") echo "selected";?>>Alabama</option>
		<option value="AK" <?PHP if($state=="AK") echo "selected";?>>Alaska</option>
		<option value="AZ" <?PHP if($state=="AZ") echo "selected";?>>Arizona</option>
		<option value="AR" <?PHP if($state=="AR") echo "selected";?>>Arkansas</option>
		<option value="CA" <?PHP if($state=="CA") echo "selected";?>>California</option>
		<option value="CO" <?PHP if($state=="CO") echo "selected";?>>Colorado</option>
		<option value="CT" <?PHP if($state=="CT") echo "selected";?>>Connecticut</option>
		<option value="DE" <?PHP if($state=="DE") echo "selected";?>>Delaware</option>
		<option value="DC" <?PHP if($state=="DC") echo "selected";?>>District of Columbia</option>
		<option value="FL" <?PHP if($state=="FL") echo "selected";?>>Florida</option>
		<option value="GA" <?PHP if($state=="GA") echo "selected";?>>Georgia</option>
		<option value="HI" <?PHP if($state=="HI") echo "selected";?>>Hawaii</option>
		<option value="ID" <?PHP if($state=="ID") echo "selected";?>>Idaho</option>
		<option value="IL" <?PHP if($state=="IL") echo "selected";?>>Illinois</option>
		<option value="IN" <?PHP if($state=="IN") echo "selected";?>>Indiana</option>
		<option value="IA" <?PHP if($state=="IA") echo "selected";?>>Iowa</option>
		<option value="KS" <?PHP if($state=="KS") echo "selected";?>>Kansas</option>
		<option value="KY" <?PHP if($state=="KY") echo "selected";?>>Kentucky</option>
		<option value="LA" <?PHP if($state=="LA") echo "selected";?>>Louisiana</option>
		<option value="ME" <?PHP if($state=="ME") echo "selected";?>>Maine</option>
		<option value="MD" <?PHP if($state=="MD") echo "selected";?>>Maryland</option>
		<option value="MA" <?PHP if($state=="MA") echo "selected";?>>Massachusetts</option>
		<option value="MI" <?PHP if($state=="MI") echo "selected";?>>Michigan</option>
		<option value="MN" <?PHP if($state=="MN") echo "selected";?>>Minnesota</option>
		<option value="MS" <?PHP if($state=="MS") echo "selected";?>>Mississippi</option>
		<option value="MO" <?PHP if($state=="MO") echo "selected";?>>Missouri</option>
		<option value="MT" <?PHP if($state=="MT") echo "selected";?>>Montana</option>
		<option value="NE" <?PHP if($state=="NE") echo "selected";?>>Nebraska</option>
		<option value="NV" <?PHP if($state=="NV") echo "selected";?>>Nevada</option>
		<option value="NH" <?PHP if($state=="NH") echo "selected";?>>New Hampshire</option>
		<option value="NJ" <?PHP if($state=="NJ") echo "selected";?>>New Jersey</option>
		<option value="NM" <?PHP if($state=="NM") echo "selected";?>>New Mexico</option>
		<option value="NY" <?PHP if($state=="NY") echo "selected";?>>New York</option>
		<option value="NC" <?PHP if($state=="NC") echo "selected";?>>North Carolina</option>
		<option value="ND" <?PHP if($state=="ND") echo "selected";?>>North Dakota</option>
		<option value="OH" <?PHP if($state=="OH") echo "selected";?>>Ohio</option>
		<option value="OK" <?PHP if($state=="OK") echo "selected";?>>Oklahoma</option>
		<option value="OR" <?PHP if($state=="OR") echo "selected";?>>Oregon</option>
		<option value="PA" <?PHP if($state=="PA") echo "selected";?>>Pennsylvania</option>
		<option value="RI" <?PHP if($state=="RI") echo "selected";?>>Rhode Island</option>
		<option value="SC" <?PHP if($state=="SC") echo "selected";?>>South Carolina</option>
		<option value="SD" <?PHP if($state=="SD") echo "selected";?>>South Dakota</option>
		<option value="TN" <?PHP if($state=="TN") echo "selected";?>>Tennessee</option>
		<option value="TX" <?PHP if($state=="TX") echo "selected";?>>Texas</option>
		<option value="UT" <?PHP if($state=="UT") echo "selected";?>>Utah</option>
		<option value="VT" <?PHP if($state=="VT") echo "selected";?>>Vermont</option>
		<option value="VA" <?PHP if($state=="VA") echo "selected";?>>Virginia</option>
		<option value="WA" <?PHP if($state=="WA") echo "selected";?>>Washington</option>
		<option value="WV" <?PHP if($state=="WV") echo "selected";?>>West Virginia</option>
		<option value="WI" <?PHP if($state=="WI") echo "selected";?>>Wisconsin</option>
		<option value="WY" <?PHP if($state=="WY") echo "selected";?>>Wyoming</option>	
	</select>			
</div></div>





<div class="cbi-value" id="cbi-sbi-meldtech-ContactZip"><label class="cbi-value-title" for="cbid.sbi.meldtech.ContactZip">Zip Code</label>
		<div class="cbi-value-field">

	<input type="text" class="cbi-input-text" onchange="cbi_d_update(this.id)" name="ContactZip" id="cbid.sbi.meldtech.ContactZip" value="<?php echo $zip; ?>" />
	
			
</div></div>





<div class="cbi-value cbi-value-last" id="cbi-sbi-meldtech-ContactCountry"><label class="cbi-value-title" for="cbid.sbi.meldtech.ContactCountry">Country</label>
		<div class="cbi-value-field">

	<input type="text" class="cbi-input-text" readonly="readonly" onchange="cbi_d_update(this.id)" name="<?php echo $C_ContactCountry; ?>" id="cbid.sbi.meldtech.ContactCountry" value="<?php echo $country; ?>" />
	
			
</div></div>
		</fieldset>
		<br />

	

	
</fieldset>
<fieldset class="cbi-section" id="cbi-sbi-device">
	<b>Where you are</b>
	<div class="cbi-section-descr"></div>
	


		

		<fieldset class="cbi-section-node" id="cbi-sbi-meldtech">
			


	<div class="cbi-value" id="cbi-sbi-meldtech-Latitude"><label class="cbi-value-title" for="cbid.sbi.meldtech.Latitude">Latitude</label>
		<div class="cbi-value-field">

	<input type="text" class="cbi-input-text" onchange="cbi_d_update(this.id)" name="<?php echo $C_Latitude; ?>" id="cbid.sbi.meldtech.Latitude" value="<?php echo $latitude; ?>" />
	
			<br />
			<div class="cbi-value-description">
				<span class="cbi-value-helpicon"><img src="images/help.gif" alt="help" /></span>
				Coordinates in decimal format
			</div>
</div></div>





<div class="cbi-value cbi-value-last" id="cbi-sbi-meldtech-Longitude"><label class="cbi-value-title" for="cbid.sbi.meldtech.Longitude">Longitude</label>
		<div class="cbi-value-field">

	<input type="text" class="cbi-input-text" onchange="cbi_d_update(this.id)" name="<?php echo $C_Longitude; ?>" id="cbid.sbi.meldtech.Longitude" value="<?php echo $longitude; ?>" />
	
			<br />
			<div class="cbi-value-description">
				<span class="cbi-value-helpicon"><img src="images/help.gif" alt="help" /></span>
				Coordinates in decimal format
			</div>
</div></div>

		</fieldset>
		<br />
	
</fieldset>
<fieldset class="cbi-section" id="cbi-sbi-device">
	<b>Device Information</b>
	<div class="cbi-section-descr"></div>
	
		<fieldset class="cbi-section-node" id="cbi-sbi-meldtech">
			


	<div class="cbi-value" id="cbi-sbi-meldtech-FccID"><label class="cbi-value-title" for="cbid.sbi.meldtech.FccID">FCC ID</label>
		<div class="cbi-value-field">

<?php echo $fccid; ?>

		
</div></div>


<div class="cbi-value" id="cbi-sbi-meldtech-MfgSerialNumber"><label class="cbi-value-title" for="cbid.sbi.meldtech.MfgSerialNumber">Serial Number</label>
		<div class="cbi-value-field">

<?php echo $serialNumber; ?>

</div></div>

<div class="cbi-value" id="cbi-sbi-meldtech-DeviceType"><label class="cbi-value-title" for="cbid.sbi.meldtech.DeviceType">Device Type</label>
		<div class="cbi-value-field">

<?php echo $deviceType; ?>

</div></div>
		</fieldset>
</br>	
</fieldset>
	<div class="cbi-page-actions">
		
		<input class="cbi-button cbi-button-apply" type="submit" name="submit" value="Save &amp; Apply" />
		
		<script type="text/javascript">cbi_d_update();</script>
	</div></form>
<h3>© 2012 Spectrum Bridge, Inc. All Rights Reserved.</h3>

</div>
<div class="clear"></div>
</div>
</div>

</body>
</html>
